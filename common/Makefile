BASEDIR = ..
LIBSRCS := Map.cpp Exception.cpp Player.cpp Polygon.cpp Point.cpp Shape.cpp Circle.cpp PacketReader.cpp \
	PacketWriter.cpp StringTokenizer.cpp math.cpp misc.cpp network.cpp team.cpp timer.cpp MapReader.cpp \
	GameParameters.cpp WeaponReader.cpp WeaponFile.cpp UDPSocket.cpp UDPPacket.cpp IPAddress.cpp PacketQueue.cpp \
	AckManager.cpp CommonNetwork.cpp PacketHeader.cpp PathManager.cpp ConfigManager.cpp Version.cpp MapObject.cpp \
	ClientMapObject.cpp Decoration.cpp Obstacle.cpp PhysicsObject.cpp Packet.cpp StandardGun.cpp Weapon.cpp 
LIBRARY := ../liblmcommon.a

include $(BASEDIR)/common.mk

all: $(LIBRARY)

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
