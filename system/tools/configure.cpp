void configure()
{
	Default() << use("ftlsystem", "..");
	Program("cat");
	Program("stat");
	Program("md5");
	Program("sort");
	Program("replace");
	Program("whoami");
}
