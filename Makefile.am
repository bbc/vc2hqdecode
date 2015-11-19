
AUTOMAKE_OPTIONS = foreign

include doxygen.am

SUBDIRS = redist vc2inversetransform_c vc2inversetransform_sse4_2 vc2hqdecode testprogs tools testsuite

EXTRA_DIST = COPYING autogen.sh

pkgconfig_DATA = vc2hqdecode-$(VC2HQDECODE_MAJORMINOR).pc

vc2hqdecode-$(VC2HQDECODE_MAJORMINOR).pc: vc2hqdecode.pc
	cp vc2hqdecode.pc vc2hqdecode-$(VC2HQDECODE_MAJORMINOR).pc

vc2hqdecode-$(VC2HQDECODE_MAJORMINOR)-uninstalled.pc: vc2hqdecode-uninstalled.pc
	cp vc2hqdecode-uninstalled.pc vc2hqdecode-$(VC2HQDECODE_MAJORMINOR)-uninstalled.pc

BUILT_SOURCES = vc2hqdecode-$(VC2HQDECODE_MAJORMINOR)-uninstalled.pc

CLEANFILES = vc2hqdecode-$(VC2HQDECODE_MAJORMINOR).pc vc2hqdecode-$(VC2HQDECODE_MAJORMINOR)-uninstalled.pc

ACLOCAL_FLAGS = -I m4
