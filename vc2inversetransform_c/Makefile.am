noinst_LTLIBRARIES = libvc2invtransform-c.la

libvc2invtransform_c_la_LDFLAGS = \
	-no-undefined \
	$(VC2HQDECODE_LDFLAGS) \
	-lpthread

libvc2invtransform_c_la_CPPFLAGS = $(VC2HQDECODE_CFLAGS) \
	-I$(top_srcdir)/vc2hqdecode

libvc2invtransform_c_la_SOURCES = \
	invtransform_c.cpp \
	dequantise_c.cpp \
	vlc_c.cpp

noinst_HEADERS = \
	vlc_c.hpp \
	dequantise_c.hpp \
	invtransform_c.hpp \
	legall_invtransform.hpp \
	haar_invtransform.hpp \
	deslauriers_dubuc_9_7_invtransform.hpp \
	deslauriers_dubuc_13_7_invtransform.hpp \
	fidelity_invtransform.hpp \
	$(top_srcdir)/common/attributes.h
