SET(PONA_HEADER
	defaults.hpp
	
	atoms
	types.hpp
	strings.hpp
	Exception.hpp
	SpinMutex.hpp
	Instance.hpp
	Alien.hpp
	Pointer.hpp
	SetNull.hpp
	Owner.hpp
	Ref.hpp
	NonCopyable.hpp
	Sequence.hpp
	Stream.hpp
	
	binary
	ByteSink.hpp
	ByteSource.hpp
	Utf8Sink.hpp
	Utf8Source.hpp
	BitSink.hpp
	BitSource.hpp
	Crc32.hpp
	HuffmanCodec.hpp
	
	container
	ArrayPolicy.hpp
	Array.hpp
	Stack.hpp
	Queue.hpp
	Heap.hpp
	ListObserver.hpp
	List.hpp
	Tree.hpp
	PrefixTree.hpp
	KeyValuePair.hpp
	BinaryTree.hpp
	AvlTree.hpp
	Map.hpp
	
	syntax
	Token.hpp
	TokenScreen.hpp
	TokenFactory.hpp
	Syntax.hpp
	SyntaxSugar.hpp
	
	strings
	UStringIndex.hpp
	UString.hpp
	IntegerLiteral.hpp
	FloatLiteral.hpp
	FormatSpecifier.hpp
	FormatSyntax.hpp
	Format.hpp
	Variant.hpp
	
	stdio
	SystemStream.hpp
	FileStatus.hpp
	File.hpp
	DirEntry.hpp
	Dir.hpp
	Path.hpp
	LineSource.hpp
	LineSink.hpp
	StandardStreams.hpp
	FormatPrint.hpp
	
	threads
	GlobalSpinMutex.hpp
	LocalStatic.hpp
	ThreadLocalOwner.hpp
	ThreadLocalSingleton.hpp
	Singleton.hpp
	Thread.hpp
	ThreadFactory.hpp
	Mutex.hpp
	Guard.hpp
	Condition.hpp
	Semaphore.hpp
	Channel.hpp
	
	process
	User.hpp
	Group.hpp
	Options.hpp
	SignalSet.hpp
	Process.hpp
	ProcessFactory.hpp
	ProcessStatus.hpp
	FileLock.hpp
	MemoryMapping.hpp
	Pipe.hpp
	
	events
	Action.hpp
	Event.hpp
	ExitEvent.hpp
	ThreadExitEvent.hpp
	SignalManager.hpp
	
	network
	SocketAddress.hpp
	SocketAddressEntry.hpp
	StreamSocket.hpp
	NetworkInterface.hpp
	
	time
	Time.hpp
	
	misc
	Random.hpp
	Rounding.hpp
)

SET(PONA_SOURCE
	# atoms
	Exception.cpp
	Stream.cpp
	
	# binary
	ByteSink.cpp
	ByteSource.cpp
	Utf8Sink.cpp
	Utf8Source.cpp
	BitSink.cpp
	BitSource.cpp
	HuffmanCodec.cpp
	Crc32.cpp
	
	# container
	Tree.cpp
	
	# syntax
	Token.cpp
	
	# strings
	UString.cpp
	IntegerLiteral.cpp
	FloatLiteral.cpp
	FormatSpecifier.cpp
	FormatSyntax.cpp
	Format.cpp
	
	# stdio
	SystemStream.cpp
	FileStatus.cpp
	File.cpp
	Dir.cpp
	Path.cpp
	LineSink.cpp
	LineSource.cpp
	StandardStreams.cpp
	
	# threads
	GlobalSpinMutex.cpp
	Mutex.cpp
	Condition.cpp
	Semaphore.cpp
	Thread.cpp
	ThreadFactory.cpp
	
	# process
	User.cpp
	Group.cpp
	Options.cpp
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
	
	# network
	SocketAddress.cpp
	SocketAddressEntry.cpp
	StreamSocket.cpp
	NetworkInterface.cpp
	
	# misc
	Random.cpp
)

ADD_LIBRARY(pona
	${PONA_HEADER}
	${PONA_SOURCE}
)

SET_TARGET_PROPERTIES(pona PROPERTIES VERSION 0.9.7 SOVERSION 0)

INSTALL(TARGETS pona ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
