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
	vc2decode.cpp

noinst_HEADERS = \
	tclap/ArgException.h  \
	tclap/CmdLine.h \
	tclap/Constraint.h \
	tclap/IgnoreRestVisitor.h \
	tclap/OptionalUnlabeledTracker.h \
	tclap/SwitchArg.h \
	tclap/ValueArg.h \
	tclap/Visitor.h \
	tclap/Arg.h \
	tclap/CmdLineInterface.h \
	tclap/DocBookOutput.h \
	tclap/MultiArg.h \
	tclap/StandardTraits.h \
	tclap/UnlabeledMultiArg.h \
	tclap/ValuesConstraint.h \
	tclap/XorHandler.h \
	tclap/ArgTraits.h \
	tclap/CmdLineOutput.h \
	tclap/HelpVisitor.h \
	tclap/MultiSwitchArg.h \
	tclap/StdOutput.h \
	tclap/UnlabeledValueArg.h \
	tclap/VersionVisitor.h \
	tclap/ZshCompletionOutput.h
