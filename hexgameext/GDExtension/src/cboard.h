#pragma once
//© Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)

// We don't need windows.h in this plugin but many others do and it throws up on itself all the time
// So best to include it and make sure CI warns us when we use something Microsoft took for their own goals....
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/classes/ref.hpp>
#include <vector>

typedef unsigned char byte;

enum {
	EMPTY = 0, BLACK, WHITE,
};

using namespace godot;

class CBoard : public RefCounted
{
    GDCLASS(CBoard, RefCounted);

    int		m_width;
    int		m_ary_width;
    int		m_ary_height;
    int		m_ary_size;
    std::vector<byte>	m_cells;

protected:
    static void _bind_methods();

    void	update_ary();

public:
    CBoard();
    ~CBoard();

    void	set_width(int width);
    int		get_width() const;
    int		xyToIX(int x, int y) const;
    void	put_color(int x, int y, byte col);
    int		get_color(int x, int y) const;
};

