#pragma once
#include "Editor/MainWindow.h"

static void log(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    MainWindow::instance()->outputPanel()->log(fmt, args);
    va_end(args);
}