#include <pona/Core.hpp>

namespace pona
{

class EchoWorker: public Thread
{
public:
	EchoWorker(Ref<TcpSocket> server, Ref<SocketAddress> address, Ref<TcpStream> stream)
		: server_(server),
		  address_(address),
		  stream_(stream),
		  name_(format("EchoWorker(%%:%%)") % address_->addressString() % address_->port())
	{}
	
	String name() const { return name_; }
	
private:
	int run()
	{
		print("%%: Started ...\n", name_);
		
		LineSource source(stream_);
		LineSink sink(stream_);
		
		try {
			while (true)
			{
				String line = source.readLine();
				
				if (line == String()) {
					print("%%: Connection closed by remote host\n", name_);
					break;
				}
				else
				if (line == String("halt")) {
					print("%%: Remote host requested to halt the server\n", name_);
					server_->close();
					break;
				}
				else
				if (line == String("close")) {
					print("%%: Remote host requested to close this session\n", name_);
					break;
				}
				
				sink.writeLine(line);
			}
		}
		catch (std::exception& ex) {
			print("%%: %%\n", name_, ex.what());
		}
		
		print("%%: Going down\n", name_);
		return 0;
	}
	
	Ref<TcpSocket, Owner> server_;
	Ref<SocketAddress, Owner> address_;
	Ref<TcpStream, Owner> stream_;
	String name_;
};

class EchoServer: public TcpSocket
{
public:
	EchoServer()
		: TcpSocket(new SocketAddress(AF_INET, "*", 8001)),
		  pool_(new Pool),
		  name_(format("EchoServer(%%:%%)") % localAddress()->addressString() % localAddress()->port())
	{}
	
	void init()
	{
		print("%%: Listening ...\n", name_);
	}
	
	void serve(Ref<SocketAddress> address, Ref<TcpStream> stream)
	{
		Ref<EchoWorker> worker = new EchoWorker(this, address, stream);
		pool_->append(worker);
		worker->start();
	}
	
	void idle()
	{
		print("%%: idle\n", name_);
	}
	
	void cleanup()
	{
		print("%%: Going down\n", name_);
		print("%%: Waiting for all worker threads to terminate...\n", name_);
		while (pool_->length() > 0) {
			print("%%: Number of threads left: %%\n", name_, pool_->length());
			Ref<EchoWorker> worker = pool_->get(0);
			int ret = worker->wait();
			print("%%: %% terminated with ret = %%\n", name_, worker->name(), ret);
			pool_->pop(0);
		}
		print("%%: All worker threads terminated\n", name_);
	}
	
private:
	typedef List< Ref<EchoWorker, Owner> > Pool;
	Ref<Pool, Owner> pool_;
	String name_;
};

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		if (String(argv[1]) == "--server")
		{
			Ref<EchoServer, Owner> server = new EchoServer;
			return server->run();
		}
		else if (String(argv[1]) == "--client")
		{
			Ref<TcpStream, Owner> socket = TcpSocket::connect(new SocketAddress(AF_INET, "127.0.0.1", 8001));
			Ref<LineSource, Owner> socketSource = new LineSource(socket);
			Ref<LineSink, Owner> socketSink = new LineSink(socket);
			while (true)
			{
				print("> ");
				String line = input()->readLine();
				socketSink->writeLine(line);
				if ((line == "close") || (line == "halt"))
					break;
				print("%%\n", socketSource->readLine());
			}
			return 0;
		}
	}
	
	print("\n");
	print("Usage: %% OPTIONS\n", fileName(argv[0]));
	print("\n");
	print("Options:\n");
	print("  --server ... TCP echo server\n");
	print("  --client ... TCP echo client\n");
	print("\n");
	
	return 1;
}

} // namespace pona

int main(int argc, char** argv)
{
	int ret = 1;

	try {
		ret = pona::main(argc, argv);
	}
	catch (std::exception& ex) {
		pona::print("%%\n", ex.what());
	}

	return ret;
}
