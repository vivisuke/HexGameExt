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

	m_board.resize(m_ary_size);
}

void CBoard::set_width(int width) {
	m_width = width;
	update_ary();
}
int CBoard::get_width() const
{
    return m_width;
}

void CBoard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_width", "value"), &CBoard::set_width, DEFVAL(3));
    ClassDB::bind_method(D_METHOD("get_width"), &CBoard::get_width);
}
