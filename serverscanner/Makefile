BASEDIR = ..
OBJS = ServerScannerNetwork.o ServerScanner.o
	#put .o filenames here (DO NOT PUT .cpp!)
include ../common.mk
CXXFLAGS += $(CLIENTFLAGS)

ifeq ($(MACHINE),Windows)
PLATFORM_DEP = legesmotus-res.o
legesmotus-res.o: legesmotus.ico
endif
ifeq ($(MACHINE),Darwin)
PLATFORM_DEP = mac.o
endif

all: serverscanner

serverscanner: ../liblmserverscanner.a ../liblmclient.a ../liblmcommon.a main.o $(PLATFORM_DEP)
	$(CXX) $(LDFLAGS) -o serverscanner -L.. main.o -llmclient -llmserverscanner -llmcommon $(PLATFORM_DEP)

../liblmserverscanner.a: $(OBJS)
	$(AR) crus ../liblmserverscanner.a $(OBJS)

clean: common-clean
	$(RM) $(OBJS) *~ main.o ../liblmserverscanner.a serverscanner $(PLATFORM_DEP)

deps: common-deps
