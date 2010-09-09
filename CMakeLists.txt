CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(libftl)

SET(BUILD_SHARED_LIBS ON CACHE BOOL "Build dynamic or static library.")

INCLUDE(cmake/BuildOptionsFTL.cmake)

INSTALL(
	FILES cmake/FindFTL.cmake cmake/UseFTL.cmake cmake/BuildOptionsFTL.cmake
	DESTINATION ${CMAKE_ROOT}/Modules
)

INSTALL(
	DIRECTORY ftl
	DESTINATION include
	PATTERN *.cpp EXCLUDE
	PATTERN *.sh EXCLUDE
	PATTERN *.txt EXCLUDE
)

SUBDIRS(ftl tests tools)
