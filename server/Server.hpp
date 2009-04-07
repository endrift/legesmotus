/*
 * server/Server.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_SERVER_SERVER_HPP
#define LM_SERVER_SERVER_HPP

#include "ServerNetwork.hpp"
#include "ServerPlayer.hpp"
#include "ServerMap.hpp"
#include <stdint.h>
#include <map>

using namespace std;

class PacketReader;

class Server {
private:
	static const int	SERVER_PROTOCOL_VERSION;
	typedef map<uint32_t, ServerPlayer> player_map;

	ServerNetwork		m_network;
	uint32_t		m_next_player_id;
	player_map		m_players;
	ServerMap		m_current_map;

	// Make sure the given channel is authorized to speak for given player ID
	bool			is_authorized(int channel, uint32_t player_id) const;

	void			rebroadcast_packet(PacketReader& packet, int exclude_channel =-1);

	void			start_game();

public:
	Server ();

	// Called upon receipt of network packets:
	void		player_update(int channel, PacketReader& packet);
	void		join(int channel, PacketReader& packet);
	void		leave(int channel, PacketReader& packet);
	void		player_shot(int channel, PacketReader& packet);
	void		gun_fired(int channel, PacketReader& packet);

	// For testing only (interface likely to change):
	void		run(int portno);
};

#endif
