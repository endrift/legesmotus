BASEDIR = ..
OBJS = ServerScannerNetwork.o ServerScanner.o
	#put .o filenames here (DO NOT PUT .cpp!)
include $(BASEDIR)/common.mk
CXXFLAGS += $(CLIENTFLAGS)

all: serverscanner

serverscanner: ../liblmserverscanner.a ../liblmcommon.a main.o $(PLATFORM_DEP)
	$(CXX) $(LDFLAGS) -o serverscanner $^

../liblmserverscanner.a: $(OBJS)
	$(AR) crus ../liblmserverscanner.a $^

clean: common-clean
	$(RM) $(OBJS) *~ main.o ../liblmserverscanner.a serverscanner $(PLATFORM_DEP)

deps: common-deps
