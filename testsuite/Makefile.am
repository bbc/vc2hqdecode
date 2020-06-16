noinst_PROGRAMS = vc2decodertest

TESTS = vc2decodertest

AM_CFLAGS = $(VC2HQDECODE_CFLAGS)

AM_CPPFLAGS = $(VC2HQDECODE_CPPFLAGS)

AM_CXXFLAGS = $(VC2HQDECODE_CXXFLAGS) \
	-I$(top_srcdir)/vc2hqdecode \
	-I$(top_srcdir)/vc2inversetransform_c/

AM_LDFLAGS = $(VC2HQDECODE_LDFLAGS)

LDADD = $(top_builddir)/vc2inversetransform_sse4_2/libvc2invtransform-sse4-2.la \
	$(top_builddir)/vc2inversetransform_c/libvc2invtransform-c.la \
	$(top_builddir)/vc2hqdecode/libvc2hqdecode_0.1_la-quantmatrix.lo \
	$(top_builddir)/vc2hqdecode/libvc2hqdecode_0.1_la-logger.lo

vc2decodertest_SOURCES = \
	tests.cpp \
	test_invtransform.cpp \
	test_dequantise.cpp \
	randomiser.cpp

noinst_HEADERS = tests.hpp randomiser.hpp
