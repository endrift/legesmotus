/*
 * common/AckManager.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_ACKMANAGER_HPP
#define LM_COMMON_ACKMANAGER_HPP

#include <string>
#include <list>
#include <map>
#include <set>
#include <stdint.h>

class PacketWriter;

class AckManager {
	enum {
		ACK_TIME = 1000,
		RETRIES = 2
	};
protected:
	struct SentPacket {
		uint32_t		send_time;
		std::set<uint32_t>	peer_ids;
		uint32_t		packet_type;
		uint32_t		packet_id;
		std::string		data;
		int			tries_left;

		SentPacket(uint32_t peer_id, const PacketWriter& packet);
		SentPacket(const std::set<uint32_t>& peer_ids, const PacketWriter& packet);

		void			ack(uint32_t peer_id);
		uint32_t		time_since_send() const;
		uint32_t		time_until_resend() const;
		void			reset_send_time();
		bool			has_peers() const { return !peer_ids.empty(); }
	};

	virtual void	kick_peer(uint32_t peer_id) = 0;
	virtual void	resend_packet(uint32_t peer_id, const std::string& data) = 0;

private:
	typedef std::list<SentPacket> Queue;

	Queue					m_packets;
	std::map<uint32_t, Queue::iterator>	m_packets_by_id;

public:
	virtual ~AckManager() { }

	void		add_packet(uint32_t peer_id, const PacketWriter& packet);
	void		add_broadcast_packet(const std::set<uint32_t>& peer_ids, const PacketWriter& packet);
	void		ack(uint32_t peer_id, uint32_t packet_id);
	bool		has_packets() const { return !m_packets.empty(); }
	uint32_t	time_until_resend() const;
	void		resend();
};

#endif
