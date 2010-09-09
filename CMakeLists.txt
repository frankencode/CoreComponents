CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(libftl)

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
