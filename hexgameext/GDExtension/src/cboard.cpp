//© Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)
#include "cboard.h"

#include <godot_cpp/core/class_db.hpp>
//#include <godot_cpp/classes/global_scope.hpp>
#include <random>
#include <iostream>

using namespace godot;
using namespace std;

typedef unsigned char uchar;

std::mt19937 rgen(std::random_device{}()); // シードを設定

CBoard::CBoard()
	: m_width(3)
{
	//cout << "a CBoard Object is created." << endl;
	UtilityFunctions::print("hello!");
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
	m_dist.resize(m_ary_size);
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
int CBoard::get_path(int x, int y) const {
	return m_path[xyToIndex(x, y)];
}
int CBoard::get_dist(int x, int y) const {
	return m_dist[xyToIndex(x, y)];
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
void CBoard::BFS_sub(int dist, int col) {
    m_next_ter.clear(); // GDScriptの #m_next_ter.clear()  と m_next_ter = PackedByteArray() に相当
    for (int ix : m_ter_lst) {
        // 上
        if (m_cells[ix - m_ary_width] == col && m_dist[ix - m_ary_width] == 0) {
            m_dist[ix - m_ary_width] = dist;
            m_next_ter.push_back(ix - m_ary_width);
        }
        // 右上
        if (m_cells[ix - m_ary_width + 1] == col && m_dist[ix - m_ary_width + 1] == 0) {
            m_dist[ix - m_ary_width + 1] = dist;
            m_next_ter.push_back(ix - m_ary_width + 1);
        }
        // 左
        if (m_cells[ix - 1] == col && m_dist[ix - 1] == 0) {
            m_dist[ix - 1] = dist;
            m_next_ter.push_back(ix - 1);
        }
        // 右
        if (m_cells[ix + 1] == col && m_dist[ix + 1] == 0) {
            m_dist[ix + 1] = dist;
            m_next_ter.push_back(ix + 1);
        }
        // 左下
        if (m_cells[ix + m_ary_width - 1] == col && m_dist[ix + m_ary_width - 1] == 0) {
            m_dist[ix + m_ary_width - 1] = dist;
            m_next_ter.push_back(ix + m_ary_width - 1);
        }
        // 下
        if (m_cells[ix + m_ary_width] == col && m_dist[ix + m_ary_width] == 0) {
            m_dist[ix + m_ary_width] = dist;
            m_next_ter.push_back(ix + m_ary_width);
        }
    }
    if (m_next_ter.empty()) return; // GDScriptの m_next_ter.is_empty() と return に相当
    m_ter_lst = m_next_ter; // GDScriptの m_ter_lst = m_next_ter.duplicate() に相当 (C++のvectorはコピー代入)
    BFS_sub(dist + 1, col);
}

// GDScriptの BFS(x, y) に相当する関数
void CBoard::BFS(int x, int y) {
    std::fill(m_dist.begin(), m_dist.end(), 0); // GDScriptの m_dist.fill(0) に相当
    int ix = xyToIndex(x, y);
    m_dist[ix] = 1;
    int col = m_cells[ix];
    m_ter_lst = {ix}; // GDScriptの m_ter_lst = [ix] に相当
    BFS_sub(2, col);
    // GDScriptの pass は C++ では不要 (何もしない)
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
int CBoard::min_dist_y(int x) {
    int mnv = std::numeric_limits<int>::max(); // 9999 の代わりに最大値を設定 (より安全)
    int mny = -1;
    for (int y = 0; y < m_width; ++y) {
        int ix = xyToIndex(x, y);
        if (m_dist[ix] != 0 && m_dist[ix] < mnv) {
            mnv = m_dist[ix];
            mny = y;
        }
    }
    return mny;
}
int CBoard::min_dist_x(int y) {
    int mnv = std::numeric_limits<int>::max(); // 9999 の代わりに最大値を設定 (より安全)
    int mnx = -1;
    for (int x = 0; x < m_width; ++x) {
        int ix = xyToIndex(x, y);
        if (m_dist[ix] != 0 && m_dist[ix] < mnv) {
            mnv = m_dist[ix];
            mnx = x;
        }
    }
    return mnx;
}
void CBoard::get_shortest_path_sub(int ix) {
    while (m_dist[ix] != 0) {
        int dist = m_dist[ix];
        m_path[ix] = dist;
        dist -= 1;
        if (m_dist[ix - m_ary_width] == dist) {
            ix = ix - m_ary_width;
        } else if (m_dist[ix - m_ary_width + 1] == dist) {
            ix = ix - m_ary_width + 1;
        } else if (m_dist[ix - 1] == dist) {
            ix = ix - 1;
        } else if (m_dist[ix + 1] == dist) {
            ix = ix + 1;
        } else if (m_dist[ix + m_ary_width - 1] == dist) {
            ix = ix + m_ary_width - 1;
        } else if (m_dist[ix + m_ary_width] == dist) {
            ix = ix + m_ary_width;
        } else {
            return;
        }
    }
}
void CBoard::get_shortest_path(uchar col) {
    std::fill(m_path.begin(), m_path.end(), 0); // m_path を 0 で埋める
    //godot::print(string("col = ") + to_string((int)col));
    if (col == WHITE) {
        int y = min_dist_y(0);
        get_shortest_path_sub(xyToIndex(0, y));
        y = min_dist_y(m_width - 1);
        get_shortest_path_sub(xyToIndex(m_width - 1, y));
    } else {
        int x = min_dist_x(0);
        get_shortest_path_sub(xyToIndex(x, 0));
        x = min_dist_x(m_width - 1);
        get_shortest_path_sub(xyToIndex(x, m_width - 1));
    }
}
void CBoard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("init"), &CBoard::init);
    ClassDB::bind_method(D_METHOD("set_width", "value"), &CBoard::set_width, DEFVAL(3));
    ClassDB::bind_method(D_METHOD("get_width"), &CBoard::get_width);
    ClassDB::bind_method(D_METHOD("get_n_empty"), &CBoard::get_n_empty);
    ClassDB::bind_method(D_METHOD("xyToIndex", "value", "value"), &CBoard::xyToIndex, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_path", "value", "value"), &CBoard::get_path, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_dist", "value", "value"), &CBoard::get_dist, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_color", "value", "value"), &CBoard::get_color, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("put_color", "value", "value", "value"), &CBoard::put_color, DEFVAL(0), DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("put_ix_color", "value", "value"), &CBoard::put_ix_color, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_ix_color", "value"), &CBoard::get_ix_color, DEFVAL(0));
    ClassDB::bind_method(D_METHOD("sel_move_random"), &CBoard::sel_move_random);
    ClassDB::bind_method(D_METHOD("BFS", "value", "value"), &CBoard::BFS, DEFVAL(0), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_shortest_path", "value"), &CBoard::get_shortest_path, DEFVAL(0));
}
