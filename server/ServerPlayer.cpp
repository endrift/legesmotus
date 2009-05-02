/*
 * server/ServerPlayer.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "ServerPlayer.hpp"
#include "Server.hpp"

using namespace std;

ServerPlayer::ServerPlayer() {
	m_channel = -1;
	m_client_version = -1;
	m_is_op = false;
	m_spawnpoint = NULL;
	m_join_time = m_last_seen_time = 0;
}

void ServerPlayer::init(uint32_t player_id, int channel, int client_version, const char* name, char team, ServerPlayer::Queue& timeout_queue) {
	Player::set_id(player_id);
	Player::set_name(name);
	Player::set_team(team);

	m_channel = channel;
	m_client_version = client_version;

	m_join_time = m_last_seen_time = SDL_GetTicks();

	timeout_queue.push_back(this);
	m_timeout_queue_position = --timeout_queue.end();
}

void ServerPlayer::reset_join_time() {
	m_join_time = SDL_GetTicks();
}

uint32_t ServerPlayer::time_until_spawn() const {
	uint32_t time_since_join = SDL_GetTicks() - m_join_time;
	return time_since_join >= Server::JOIN_DELAY ? 0 : Server::JOIN_DELAY - time_since_join;
}

void ServerPlayer::seen(Queue& timeout_queue) {
	m_last_seen_time = SDL_GetTicks();

	timeout_queue.erase(m_timeout_queue_position);
	timeout_queue.push_back(this);
	m_timeout_queue_position = --timeout_queue.end();
}

bool ServerPlayer::has_timed_out() const {
	return SDL_GetTicks() - m_last_seen_time >= Server::PLAYER_TIMEOUT;
}


