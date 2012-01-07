INCLUDE(cmake/BuildOptionsFTLSystem.cmake)

INSTALL(
	FILES cmake/FindFTLSystem.cmake cmake/UseFTLSystem.cmake system/BuildOptionsFTLSystem.cmake
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

SUBDIRS(ftl tests tools misc)
