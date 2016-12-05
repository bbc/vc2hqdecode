noinst_LTLIBRARIES = libvc2invtransform-sse4-2.la

libvc2invtransform_sse4_2_la_LDFLAGS = \
	-no-undefined \
	$(VC2HQDECODE_LDFLAGS) \
	-lpthread

libvc2invtransform_sse4_2_la_CPPFLAGS = $(VC2HQDECODE_CFLAGS) \
	$(SSE4_2_FLAGS) \
	-I$(top_srcdir)/vc2hqdecode

libvc2invtransform_sse4_2_la_SOURCES = \
	invtransform_sse4_2.cpp \
	dequantise_sse4_2.cpp \
	vlc_sse4_2.cpp

noinst_HEADERS = \
	dequantise_sse4_2.hpp \
	invtransform_sse4_2.hpp \
	legall_invtransform.hpp \
	haar_invtransform.hpp \
	vlc_sse4_2.hpp \
        $(top_srcdir)/common/attributes.h
