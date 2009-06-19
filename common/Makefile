BASEDIR = ..
include ../Makefile.in
OBJS = Map.o LMException.o Player.o Polygon.o Point.o PacketReader.o PacketWriter.o StringTokenizer.o math.o misc.o network.o team.o timer.o MapReader.o \
	UDPSocket.o UDPPacket.o IPAddress.o \
	PathManager.o AckManager.o ConfigManager.o Version.o #put .o filenames here (DO NOT PUT .cpp!)

all: ../liblmcommon.a

../liblmcommon.a: $(OBJS)
	$(AR) crus ../liblmcommon.a $(OBJS)

clean:
	rm -f $(OBJS) ../liblmcommon.a *~

# ==================================== #

LMException.o: $(H_LMEXCEPTION)
