//----------------------------------------------------------------------
//
//			File:			"MCTS.h"
//			Created:		16-3-2025
//			Author:			�Óc�L�G
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#include <vector>

typedef unsigned char uchar;

struct MCTSNode {
	MCTSNode	*m_parent;		//	�e�m�[�h�ւ̃|�C���^
	uchar	m_ix;				//	���̃m�[�h�Ɏ��钅��ʒu
	uchar	m_col;
	int		m_visits;			//	�K���
	int		m_wins;				//	������
	std::vector<MCTSNode>	m_children;		//	�q�m�[�h���X�g
};

class MCTS {
	MCTSNode	m_toot;

};
