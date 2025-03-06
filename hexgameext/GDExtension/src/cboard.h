#pragma once
//© Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)

// We don't need windows.h in this plugin but many others do and it throws up on itself all the time
// So best to include it and make sure CI warns us when we use something Microsoft took for their own goals....
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/classes/ref.hpp>

using namespace godot;

class CBoard : public RefCounted
{
    GDCLASS(CBoard, RefCounted);

    int m_width;

protected:
    static void _bind_methods();

public:
    CBoard(int width = 3);
    ~CBoard();

    int get_width() const;
};

