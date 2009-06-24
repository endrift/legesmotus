/*
 * client/TableBackground.cpp
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

#include "TableBackground.hpp"
#include "common/Exception.hpp"
#include <algorithm>
#include "common/math.hpp"

using namespace LM;
using namespace std;

const int TableBackground::CORNER_RESOLUTION = 10;

TableBackground::TableBackground(int num_rows, double width) {
	m_num_rows = 0; // Pre-set this because set_num_rows depends on the old value
	set_num_rows(num_rows);
	m_image_width = width;
	m_border_width = 1.0;
	m_corner_radius = 0.0;
	m_border_collapse = false;
	set_center_x(width*0.5);
}

TableBackground* TableBackground::clone() const {
	return new TableBackground(*this);
}

void TableBackground::draw_corner(double x, double y, double rx, double ry) const {
	GLdouble vertices[2*(CORNER_RESOLUTION + 2)] = { x, y };
	for (int i = 0; i <= CORNER_RESOLUTION; ++i) {
		vertices[2*(i+1)] = x - cos(i*M_PI/(2*CORNER_RESOLUTION))*rx;
		vertices[2*(i+1)+1] = y - sin(i*M_PI/(2*CORNER_RESOLUTION))*ry;
	}
	glVertexPointer(2, GL_DOUBLE, 0, vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CORNER_RESOLUTION + 2);
}

void TableBackground::draw_row(int row) const {
	double height = m_row_heights[row];
	double top_offset = m_border_width;
	double bottom_offset = m_border_width;
	double side_offset = m_border_width;
	double diff = max<double>(0, m_corner_radius - m_border_width);

	if (row == 0) {
		if (m_corner_radius > 0) {
			top_offset = max<double>(top_offset, m_corner_radius);
			side_offset = max<double>(side_offset, m_corner_radius);

			if(diff > 0) {
				//TODO draw_corner func
				glColor4d(m_cell_colors[row].r, m_cell_colors[row].g, m_cell_colors[row].b, m_cell_colors[row].a);
				draw_corner(side_offset, top_offset, diff, diff);
				draw_corner(m_image_width - side_offset, top_offset, -diff, diff);
			}
		}
		if (m_border_collapse) {
			bottom_offset *= 0.5;
		}
	} else if (row == m_num_rows - 1) {
		if (m_corner_radius > 0) {
			bottom_offset = max<double>(top_offset, m_corner_radius);
			side_offset = max<double>(side_offset, m_corner_radius);
			
			double diff = m_corner_radius - m_border_width;
			if(diff > 0) {
				glColor4d(m_cell_colors[row].r, m_cell_colors[row].g, m_cell_colors[row].b, m_cell_colors[row].a);
				draw_corner(side_offset, height - top_offset - diff, diff, -diff);
				draw_corner(m_image_width - side_offset, height - top_offset - diff, -diff, -diff);
			}
		}
		if (m_border_collapse) {
			top_offset *= 0.5;
		}
	} else if (m_border_collapse) {
		top_offset *= 0.5;
		bottom_offset *= 0.5;
	}

	if (top_offset + bottom_offset < height) {
		glColor4d(m_cell_colors[row].r, m_cell_colors[row].g, m_cell_colors[row].b, m_cell_colors[row].a);
		draw_rect(side_offset, top_offset, m_image_width - side_offset, height - bottom_offset);
		if (top_offset > m_border_width) {
			draw_rect(side_offset, m_border_width, m_image_width - side_offset, top_offset);
		}
		if (bottom_offset > m_border_width) {
			draw_rect(side_offset, height - bottom_offset, m_image_width - side_offset, height - m_border_width);
		}
		if (side_offset > m_border_width) {
			draw_rect(m_border_width, top_offset, side_offset, height - bottom_offset);
			draw_rect(m_image_width - side_offset, top_offset, m_image_width - m_border_width, height - bottom_offset);
		}
	}

	glColor4d(m_border_color.r, m_border_color.g, m_border_color.b, m_border_color.a);
	if (row == 0) {
		bottom_offset = 0;
		if (m_corner_radius > 0) {
			draw_corner(side_offset, top_offset, side_offset, top_offset);
			draw_corner(m_image_width - side_offset, top_offset, -side_offset, top_offset);
		} else {
			side_offset = 0;
		}
	} else if (row == m_num_rows - 1) {
		top_offset = 0;
		if (m_corner_radius > 0) {
			draw_corner(side_offset, height - bottom_offset, side_offset, -bottom_offset);
			draw_corner(m_image_width - side_offset, height - bottom_offset, -side_offset, -bottom_offset);
		} else {
			side_offset = 0;
		}
	} else {
		top_offset = 0;
		bottom_offset = 0;
		side_offset = 0;
	}

	draw_rect(0, top_offset, m_image_width, height - bottom_offset);
	draw_rect(side_offset, 0, m_image_width - side_offset, height);
	glTranslated(0, height, 0);
}

int TableBackground::get_num_rows() const {
	return m_num_rows;
}

double TableBackground::get_row_height(int row) const {
	if (row >= m_num_rows || row < 0) {
		throw Exception("Row out of bounds");
	}
	
	return m_row_heights[row];
}

void TableBackground::set_num_rows(int num_rows) {
	int old_num_rows = m_num_rows;
	m_cell_colors.resize(num_rows);
	m_row_heights.resize(num_rows);
	m_num_rows = num_rows;
	for (int i = old_num_rows; i < num_rows; ++i) {
		m_cell_colors[i] = Color(0,0,0,0);
		m_row_heights[i] = 0.0;
	}
}

void TableBackground::set_row_height(int row, double height) {
	if (row >= m_num_rows || row < 0) {
		throw Exception("Row out of bounds");
	}
	
	double delta = height - m_row_heights[row];
	m_row_heights[row] = height;

	m_image_height += delta;
}

void TableBackground::set_image_width(double width) {
	m_image_width = width;
}

void TableBackground::set_border_width(double pixels) {
	m_border_width = pixels;
}

void TableBackground::set_border_color(Color color) {
	m_border_color = color;
}

void TableBackground::set_cell_color(int row, Color color) {
	if (row >= m_num_rows || row < 0) {
		throw Exception("Row out of bounds");
	}
	
	m_cell_colors[row] = color;
}

void TableBackground::set_corner_radius(double radius) {
	m_corner_radius  = radius;
}

void TableBackground::set_border_collapse(bool collapse) {
	m_border_collapse = collapse;
}

void TableBackground::set_height(double height) {
	double total_height = 0;
	for (int i = 0; i < m_num_rows; ++i) {
		total_height += get_row_height(i);
	}
	for (int i = 0; i < m_num_rows; ++i) {
		set_row_height(i, get_row_height(i)/total_height*height);
	}
}

void TableBackground::draw(const GameWindow* window) const {
	(void)(window); // Unused
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST); // Enable depth test so parts aren't redrawn
	glClearDepth(-1);
	glDepthFunc(GL_GREATER);
	glClear(GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	transform_gl();
	for (int i = 0; i < m_num_rows; ++i) {
		draw_row(i);
	}
	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}
