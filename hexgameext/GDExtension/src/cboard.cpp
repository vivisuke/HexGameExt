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
	for(auto& v: m_cells) v = BWALL;
	init();
}
void CBoard::init() {
	//m_seq_gid = 0;
	m_n_empty = m_width * m_width;
	for(int y = 0; y != m_width; ++y) {
		auto ix = xyToIndex(0, y);
		m_cells[ix-1] = WWALL;
		m_cells[ix+m_width] = WWALL;
		for(int x = 0; x != m_width; ++x) {
			m_cells[xyToIndex(x, y)] = EMPTY;
		}
	}
	//for(auto &v : m_gid) v = 0;
}
void CBoard::set_width(int width) {
	m_width = width;
	update_ary();
}
int CBoard::get_width() const
{
    return m_width;
}
int CBoard::xyToIndex(int x, int y) const {	//	x, y: [0, m_width)
	return (y+1)*m_width + x;
}
int CBoard::get_color(int x, int y) const {
	return m_cells[xyToIndex(x, y)];
}
void CBoard::put_color(int x, int y, byte col) {
	//m_cells[xyToIndex(x, y)] = col;
	put_ix_color(xyToIndex(x, y), col);
}
int CBoard::get_ix_color(int ix) const {
	return m_cells[ix];
}
void CBoard::put_ix_color(int ix, byte col) {
	m_n_empty -= 1;
	m_cells[ix] = col;
}

void CBoard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_width", "value"), &CBoard::set_width, DEFVAL(3));
    ClassDB::bind_method(D_METHOD("get_width"), &CBoard::get_width);
    ClassDB::bind_method(D_METHOD("get_n_empty"), &CBoard::get_n_empty);
    ClassDB::bind_method(D_METHOD("xyToIndex", "value", "value"), &CBoard::xyToIndex, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_color", "value", "value"), &CBoard::get_color, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("put_color", "value", "value", "value"), &CBoard::put_color, DEFVAL(0), DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("put_ix_color", "value", "value"), &CBoard::put_ix_color, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_ix_color", "value"), &CBoard::get_ix_color, DEFVAL(0));
}
