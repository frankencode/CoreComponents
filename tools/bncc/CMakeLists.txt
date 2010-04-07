LINK_LIBRARIES(pona)
INCLUDE_DIRECTORIES(..)

ADD_EXECUTABLE(bncc
	use_pona
	bncc.cpp
	AbnfCompiler.hpp
	AbnfCompiler.cpp
)

INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/bncc DESTINATION bin)
