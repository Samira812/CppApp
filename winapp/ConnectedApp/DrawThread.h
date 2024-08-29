#pragma once
#include "CommonObject.h"
#ifndef DRAWTTHREAD_H
#define DRAWTTHREAD_H
#include "GuiMain.h"


// Function declaration for drawing the GUI
void DrawAppWindow(void* common_ptr);

// Class to handle the drawing thread
class DrawThread {
public:
    void operator()(CommonObjects& common);
};
#endif // DRAWTTHREAD_H