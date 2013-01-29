#include <ftl/stdio>
#include <ftl/process>

namespace ftl
{

int echo(int argc, char **argv);

class TestFactory: public ProcessFactory
{
public:
	inline static hook<TestFactory> create() {
		return new TestFactory;
	}
	int incarnate() {
		print("Good morning %%.\n", User::lookup(Process::realUserId())->fullName());
		return 7;
	}
};

int main(int argc, char **argv)
{
	if (argc == 2)
		return echo(argc, argv);

	{
		print("(1) Worker clone\n\n");

		hook<ProcessFactory> factory = TestFactory::create();
		hook<Process> worker = factory->produce();
		int ret = worker->wait();
		print("ret = %%\n", ret);
		print("\n");
	}

	{
		print("(2) I/O Redirection, passing of arguments and environment variables\n\n");

		hook<ProcessFactory> factory = ProcessFactory::create();
		factory->setExecPath(argv[0]);
		factory->setIoPolicy(Process::ForwardInput | Process::ForwardOutput);
		factory->arguments()->append(argv[0]);
		factory->arguments()->append("--echo 123");
		factory->envMap()->establish("Hello", "World!");

		hook<Process> process = factory->produce();

		print("Created child process with pid = %%\n", unsigned(process->id()));

		const char *message =
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

int echo(int argc, char **argv)
{
	hook<StringList> commandLine = StringList::create();
	for (int i = 0; i < argc; ++i)
		commandLine->append(argv[i]);
	print("Process::cwd() = \"%%\"\n", Process::cwd());
	print("Process::env(\"Hello\") = \"%%\"\n", Process::env("Hello"));
	Process::setEnv("Hello", "Echo");
	print("Process::env(\"Hello\") = \"%%\"\n", Process::env("Hello"));
	print("commandLine = \"%%\"\n", commandLine->join(" "));

	while (true) {
		String line = input()->readLine();
		if ((line == "") || (line == "exit"))
			break;
		output()->writeLine(line);
	}

	return 13;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
