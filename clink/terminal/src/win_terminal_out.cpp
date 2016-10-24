// Copyright (c) 2016 Martin Ridgers
// License: http://opensource.org/licenses/MIT

#include "pch.h"
#include "win_terminal_out.h"

#include <core/base.h>
#include <core/str_iter.h>

//------------------------------------------------------------------------------
void win_terminal_out::begin()
{
    m_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(m_stdout, &m_prev_mode);
}

//------------------------------------------------------------------------------
void win_terminal_out::end()
{
    SetConsoleMode(m_stdout, m_prev_mode);
    m_stdout = nullptr;
}

//------------------------------------------------------------------------------
void win_terminal_out::write(const char* chars, int length)
{
    str_iter iter(chars, length);
    while (length > 0)
    {
        wchar_t wbuf[384];
        int n = min<int>(sizeof_array(wbuf), length + 1);
        n = to_utf16(wbuf, n, iter);

        write(wbuf, n);

        n = int(iter.get_pointer() - chars);
        length -= n;
        chars += n;
    }
}

//------------------------------------------------------------------------------
void win_terminal_out::write(const wchar_t* chars, int length)
{
    DWORD written;
    WriteConsoleW(m_stdout, chars, length, &written, nullptr);
}

//------------------------------------------------------------------------------
void win_terminal_out::flush()
{
    // When writing to the console conhost.exe will restart the cursor blink
    // timer and hide it which can be disorientating, especially when moving
    // around a line. The below will make sure it stays visible.
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_stdout, &csbi);
    SetConsoleCursorPosition(m_stdout, csbi.dwCursorPosition);
}

//------------------------------------------------------------------------------
int win_terminal_out::get_columns() const
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_stdout, &csbi);
    return csbi.dwSize.X;
}

//------------------------------------------------------------------------------
int win_terminal_out::get_rows() const
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_stdout, &csbi);
    return (csbi.srWindow.Bottom - csbi.srWindow.Top) + 1;
}

//------------------------------------------------------------------------------
void* win_terminal_out::get_handle() const
{
    return m_stdout;
}
