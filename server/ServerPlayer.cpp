/*
 * server/ServerPlayer.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "ServerPlayer.hpp"

using namespace std;

ServerPlayer::ServerPlayer() {
	m_channel = -1;
	m_client_version = -1;
}

void ServerPlayer::init(uint32_t player_id, int channel, int client_version, const char* name, char team) {
	Player::set_id(player_id);
	Player::set_name(name);
	Player::set_team(team);

	m_channel = channel;
	m_client_version = client_version;
}
