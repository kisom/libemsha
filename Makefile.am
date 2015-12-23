ACLOCAL_AMFLAGS =	-I m4
SUBDIRS =		src

TESTS = src/emsha_core_test		\
	src/emsha_sha256_test		\
	src/emsha_hmac_test

dist_data_DATA =   LICENSE		\
		   README.rst		\
		   doc/libemsha.rst
dist_noinst_DATA = autobuild		\
		   doc/source		\
		   doc/Makefile

.PHONY: valgrind-check
valgrind-check:
	cd src && make $@

.PHONY: cloc-report
cloc-report:
	cd src && make $@

.PHONY: coverity-scan
coverity-scan:
	cd src && make $@

epub:
	cd doc && make $@
