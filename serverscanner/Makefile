BASEDIR = ..
BINSRCS := ServerScannerNetwork.cpp ServerScanner.cpp ServerList.cpp OutputGenerator.cpp JsonGenerator.cpp \
	ReadableGenerator.cpp
BINSRCS += main.cpp
include $(BASEDIR)/common.mk
CXXFLAGS += $(CLIENTFLAGS)

all: lmscan

lmscan: $(BINOBJS) ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmscan $^

clean: common-clean
	@$(RM) lmscan

deps: common-deps
