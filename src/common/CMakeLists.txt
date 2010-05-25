# CMake list for Leges Motus common code

cmake_minimum_required(VERSION 2.6)

add_library(lmcommon STATIC
	Map.cpp
	Exception.cpp
	Player.cpp
	Polygon.cpp
	Point.cpp
	Shape.cpp
	Circle.cpp
	PacketReader.cpp
	PacketWriter.cpp
	StringTokenizer.cpp
	math.cpp
	misc.cpp
	network.cpp
	team.cpp
	timer.cpp
	MapReader.cpp
	GameParameters.cpp
	WeaponReader.cpp
	WeaponFile.cpp
	UDPSocket.cpp
	UDPPacket.cpp
	IPAddress.cpp
	PacketQueue.cpp
	AckManager.cpp
	CommonNetwork.cpp
	PacketHeader.cpp
	PathManager.cpp
	ConfigManager.cpp
	Version.cpp)

set_target_properties(lmcommon PROPERTIES COMPILE_FLAGS ${CXXFLAGS})
set_target_properties(lmcommon PROPERTIES LINK_FLAGS ${LDFLAGS})
