/*
 * client/Curve.hpp
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

#ifndef LM_CLIENT_CURVE_HPP
#define LM_CLIENT_CURVE_HPP

namespace LM {
	class Curve {
	private:
		double	m_start_mapping;
		double	m_end_mapping;
	protected:
		// Must return a value in [0, 1]
		virtual double	map_progress(double t) const = 0;
		Curve(double start, double end);
	public:
		virtual ~Curve() {}
		double operator()(double t) const;
	
		void set_start(double start);
		void set_end(double end);
	
		double get_start() const;
		double get_end() const;
	};
	
	class ConstantCurve : public Curve {
	protected:
		virtual double	map_progress(double t) const;
	public:
		ConstantCurve(double start, double end);
	};
	
	class LinearCurve : public Curve {
	protected:
		virtual double	map_progress(double t) const;
	public:
		LinearCurve(double start, double end);
	};
	
	class SinusoidalCurve : public Curve {
	private:
		double m_frequency;
		double m_phase;
	protected:
		virtual double	map_progress(double t) const;
	public:
		SinusoidalCurve(double start, double end, double frequency, double phase);
	};

	class LogisticCurve : public Curve {
	private:
		double m_coeff;
		double m_width;
	protected:
		virtual double	map_progress(double t) const;
	public:
		LogisticCurve(double start, double end, double width);
	};
}

#endif
