BASEDIR = ..
BINSRCS := ServerScannerNetwork.cpp ServerScanner.cpp ServerList.cpp OutputGenerator.cpp JsonGenerator.cpp \
	ReadableGenerator.cpp
BINSRCS += main.cpp
include $(BASEDIR)/common.mk

all: lmscan

lmscan: $(BINOBJS) ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmscan $^ $(LIBS)

clean: common-clean
	@$(RM) lmscan

deps: common-deps
