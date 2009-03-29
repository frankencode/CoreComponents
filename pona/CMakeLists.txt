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
	BooleanLiteral.hpp
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
	Mutex.hpp
	Condition.hpp
	Semaphore.hpp
	Channel.hpp
	OnThreadExitManager.hpp
	
	context
	context.hpp
	User.hpp
	Group.hpp
	Variant.hpp
	ToolOptions.hpp
	Process.hpp
	FileLock.hpp
	MemoryMapping.hpp
	OnExitManager.hpp
	
	network
	SocketAddress.hpp
	TcpSocket.hpp
	
	time
	time.hpp
	TimeStamp.hpp
	
	misc
	Random.hpp
	Rounding.hpp
	
	EventHandler.hpp
)

SET(PONA_SOURCE
	StandardStreams.cpp
	Stream.cpp
	LineSource.cpp
	LineSink.cpp
	BitSink.cpp
	BitSource.cpp
	ByteSink.cpp
	ByteSource.cpp
	Utf8Sink.cpp
	Utf8Source.cpp
	HuffmanCodec.cpp
	String.cpp
	Format.cpp
	SocketAddress.cpp
	FormatSyntax.cpp
	FormatSpecifier.cpp
	BooleanLiteral.cpp
	IntegerLiteral.cpp
	FloatLiteral.cpp
	TcpSocket.cpp
	ToolOptions.cpp
	Exception.cpp
	SystemStream.cpp
	File.cpp
	FileStatus.cpp
	Thread.cpp
	Process.cpp
	User.cpp
	Group.cpp
	context.cpp
	OnExitManager.cpp
	OnThreadExitManager.cpp
	Random.cpp
	MemoryMapping.cpp
)

ADD_LIBRARY(pona
	${PONA_HEADER}
	${PONA_SOURCE}
)

SET_TARGET_PROPERTIES(pona PROPERTIES VERSION 0.9.1 SOVERSION 0)

INSTALL(TARGETS pona ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
