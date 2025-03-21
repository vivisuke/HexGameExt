//© Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)
#include "cboard.h"

#include <godot_cpp/core/class_db.hpp>
//#include <godot_cpp/classes/global_scope.hpp>
#include <random>
#include <iostream>
#include "MCTS.h"

using namespace godot;
using namespace std;

typedef unsigned char uchar;

std::mt19937 rgen(std::random_device{}()); // シードを設定

char buf[256];

CBoardBasic g_bd;

int CBoardBasic::sel_move_random() const {
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
int CBoardBasic::sel_move_PMC(uchar col) const {
	int bestix = 0;
	double best = -1;
	const int N_ROLLOUT = 10000;
	for(int y = 0; y != m_width; ++y) {
		string txt;
		for(int x = 0; x != m_width; ++x) {
			if( get_color(x, y) == EMPTY ) {
				int wcnt = 0;
				for(int i = 0; i != N_ROLLOUT; ++i) {
					if( rollout(x, y, col) == col )
						++wcnt;
				}
				auto r = 100.0 * wcnt / N_ROLLOUT;
				sprintf(buf, "%6.1f%%", r);
				txt += string(buf);
				if( r > best ) {
					best = r;
					bestix = xyToIndex(x, y);
				}
			} else {
				txt += "  -----";
			}
		}
		UtilityFunctions::print(&txt[0]);
	}
	//sprintf(buf, "best = %6.1f%%", best);
	//string txt = buf;
	//UtilityFunctions::print(&txt[0]);
	//sprintf(buf, "(x, y) = %d, %d", ixToX(bestix), ixToY(bestix));
	//txt = buf;
	//UtilityFunctions::print(&txt[0]);
	return bestix;
}
uchar CBoardBasic::rollout(int x, int y, uchar col) const {
	return rollout(xyToIndex(x, y), col);
}
uchar CBoardBasic::rollout(int ix, uchar col) const {
#if 1
	g_bd = *this;
	if( g_bd.put_ix_color(ix, col) ) return col;
	for(;;) {
		col = (BLACK + WHITE) - col;
		auto ix = g_bd.sel_move_random();
		if( g_bd.put_ix_color(ix, col) ) return col;
	}
#else
	CBoardBasic b2(*this);
	if( b2.put_color(x, y, col) ) return col;
	for(;;) {
		col = (BLACK + WHITE) - col;
		auto ix = b2.sel_move_random();
		if( b2.put_ix_color(ix, col) ) return col;
	}
#endif
}
int CBoardBasic::sel_move_MCTS(uchar col) const {
	MCTS mcts;
	g_bd = *this;
	mcts.search(g_bd, col, 1);
	return 0;
}
//--------------------------------------------------------------------------------

CBoard::CBoard()
	//: m_width(3)
{
	//cout << "a CBoard Object is created." << endl;
	UtilityFunctions::print("hello!");
	m_bd.update_ary();
}
CBoard::~CBoard()
{
}
String CBoard::test_str() const {
#if 0
	std::string txt = "hoge";
	//auto txt = String("xyz = {}").format(123);
	return String(&txt[0]);
#else
	char buf[256];
	sprintf(buf, "a = %4d", 123);
	string txt(buf);
	return String(&txt[0]);
	//return String(buf);
#endif
}
void CBoard::init() {
	m_bd.init();
}
void CBoard::set_width(int width) {
	m_bd.set_width(width);
#if 0
	m_width = width;
	update_ary();
#endif
}
int CBoard::get_width() const
{
	return m_bd.get_width();
    //return m_width;
}
int CBoard::xyToIndex(int x, int y) const {	//	x, y: [0, m_width)
	return m_bd.xyToIndex(x, y);
	//return (y+1)*m_ary_width + x;
}
int CBoard::get_path(int x, int y) const {
	return m_bd.get_path(x, y);
	//return m_path[xyToIndex(x, y)];
}
int CBoard::get_dist(int x, int y) const {
	return m_bd.get_dist(x, y);
	//return m_dist[xyToIndex(x, y)];
}
int CBoard::get_color(int x, int y) const {
	return m_bd.get_color(x, y);
	//return m_cells[xyToIndex(x, y)];
}
bool CBoard::put_color(int x, int y, uchar col) {
	return put_ix_color(xyToIndex(x, y), col);
	//return put_ix_color(xyToIndex(x, y), col);
}
int CBoard::get_ix_color(int ix) const {
	return m_bd.get_ix_color(ix);
	//return m_cells[ix];
}
#if 0
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
#endif
bool CBoard::put_ix_color(int ix, uchar col) {
	return m_bd.put_ix_color(ix, col);
#if 0
	m_n_empty -= 1;
	m_cells[ix] = col;
	update_gid(ix, col);
	auto gid = m_gid[ix];
	if( col == BLACK ) {
		return find_horz(gid, 0) && find_horz(gid, m_width-1);		//	上下辺と連結？
	} else {
		return find_vert(gid, 0) && find_vert(gid, m_width-1);		//	左右辺と連結？
	}
#endif
}
#if 0
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
#endif
#if 0
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
#endif

// GDScriptの BFS(x, y) に相当する関数
void CBoard::BFS(int x, int y) {
    m_bd.BFS(x, y);
#if 0
    std::fill(m_dist.begin(), m_dist.end(), 0); // GDScriptの m_dist.fill(0) に相当
    int ix = xyToIndex(x, y);
    m_dist[ix] = 1;
    int col = m_cells[ix];
    m_ter_lst = {ix}; // GDScriptの m_ter_lst = [ix] に相当
    BFS_sub(2, col);
    // GDScriptの pass は C++ では不要 (何もしない)
#endif
}
int CBoard::sel_move_random() const {
	return m_bd.sel_move_random();
#if 0
	if( m_n_empty == 0 ) return 0;
	int r = rgen() % m_n_empty + 1;
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_width-1, m_width-1); ++ix) {
		if( m_cells[ix] == EMPTY ) {
			if( --r == 0 )
				return ix;
		}
	}
	return 0;
#endif
}
int CBoard::sel_move_PMC(uchar col) const {
	return m_bd.sel_move_PMC(col);
}
int CBoard::sel_move_MCTS(uchar col) const {
	return m_bd.sel_move_MCTS(col);
}
#if 0
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
#endif
#if 0
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
#endif
void CBoard::get_shortest_path(uchar col) {
    m_bd.get_shortest_path(col);
#if 0
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
#endif
}
uchar CBoard::rollout(int x, int y, uchar col) const {
	return m_bd.rollout(x, y, col);
	//return col;
#if 0
	CBoardRO b2(*this);
	if( b2.put_color(x, y, col) ) return col;
	for(;;) {
		col = (BLACK + WHITE) - col;
		auto ix = b2.sel_move_random();
		if( b2.put_ix_color(ix, col) ) return col;
	}
#endif
}
void CBoard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("test_str"), &CBoard::test_str);
    ClassDB::bind_method(D_METHOD("init"), &CBoard::init);
    ClassDB::bind_method(D_METHOD("set_width", "width"), &CBoard::set_width, DEFVAL(3));
    ClassDB::bind_method(D_METHOD("get_width"), &CBoard::get_width);
    ClassDB::bind_method(D_METHOD("get_n_empty"), &CBoard::get_n_empty);
    ClassDB::bind_method(D_METHOD("xyToIndex", "x", "y"), &CBoard::xyToIndex);
    ClassDB::bind_method(D_METHOD("get_path", "x", "y"), &CBoard::get_path);
    ClassDB::bind_method(D_METHOD("get_dist", "x", "y"), &CBoard::get_dist);
    ClassDB::bind_method(D_METHOD("get_color", "x", "y"), &CBoard::get_color);
    ClassDB::bind_method(D_METHOD("put_color", "x", "y", "color"), &CBoard::put_color);
    ClassDB::bind_method(D_METHOD("put_ix_color", "ix", "color"), &CBoard::put_ix_color);
    ClassDB::bind_method(D_METHOD("get_ix_color", "ix"), &CBoard::get_ix_color);
    ClassDB::bind_method(D_METHOD("sel_move_random"), &CBoard::sel_move_random);
    ClassDB::bind_method(D_METHOD("sel_move_PMC", "color"), &CBoard::sel_move_PMC);
    ClassDB::bind_method(D_METHOD("sel_move_MCTS", "color"), &CBoard::sel_move_MCTS);
    ClassDB::bind_method(D_METHOD("BFS", "x", "y"), &CBoard::BFS);
    ClassDB::bind_method(D_METHOD("get_shortest_path", "color"), &CBoard::get_shortest_path);
    ClassDB::bind_method(D_METHOD("rollout", "x", "y", "color"), &CBoard::rollout);
}
