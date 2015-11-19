noinst_PROGRAMS = vc2decodertest

TESTS = vc2decodertest

AM_CFLAGS = $(VC2HQDECODE_CFLAGS)

AM_CXXFLAGS = $(VC2HQDECODE_CFLAGS) \
	-I$(top_srcdir)/vc2hqdecode \
	-I$(top_srcdir)/vc2inversetransform_c/

AM_LDFLAGS = $(VC2HQDECODE_LDFLAGS)

LDADD = $(VC2HQDECODE_LIBS)

vc2decodertest_SOURCES = \
	tests.cpp \
	test_invtransform.cpp \
	test_dequantise.cpp \
	randomiser.cpp

noinst_HEADERS = tests.hpp randomiser.hpp
