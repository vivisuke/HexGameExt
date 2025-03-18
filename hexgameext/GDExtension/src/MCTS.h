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

typedef unsigned char uchar;

struct MCTSNode {
	MCTSNode	*m_parent;		//	親ノードへのポインタ
	uchar	m_ix;				//	このノードに至る着手位置
	uchar	m_col;
	int		m_visits;			//	訪問回数
	int		m_wins;				//	勝利回数
	std::vector<MCTSNode>	m_children;		//	子ノードリスト
};

class MCTS {
	MCTSNode	m_toot;

};
