#include <pona/stdio>
#include <pona/context>

namespace pona
{

int echo(int argc, char** argv);

int main(int argc, char** argv)
{
	if (argc > 1)
		return echo(argc, argv);
	
	{
		class TestProcess: public Process
		{
		public:
			int run()
			{
				print("Hello, world!\n");
				return 7;
			}
		};
		
		print("(1) Worker clone\n\n");
		
		TestProcess process;
		process.start();
		int ret = process.wait();
		print("ret = %%\n", ret);
		print("\n");
	}
	
	{
		print("(2) I/O Redirection, passing of arguments and environment variables\n\n");
		
		Process process;
		process.setExecPath(argv[0]);
		process.setWorkingDirectory("/");
		process.setIoPolicy(Process::ForwardInput | Process::ForwardOutput);
		process.options()->append("--echo 123");
		process.envMap()->set("Hello", "World!");
		process.start();
		
		print("Created child process with pid = %%\n", unsigned(process.pid()));

		const char* message =
			"Hello, world!\n"
			"exit\n";
		process.rawInput()->write(message, strlen(message));
		process.rawInput()->close();
		
		const int bufSize = 16;
		uint8_t buf[bufSize];
		while (true) {
			int bufFill = process.rawOutput()->readAvail(buf, bufSize);
			if (bufFill == 0) break;
			rawOutput()->write(buf, bufFill);
		}
		
		print("Waiting for child process to finish...\n");
		
		int ret = process.wait();
		print("ret = %%\n", ret);
		print("\n");
	}
	
	{
		print("(3) Current context\n\n");
		print("cwd() = %%\n", cwd());
		print("isSuperUser() = %%\n", isSuperUser());
		print("\n");
	}
	
	return 0;
}

int echo(int argc, char** argv)
{
	String commandLine;
	for (int i = 0; i < argc; ++i) {
		commandLine << argv[i];
		if (i != argc - 1)
			commandLine << " ";
	}
	print("cwd() = \"%%\"\n", cwd());
	print("env(\"Hello\") = \"%%\"\n", env("Hello"));
	setEnv("Hello", "Echo");
	print("env(\"Hello\") = \"%%\"\n", env("Hello"));
	print("commandLine = \"%%\"\n", commandLine);
	
	while (true)
	{
		String line = input()->readLine();
		if ((line == String()) || (line == "exit"))
			break;
		output()->writeLine(line);
	}
	
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
