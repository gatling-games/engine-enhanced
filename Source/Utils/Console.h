#pragma once
#include "Editor/MainWindow.h"

static void log(const char* fmt, ...)
{
    MainWindow::instance()->outputPanel()->log(fmt);
}