#include <termios.h> // termios
#include <unistd.h> // tcgetattr, tcsetattr
#include "Options.hpp"
#include "LineForwarder.hpp"
#include "BinaryForwarder.hpp"
#include "ProcessObserver.hpp"
#include "Service.hpp"

namespace rget
{

Service::Service()
	: StreamSocket(options()->address()),
	  connectLog_(new LogFile(options()->address(), LogFile::Connect)),
	  repeat_(options()->repeat_),
	  loop_(options()->loop_),
	  fileIndex_(0)
{
	closeOnExec();
	
	abortEvent_ = new EventManager;
	onSignal(SIGINT)->pushBack(abortEvent_);
	onSignal(SIGTERM)->pushBack(abortEvent_);
	onSignal(SIGCHLD)->pushBack(abortEvent_); // quick HACK
	
	class AbortHandler: public EventHandler {
	public:
		AbortHandler(Ref<Service> service)
			: service_(service)
		{}
		virtual void run() {
			print("(%%) Service::AbortHandler::run()\n", options()->execName());
			service_->abort();
		}
	private:
		Ref<Service, Owner> service_;
	};
	
	abortEvent_->pushBack(new AbortHandler(this));
}

void Service::init()
{
	if (options()->server_)
		connectLog_->writeLine(address(), "Start listening");
}

void Service::idle()
{
	print("(%%) Service::idle()\n", options()->execName());
}

void Service::serve(Ref<StreamSocket> socket)
{
	connectLog_->writeLine(socket->address(), "Connection established");
	
	socket->closeOnExec();
	
	String entity;
	
	if (options()->files_->length() > 0)
	{
		entity = options()->files_->get(fileIndex_);
		fileIndex_ = (fileIndex_ + 1) % options()->files_->length();
	}
	
	if (options()->canon_)
		canonSession(socket, entity);
	else
		binarySession(socket, entity);
	
	repeat_ -= (repeat_ > 0);
	
	connectLog_->writeLine(socket->address(), "Connection closed");
	
	if ((repeat_ == 0) && (!loop_))
		abort();
}

void Service::cleanup()
{
	if (options()->server_)
		connectLog_->writeLine(address(), "Stop listening");
}

Ref<Process, Owner> Service::exec(String entity)
{
	print("(%%) Executing %%\n", options()->execName(), entity);
	
	Ref<Process, Owner> process = new Process;
	process->setType(Process::GroupLeader);
	{
		Ref<StringList, Owner> l = entity / ' ';
		if (l->length() > 1) {
			entity = l->get(0);
			for (int i = 1; i < l->length(); ++i)
				process->options()->append(l->get(i));
		}
	}
	if ((entity / '/')->length() == 1) {
		String s = lookupPath(env("PATH") / ':', entity);
		if (s != "") entity = s;
	}
	process->setExecPath(entity);
	process->setIoPolicy(Process::ForwardInput|Process::ForwardOutput|Process::ErrorToOutput);
	process->start();
	
	return process;
}

Ref<StreamSocket, Owner> Service::openTunnel(String entity)
{
	Ref<StringList, Owner> pair = entity / ':';
	String host = pair->get(0);
	int port = 7373;
	if (pair->length() == 2) {
		bool ok = false;
		port = toInt(pair->get(1), &ok);
		if (!ok) port = 7373;
	}
	print("(%%) Connection to tunnel target %%:%%\n", options()->execName(), host, port);
	Ref<SocketAddressList, Owner> choice = SocketAddress::resolve(host, Format("%%") << int(port), bool(options()->inet6_) ? AF_INET6 : AF_UNSPEC);
	Ref<SocketAddress, Owner> address = choice->first();
	Ref<StreamSocket, Owner>  socket = new StreamSocket(address);
	socket->connect();
	while (!socket->established(3))
		print("(%%) No response from tunnel target %%\n", options()->execName(), host, port);
	return socket;
}

void Service::canonSession(Ref<StreamSocket> socket, String entity)
{
	Ref<SystemStream, Owner> recvSink;
	Ref<SystemStream, Owner> sendSource;
	
	Ref<Process, Owner> process;
	Ref<ProcessObserver, Owner> processObserver;
	Ref<Process, Owner> editor;
	
	Time t0 = now();
	Ref<LogFile, Owner> mergedLog = new LogFile(socket->address(), LogFile::Merged, t0);
	Ref<LogFile, Owner> recvLog = new LogFile(socket->address(), LogFile::Recv, t0, mergedLog);
	Ref<LogFile, Owner> sendLog = new LogFile(socket->address(), LogFile::Send, t0, mergedLog);
	
	if (options()->exec_) {
		if (entity != "") {
			process = exec(entity);
			processObserver = new ProcessObserver(process);
			processObserver->finishedEvent()->pushBack(abortEvent_);
			processObserver->start();
			recvSink = process->rawInput();
			sendSource = process->rawOutput();
		}
	}
	else if (options()->tunnel_) {
	 	if (entity != "") {
			Ref<StreamSocket, Owner> tunnelSocket = openTunnel(entity);
			recvSink = tunnelSocket;
			sendSource = tunnelSocket;
		}
	}
	else {
		if (entity != "") {
			Ref<File, Owner> file = new File(entity);
			file->open(File::Read);
			Ref<LineSource, Owner> lineSource = new LineSource(file);
			Ref<LineSink, Owner> lineSink = new LineSink(socket, options()->ioUnit_, options()->eol_);
			bool eoi = false;
			while (true) {
				String line = lineSource->readLine(&eoi);
				if (eoi) break;
				if (sendLog)
					sendLog->writeLine(line);
				lineSink->writeLine(line);
			}
		}
		
		Ref<Pipe, Owner> recvPipe = new Pipe(Pipe::Output);
		Ref<Pipe, Owner> sendPipe = new Pipe(Pipe::Input);
		
		editor = new Process;
		editor->setExecPath(options()->editorPath_);
		editor->options()->append(Format("--fdr=%%") << recvPipe->childFd());
		editor->options()->append(Format("--fds=%%") << sendPipe->childFd());
		editor->start();
		
		recvPipe->open();
		sendPipe->open();
		
		recvSink = recvPipe;
		sendSource = sendPipe;
	}
	
	Ref<LineForwarder, Owner> recvForwarder = new LineForwarder(socket, recvSink, options()->eol_, "\012", recvLog, abortEvent_);
	Ref<LineForwarder, Owner> sendForwarder = new LineForwarder(sendSource, socket, "\012", options()->eol_, sendLog, abortEvent_);
	recvForwarder->start();
	sendForwarder->start();
	
	print("(%%) recvForwarder->wait()\n", options()->execName());
	recvForwarder->wait();
	print("(%%) sendForwarder->wait()\n", options()->execName());
	sendForwarder->wait();
	
	if (editor) {
		print("(%%) editor->wait()\n", options()->execName());
		try { editor->sendSignal(SIGINT); } catch(AnyException& ex) {}
		editor->wait();
	}
	if (process) {
		print("(%%) process->wait()\n", options()->execName());
		try { process->sendSignal(SIGKILL); } catch(AnyException& ex) {}
		int ret = processObserver->wait();
		print("(%%) ret = %%\n", options()->execName(), ret);
	}
}

void Service::binarySession(Ref<StreamSocket> socket, String entity)
{
	struct termios tioSaved;
	
	Ref<SystemStream, Owner> recvSink = rawOutput();
	Ref<SystemStream, Owner> sendSource = rawInput();
	
	if (sendSource->interactive()) {
		if (::tcgetattr(sendSource->fd(), &tioSaved) == -1)
			PONA_SYSTEM_EXCEPTION;
		struct termios tio = tioSaved;
		tio.c_lflag = tio.c_lflag & (~ICANON);
		if (::tcsetattr(sendSource->fd(), TCSANOW, &tio) == -1)
			PONA_SYSTEM_EXCEPTION;
	}
	
	Ref<Process, Owner> process;
	Ref<ProcessObserver, Owner> processObserver;
	
	Time t0 = now();
	Ref<LogFile, Owner> mergedLog = new LogFile(socket->address(), LogFile::Merged, t0);
	Ref<LogFile, Owner> recvLog = new LogFile(socket->address(), LogFile::Recv, t0, mergedLog);
	Ref<LogFile, Owner> sendLog = new LogFile(socket->address(), LogFile::Send, t0, mergedLog);
	
	if (entity != "") {
		if (options()->exec_) {
			process = exec(entity);
			processObserver = new ProcessObserver(process);
			processObserver->finishedEvent()->pushBack(abortEvent_);
			processObserver->start();
			recvSink = process->rawInput();
			sendSource = process->rawOutput();
		}
		else if (options()->tunnel_) {
			Ref<StreamSocket, Owner> tunnelSocket = openTunnel(entity);
			recvSink = tunnelSocket;
			sendSource = tunnelSocket;
		}
		else {
			Ref<File, Owner> file = new File(entity);
			file->open(File::Read);
			Array<uint8_t> buf(options()->ioUnit_);
			while (true) {
				int fill = file->readAvail(buf, buf.size());
				if (fill == 0) break;
				if (sendLog)
					sendLog->write(buf, fill);
				socket->write(buf, fill);
			}
		}
	}
	
	{
		Ref<BinaryForwarder, Owner> sendForwarder = new BinaryForwarder(sendSource, socket, sendLog, abortEvent_);
		Ref<BinaryForwarder, Owner> recvForwarder = new BinaryForwarder(socket, recvSink, recvLog, abortEvent_);
		sendForwarder->start();
		recvForwarder->start();
		print("(%%) recvForwarder->wait() ...\n", options()->execName());
		recvForwarder->wait();
		sendForwarder->abort();
		print("(%%) sendForwarder->wait() ...\n", options()->execName());
		sendForwarder->wait();
	}
	
	if (process) {
		print("(%%) process->wait() ...\n", options()->execName());
		try { process->sendSignal(SIGKILL); } catch(AnyException& ex) {}
		int ret = processObserver->wait();
		print("(%%) ret = %%\n", options()->execName(), ret);
	}
	
	if (sendSource->interactive()) {
		if (::tcsetattr(sendSource->fd(), TCSAFLUSH, &tioSaved) == -1)
			PONA_SYSTEM_EXCEPTION;
	}
}

} // namespace rget
