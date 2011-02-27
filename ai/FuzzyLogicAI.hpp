/*
 * ai/FuzzyLogicAI.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_AI_FUZZYLOGICAI_HPP
#define LM_AI_FUZZYLOGICAI_HPP

#include "AI.hpp"
#include "FuzzyLogic.hpp"
#include "common/Configuration.hpp"
#include "common/GameLogic.hpp"

namespace LM {
	class FuzzyLogicAI : public AI {
	private:
		FuzzyLogic* m_fuzzy;
		FuzzyEnvironment m_fuzzy_env;
		const Configuration* m_config;

		void populate_environment();

	public:
		FuzzyLogicAI(const Configuration* config, const GameLogic* logic = NULL);
		virtual ~FuzzyLogicAI();
		
		virtual void update(const GameLogic& logic, uint64_t diff);
		virtual float find_desired_aim();
		virtual AimReason get_aim_reason();
	};
}

#endif
