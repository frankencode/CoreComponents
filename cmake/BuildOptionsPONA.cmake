ADD_DEFINITIONS(
	-D_FILE_OFFSET_BITS=64
)

IF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	ADD_DEFINITIONS(-D_POSIX_PTHREAD_SEMANTICS)
	SET(PONA_BUILD64 ON CACHE BOOL "Build 64 bit version of libPONA.")
	IF(PONA_BUILD64)
		SET(CMAKE_CXX_FLAGS "-m64 ${CMAKE_CXX_FLAGS}")
		SET(CMAKE_SHARED_LINKER_FLAGS "-m64 ${CMAKE_SHARED_LINKER_FLAGS}")
		SET(CMAKE_EXE_LINKER_FLAGS "-m64 ${CMAKE_EXE_LINKER_FLAGS}")
	ENDIF(PONA_BUILD64)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	# Darwin 10.0: Snow (aka 10.6)
	# Darwin 9.x: Leopard (aka 10.5)
	# Darwin 8.x: Tiger (aka 10.4)
	IF(CMAKE_HOST_SYSTEM_VERSION GREATER 9.0.0)
		# assuming we compile with gcc 4.2 on leopard
		IF(CMAKE_OSX_DEPLOYMENT_TARGET GREATER 10.4)
			ADD_DEFINITIONS(-D_XOPEN_SOURCE)
			SET(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
		ELSE(CMAKE_OSX_DEPLOYMENT_TARGET GREATER 10.4)
			ADD_DEFINITIONS(-D_REENTRANT)
			LINK_LIBRARIES(pthread)
		ENDIF(CMAKE_OSX_DEPLOYMENT_TARGET GREATER 10.4)
	ELSE(CMAKE_HOST_SYSTEM_VERSION GREATER 9.0.0)
		ADD_DEFINITIONS(-D_REENTRANT)
		LINK_LIBRARIES(pthread)
	ENDIF(CMAKE_HOST_SYSTEM_VERSION GREATER 9.0.0)
	SET(CMAKE_SHARED_LINKER_FLAGS "-U_undefined -undefined dynamic_lookup ${CMAKE_SHARED_LINKER_FLAGS}")
	SET(PONA_BUILD64 ON CACHE BOOL "Build 64 bit version of libPONA.")
	IF(PONA_BUILD64)
		SET(CMAKE_CXX_FLAGS "-m64 ${CMAKE_CXX_FLAGS}")
		SET(CMAKE_SHARED_LINKER_FLAGS "-m64 ${CMAKE_SHARED_LINKER_FLAGS}")
		SET(CMAKE_EXE_LINKER_FLAGS "-m64 ${CMAKE_EXE_LINKER_FLAGS}")
	ELSE(PONA_BUILD64)
		SET(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
		SET(CMAKE_SHARED_LINKER_FLAGS "-m32 ${CMAKE_SHARED_LINKER_FLAGS}")
		SET(CMAKE_EXE_LINKER_FLAGS "-m32 ${CMAKE_EXE_LINKER_FLAGS}")
	ENDIF(PONA_BUILD64)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	ADD_DEFINITIONS(-D_XOPEN_SOURCE=600)
	SET(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
	LINK_LIBRARIES(rt util)
ELSE(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	SET(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
	LINK_LIBRARIES(socket nsl)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")

SET(CMAKE_CXX_FLAGS_DEBUG "-Wall ${CMAKE_CXX_FLAGS_DEBUG}")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build CACHE PATH "")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build CACHE PATH "")

# MESSAGE("CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
# MESSAGE("CMAKE_HOST_SYSTEM_PROCESSOR = ${CMAKE_HOST_SYSTEM_PROCESSOR}")
