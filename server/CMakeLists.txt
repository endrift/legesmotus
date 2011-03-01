add_executable(lmserver
	GateStatus.cpp
	Server.cpp
	ServerConfig.cpp
	ServerMap.cpp
	ServerNetwork.cpp
	ServerPlayer.cpp
	Spawnpoint.cpp
	GameModeHelper.cpp
	ClassicMode.cpp
	DeathmatchMode.cpp
	ZombieMode.cpp
	main.cpp
)

target_link_libraries(lmserver lmcommon ${LIBS})
