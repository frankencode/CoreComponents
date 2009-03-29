CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(libPONA)

SET(BUILD_SHARED_LIBS ON CACHE BOOL "Build dynamic or static library.")

INCLUDE(cmake/BuildOptionsPONA.cmake)

INSTALL(FILES cmake/FindPONA.cmake cmake/UsePONA.cmake cmake/BuildOptionsPONA.cmake DESTINATION ${CMAKE_ROOT}/Modules)
INSTALL(DIRECTORY pona DESTINATION include PATTERN *.cpp EXCLUDE)

SUBDIRS(pona test examples)
