#include <pona/Core.hpp>

namespace pona
{

int echo(int argc, char** argv);

int main(int argc, char** argv)
{
	if (argc > 1)
		return echo(argc, argv);
	
	#ifndef PONA_WINDOWS
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
	#endif
	
	{
		print("(2) I/O Redirection, passing of arguments and environment variables\n\n");
		
		Process process;
#ifdef PONA_POSIX
		process.setExecPath("testProcess");
		process.setWorkingDirectory("/");
#else // if PONA_WINDOWS
		process.setExecPath("testProcess.exe");
		process.setWorkingDirectory("C:\\");
#endif
		process.setIoPolicy(Process::ForwardInput | Process::ForwardOutput);
		process.arguments()->append("--echo 123");
		process.environment()->set("Hello", "World!");
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
		print("(3) Current process information\n\n");
		print("  ProcessStatus::workingDirectory() = %%\n", ProcessStatus::workingDirectory());
		print("  ProcessStatus::userName() = %%\n", ProcessStatus::userName());
		print("  ProcessStatus::groupName() = %%\n", ProcessStatus::groupName());
		print("  ProcessStatus::isSuperUser() = %%\n", ProcessStatus::isSuperUser());
		print("\n");
	}
	
	return 0;
}

int echo(int argc, char** argv)
{
	String commandLine;
	for (int i = 0; i < argc; ++i) {
		commandLine.append(argv[i]);
		if (i != argc - 1)
			commandLine.append(" ");
	}
	print("ProcessStatus::workingDirectory() = \"%%\"\n", ProcessStatus::workingDirectory());
	print("ProcessStatus::env(\"Hello\") = \"%%\"\n", ProcessStatus::env("Hello"));
	ProcessStatus::setEnv("Hello", "Echo");
	print("ProcessStatus::env(\"Hello\") = \"%%\"\n", ProcessStatus::env("Hello"));
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
