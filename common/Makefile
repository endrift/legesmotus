BASEDIR = ..
OBJS = Map.o Exception.o Player.o Polygon.o Point.o Shape.o Circle.o PacketReader.o PacketWriter.o StringTokenizer.o math.o misc.o network.o team.o timer.o MapReader.o GameParameters.o WeaponReader.o WeaponFile.o \
	UDPSocket.o UDPPacket.o IPAddress.o PacketQueue.o AckManager.o CommonNetwork.o PacketHeader.o \
	PathManager.o ConfigManager.o Version.o #put .o filenames here (DO NOT PUT .cpp!)
include $(BASEDIR)/common.mk

all: ../liblmcommon.a

../liblmcommon.a: $(OBJS)

clean: common-clean
	@$(RM) ../liblmcommon.a

deps: common-deps
