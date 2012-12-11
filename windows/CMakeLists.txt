INCLUDE(../core/cmake/BuildOptionsFTLSystem.cmake)
INCLUDE(cmake/BuildOptionsFTLWindows.cmake)

INSTALL(
	FILES cmake/FindFTLWindows.cmake cmake/UseFTLWindows.cmake system/BuildOptionsFTLWindows.cmake
	DESTINATION ${CMAKE_ROOT}/Modules
)

INSTALL(
	DIRECTORY ftl
	DESTINATION include
	PATTERN *.cpp EXCLUDE
	PATTERN *.sh EXCLUDE
	PATTERN *.txt EXCLUDE
)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib CACHE PATH "")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "")

SUBDIRS(ftl tools)
