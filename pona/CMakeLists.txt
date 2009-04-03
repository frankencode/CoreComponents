SET(PONA_HEADER
	defaults.hpp
	
	atoms
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
	Vector.hpp
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
	Condition.hpp
	Semaphore.hpp
	Channel.hpp
	
	context
	context.hpp
	User.hpp
	Group.hpp
	Variant.hpp
	ToolOptions.hpp
	Process.hpp
	FileLock.hpp
	MemoryMapping.hpp
	
	event
	OnExitManager.hpp
	OnThreadExitManager.hpp
	OnSignalManager.hpp
	
	network
	SocketAddress.hpp
	StreamSocket.hpp
	
	time
	time.hpp
	TimeStamp.hpp
	
	misc
	Random.hpp
	Rounding.hpp
	
	EventHandler.hpp
)

SET(PONA_SOURCE
	# atoms
	Exception.cpp
	
	# container
	Tree.cpp
	String.cpp
	
	# stdio
	Stream.cpp
	SystemStream.cpp
	File.cpp
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
	Semaphore.cpp
	Thread.cpp
	
	# context
	context.cpp
	User.cpp
	Group.cpp
	ToolOptions.cpp
	Process.cpp
	FileLock.cpp
	MemoryMapping.cpp
	
	# event
	OnExitManager.cpp
	OnThreadExitManager.cpp
	OnSignalManager.cpp
	
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

SET_TARGET_PROPERTIES(pona PROPERTIES VERSION 0.9.3 SOVERSION 0)

INSTALL(TARGETS pona ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
