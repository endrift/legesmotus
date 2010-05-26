BASEDIR = ..
OBJS = ServerScannerNetwork.o ServerScanner.o
	#put .o filenames here (DO NOT PUT .cpp!)
include $(BASEDIR)/common.mk
CXXFLAGS += $(CLIENTFLAGS)

all: serverscanner

serverscanner: main.o ../liblmserverscanner.a ../liblmcommon.a $(PLATFORM_DEP)
	$(CXX) $(LDFLAGS) -o serverscanner $^

../liblmserverscanner.a: $(OBJS)
	$(AR) crus ../liblmserverscanner.a $^

clean: common-clean
	@$(RM) ../liblmserverscanner.a serverscanner

deps: common-deps
