CMAKEWIZARDS=cmake/wizards

local:
	${CMAKEWIZARDS}/build-ftl

all: local

git:
	${CMAKEWIZARDS}/build-ftl git

conf-clean:
	${CMAKEWIZARDS}/build-ftl conf-clean

clean:
	${CMAKEWIZARDS}/build-ftl clean

help:
	${CMAKEWIZARDS}/build-ftl help
	
