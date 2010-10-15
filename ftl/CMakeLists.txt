SET(FTL_HEADER
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
	ArrayPolicy.hpp
	Array.hpp
	Buffer.hpp
	
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
	Path.hpp
	
	streams
	Stream.hpp
	SystemStream.hpp
	FileStatus.hpp
	File.hpp
	DirEntry.hpp
	Dir.hpp
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
	
	sockets
	SocketAddress.hpp
	SocketAddressEntry.hpp
	StreamSocket.hpp
	NetworkInterface.hpp
	
	utils
	Time.hpp
	DateTime.hpp
	Random.hpp
	rounding.hpp
	CommandOption.hpp
	CommandLine.hpp
	LinkInfo.hpp
	SymbolicLink.hpp
)

SET(FTL_SOURCE
	# atoms
	strings.cpp
	Exception.cpp
	Buffer.cpp
	
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
	
	# syntax
	Token.cpp
	
	# strings
	String.cpp
	IntegerLiteral.cpp
	FloatLiteral.cpp
	FormatSpecifier.cpp
	FormatSyntax.cpp
	Format.cpp
	Path.cpp
	
	# streams
	Stream.cpp
	SystemStream.cpp
	FileStatus.cpp
	File.cpp
	Dir.cpp
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
	
	# utils
	DateTime.cpp
	Random.cpp
	CommandLine.cpp
	LinkInfo.cpp
	SymbolicLink.cpp
)

ADD_LIBRARY(ftl
	${FTL_HEADER}
	${FTL_SOURCE}
)

SET_TARGET_PROPERTIES(ftl PROPERTIES VERSION 0.1.0 SOVERSION 0)

INSTALL(TARGETS ftl ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
