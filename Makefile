CMAKEWIZARD=cmake/wizard

all:
	@${CMAKEWIZARD}/build

conf-clean:
	@${CMAKEWIZARD}/build conf-clean

clean:
	@${CMAKEWIZARD}/build clean

app: application

application:
	@cd ${CMAKEWIZARD} && ./create-application

cache:
	@${CMAKEWIZARD}/build cache

help:
	@cd ${CMAKEWIZARD} && ./print-help make
