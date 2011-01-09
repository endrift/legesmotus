/*
 * client/Curve.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_CLIENT_CURVE_HPP
#define LM_CLIENT_CURVE_HPP

namespace LM {
	class Curve {
	private:
		float	m_start_mapping;
		float	m_end_mapping;
	protected:
		// Must return a value in [0, 1]
		virtual float	map_progress(float t) const = 0;
		Curve(float start, float end);
	public:
		virtual ~Curve() {}
		float operator()(float t) const;
	
		void set_start(float start);
		void set_end(float end);
	
		float get_start() const;
		float get_end() const;
	};
	
	class ConstantCurve : public Curve {
	protected:
		virtual float	map_progress(float t) const;
	public:
		ConstantCurve(float start, float end);
	};
	
	class LinearCurve : public Curve {
	protected:
		virtual float	map_progress(float t) const;
	public:
		LinearCurve(float start, float end);
	};
	
	class SinusoidalCurve : public Curve {
	private:
		float m_frequency;
		float m_phase;
	protected:
		virtual float	map_progress(float t) const;
	public:
		SinusoidalCurve(float start, float end, float frequency, float phase);
	};

	class LogisticCurve : public Curve {
	private:
		float m_coeff;
		float m_width;
	protected:
		virtual float	map_progress(float t) const;
	public:
		LogisticCurve(float start, float end, float width = 6.0);
	};
}

#endif
