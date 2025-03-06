//© Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)
#include "cboard.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

CBoard::CBoard()
	: m_width(3)
{
	update_ary();
}

CBoard::~CBoard()
{
}

void CBoard::update_ary() {
	m_ary_width = m_width + 1;
	m_ary_height = m_width + 2;
	m_ary_size = m_ary_width * m_ary_height;

	m_cells.resize(m_ary_size);
}

void CBoard::set_width(int width) {
	m_width = width;
	update_ary();
}
int CBoard::get_width() const
{
    return m_width;
}
int CBoard::xyToIX(int x, int y) const {	//	x, y: [0, m_width)
	return (y+1)*m_width + x;
}
void CBoard::put_color(int x, int y, byte col) {
	m_cells[xyToIX(x, y)] = col;
}
int CBoard::get_color(int x, int y) const
{
	return m_cells[xyToIX(x, y)];
}

void CBoard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_width", "value"), &CBoard::set_width, DEFVAL(3));
    ClassDB::bind_method(D_METHOD("get_width"), &CBoard::get_width);
    ClassDB::bind_method(D_METHOD("xyToIX", "value", "value"), &CBoard::xyToIX, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("put_color", "value", "value", "value"), &CBoard::put_color, DEFVAL(0), DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_color", "value", "value"), &CBoard::get_color, DEFVAL(0), DEFVAL(0));
}
