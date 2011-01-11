/*
 * client/ConvolveKernel.cpp
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

#include "ConvolveKernel.hpp"
#include "common/math.hpp"
#include <iostream>

using namespace LM;
using namespace std;

const int ConvolveKernel::SUPERSAMPLE = 2;

ConvolveKernel::ConvolveKernel(const Curve* curve, int kwidth, int kheight, float normalization) {
	float sum = 0;
	m_width = kwidth;
	m_height = kheight;
	m_data = new float[kwidth * kheight];
	for (int x = 0; x < kwidth; ++x) {
		for (int y = 0; y < kheight; ++y) {
			float k = 0;
			for (int sx = 0; sx < SUPERSAMPLE; ++sx) {
				for (int sy = 0; sy < SUPERSAMPLE; ++sy) {
					float x_scaled = (2.0 * (x + (sx - SUPERSAMPLE*0.5)/SUPERSAMPLE))/(kwidth - 1.0) - 1.0;
					float y_scaled = (2.0 * (y + (sy - SUPERSAMPLE*0.5)/SUPERSAMPLE))/(kheight - 1.0) - 1.0;
					float hypot = sqrt(x_scaled*x_scaled + y_scaled*y_scaled);
					float val = (*curve)(hypot);
					if (hypot > 1) {
						val = 0;
					}
					k += val;
				}
			}
			k /= (SUPERSAMPLE*SUPERSAMPLE);
			m_data[x + kwidth*y] = k;
			sum += k;
		}
	}

	m_extend = false; // TODO: setter
	m_normalization = normalization == 0 ? sum : normalization;
}

ConvolveKernel::ConvolveKernel(const float data[], int kwidth, int kheight, float normalization) {
	float sum = 0;
	m_width = kwidth;
	m_height = kheight;
	m_data = new float[kwidth * kheight];
	for (int x = 0; x < kwidth; ++x) {
		for (int y = 0; y < kheight; ++y) {
			float k = data[x + kwidth*y];
			m_data[x + kwidth*y] = k;
			sum += k;
		}
	}

	m_extend = false; // TODO: setter
	m_normalization = normalization == 0 ? sum : normalization;
}

ConvolveKernel::~ConvolveKernel() {
	delete[] m_data;
}

Image ConvolveKernel::convolve(const Image& source) const {
	Image dest(source.get_width() + m_width, source.get_height() + m_height, "", NULL);

	const uint8_t *src_pixels = source.get_pixels();
	uint8_t *dst_pixels = dest.get_pixels();
	for (int y = 0; y < dest.get_height(); ++y) {
		for (int x = 0; x < dest.get_width(); ++x) {
			float conv_sum[4] = { 0.0, 0.0, 0.0, 0.0 };
			for (int ky = 0; ky < m_height; ++ky) {
				for (int kx = 0; kx < m_width; ++kx) {
					int conv_x = x + kx - m_width;
					int conv_y = y + ky - m_height;
					float current_k = m_data[kx + ky*m_width];

					if (m_extend) {
						// Extend edges of convolved image
						conv_x = max<int>(conv_x, 0);
						conv_x = min<int>(conv_x, source.get_width() - 1);
						conv_y = max<int>(conv_y, 0);
						conv_y = min<int>(conv_y, source.get_height() - 1);
					} else {
						if (conv_x < 0 || conv_x >= source.get_width()) {
							continue;
						}
						if (conv_y < 0 || conv_y >= source.get_height()) {
							continue;
						}
					}

					conv_sum[0] += src_pixels[conv_x*4 + conv_y*source.get_pitch()]*current_k;
					conv_sum[1] += src_pixels[conv_x*4 + conv_y*source.get_pitch() + 1]*current_k;
					conv_sum[2] += src_pixels[conv_x*4 + conv_y*source.get_pitch() + 2]*current_k;
					conv_sum[3] += src_pixels[conv_x*4 + conv_y*source.get_pitch() + 3]*current_k;
				}
			}

			dst_pixels[x*4 + y*dest.get_pitch()] = min<float>(conv_sum[0]/m_normalization, 255.0);
			dst_pixels[x*4 + y*dest.get_pitch() + 1] = min<float>(conv_sum[1]/m_normalization, 255.0);
			dst_pixels[x*4 + y*dest.get_pitch() + 2] = min<float>(conv_sum[2]/m_normalization, 255.0);
			dst_pixels[x*4 + y*dest.get_pitch() + 3] = min<float>(conv_sum[3]/m_normalization, 255.0);
		}
	}

	return dest;
}

int ConvolveKernel::get_width() const {
	return m_width;
}

int ConvolveKernel::get_height() const {
	return m_height;
}
