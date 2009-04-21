ADD_EXECUTABLE(rget
	Options.hpp
	Options.cpp
	LogFile.hpp
	LogFile.cpp
	BinaryForwarder.hpp
	BinaryForwarder.cpp
	LineForwarder.hpp
	LineForwarder.cpp
	ProcessObserver.hpp
	Service.hpp
	Service.cpp
	rget.cpp
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I")

IF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	INCLUDE_DIRECTORIES(/opt/sfw/include)
	IF(PONA_BUILD64)
		LINK_DIRECTORIES(/opt/sfw/lib64)
	ELSE(PONA_BUILD64)
		LINK_DIRECTORIES(/opt/sfw/lib)
	ENDIF(PONA_BUILD64)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")

ADD_EXECUTABLE(rget_rl
	rget_rl.cpp
)

IF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
TARGET_LINK_LIBRARIES(rget_rl readline curses)
ELSE(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
TARGET_LINK_LIBRARIES(rget_rl readline)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
