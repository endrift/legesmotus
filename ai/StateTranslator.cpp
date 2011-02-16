/*
 * ai/StateTranslator.cpp
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

#include "StateTranslator.hpp"
#include "common/Configuration.hpp"
#include <limits>
#include <cmath>
#include <sstream>

using namespace LM;
using namespace std;

StateTranslator::Functor::Functor(Function func) {
	m_func = func;

	switch (func) {
	case POW:
		m_op2.f = 1.0f;
		break;
	case EXP:
		m_op2.f = M_E;
		break;
	default:
		m_op2.f = 0.0f;
	}
}

StateTranslator::Functor::Functor(Function func, float op2) {
	m_func = func;
	m_op2.f = op2;
}

float StateTranslator::Functor::operator()(float input) const {
	switch (m_func) {
	case IDENT:
		return input;
	case BOOL:
		return fabsf(input) < numeric_limits<float>::epsilon();
	case GREATER:
		return input > m_op2.f;
	case LESS:
		return input < m_op2.f;
	case EQUAL:
		return fabsf(input - m_op2.f)/fabsf(m_op2.f) < numeric_limits<float>::epsilon();
	case QUAD:
		return input*input;
	case POW:
		return powf(input, m_op2.f);
	case EXP:
		return powf(m_op2.f, input);
	default:
		return input;
	}
}

StateTranslator::StateTranslator(Configuration* config) {
	m_config = config;
}

float StateTranslator::get_value(const list<pair<const char*, float> >& vars) {
	float running_sum;
	float running_coeff;
	for (list<pair<const char*, float> >::const_iterator iter = vars.begin(); iter != vars.end(); ++iter) {
		TranslationNode n;
		float part;
		if (m_cache.find(iter->first) != m_cache.end()) {
			n = m_cache[iter->first];
		} else {
			stringstream s("Vars");
			if (!m_supersection.empty()) {
				s << m_supersection << ".";
			}
			s << iter->first;
			n.coeff = m_config->get_float(s.str().c_str(), "coeff");
			const char* combine = m_config->get_string(s.str().c_str(), "combine", "add");
			const char* function = m_config->get_string(s.str().c_str(), "function", "ident");
			bool hasop2 = m_config->key_exists(s.str().c_str(), "operand");

			if (strcmp(combine, "void") == 0) {
				n.combine = VOID;
			} else if (strcmp(combine, "add") == 0) {
				n.combine = ADD;
			} else if (strcmp(combine, "sub") == 0) {
				n.combine = SUB;
			} else if (strcmp(combine, "mult") == 0) {
				n.combine = MULT;
			} else if (strcmp(combine, "div") == 0) {
				n.combine = DIV;
			} else {
				n.combine = ADD;
			}

			Functor::Function f;
			if (strcmp(function, "ident") == 0) {
				f = Functor::IDENT;
			} else if (strcmp(function, "bool") == 0) {
				f = Functor::BOOL;
			} else if (strcmp(function, "greater") == 0) {
				f = Functor::GREATER;
			} else if (strcmp(function, "less") == 0) {
				f = Functor::LESS;
			} else if (strcmp(function, "equal") == 0) {
				f = Functor::EQUAL;
			} else if (strcmp(function, "quad") == 0) {
				f = Functor::QUAD;
			} else if (strcmp(function, "pow") == 0) {
				f = Functor::POW;
			} else if (strcmp(function, "exp") == 0) {
				f = Functor::EXP;
			} else {
				f = Functor::IDENT;
			}

			if (hasop2) {
				n.func = Functor(f, m_config->get_float(s.str().c_str(), "operand"));
			} else {
				n.func = Functor(f);
			}
			m_cache[iter->first] = n;
		}
		part = n.coeff * n.func(iter->second);
		switch (n.combine) {
		case VOID:
			break;
		case ADD:
			running_sum += part;
			break;
		case SUB:
			running_sum -= part;
			break;
		case MULT:
			running_coeff *= part;
			break;
		case DIV:
			running_coeff /= part;
			break;
		}
	}

	return running_sum * running_coeff;
}
