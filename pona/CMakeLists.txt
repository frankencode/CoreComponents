SET(PONA_HEADER
	defaults.hpp
	
	atom
	types.hpp
	Exception.hpp
	Instance.hpp
	Alien.hpp
	Pointer.hpp
	SetNull.hpp
	Owner.hpp
	Ref.hpp
	
	container
	RandomAccessMedia.hpp
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
	
	thread
	ThreadLocalOwner.hpp
	Thread.hpp
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
	
	event
	Action.hpp
	Event.hpp
	ExitEvent.hpp
	ThreadExitEvent.hpp
	SignalManager.hpp
	
	network
	SocketAddress.hpp
	StreamSocket.hpp
	
	time
	Time.hpp
	
	misc
	Random.hpp
	Rounding.hpp
)

SET(PONA_SOURCE
	# atom
	Exception.cpp
	
	# container
	Tree.cpp
	String.cpp
	
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
	
	# thread
	Mutex.cpp
	Semaphore.cpp
	Thread.cpp
	
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
	
	# event
	Event.cpp
	ExitEvent.cpp
	ThreadExitEvent.cpp
	SignalManager.cpp
	
	# network
	SocketAddress.cpp
	StreamSocket.cpp
	
	# misc
	Random.cpp
)

ADD_LIBRARY(pona
	${PONA_HEADER}
	${PONA_SOURCE}
)

SET_TARGET_PROPERTIES(pona PROPERTIES VERSION 0.9.4 SOVERSION 0)

INSTALL(TARGETS pona ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
