LINK_LIBRARIES(ftl)
INCLUDE_DIRECTORIES(..)

ADD_EXECUTABLE(bncc
	use_ftl
	bncc.cpp
	AbnfCompiler.hpp
	AbnfCompiler.cpp
)

INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/bncc DESTINATION bin)
