void configure()
{
	Library("ftlsystem")
		<< source("ftl")
		<< version(0,2,1)
		<< link("rt")
		<< link("util");
}
