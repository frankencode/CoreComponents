local:
	wizards/build-ftl

all: local

git:
	wizards/build-ftl git

conf-clean:
	wizards/build-ftl conf-clean

clean:
	wizards/build-ftl clean

help:
	wizards/build-ftl help
	
