//
// Created by Benjamin Sylvanus on 5/25/23.
//
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#ifndef GLMETHODS_H
#define GLMETHODS_H
#ifdef __APPLE__

#include <GLUT/glut.h>

#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <iostream>
#include "fstream"
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include "Vectors.h"
#include "datatemplate.h"

namespace GLM {
///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////
    const int SCREEN_WIDTH = 600;
    const int SCREEN_HEIGHT = 600;

///////////////////////////////////////////////////////////////////////////////
// GLOBALS
///////////////////////////////////////////////////////////////////////////////
    void extern *font;
    bool extern mouseLeftDown;
    bool extern mouseRightDown;
    bool extern showBoxes;
    float extern mouseX, mouseY;
    float extern cameraAngleX;
    float extern cameraAngleY;
    float extern cameraDistance;
    int extern screenWidth, screenHeight;
    int extern drawMode;
    int extern currIndex;
    bool extern animating;
    std::vector<Vector3> extern path_test;
    std::vector<Vector3> extern gstart;
    std::vector<Vector3> extern gend;
    std::vector<std::vector<Vector3> > extern path_test_v;
    std::vector<Vector3> extern colors;
    std::vector<float> extern Radii;
    Vector3 extern center;
    std::vector<Vector3> extern cubes;
    std::vector<std::vector<float> > extern data;
    std::vector<std::vector<float> > extern swcdata;
    std::vector<std::vector<uint64_t> > extern dims;
    std::vector<std::uint64_t> extern lut;
    std::vector<std::vector<std::vector<int> > > extern lutdata;
    std::vector<int> extern lutindex;

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS
///////////////////////////////////////////////////////////////////////////////
    void displayCB();

    void reshapeCB(int w, int h);

    void timerCB(int millisec);

    void keyboardCB(unsigned char key, int x, int y);

    void mouseCB(int button, int stat, int x, int y);

    void mouseMotionCB(int x, int y);

    void mousePassiveMotionCB(int x, int y);

    void exitCB();
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
    void initGL();

    int initGLUT(int argc, char **argv);

    bool initSharedMem();

    void clearSharedMem();

    void initLights();

    void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);

    void drawString(const char *str, int x, int y, float color[4], void *pVoid);

    void showInfo();

    void drawSphere();

    void drawCube();

    void drawPath();

    void draw();

    std::vector<Vector3> buildSpiralPathModified(float r1, float r2, Vector3 l1, Vector3 l2, float turns, int points);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif
