LINK_LIBRARIES(ftlwindows)
INCLUDE_DIRECTORIES(../../system)
INCLUDE_DIRECTORIES(..)

ADD_EXECUTABLE(displayInfo displayInfo.cpp)
ADD_EXECUTABLE(xdpyinfo xdpyinfo.cpp)
ADD_EXECUTABLE(xauth xauth.cpp)
