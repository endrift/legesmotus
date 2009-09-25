/*
 * common/GameParameters.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#ifndef LM_COMMON_GAMEPARAMETERS_HPP
#define LM_COMMON_GAMEPARAMETERS_HPP

#include <iosfwd>
#include <stddef.h>
#include <stdint.h>

namespace LM {
	class StringTokenizer;
	class ConfigManager;
	class PacketReader;

	enum GameMode {
		CLASSIC,
		DEATHMATCH,
		RACE,
		ZOMBIE
	};
	enum RadarMode {
		RADAR_OFF,
		RADAR_AURAL,	// only players who have fired recently are displayed
		RADAR_ON
	};

	class GameParameters {
	public:
		// All times and delays are in milliseconds

		int		max_players;
		uint64_t	gate_open_time;
		uint64_t	gate_close_time;
		uint64_t	gate_stick_time;
		uint64_t	freeze_time;
		bool		friendly_fire;
		uint64_t	game_timeout;		// 0 for no timeout
		GameMode	game_mode;		// NOT YET IMPLEMENTED
		RadarMode	radar_mode;
		double		radar_scale;
		uint64_t	radar_blip_duration;	// in aural mode, how long do blips appear?
		uint64_t	game_start_delay;
		uint64_t	late_join_delay;
		uint64_t	team_change_period;
		bool		autobalance_teams;
		int		recharge_amount;	// How much to recharge energy by
		uint64_t	recharge_rate;		// How often to recharge energy
		uint64_t	recharge_delay;		// How much long to wait after being damaged before recharging
		uint64_t	recharge_continuously;	// Keep recharging, even when actively taking damage?

		GameParameters() { reset(); }

		void		init_from_config(const ConfigManager& config);

		bool		process_param_packet(PacketReader& packet);

		void		reset();
	};

	std::ostream&		operator<<(std::ostream&, GameMode);
	std::ostream&		operator<<(std::ostream&, RadarMode);
	std::istream&		operator>>(std::istream&, GameMode&);
	std::istream&		operator>>(std::istream&, RadarMode&);
	StringTokenizer&	operator>>(StringTokenizer&, GameMode&);
	StringTokenizer&	operator>>(StringTokenizer&, RadarMode&);

	GameMode		parse_game_mode(const char* str);
	RadarMode		parse_radar_mode(const char* str);
	const char*		format_game_mode(GameMode);
	const char*		format_radar_mode(RadarMode);
}

#endif
