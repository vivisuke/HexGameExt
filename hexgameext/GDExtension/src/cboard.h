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

using namespace godot;

class CBoard : public RefCounted
{
    GDCLASS(CBoard, RefCounted);

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

protected:
    static void _bind_methods();

    void	update_ary();
	void	update_gid(int ix, uchar col);
	void	update_gid_sub(int ix, int ix2);
	bool	find_horz(uchar id, int y);
	bool	find_vert(uchar id, int x);
	int		CBoard::min_dist_y(int x);
	int		CBoard::min_dist_x(int y);
	void	BFS_sub(int dist, int col);
	void	CBoard::get_shortest_path_sub(int ix);

public:
    CBoard();
    CBoard(const CBoard&);
    ~CBoard();

    void	init();
    void	set_width(int width);
    int		get_width() const;
    int		get_n_empty() const { return m_n_empty; }
    int		xyToIndex(int x, int y) const;
    int		get_color(int x, int y) const;
    bool	put_color(int x, int y, uchar col);
    int		get_ix_color(int ix) const;
    bool	put_ix_color(int ix, uchar col);
    int		sel_move_random() const;
    int		get_path(int x, int y) const;
    int		get_dist(int x, int y) const;
	void	BFS(int x, int y);							//	幅優先探索により、(x, y) からの距離+1を m_dist[] に格納
	void	CBoard::get_shortest_path(uchar col);
	uchar	rollout(int x, int y, uchar col) const;		//	初手：(x, y, col)、ランダムゲーム、勝色を返す
};

