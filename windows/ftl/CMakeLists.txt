INCLUDE_DIRECTORIES(../../system)

SET(FTLWINDOWS_HEADER
	XAuthFile.hpp
	XDisplayInfo.hpp
	XMessage.hpp
	XMessageFilter.hpp
	XMessageLogger.hpp
	XClient.hpp
	XWindow.hpp
)

SET(FTLWINDOWS_SOURCE
	XAuthFile.cpp
	XDisplayInfo.cpp
	XMessageFilter.cpp
	XMessageLogger.cpp
	XClient.cpp
	XWindow.cpp
)

ADD_LIBRARY(ftlwindows
	${FTLWINDOWS_HEADER}
	${FTLWINDOWS_SOURCE}
)

SET_TARGET_PROPERTIES(ftlwindows PROPERTIES VERSION 0.1.0 SOVERSION 0)

INSTALL(TARGETS ftlwindows ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
