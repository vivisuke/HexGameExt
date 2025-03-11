//© Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)
#include "cboard.h"

#include <godot_cpp/core/class_db.hpp>
#include <random>

using namespace godot;
using namespace std;

typedef unsigned char uchar;

std::mt19937 rgen(std::random_device{}()); // シードを設定

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
	m_gid.resize(m_ary_size);
	m_path.resize(m_ary_size);
	init();
}
void CBoard::init() {
	m_seq_gid = 0;
	m_n_empty = m_width * m_width;
	for(int y = 0; y != m_width; ++y) {
		auto ix = xyToIndex(0, y);
		m_cells[ix-1] = WWALL;
		m_cells[ix+m_width] = WWALL;
		for(int x = 0; x != m_width; ++x) {
			m_cells[xyToIndex(x, y)] = EMPTY;
		}
	}
	for(auto &v : m_gid) v = 0;
	for(auto &v : m_path) v = 0;
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
	return (y+1)*m_ary_width + x;
}
int CBoard::get_color(int x, int y) const {
	return m_cells[xyToIndex(x, y)];
}
bool CBoard::put_color(int x, int y, uchar col) {
	return put_ix_color(xyToIndex(x, y), col);
}
int CBoard::get_ix_color(int ix) const {
	return m_cells[ix];
}
bool CBoard::find_horz(uchar gid, int y) {
	for(int x = 0; x != m_width; ++x) {
		if( m_gid[xyToIndex(x, y)] == gid )
			return true;
	}
	return false;
}
bool CBoard::find_vert(uchar gid, int x) {
	for(int y = 0; y != m_width; ++y) {
		if( m_gid[xyToIndex(x, y)] == gid )
			return true;
	}
	return false;
}
bool CBoard::put_ix_color(int ix, uchar col) {
	m_n_empty -= 1;
	m_cells[ix] = col;
	update_gid(ix, col);
	auto gid = m_gid[ix];
	if( col == BLACK ) {
		return find_horz(gid, 0) && find_horz(gid, m_width-1);		//	上下辺と連結？
	} else {
		return find_vert(gid, 0) && find_vert(gid, m_width-1);		//	左右辺と連結？
	}
}
void CBoard::update_gid_sub(int ix, int ix2) {
	if( m_cells[ix2] != m_cells[ix] ) return;
	if (m_gid[ix] == 0)		// 最初の連結の場合
		m_gid[ix] = m_gid[ix2];
	else if( m_gid[ix] != m_gid[ix2] ) {		// グループid のマージが必要
		auto o = m_gid[ix2];
		auto n = m_gid[ix];
		for(auto i = xyToIndex(0, 0); i <= xyToIndex(m_width-1, m_width-1); ++i) {
			if (m_gid[i] == o)
				m_gid[i] = n;
		}
	}
}
void CBoard::update_gid(int ix, uchar col) {
	update_gid_sub(ix, ix-m_ary_width);
	update_gid_sub(ix, ix-m_ary_width+1);
	update_gid_sub(ix, ix-1);
	update_gid_sub(ix, ix+1);
	update_gid_sub(ix, ix+m_ary_width-1);
	update_gid_sub(ix, ix+m_ary_width);
	if( m_gid[ix] == 0 ) {
		m_gid[ix] = ++m_seq_gid;
	}
}
int CBoard::sel_move_random() const {
	if( m_n_empty == 0 ) return 0;
	int r = rgen() % m_n_empty + 1;
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_width-1, m_width-1); ++ix) {
		if( m_cells[ix] == EMPTY ) {
			if( --r == 0 )
				return ix;
		}
	}
	return 0;
}

void CBoard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("init"), &CBoard::init);
    ClassDB::bind_method(D_METHOD("set_width", "value"), &CBoard::set_width, DEFVAL(3));
    ClassDB::bind_method(D_METHOD("get_width"), &CBoard::get_width);
    ClassDB::bind_method(D_METHOD("get_n_empty"), &CBoard::get_n_empty);
    ClassDB::bind_method(D_METHOD("xyToIndex", "value", "value"), &CBoard::xyToIndex, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_color", "value", "value"), &CBoard::get_color, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("put_color", "value", "value", "value"), &CBoard::put_color, DEFVAL(0), DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("put_ix_color", "value", "value"), &CBoard::put_ix_color, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_ix_color", "value"), &CBoard::get_ix_color, DEFVAL(0));
    ClassDB::bind_method(D_METHOD("sel_move_random"), &CBoard::sel_move_random);
}
