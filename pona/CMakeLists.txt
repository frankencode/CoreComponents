SET(PONA_HEADER
	defaults.hpp
	
	atoms
	types.hpp
	strings.hpp
	generics.hpp
	Exception.hpp
	CoreMutex.hpp
	Instance.hpp
	Pointer.hpp
	SetNull.hpp
	Owner.hpp
	Ref.hpp
	
	binary
	ByteEncoder.hpp
	ByteDecoder.hpp
	Utf8Encoder.hpp
	Utf8Decoder.hpp
	Utf8Walker.hpp
	BitEncoder.hpp
	BitDecoder.hpp
	Crc32.hpp
	HuffmanCodec.hpp
	
	container
	containers.hpp
	ArrayPolicy.hpp
	Array.hpp
	Stack.hpp
	Queue.hpp
	Heap.hpp
	ListNode.hpp
	ListWalker.hpp
	List.hpp
	Tree.hpp
	PrefixTree.hpp
	Pair.hpp
	BinaryNode.hpp
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
	String.hpp
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
	CommandOption.hpp
	CommandLine.hpp
	
	threads
	GlobalCoreMutex.hpp
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
	
	network
	SocketAddress.hpp
	SocketAddressEntry.hpp
	StreamSocket.hpp
	NetworkInterface.hpp
	
	time
	Time.hpp
	DateTime.hpp
	
	math
	Random.hpp
	rounding.hpp
)

SET(PONA_SOURCE
	# atoms
	strings.cpp
	generics.cpp
	Exception.cpp
	
	# binary
	ByteEncoder.cpp
	ByteDecoder.cpp
	Utf8Encoder.cpp
	Utf8Decoder.cpp
	BitEncoder.cpp
	BitDecoder.cpp
	HuffmanCodec.cpp
	Crc32.cpp
	
	# container
	Tree.cpp
	
	# syntax
	Token.cpp
	
	# strings
	String.cpp
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
	CommandLine.cpp
	
	# threads
	GlobalCoreMutex.cpp
	Mutex.cpp
	Condition.cpp
	Semaphore.cpp
	Thread.cpp
	ThreadFactory.cpp
	
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
	
	# network
	SocketAddress.cpp
	SocketAddressEntry.cpp
	StreamSocket.cpp
	NetworkInterface.cpp
	
	# time
	DateTime.cpp

	# math
	Random.cpp
)

ADD_DEFINITIONS(-std=c++98)

ADD_LIBRARY(pona
	${PONA_HEADER}
	${PONA_SOURCE}
)

SET_TARGET_PROPERTIES(pona PROPERTIES VERSION 0.9.7 SOVERSION 0)

INSTALL(TARGETS pona ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
