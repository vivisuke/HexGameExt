//© Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)
#include "cboard.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

CBoard::CBoard(int width)
	: m_width(width)
{
}

CBoard::~CBoard()
{
}

int CBoard::get_width() const
{
    return m_width;
}

void CBoard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_width"), &CBoard::get_width);
}