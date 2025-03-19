//----------------------------------------------------------------------
//
//			File:			"MCTS.h"
//			Created:		16-3-2025
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#include <vector>
#include <limits>
#include <math.h>
#include "cboard.h"

const double C_PUCT = 1.4;

typedef unsigned char uchar;

struct MCTSNode {
public:
	MCTSNode() {
		m_ix = 0;
		m_col = 0;
		m_visits = 0;
		m_wins = 0;
	}
	//	ノードの m_children[ix] の UCB を計算
	double calculate_ucb(int ix) const {
		const MCTSNode* node = &m_children[ix];
		if( node->m_visits == 0 ) return std::numeric_limits<double>::max();
		double exploitation_term = (double)node->m_wins / node->m_visits;		// 活用 (勝率)
		double exploration_term = C_PUCT * sqrt(log(m_visits)) / node->m_visits;		// 探索
		return exploitation_term + exploration_term;
	}
	//	return: 次に探索すべき子ノード ix
	int select_child_ucb() const {
		int best_ix = -1;
		double best_ucb = std::numeric_limits<double>::min();
		for(int ix = 0; ix != m_children.size(); ++ix) {
			auto ucb = calculate_ucb(ix);
			if( ucb > best_ucb ) {
				best_ucb = ucb;
				best_ix = ix;
			}
		}
		return best_ix;
	}
public:
	//MCTSNode	*m_parent;		//	親ノードへのポインタ
	uchar	m_ix;				//	このノードに至る着手位置
	uchar	m_col;
	int		m_visits;			//	訪問回数
	int		m_wins;				//	勝利回数
	std::vector<MCTSNode>	m_children;		//	子ノードリスト
};

class MCTS {
public:
	MCTS(/*CBoardBasic *bd*/)
		//: m_vd(bd)
	{
	}
public:
	void expand(MCTSNode *node, CBoardBasic& bd, uchar col) {
		node->m_children.reize(bd.get_n_empty());
		int ix = bd.xyToIndex(0, 0);
		for(int i = 0; i != bd.get_n_empty(); ++i) {
			while( bd.get_ix_color(ix) != EMPTY ) ++ix;
			node->m_children[i].m_ix = ix;
			node->m_children[i].m_col = col;
		}
	}
	int	search(CBoardBasic& bd, uchar col, int n_itr) {
		for(int i = 0; i != n_itr; ++i) {
			MCTSNode *node = m_root;
		}
		return 0;
	}

public:
	CBoardBasic	*m_bd;
	MCTSNode	m_root;
	std::vector<MCTSNode*>	m_node_list;		//	探索パス

};
