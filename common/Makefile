BASEDIR = ..
OBJS = Map.o Exception.o Player.o Polygon.o Point.o Shape.o Circle.o PacketReader.o PacketWriter.o StringTokenizer.o math.o misc.o network.o team.o timer.o MapReader.o GameParameters.o WeaponReader.o \
	UDPSocket.o UDPPacket.o IPAddress.o \
	PathManager.o AckManager.o ConfigManager.o Version.o #put .o filenames here (DO NOT PUT .cpp!)
include ../common.mak

all: ../liblmcommon.a

../liblmcommon.a: $(OBJS)
	$(AR) crus ../liblmcommon.a $(OBJS)

clean: common-clean
	$(RM) $(OBJS) ../liblmcommon.a *~

deps: common-deps
