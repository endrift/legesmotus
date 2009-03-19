/*
 * server/ServerPlayer.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_SERVER_SERVERPLAYER_HPP
#define LM_SERVER_SERVERPLAYER_HPP

#include "common/Player.hpp"

class ServerPlayer : public Player {
private:
	int		m_channel;		// The SDL channel that this player is bound to.
	int		m_client_version;	// The version of the client that this player is using.

public:
	ServerPlayer();

	int		get_channel() const { return m_channel; }
	int		get_client_version() const { return m_client_version; }

	void		init(uint32_t player_id, int channel, int client_version, const char* name, char team);
};

#endif
