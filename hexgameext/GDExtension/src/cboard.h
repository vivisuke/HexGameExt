#pragma once
//© Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)

// We don't need windows.h in this plugin but many others do and it throws up on itself all the time
// So best to include it and make sure CI warns us when we use something Microsoft took for their own goals....
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/classes/ref.hpp>
#include <vector>

typedef unsigned char uchar;

enum {
	EMPTY = 0, BLACK, WHITE, BWALL, WWALL,		//	上下：BWALL, 左右：WWALL
};

//--------------------------------------------------------------------------------
class CBoardBasic {		//	盤面クラス
public:
	CBoardBasic() {
	}
	CBoardBasic(const CBoardBasic& x) {
		*this = x;
	}
	int xyToIndex(int x, int y) const {	//	x, y: [0, m_width)
		return (y+1)*m_ary_width + x;
	}
    void	init() {
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
    void	set_width(int width) {
		m_width = width;
		update_ary();
    }
    void	update_ary() {
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
    int		get_width() const { return m_width; };
    int		get_n_empty() const { return m_n_empty; }
    int		get_color(int x, int y) const {
		return m_cells[xyToIndex(x, y)];
    }
    int		get_ix_color(int ix) const {
		return m_cells[ix];
    }
	void update_gid_sub(int ix, int ix2) {
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
	void update_gid(int ix, uchar col) {
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
	bool find_horz(uchar gid, int y) const {
		for(int x = 0; x != m_width; ++x) {
			if( m_gid[xyToIndex(x, y)] == gid )
				return true;
		}
		return false;
	}
	bool find_vert(uchar gid, int x) const {
		for(int y = 0; y != m_width; ++y) {
			if( m_gid[xyToIndex(x, y)] == gid )
				return true;
		}
		return false;
	}
    bool	put_ix_color(int ix, uchar col) {
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
	uchar put_color(int x, int y, uchar col) {
		auto ix = xyToIndex(x, y);
		return put_ix_color(ix, col);
	}
	int get_path(int x, int y) const {
		return m_path[xyToIndex(x, y)];
	}
	int get_dist(int x, int y) const {
		return m_dist[xyToIndex(x, y)];
	}
	void BFS_sub(int dist, int col) {
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
	void BFS(int x, int y) {
	    std::fill(m_dist.begin(), m_dist.end(), 0); // GDScriptの m_dist.fill(0) に相当
	    int ix = xyToIndex(x, y);
	    m_dist[ix] = 1;
	    int col = m_cells[ix];
	    m_ter_lst = {ix}; // GDScriptの m_ter_lst = [ix] に相当
	    BFS_sub(2, col);
	    // GDScriptの pass は C++ では不要 (何もしない)
	}
	int min_dist_y(int x) {
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
	int min_dist_x(int y) {
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
	void get_shortest_path_sub(int ix) {
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
	void get_shortest_path(uchar col) {
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
	int sel_move_random() const;
    int sel_move_PMC(uchar) const;				//	勝率による純粋モンテカルロ法着手選択
	uchar rollout(int x, int y, uchar col) const;		//	初手：(x, y, col)、ランダムゲーム、勝色を返す
	

public:
    int		m_width;
    int		m_ary_width;
    int		m_ary_height;
    int		m_ary_size;
    int		m_n_empty;
	uchar	m_seq_gid;
	__int64		m_n_node;					//	末端ノード数
    std::vector<uchar>	m_cells;
	std::vector<uchar>	m_gid;				//	石グループID
	std::vector<uchar>	m_gid_stack;
	std::vector<uchar>	m_path;				//	要素：1以上ならば最短パス
	std::vector<uchar>	m_dist;				//	0 for 未探索、1以上 for 距離+1
	std::vector<int>	m_put_stack;		//	着手箇所
	std::vector<int>	m_seq_stack;		//	for m_seq_gid
	std::vector<int>	m_ter_lst;			//	末端位置リスト
	std::vector<int>	m_next_ter;			//	次の末端位置リスト
};

//--------------------------------------------------------------------------------

using namespace godot;

class CBoard : public RefCounted
{
    GDCLASS(CBoard, RefCounted);

    CBoardBasic		m_bd;
#if 0
    int		m_width;
    int		m_ary_width;
    int		m_ary_height;
    int		m_ary_size;
    int		m_n_empty;
	uchar	m_seq_gid;
	__int64		m_n_node;					//	末端ノード数
    std::vector<uchar>	m_cells;
	std::vector<uchar>	m_gid;				//	石グループID
	std::vector<uchar>	m_gid_stack;
	std::vector<uchar>	m_path;				//	要素：1以上ならば最短パス
	std::vector<uchar>	m_dist;				//	0 for 未探索、1以上 for 距離+1
	std::vector<int>	m_put_stack;		//	着手箇所
	std::vector<int>	m_seq_stack;		//	for m_seq_gid
	std::vector<int>	m_ter_lst;			//	末端位置リスト
	std::vector<int>	m_next_ter;			//	次の末端位置リスト
#endif

protected:
    static void _bind_methods();

#if 0
    void	update_ary();
	void	update_gid(int ix, uchar col);
	void	update_gid_sub(int ix, int ix2);
	bool	find_horz(uchar id, int y);
	bool	find_vert(uchar id, int x);
	int		min_dist_y(int x);
	int		min_dist_x(int y);
	void	BFS_sub(int dist, int col);
	void	get_shortest_path_sub(int ix);
	void	saveStat();			//	状態保存
#endif

public:
    CBoard();
    CBoard(const CBoard&);
    ~CBoard();

    void	init();
    void	set_width(int width);
    int		get_width() const;
    int		get_n_empty() const { return m_bd.get_n_empty(); }
    int		xyToIndex(int x, int y) const;
    int		get_color(int x, int y) const;
    bool	put_color(int x, int y, uchar col);
    int		get_ix_color(int ix) const;
    bool	put_ix_color(int ix, uchar col);
    int		sel_move_random() const;
    int		sel_move_PMC(uchar) const;				//	勝率による純粋モンテカルロ法着手選択
    int		get_path(int x, int y) const;
    int		get_dist(int x, int y) const;
	void	BFS(int x, int y);							//	幅優先探索により、(x, y) からの距離+1を m_dist[] に格納
	void	get_shortest_path(uchar col);
	uchar	rollout(int x, int y, uchar col) const;		//	初手：(x, y, col)、ランダムゲーム、勝色を返す
};

