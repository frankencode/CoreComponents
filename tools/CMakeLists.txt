LINK_LIBRARIES(ftl)
INCLUDE_DIRECTORIES(..)

ADD_EXECUTABLE(whoami whoami.cpp)
ADD_EXECUTABLE(ls ls.cpp)
ADD_EXECUTABLE(stat stat.cpp)

SUBDIRS(rio bncc)
