#! /bin/bash -xe

pushd ../ftl
g++ -fpic -Wall -pthread -c -o AbnfCoreSyntax.o AbnfCoreSyntax.cpp
g++ -fpic -Wall -pthread -c -o syntax.o syntax.cpp
g++ -fpic -Wall -pthread -c -o File.o File.cpp
g++ -fpic -Wall -pthread -c -o MemoryMapping.o MemoryMapping.cpp
g++ -fpic -Wall -pthread -c -o new.o new.cpp
g++ -fpic -Wall -pthread -c -o FormatSpecifier.o FormatSpecifier.cpp
g++ -fpic -Wall -pthread -c -o Date.o Date.cpp
g++ -fpic -Wall -pthread -c -o SyntaxState.o SyntaxState.cpp
g++ -fpic -Wall -pthread -c -o Dir.o Dir.cpp
g++ -fpic -Wall -pthread -c -o GccToolChain.o GccToolChain.cpp
g++ -fpic -Wall -pthread -c -o LineSource.o LineSource.cpp
g++ -fpic -Wall -pthread -c -o AbnfSyntax.o AbnfSyntax.cpp
g++ -fpic -Wall -pthread -c -o LinkInfo.o LinkInfo.cpp
g++ -fpic -Wall -pthread -c -o UriSyntax.o UriSyntax.cpp
g++ -fpic -Wall -pthread -c -o BuildLine.o BuildLine.cpp
g++ -fpic -Wall -pthread -c -o IntegerLiteral.o IntegerLiteral.cpp
g++ -fpic -Wall -pthread -c -o Token.o Token.cpp
g++ -fpic -Wall -pthread -c -o ByteEncoder.o ByteEncoder.cpp
g++ -fpic -Wall -pthread -c -o DependencyCache.o DependencyCache.cpp
g++ -fpic -Wall -pthread -c -o User.o User.cpp
g++ -fpic -Wall -pthread -c -o BitEncoder.o BitEncoder.cpp
g++ -fpic -Wall -pthread -c -o FileStatus.o FileStatus.cpp
g++ -fpic -Wall -pthread -c -o SyntaxDebugFactory.o SyntaxDebugFactory.cpp
g++ -fpic -Wall -pthread -c -o FileLock.o FileLock.cpp
g++ -fpic -Wall -pthread -c -o Condition.o Condition.cpp
g++ -fpic -Wall -pthread -c -o Group.o Group.cpp
g++ -fpic -Wall -pthread -c -o SystemStream.o SystemStream.cpp
g++ -fpic -Wall -pthread -c -o SocketAddressEntry.o SocketAddressEntry.cpp
g++ -fpic -Wall -pthread -c -o SyntaxDefinition.o SyntaxDefinition.cpp
g++ -fpic -Wall -pthread -c -o SyntaxDebugger.o SyntaxDebugger.cpp
g++ -fpic -Wall -pthread -c -o Buffer.o Buffer.cpp
g++ -fpic -Wall -pthread -c -o Glob.o Glob.cpp
g++ -fpic -Wall -pthread -c -o ProcessFactory.o ProcessFactory.cpp
g++ -fpic -Wall -pthread -c -o Pipe.o Pipe.cpp
g++ -fpic -Wall -pthread -c -o InetAddressSyntax.o InetAddressSyntax.cpp
g++ -fpic -Wall -pthread -c -o ThreadExitEvent.o ThreadExitEvent.cpp
g++ -fpic -Wall -pthread -c -o Event.o Event.cpp
g++ -fpic -Wall -pthread -c -o Mutex.o Mutex.cpp
g++ -fpic -Wall -pthread -c -o ByteDecoder.o ByteDecoder.cpp
g++ -fpic -Wall -pthread -c -o GlobalCoreMutex.o GlobalCoreMutex.cpp
g++ -fpic -Wall -pthread -c -o LineSink.o LineSink.cpp
g++ -fpic -Wall -pthread -c -o BitDecoder.o BitDecoder.cpp
g++ -fpic -Wall -pthread -c -o ExitEvent.o ExitEvent.cpp
g++ -fpic -Wall -pthread -c -o BuildPlan.o BuildPlan.cpp
g++ -fpic -Wall -pthread -c -o Format.o Format.cpp
g++ -fpic -Wall -pthread -c -o SocketAddress.o SocketAddress.cpp
g++ -fpic -Wall -pthread -c -o NetworkInterface.o NetworkInterface.cpp
g++ -fpic -Wall -pthread -c -o AbnfCompiler.o AbnfCompiler.cpp
g++ -fpic -Wall -pthread -c -o Wire.o Wire.cpp
g++ -fpic -Wall -pthread -c -o Json.o Json.cpp
g++ -fpic -Wall -pthread -c -o Md5.o Md5.cpp
g++ -fpic -Wall -pthread -c -o Pattern.o Pattern.cpp
g++ -fpic -Wall -pthread -c -o ByteArray.o ByteArray.cpp
g++ -fpic -Wall -pthread -c -o String.o String.cpp
g++ -fpic -Wall -pthread -c -o Exception.o Exception.cpp
g++ -fpic -Wall -pthread -c -o ProcessStatus.o ProcessStatus.cpp
g++ -fpic -Wall -pthread -c -o Random.o Random.cpp
g++ -fpic -Wall -pthread -c -o FloatLiteral.o FloatLiteral.cpp
g++ -fpic -Wall -pthread -c -o Time.o Time.cpp
g++ -fpic -Wall -pthread -c -o Semaphore.o Semaphore.cpp
g++ -fpic -Wall -pthread -c -o Base64.o Base64.cpp
g++ -fpic -Wall -pthread -c -o HuffmanCodec.o HuffmanCodec.cpp
g++ -fpic -Wall -pthread -c -o Config.o Config.cpp
g++ -fpic -Wall -pthread -c -o Thread.o Thread.cpp
g++ -fpic -Wall -pthread -c -o SignalSet.o SignalSet.cpp
g++ -fpic -Wall -pthread -c -o Memory.o Memory.cpp
g++ -fpic -Wall -pthread -c -o Process.o Process.cpp
g++ -fpic -Wall -pthread -c -o debug.o debug.cpp
g++ -fpic -Wall -pthread -c -o StreamSocket.o StreamSocket.cpp
g++ -fpic -Wall -pthread -c -o ThreadFactory.o ThreadFactory.cpp
g++ -fpic -Wall -pthread -c -o strings.o strings.cpp
g++ -fpic -Wall -pthread -c -o Character.o Character.cpp
g++ -fpic -Wall -pthread -c -o Crc32.o Crc32.cpp
g++ -fpic -Wall -pthread -c -o Uri.o Uri.cpp
g++ -fpic -Wall -pthread -c -o Stream.o Stream.cpp
g++ -fpic -Wall -pthread -c -o StandardStreams.o StandardStreams.cpp
g++ -fpic -Wall -pthread -c -o SignalManager.o SignalManager.cpp
g++ -shared -pthread -Wl,-soname,libftlcore.so.0 -o libftlcore.so.0.3.0 AbnfCoreSyntax.o syntax.o File.o MemoryMapping.o new.o FormatSpecifier.o Date.o SyntaxState.o Dir.o GccToolChain.o LineSource.o AbnfSyntax.o LinkInfo.o UriSyntax.o BuildLine.o IntegerLiteral.o Token.o ByteEncoder.o DependencyCache.o User.o BitEncoder.o FileStatus.o SyntaxDebugFactory.o FileLock.o Condition.o Group.o SystemStream.o SocketAddressEntry.o SyntaxDefinition.o SyntaxDebugger.o Buffer.o Glob.o ProcessFactory.o Pipe.o InetAddressSyntax.o ThreadExitEvent.o Event.o Mutex.o ByteDecoder.o GlobalCoreMutex.o LineSink.o BitDecoder.o ExitEvent.o BuildPlan.o Format.o SocketAddress.o NetworkInterface.o AbnfCompiler.o Wire.o Json.o Md5.o Pattern.o ByteArray.o String.o Exception.o ProcessStatus.o Random.o FloatLiteral.o Time.o Semaphore.o Base64.o HuffmanCodec.o Config.o Thread.o SignalSet.o Memory.o Process.o debug.o StreamSocket.o ThreadFactory.o strings.o Character.o Crc32.o Uri.o Stream.o StandardStreams.o SignalManager.o -lrt -lutil -ldl
ln -sf libftlcore.so.0.3.0 libftlcore.so.0.3
ln -sf libftlcore.so.0.3.0 libftlcore.so.0
ln -sf libftlcore.so.0.3.0 libftlcore.so
popd
g++ -Wall -pthread -I.. -c -o mach.o mach.cpp
g++ -pthread -o mach mach.o -L../ftl -lftlcore -Wl,--enable-new-dtags,-rpath=../ftl
