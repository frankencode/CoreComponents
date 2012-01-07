#include <ftl/streams>
#include <ftl/process>

namespace ftl
{

int echo(int argc, char** argv);

int main(int argc, char** argv)
{
	if (argc == 2)
		return echo(argc, argv);
	
	{
		class TestFactory: public ProcessFactory
		{
		public:
			int incarnate() {
				print("Good morning %%.\n", User(Process::realUserId()).fullName());
				return 7;
			}
		};
		
		print("(1) Worker clone\n\n");
		
		Ref<ProcessFactory, Owner> factory = new TestFactory;
		Ref<Process, Owner> worker = factory->produce();
		int ret = worker->wait();
		print("ret = %%\n", ret);
		print("\n");
	}
	
	{
		print("(2) I/O Redirection, passing of arguments and environment variables\n\n");
		
		Ref<ProcessFactory, Owner> factory = new ProcessFactory;
		factory->setExecPath(argv[0]);
		factory->setIoPolicy(Process::ForwardInput | Process::ForwardOutput);
		factory->options()->append("--echo 123");
		factory->envMap()->define("Hello", "World!");
		
		Ref<Process, Owner> process = factory->produce();
		
		print("Created child process with pid = %%\n", unsigned(process->id()));

		const char* message =
			"Hello, world!\n"
			"exit\n";
		process->rawInput()->write(message, str::len(message));
		process->rawInput()->close();
		
		const int bufSize = 16;
		uint8_t buf[bufSize];
		while (true) {
			int bufFill = process->rawOutput()->readAvail(buf, bufSize);
			if (bufFill == 0) break;
			rawOutput()->write(buf, bufFill);
		}
		
		print("Waiting for child process to finish...\n");
		
		int ret = process->wait();
		print("ret = %%\n", ret);
		print("\n");
	}
	
	{
		print("(3) Current process\n\n");
		print("Process::cwd() = %%\n", Process::cwd());
		print("Process::execPath() = %%\n", Process::execPath());
		print("Process::isSuperUser() = %%\n", Process::isSuperUser());
		print("\n");
	}
	
	return 0;
}

int echo(int argc, char** argv)
{
	Ref<StringList, Owner> commandLine = new StringList;
	for (int i = 0; i < argc; ++i) {
		commandLine->append(argv[i]);
		if (i != argc - 1)
			commandLine->append(" ");
	}
	print("Process::cwd() = \"%%\"\n", Process::cwd());
	print("Process::env(\"Hello\") = \"%%\"\n", Process::env("Hello"));
	Process::setEnv("Hello", "Echo");
	print("Process::env(\"Hello\") = \"%%\"\n", Process::env("Hello"));
	print("commandLine = \"%%\"\n", commandLine->join());
	
	while (true) {
		String line = input()->readLine();
		if ((line == "") || (line == "exit"))
			break;
		output()->writeLine(line);
	}
	
	return 13;
}

} // namespace ftl

int main(int argc, char** argv)
{
	return ftl::main(argc, argv);
}
