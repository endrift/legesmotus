/*
 * client/ConvolveKernel.cpp
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

#include "ConvolveKernel.hpp"
#include "common/math.hpp"
#include <iostream>

using namespace LM;
using namespace std;

const int ConvolveKernel::SUPERSAMPLE = 2;

ConvolveKernel::ConvolveKernel(Curve* curve, int kwidth, int kheight, double normalization) {
	double sum = 0;
	m_width = kwidth;
	m_height = kheight;
	m_data = new double[kwidth * kheight];
	for (int x = 0; x < kwidth; ++x) {
		for (int y = 0; y < kheight; ++y) {
			double k = 0;
			for (int sx = 0; sx < SUPERSAMPLE; ++sx) {
				for (int sy = 0; sy < SUPERSAMPLE; ++sy) {
					double x_scaled = (2.0 * (x + (sx - SUPERSAMPLE*0.5)/SUPERSAMPLE))/(kwidth - 1.0) - 1.0;
					double y_scaled = (2.0 * (y + (sy - SUPERSAMPLE*0.5)/SUPERSAMPLE))/(kheight - 1.0) - 1.0;
					double hypot = sqrt(x_scaled*x_scaled + y_scaled*y_scaled);
					double val = (*curve)(hypot);
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

ConvolveKernel::~ConvolveKernel() {
	delete[] m_data;
}

SDL_Surface* ConvolveKernel::convolve(SDL_Surface* source) const {
	SDL_Surface* dest = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, source->w + m_width, source->h + m_height, source->format->BitsPerPixel,
		source->format->Rmask, source->format->Gmask, source->format->Bmask, source->format->Amask);

	uint8_t *src_pixels = (uint8_t*)source->pixels;
	uint8_t *dst_pixels = (uint8_t*)dest->pixels;
	SDL_LockSurface(source);
	SDL_LockSurface(dest);
	for (int x = 0; x < dest->w; ++x) {
		for (int y = 0; y < dest->h; ++y) {
			double conv_sum[4] = { 0.0, 0.0, 0.0, 0.0 };
			for (int kx = 0; kx < m_width; ++kx) {
				for (int ky = 0; ky < m_height; ++ky) {
					int conv_x = x + kx - m_width;
					int conv_y = y + ky - m_height;
					double current_k = m_data[kx + ky*m_width];

					if (m_extend) {
						// Extend edges of convolved image
						conv_x = max<int>(conv_x, 0);
						conv_x = min<int>(conv_x, source->w - 1);
						conv_y = max<int>(conv_y, 0);
						conv_y = min<int>(conv_y, source->h - 1);
					} else {
						if (conv_x < 0 || conv_x >= source->w) {
							continue;
						}
						if (conv_y < 0 || conv_y >= source->h) {
							continue;
						}
					}

					conv_sum[0] += src_pixels[conv_x*source->format->BytesPerPixel + conv_y*source->pitch]*current_k;
					if (source->format->BytesPerPixel >= 2) {
						conv_sum[1] += src_pixels[conv_x*source->format->BytesPerPixel + conv_y*source->pitch + 1]*current_k;
					}
					if (source->format->BytesPerPixel >= 3) {
						conv_sum[2] += src_pixels[conv_x*source->format->BytesPerPixel + conv_y*source->pitch + 2]*current_k;
					}
					if (source->format->BytesPerPixel >= 4) {
						conv_sum[3] += src_pixels[conv_x*source->format->BytesPerPixel + conv_y*source->pitch + 3]*current_k;
					}
				}
			}
			dst_pixels[x*dest->format->BytesPerPixel + y*dest->pitch] = min<double>(conv_sum[0]/m_normalization, 255.0);
			if (source->format->BytesPerPixel >= 2) {
				dst_pixels[x*dest->format->BytesPerPixel + y*dest->pitch + 1] = min<double>(conv_sum[1]/m_normalization, 255.0);
			}
			if (source->format->BytesPerPixel >= 3) {
				dst_pixels[x*dest->format->BytesPerPixel + y*dest->pitch + 2] = min<double>(conv_sum[2]/m_normalization, 255.0);
			}
			if (source->format->BytesPerPixel >= 4) {
				dst_pixels[x*dest->format->BytesPerPixel + y*dest->pitch + 3] = min<double>(conv_sum[3]/m_normalization, 255.0);
			}
		}
	}
	SDL_UnlockSurface(source);
	SDL_UnlockSurface(dest);

	return dest;
}

int ConvolveKernel::get_width() const {
	return m_width;
}

int ConvolveKernel::get_height() const {
	return m_height;
}
