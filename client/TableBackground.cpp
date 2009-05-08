/*
 * client/TableBackground.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "TableBackground.hpp"
#include "common/LMException.hpp"

TableBackground::TableBackground(int num_rows, int width) {
	m_num_rows = 0; // Pre-set this because set_num_rows depends on the old value
	set_num_rows(num_rows);
	m_image_width = width;
	m_intensity = Color(1.0, 1.0, 1.0, 1.0);
	m_border_width = 1;
	set_center_x(width*0.5);
}

TableBackground* TableBackground::clone() const {
	return new TableBackground(*this);
}

void TableBackground::draw_row(int row) const {
	int height = m_row_heights[row];
	glBegin(GL_QUADS);
	glColor4d(m_cell_colors[row].r, m_cell_colors[row].g, m_cell_colors[row].b, m_cell_colors[row].a);
	glVertex3d(m_border_width, m_border_width, 1);
	glVertex3d(m_image_width - m_border_width, m_border_width, 1);
	glVertex3d(m_image_width - m_border_width, height - m_border_width, 1);
	glVertex3d(m_border_width, height - m_border_width, 1);
	glColor4d(m_border_color.r, m_border_color.g, m_border_color.b, m_border_color.a);
	glVertex3d(0, 0, 0);
	glVertex3d(m_image_width, 0, 0);
	glVertex3d(m_image_width, height, 0);
	glVertex3d(0, height, 0);
	glEnd();
	glTranslated(0, height, 0);
}

int TableBackground::get_num_rows() const {
	return m_num_rows;
}

int TableBackground::get_row_height(int row) const {
	if (row >= m_num_rows || row < 0) {
		throw LMException("Row out of bounds");
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
		m_row_heights[i] = 0;
	}
}

void TableBackground::set_row_height(int row, int height) {
	if (row >= m_num_rows || row < 0) {
		throw LMException("Row out of bounds");
	}
	
	int delta = height - m_row_heights[row];
	m_row_heights[row] = height;

	m_image_height += delta;
}

void TableBackground::set_image_width(int width) {
	m_image_width = width;
}

void TableBackground::set_border_width(int pixels) {
	m_border_width = pixels;
}

void TableBackground::set_border_color(Color color) {
	m_border_color = color;
}

void TableBackground::set_cell_color(int row, Color color) {
	if (row >= m_num_rows || row < 0) {
		throw LMException("Row out of bounds");
	}
	
	m_cell_colors[row] = color;
}

void TableBackground::set_alpha(double alpha) {
	m_intensity.a = alpha;
}

void TableBackground::set_red_intensity(double r) {
	m_intensity.r = r;
}

void TableBackground::set_green_intensity(double g) {
	m_intensity.g = g;
}

void TableBackground::set_blue_intensity(double b) {
	m_intensity.b = b;
}

void TableBackground::draw(const GameWindow* window) const {
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST); // Enable depth test so parts aren't redrawn
	glClearDepth(-1);
	glDepthFunc(GL_GREATER);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(m_intensity.r != 0.0, m_intensity.g != 0.0, m_intensity.b != 0.0, m_intensity.a != 0.0);

	glPushMatrix();
	transform_gl();
	for (int i = 0; i < m_num_rows; ++i) {
		draw_row(i);
	}
	glPopMatrix();

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}
