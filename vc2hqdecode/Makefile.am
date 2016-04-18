configfile_DATA =

pkgincludedir = $(includedir)/vc2hqdecode-@VC2HQDECODE_MAJORMINOR@/vc2hqdecode

lib_LTLIBRARIES = libvc2hqdecode-@VC2HQDECODE_MAJORMINOR@.la

libvc2hqdecode_@VC2HQDECODE_MAJORMINOR@_la_LIBADD = \
	$(NUMA_LIBS) \
	$(top_builddir)/vc2inversetransform_c/libvc2invtransform-c.la \
	$(top_builddir)/vc2inversetransform_sse4_2/libvc2invtransform-sse4-2.la

libvc2hqdecode_@VC2HQDECODE_MAJORMINOR@_la_LDFLAGS = \
  -no-undefined \
  -version-info $(VC2HQDECODE_LIBVERSION) \
  $(VC2HQDECODE_LDFLAGS) \
  -lpthread \
  -export-symbols-regex 'vc2decode' \
  -lgcc

libvc2hqdecode_@VC2HQDECODE_MAJORMINOR@_la_CPPFLAGS = $(VC2HQDECODE_CFLAGS) \
							$(REDIST_CFLAGS)

libvc2hqdecode_@VC2HQDECODE_MAJORMINOR@_la_SOURCES = vc2hqdecode.cpp \
	logger.cpp \
	VC2Decoder.cpp \
	quantmatrix.cpp \
	stream.cpp

pkginclude_HEADERS = \
	vc2hqdecode.h \
	vc2hqdecodestrings.h

noinst_HEADERS = VC2Decoder.hpp \
	lut.hpp \
	datastructures.hpp \
	invtransform.hpp \
	vlc.hpp \
	dequantise.hpp \
	VideoFormat.hpp \
	logger.hpp \
	ThreadPool.hpp \
	stream.hpp \
	internal.h \
	thread_group.hpp \
	platform_variant.hpp

nodist_pkginclude_HEADERS = vc2hqdecode-stdint.h

DISTCLEANFILES = vc2hqdecode-stdint.h

EXTRA_DIST =
