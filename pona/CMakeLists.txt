SET(PONA_HEADER
	defaults.hpp
	visibility.hpp
	
	atoms
	types.hpp
	Exception.hpp
	SpinMutex.hpp
	Instance.hpp
	Alien.hpp
	Pointer.hpp
	SetNull.hpp
	Owner.hpp
	Ref.hpp
	
	container
	Sequence.hpp
	Stack.hpp
	Queue.hpp
	Heap.hpp
	Array.hpp
	ListObserver.hpp
	List.hpp
	Tree.hpp
	PrefixTree.hpp
	KeyValuePair.hpp
	BinaryTree.hpp
	AvlTree.hpp
	Map.hpp
	CString.hpp
	String.hpp
	
	syntax
	Token.hpp
	TokenScreen.hpp
	TokenFactory.hpp
	Syntax.hpp
	SyntaxDefinition.hpp
	SyntaxSugar.hpp
	
	stdio
	Stream.hpp
	SystemStream.hpp
	FileStatus.hpp
	File.hpp
	Path.hpp
	LineSource.hpp
	LineSink.hpp
	StandardStreams.hpp
	IntegerLiteral.hpp
	FloatLiteral.hpp
	FormatSpecifier.hpp
	FormatSyntax.hpp
	Format.hpp
	FormatPrint.hpp
	
	binary
	ByteSink.hpp
	ByteSource.hpp
	Utf8Sink.hpp
	Utf8Source.hpp
	BitSink.hpp
	BitSource.hpp
	Crc32.hpp
	HuffmanCodec.hpp
	
	threads
	ThreadLocalOwner.hpp
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
	Variant.hpp
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
	SpinMutex.cpp
	
	# container
	Tree.cpp
	String.cpp
	
	# syntax
	Token.cpp
	
	# stdio
	Stream.cpp
	SystemStream.cpp
	File.cpp
	Path.cpp
	FileStatus.cpp
	LineSink.cpp
	LineSource.cpp
	StandardStreams.cpp
	IntegerLiteral.cpp
	FloatLiteral.cpp
	FormatSpecifier.cpp
	FormatSyntax.cpp
	Format.cpp
	
	# binary
	ByteSink.cpp
	ByteSource.cpp
	Utf8Sink.cpp
	Utf8Source.cpp
	BitSink.cpp
	BitSource.cpp
	HuffmanCodec.cpp
	Crc32.cpp
	
	# threads
	Thread.cpp
	ThreadFactory.cpp
	Mutex.cpp
	Condition.cpp
	Semaphore.cpp
	
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

SET_TARGET_PROPERTIES(pona PROPERTIES VERSION 0.9.6 SOVERSION 0)

INSTALL(TARGETS pona ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
