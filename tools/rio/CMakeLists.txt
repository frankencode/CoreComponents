ADD_EXECUTABLE(rio
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
	rio.cpp
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I")

IF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	INCLUDE_DIRECTORIES(/opt/sfw/include)
	LINK_DIRECTORIES(/opt/sfw/lib64)
	# LINK_DIRECTORIES(/opt/sfw/lib)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")

ADD_EXECUTABLE(rio_rl
	rio_rl.cpp
)

IF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
TARGET_LINK_LIBRARIES(rio_rl readline curses)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
TARGET_LINK_LIBRARIES(rio_rl readline)
ELSE(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
TARGET_LINK_LIBRARIES(rio_rl readline termcap)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")

INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/rio DESTINATION bin)
INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/rio_rl DESTINATION bin)
INSTALL(FILES rio.1 DESTINATION man/man1)
