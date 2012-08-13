SET(FTLSYSTEM_HEADER
	defaults.hpp

	atoms
	debug.hpp
	types.hpp
	strings.hpp
	generics.hpp
	Exception.hpp
	SpinLock.hpp
	Instance.hpp
	Pointer.hpp
	SetNull.hpp
	Owner.hpp
	OnDemand.hpp
	Ref.hpp
	Buffer.hpp
	ByteArray.hpp
	Character.hpp

	binary
	ByteEncoder.hpp
	ByteDecoder.hpp
	Utf8Encoder.hpp
	Utf8Decoder.hpp
	Utf8Walker.hpp
	Utf16Encoder.hpp
	Utf16Decoder.hpp
	Utf16Walker.hpp
	BitEncoder.hpp
	BitDecoder.hpp
	Crc32.hpp
	Md5.hpp
	Base64.hpp
	HuffmanCodec.hpp

	container
	containers.hpp
	Array.hpp
	Stack.hpp
	CircularBuffer.hpp
	Heap.hpp
	Tree.hpp
	PrefixTree.hpp
	PrefixTreeWalker.hpp
	BinaryTree.hpp
	AvlTree.hpp
	OrdinalTree.hpp
	List.hpp
	Map.hpp
	Set.hpp

	syntax
	Token.hpp
	TokenScreen.hpp
	TokenFactory.hpp
	syntax.hpp
	SyntaxSugar.hpp
	SyntaxState.hpp
	SyntaxNode.hpp
	SyntaxDebugFactory.hpp
	SyntaxDefinition.hpp
	SyntaxDebugger.hpp

	strings
	String.hpp
	IntegerLiteral.hpp
	FloatLiteral.hpp
	FormatSpecifier.hpp
	Format.hpp
	Pattern.hpp
	Variant.hpp
	Path.hpp

	stdio
	Stream.hpp
	SystemStream.hpp
	FileStatus.hpp
	File.hpp
	DirEntry.hpp
	Dir.hpp
	Glob.hpp
	LineSource.hpp
	LineSink.hpp
	StandardStreams.hpp
	PrintDebug.hpp

	threads
	GlobalCoreMutex.hpp
	LocalStatic.hpp
	ThreadLocalOwner.hpp
	ThreadLocalSingleton.hpp
	Singleton.hpp
	Default.hpp
	Thread.hpp
	ThreadFactory.hpp
	Mutex.hpp
	Guard.hpp
	Condition.hpp
	Semaphore.hpp
	Rondezvous.hpp
	Conveyor.hpp
	Channel.hpp
	Memory.hpp

	process
	User.hpp
	Group.hpp
	SignalSet.hpp
	Process.hpp
	ProcessFactory.hpp
	ProcessStatus.hpp
	FileLock.hpp
	MemoryMapping.hpp
	Pipe.hpp

	events
	Event.hpp
	ExitEvent.hpp
	ThreadExitEvent.hpp
	SignalManager.hpp

	sockets
	SocketAddress.hpp
	SocketAddressEntry.hpp
	StreamSocket.hpp
	NetworkInterface.hpp
	InetAddressSyntax.hpp

	utils
	rounding.hpp
	Time.hpp
	Date.hpp
	Random.hpp
	SymbolicLink.hpp
	CommandLine.hpp
	CommandOption.hpp

	misc
	LinkInfo.hpp
	UriSyntax.hpp
	Uri.hpp
	AbnfCoreSyntax.hpp
	AbnfSyntax.hpp
	AbnfCompiler.hpp

	Node.hpp
	Wire.hpp
	WireObject.hpp
	Config.hpp
	Json.hpp

	Property.hpp

	MachObject.hpp
	MachDependencyCache.hpp
	MachCompiler.hpp
	GccCompiler.hpp
)

SET(FTLSYSTEM_SOURCE
	# atoms
	new.cpp
	debug.cpp
	strings.cpp
	Exception.cpp
	Buffer.cpp
	ByteArray.cpp
	Character.cpp

	# binary
	ByteEncoder.cpp
	ByteDecoder.cpp
	BitEncoder.cpp
	BitDecoder.cpp
	HuffmanCodec.cpp
	Crc32.cpp
	Md5.cpp
	Base64.cpp

	# container

	# syntax
	Token.cpp
	syntax.cpp
	SyntaxState.cpp
	SyntaxDebugFactory.cpp
	SyntaxDefinition.cpp

	# strings
	String.cpp
	IntegerLiteral.cpp
	FloatLiteral.cpp
	FormatSpecifier.cpp
	Format.cpp
	Pattern.cpp
	Path.cpp

	# stdio
	Stream.cpp
	SystemStream.cpp
	FileStatus.cpp
	File.cpp
	Dir.cpp
	Glob.cpp
	LineSink.cpp
	LineSource.cpp
	StandardStreams.cpp

	# threads
	GlobalCoreMutex.cpp
	Mutex.cpp
	Condition.cpp
	Semaphore.cpp
	Thread.cpp
	ThreadFactory.cpp
	Memory.cpp

	# process
	User.cpp
	Group.cpp
	SignalSet.cpp
	Process.cpp
	ProcessFactory.cpp
	ProcessStatus.cpp
	FileLock.cpp
	MemoryMapping.cpp
	Pipe.cpp

	# events
	Event.cpp
	ExitEvent.cpp
	ThreadExitEvent.cpp
	SignalManager.cpp

	# sockets
	SocketAddress.cpp
	SocketAddressEntry.cpp
	StreamSocket.cpp
	NetworkInterface.cpp
	InetAddressSyntax.cpp

	# utils
	Date.cpp
	Random.cpp
	SymbolicLink.cpp
	CommandLine.cpp

	# misc
	LinkInfo.cpp
	UriSyntax.cpp
	Uri.cpp
	AbnfCoreSyntax.cpp
	AbnfSyntax.cpp
	AbnfCompiler.cpp

	Wire.cpp
	WireObject.cpp
	Config.cpp
	Json.cpp

	MachDependencyCache.cpp
	MachCompiler.cpp
	GccCompiler.cpp
)

ADD_LIBRARY(ftlsystem
	${FTLSYSTEM_HEADER}
	${FTLSYSTEM_SOURCE}
)

SET_TARGET_PROPERTIES(ftlsystem PROPERTIES VERSION 0.2.0 SOVERSION 0)

INSTALL(TARGETS ftlsystem ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
