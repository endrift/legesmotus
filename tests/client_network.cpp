#include "client/ClientNetwork.hpp"
#include "common/UDPPacket.hpp"
#include "common/network.hpp"
#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_mutex.h"
#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

class LockedClientNetwork {
	ClientNetwork&		m_network;
	SDL_mutex*		m_lock;

public:
	explicit LockedClientNetwork(ClientNetwork& network) : m_network(network) {
		m_lock = SDL_CreateMutex();
	}

	bool receive_raw_packet(UDPPacket& packet) {
		SDL_mutexP(m_lock);
		bool	result = m_network.receive_raw_packet(packet);
		SDL_mutexV(m_lock);
		return result;
	}
	void send_raw_packet(const UDPPacket& packet) {
		SDL_mutexP(m_lock);
		m_network.send_raw_packet(packet);
		SDL_mutexV(m_lock);
	}
};

static int do_recv_thread(void* data) {
	LockedClientNetwork&	network(*static_cast<LockedClientNetwork*>(data));
	UDPPacket		packet(MAX_PACKET_LENGTH);

	while (true) {
		if (network.receive_raw_packet(packet)) {
			cout.write(packet.get_data(), packet.get_length());
			cout << endl;
		} else {
			SDL_Delay(10);
		}
	}
	return 0;
}

int main (int argc, char** argv) {
	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " hostname portno" << endl;
		return 2;
	}

	ClientNetwork		network;

	if (!network.connect(argv[1], atoi(argv[2]))) {
		cerr << argv[0] << ": could not connect to server" << endl;
		return 2;
	}

	LockedClientNetwork	locked_network(network);

	SDL_Thread*		recv_thread = SDL_CreateThread(do_recv_thread, &locked_network);

	string			line;
	while (getline(cin, line)) {
		UDPPacket	packet(MAX_PACKET_LENGTH);

		packet.set_address(network.get_server_address());
		packet.fill(line.c_str(), line.length());
		locked_network.send_raw_packet(packet);
	}

	SDL_KillThread(recv_thread);

	return 0;
}
