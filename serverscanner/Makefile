BASEDIR = ..
OBJS = main.o ServerScannerNetwork.o ServerScanner.o ServerList.o \
	OutputGenerator.o JsonGenerator.o ReadableGenerator.o
	#put .o filenames here (DO NOT PUT .cpp!)
include $(BASEDIR)/common.mk
CXXFLAGS += $(CLIENTFLAGS)

all: lmscan

lmscan: $(OBJS) ../liblmcommon.a $(PLATFORM_DEP)
	$(CXX) $(LDFLAGS) -o lmscan $^

clean: common-clean
	@$(RM) lmscan

deps: common-deps
