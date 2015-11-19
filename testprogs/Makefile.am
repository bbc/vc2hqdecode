if IS_LINUX
  linux_only_progs =
else
  linux_only_progs =
endif

noinst_PROGRAMS = vc2decode $(linux_only_progs)

AM_CFLAGS = $(VC2HQDECODE_CFLAGS)

AM_CXXFLAGS = $(VC2HQDECODE_CFLAGS)

AM_LDFLAGS = $(VC2HQDECODE_LDFLAGS)

LDADD = $(VC2HQDECODE_LIBS)

vc2decode_SOURCES = \
	vc2decode.cpp \
	program_options_lite.cpp

noinst_HEADERS = program_options_lite.h
