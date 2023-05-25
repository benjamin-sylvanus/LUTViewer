//
// Created by Benjamin Sylvanus on 5/25/23.
//
#include "GLMethods.h"

namespace GLM {
///////////////////////////////////////////////////////////////////////////////
// GLOBALS
///////////////////////////////////////////////////////////////////////////////
    void *font = GLUT_BITMAP_8_BY_13;
    bool mouseLeftDown;
    bool mouseRightDown;
    bool showBoxes;
    float mouseX, mouseY;
    float cameraAngleX;
    float cameraAngleY;
    float cameraDistance;
    int screenWidth, screenHeight;
    int drawMode;
    int currIndex;
    bool animating;
    std::vector<Vector3> path_test;
    std::vector<Vector3> gstart;
    std::vector<Vector3> gend;
    std::vector<std::vector<Vector3> > path_test_v;
    std::vector<Vector3> colors;
    std::vector<float> Radii;
    Vector3 center;
    std::vector<Vector3> cubes;
    std::vector<std::vector<float> > data;
    std::vector<std::vector<float> > swcdata;
    std::vector<std::vector<uint64_t> > dims;
    std::vector<std::uint64_t> lut;
    std::vector<std::vector<std::vector<int> > > lutdata;
    std::vector<int> lutindex;

///////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////
    void initGL() {
        glShadeModel(GL_FLAT);                    // shading mathod: GL_SMOOTH or GL_FLAT
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

        // enable /disable features
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

        glClearColor(0, 0, 0, 0);                   // background color
        glClearStencil(0);                          // clear stencil buffer
        glClearDepth(1.0f);                         // 0 is near, 1 is far
        glDepthFunc(GL_LEQUAL);

        initLights();
        setCamera(0, 0, 0, center.x, center.y, center.z);
    }

    int initGLUT(int argc, char **argv) {
        // GLUT stuff for windowing
        // initialization openGL window.
        // It must be called before any other GLUT routine.
        glutInit(&argc, argv);

        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_ALPHA);   // display mode

        glutInitWindowSize(screenWidth, screenHeight);              // window size

        glutInitWindowPosition(100, 100);                           // window location

        // finally, create a window with openGL context
        // Window will not displayed until glutMainLoop() is called
        // It returns a unique ID.
        int handle = glutCreateWindow(argv[0]);     // param is the title of window

        // register GLUT callback functions
        glutDisplayFunc(displayCB);
        glutTimerFunc(100, timerCB, 100);             // redraw only every given millisec
        //glutIdleFunc(idleCB);                       // redraw whenever system is idle
        glutReshapeFunc(reshapeCB);
        glutKeyboardFunc(keyboardCB);
        glutMouseFunc(mouseCB);
        glutMotionFunc(mouseMotionCB);
        glutPassiveMotionFunc(mousePassiveMotionCB);

        return handle;
    }

    bool initSharedMem() {
        screenHeight = SCREEN_HEIGHT;
        screenWidth = SCREEN_WIDTH;
        mouseLeftDown = mouseRightDown = false;
        showBoxes = true;
        cameraAngleX = cameraAngleY = 0;
        cameraDistance = 10;
        drawMode = 0;
        for (int i = 0; i < gstart.size(); i++) {
            std::vector<Vector3> tpath = buildSpiralPathModified(0.01f, 0.01f, gstart[i], gend[i], 1, 10);
            path_test_v.push_back(tpath);
        }
        return true;
    }

    void clearSharedMem() {

    }

    void initLights() {
        // set up light colors (ambient, diffuse, specular)
        GLfloat lightKa[] = {0.5f, 0.5f, 0.5f, 1.0f};  // ambient light
        GLfloat lightKd[] = {0.1f, 1.0f, 1.0f, 0.1f};  // diffuse light
        GLfloat lightKs[] = {0.5f, 0.5f, 0.5f, 0.1f};           // specular light
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

        // position the light
        float lightPos[4] = {1, 0, 1, 0}; // positional light
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

        glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration

//    GL_LIGHT1
        // set up light colors (ambient, diffuse, specular)
        GLfloat lightKa1[] = {0.65f, 0.6f, 0.6f, 0.8f};  // ambient light
        GLfloat lightKd1[] = {0.65f, 0.65f, 0.65f, 1};  // diffuse light
        GLfloat lightKs1[] = {0.5f, 0.5f, 0.5f, 0.01f};           // specular light
        glLightfv(GL_LIGHT1, GL_AMBIENT, lightKa1);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, lightKd1);
        glLightfv(GL_LIGHT1, GL_SPECULAR, lightKs1);

        // position the light
        float lightPos1[4] = {0, 1, 0, 1}; // positional light
        glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

        glEnable(GL_LIGHT1);
    }

    void drawString(const char *str, int x, int y, float color[4], void *pVoid) {
        glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
        glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
        glDisable(GL_TEXTURE_2D);

        glColor4fv(color);          // set text color
        glRasterPos2i(x, y);        // place text position

        // loop all characters in the string
        while (*str) {
            glutBitmapCharacter(pVoid, *str);
            ++str;
        }

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glPopAttrib();
    }

    void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
    }

    void showInfo() {
        // backup current model-view matrix
        glPushMatrix();                     // save current modelview matrix
        glLoadIdentity();                   // reset modelview matrix

        // set to 2D orthogonal projection
        glMatrixMode(GL_PROJECTION);        // switch to projection matrix
        glPushMatrix();                     // save current projection matrix
        glLoadIdentity();                   // reset projection matrix
        gluOrtho2D(0, screenWidth, 0, screenHeight);  // set to orthogonal projection

        const int FONT_HEIGHT = 14;
        float color[4] = {1, 1, 1, 1};

        // for print infos
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << "Press D to change drawing mode." << std::ends;
        drawString(ss.str().c_str(), 1, 1, color, font);
        ss.str("");

        ss << "Press B to toggle Lookup Table." << std::ends;
        drawString(ss.str().c_str(), 1, 1 + FONT_HEIGHT, color, font);
        ss.str("");

        // unset floating format
        ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

        // restore projection matrix
        glPopMatrix();                   // restore to previous projection matrix

        // restore modelview matrix
        glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
        glPopMatrix();                   // restore to previous modelview matrix
    }

    void drawSphere() {
        // set material
        float ambient[] = {0.5f, 0.5f, 0.5f, 1};
        float diffuse[] = {0.7f, 0.7f, 0.7f, 1};
        float specular[] = {1.0f, 1.0f, 1.0f, 1};
        float shininess = 128;
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
        float lineColor[] = {0.2f, 0.2f, 0.2f, 1};
        // draw spheres at each point
        for (int k = 0; k < path_test_v.size(); k++) {
            glDisable(GL_LIGHTING);
            Vector3 color = colors[k];
            // draw first point
            Vector3 path = path_test_v[k][0];
            glColor4f(color.x, color.y, color.z, 0.5f);
            glPushMatrix();
            glTranslatef(path.x, path.y, path.z);
            glutSolidSphere(Radii.at(k), 7, 7); // draw sphere at each point ()
            glPopMatrix();
            glEnable(GL_LIGHTING);
        }
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse); // reset diffuse
    }

    void drawCube() {
        // set material
        float ambient[] = {0.5f, 0.5f, 0.5f, 1};
        float diffuse[] = {0.7f, 0.7f, 0.7f, 1};
        float specular[] = {1.0f, 1.0f, 1.0f, 1};
        float shininess = 128;
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
        float lineColor[] = {0.2f, 0.2f, 0.2f, 1};
        // draw cubes for each cube point
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        for (int k = 0; k < cubes.size(); k++) {

            // draw first point
            Vector3 cube = cubes[k];
            glColor4f(0.2f, 0, 0.3f, 0.1f);
            glPushMatrix();
            glTranslatef(cube.x, cube.y, cube.z);
            glutSolidCube(1); // draw cube at each point ()
            glPopMatrix();

        }
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);

        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse); // reset diffuse
    }

    void drawPath() {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        for (int k = 0; k < path_test_v.size(); k++) {

            // lines
            Vector3 color = colors[k];
            glColor3f(color.x, color.y, color.z);
            glLineWidth(1.0f);
            glBegin(GL_LINES);
            int count = path_test_v[k].size();
            for (int i = 0; i < count - 1; ++i) {
                glVertex3fv(&path_test_v[k].at(i).x);
                glVertex3fv(&path_test_v[k].at(i + 1).x);
            }
            glEnd();

            // points
            glColor4f(color.x, color.y, color.z, 0.5f);
            glPointSize(1.0f);
            glBegin(GL_POINTS);
            for (int i = 0; i < count; ++i) {
                glVertex3fv(&path_test_v[k].at(i).x);
            }
            glEnd();

        }
        glPointSize(1); // reset

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }

    void draw() {
        drawPath();
        drawSphere();
        if (showBoxes)
            drawCube();
    }

    std::vector<Vector3> buildSpiralPathModified(float r1, float r2, Vector3 l1, Vector3 l2, float turns, int points) {
        std::vector<Vector3> vertices;
        // loop over all points

        // define a line from point A to point B
        Vector3 p1 = l1;
        Vector3 p2 = l2;

        float x1 = p1.x;
        float y1 = p1.y;
        float z1 = p1.z;

        float x2 = p2.x;
        float y2 = p2.y;
        float z2 = p2.z;
        // define step size xyz2-xyz1 / points -1
        float xStep = (x2 - x1) / (points - 1);
        float yStep = (y2 - y1) / (points - 1);
        float zStep = (z2 - z1) / (points - 1);

        // define a point on the line
        Vector3 vertex;
        for (int j = 0; j < points; ++j) {
            vertex.x = x1 + xStep * j;
            vertex.y = y1 + yStep * j;
            vertex.z = z1 + zStep * j;
            // add point to vertices
            vertices.push_back(vertex);
        }
        return vertices;
    }
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////
    void displayCB() {
        // clear framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glPushMatrix();

        // tramsform camera
        glTranslatef(0, 0, -cameraDistance);
        glRotatef(cameraAngleX, 1, 0, 0);   // pitch
        glRotatef(cameraAngleY, 0, 1, 0);   // heading

        // draw 3D
        draw();

        showInfo();

        glPopMatrix();
        glutSwapBuffers();
    }

    void reshapeCB(int width, int height) {
        screenWidth = width;
        screenHeight = height;

        // set viewport to be the entire window
        glViewport(0, 0, (GLsizei) width, (GLsizei) height);

        // set perspective viewing frustum
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0f, (float) (width) / height, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

        // switch to modelview matrix in order to set scene
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void timerCB(int millisec) {
        glutTimerFunc(millisec, timerCB, millisec);
        glutPostRedisplay();
    }

    void keyboardCB(unsigned char key, int x, int y) {
        switch (key) {
            case 27: // ESCAPE
                exit(0);
                break;
            case 'd': // switch rendering modes (fill -> wire -> point)
            case 'D':
                drawMode = ++drawMode % 3;
                if (drawMode == 0)        // fill mode
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glEnable(GL_DEPTH_TEST);
                    glEnable(GL_CULL_FACE);
                } else if (drawMode == 1)  // wireframe mode
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_CULL_FACE);
                } else                    // point mode
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_CULL_FACE);
                }
                break;

            case 'b': // toggle lookup table
            case 'B':
                showBoxes = !showBoxes;
                break;
            default:;
        }

        glutPostRedisplay();
    }

    void mouseCB(int button, int state, int x, int y) {
        mouseX = x;
        mouseY = y;

        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN) {
                mouseLeftDown = true;
            } else if (state == GLUT_UP) {
                mouseLeftDown = false;
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            if (state == GLUT_DOWN) {
                mouseRightDown = true;
            } else if (state == GLUT_UP) {
                mouseRightDown = false;
            }
        }
    }

    void mouseMotionCB(int x, int y) {
        if (mouseLeftDown) {
            cameraAngleY += (x - mouseX);
            cameraAngleX += (y - mouseY);
            mouseX = x;
            mouseY = y;
        }
        if (mouseRightDown) {
            cameraDistance -= (y - mouseY) * 0.2f;
            mouseY = y;
        }

    }

    void mousePassiveMotionCB(int x, int y) {
        mouseX = x;
        mouseY = y;
    }

    void exitCB() {
        clearSharedMem();
    }

}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

