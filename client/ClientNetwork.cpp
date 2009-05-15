/*
 * ClientNetwork.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#include <string>
#include "ClientNetwork.hpp"
#include "GameController.hpp"
#include "common/network.hpp"
#include "common/PacketWriter.hpp"
#include "common/PacketReader.hpp"
#include "common/RawPacket.hpp"
#include <iostream>

using namespace std;

ClientNetwork::ClientNetwork() {
	m_socket = SDLNet_UDP_Open(0);
	m_server_channel = -1;
}

ClientNetwork::~ClientNetwork() {
	disconnect();
	SDLNet_UDP_Close(m_socket);
	m_socket = NULL;
}

bool	ClientNetwork::connect(const char* hostname, unsigned int portno) {
	disconnect();

	if (SDLNet_ResolveHost(&m_server_address, const_cast<char*>(hostname), portno) == -1) {
		return false;
	}

	m_server_channel = SDLNet_UDP_Bind(m_socket, -1, &m_server_address);
	if (m_server_channel == -1) {
		return false;
	}

	return true;
}

void	ClientNetwork::disconnect() {
	if (is_connected()) {
		SDLNet_UDP_Unbind(m_socket, m_server_channel);
		m_server_channel = -1;
	}
}

void	ClientNetwork::send_packet(const PacketWriter& packet) {
	RawPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.fill(packet);
	send_raw_packet(m_server_channel, raw_packet);
}

void	ClientNetwork::send_unbound_packet(const IPaddress& dest, const PacketWriter& packet) {
	RawPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(dest);
	raw_packet.fill(packet);
	send_raw_packet(-1, raw_packet);
}

void	ClientNetwork::send_raw_packet(int channel, RawPacket& raw_packet) {
	if (m_socket == NULL) {
		return;
	}
	SDLNet_UDP_Send(m_socket, channel, raw_packet);
}

void	ClientNetwork::send_raw_packet(RawPacket& raw_packet) {
	send_raw_packet(m_server_channel, raw_packet);
}

bool	ClientNetwork::receive_raw_packet(RawPacket& raw_packet) {
	if (m_socket == NULL) {
		return false;
	}
	return SDLNet_UDP_Recv(m_socket, raw_packet) == 1;
}

void	ClientNetwork::receive_packets(GameController& controller) {
	RawPacket	raw_packet(MAX_PACKET_LENGTH);

	// Keep receiving packets for as long as we can.
	while (receive_raw_packet(raw_packet)) {
		if (raw_packet->channel == m_server_channel) {
			process_bound_packet(controller, raw_packet);
		} else {
			process_unbound_packet(controller, raw_packet);
		}
	}
}

void	ClientNetwork::process_bound_packet(GameController& controller, const RawPacket& raw_packet) {
	PacketReader	reader(raw_packet);
	
	switch (reader.packet_type()) {
	case PLAYER_UPDATE_PACKET:
		controller.player_update(reader);
		break;

	case GUN_FIRED_PACKET:
		controller.gun_fired(reader);
		break;

	case PLAYER_SHOT_PACKET:
		//send_ack(reader.packet_id());
		controller.player_shot(reader);
		break;

	case MESSAGE_PACKET:
		controller.message(reader);
		break;

	case GAME_START_PACKET:
		//send_ack(reader.packet_id());
		controller.game_start(reader);
		break;

	case GAME_STOP_PACKET:
		//send_ack(reader.packet_id());
		controller.game_stop(reader);
		break;

	case SCORE_UPDATE_PACKET:
		controller.score_update(reader);
		break;

	case WELCOME_PACKET:
		//send_ack(reader.packet_id());
		controller.welcome(reader);
		break;

	case ANNOUNCE_PACKET:
		controller.announce(reader);
		break;

	case GATE_UPDATE_PACKET:
		controller.gate_update(reader);
		break;

	case SHUTDOWN_PACKET:
		//controller.shutdown(reader);
		break;

	case LEAVE_PACKET:
		controller.leave(reader);
		break;

	case PLAYER_ANIMATION_PACKET:
		controller.animation_packet(reader);
		break;

	case REQUEST_DENIED_PACKET:
		controller.request_denied(reader);
		break;

	case NAME_CHANGE_PACKET:
		controller.name_change(reader);
		break;

	case TEAM_CHANGE_PACKET:
		controller.team_change(reader);
		break;
	}
}

void	ClientNetwork::process_unbound_packet(GameController& controller, const RawPacket& raw_packet) {
	PacketReader	reader(raw_packet);
	
	switch (reader.packet_type()) {
	case INFO_PACKET:
		controller.server_info(raw_packet->address, reader);
		break;
	}
}

