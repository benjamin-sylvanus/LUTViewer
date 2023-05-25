///////////////////////////////////////////////////////////////////////////////
// main.cpp
//
//  AUTHOR: Benjamin Sylvanus
// CREATED: 2023-05-20
///////////////////////////////////////////////////////////////////////////////

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
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
#include "GLMethods.h"

using namespace GLM;

std::vector<std::vector<float> > loadSWC(std::string basicString);

std::vector<std::vector<uint64_t> > readdims(std::string path);

int s2i(std::vector<int> v, std::vector<int> dims);

void preprocessData();

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
    // read an swc file and create a pipe from it
    /// read swc file
    std::string filename;

    // if no filename is given, use the default
    if (argc < 2) {
        std::cout << "No filename given, using default" << std::endl;
        std::cout << "Usage: ./swcviewer <filename>" << std::endl;

        filename = "/Users/benjaminsylvanus/Documents/work/LUTViewer/src/data/cell.swc";
    } else {
        filename = argv[1];
    }

    // read the data
    data = loadSWC(filename);

    // load the lookup table using the datatemplate class
    datatemplate<std::uint64_t> lutreader("/Users/benjaminsylvanus/Documents/work/LUTViewer/src/data/lut.bin");
    lut = lutreader.data;

    // load the index table using the datatemplate class
    datatemplate<int> indexreader("/Users/benjaminsylvanus/Documents/work/LUTViewer/src/data/index.bin");
    lutindex = indexreader.data;

    dims = readdims("/Users/benjaminsylvanus/Documents/work/LUTViewer/src/data/dims.txt");

    // print some info
    std::cout << "LUT size: " << lut.size() << std::endl;
    std::cout << "Index size: " << lutindex.size() << std::endl;
    std::cout << "Dims size: " << dims.size() << std::endl;

    // Preprocess the data
    preprocessData();


    // init shared memory
    initSharedMem();

    // register exit callback
    atexit(exitCB);

    // init GLUT and GL
    initGLUT(argc, argv);
    initGL();

    glutMainLoop(); /* Start GLUT event-processing loop */

    return 0;
}

std::vector<std::vector<float> > loadSWC(std::string basicString) {
    std::ifstream file(basicString);
    std::string str;
    std::vector<std::vector<float> > swc;
    while (std::getline(file, str)) {
        std::istringstream iss(str);
        if (str.empty())
            continue;
        else if (str[0] == '#' || str[0] == ' ' || str[0] == '\n') {
            continue;
        } else {
            std::vector<float> node;
            std::vector<int> edge;
            float x, y, z, radius, parent, id;
            if (!(iss >> id >> x >> y >> z >> radius >> parent)) {
                break;
            } // error
            node.push_back(id);
            node.push_back(x);
            node.push_back(y);
            node.push_back(z);
            node.push_back(radius);
            node.push_back(parent);
            swc.push_back(node);
        }
    }
    return swc;
}

std::vector<std::vector<std::uint64_t> > readdims(std::string path) {
    std::cout << "Path: " << path << std::endl;
    std::fstream newfile;
    newfile.open(path, std::ios::in); //open a file to perform read operation using file object
    std::vector<std::vector<std::uint64_t> > v;
    if (newfile.is_open()) {   //checking whether the file is open
        std::string tp;
        while (getline(newfile, tp)) {
            std::vector<std::uint64_t> temp;
            size_t pos = 0;
            std::string token;
            std::string delimiter = "\t";
            while ((pos = tp.find(delimiter)) != std::string::npos) {
                token = tp.substr(0, pos);
                temp.push_back(std::stoi(token));
                tp.erase(0, pos + delimiter.length());
            }
            v.push_back(temp);
        }
        newfile.close(); //close the file object.
    }
    return v;
}

int s2i(std::vector<int> v, std::vector<int> d) {
    int bx = d[0];
    int by = d[1];
    int bz = d[2];
    int ix = v[0];
    int iy = v[1];
    int iz = v[2];

    return 0 + bx * (by * iz + iy) + ix;
}

void preprocessData() {
// extract the dimensions of the swc lut and index from the dims
    int SWCDIMS[2] = {static_cast<int>(dims[0][0]), static_cast<int>(dims[0][1])};
    int LUTDIMS[3] = {static_cast<int>(dims[1][0]), static_cast<int>(dims[1][1]), static_cast<int>(dims[1][2])};
    int INDDIMS[3] = {static_cast<int>(dims[2][0]), static_cast<int>(dims[2][1]), static_cast<int>(dims[2][2])};

    std::cout << "SWC dims: " << SWCDIMS[0] << " " << SWCDIMS[1] << std::endl;
    std::cout << "LUT dims: " << LUTDIMS[0] << " " << LUTDIMS[1] << " " << LUTDIMS[2] << std::endl;
    std::cout << "Index dims: " << INDDIMS[0] << " " << INDDIMS[1] << " " << INDDIMS[2] << std::endl;

    // set the dims of the lut
    lutdata.resize(LUTDIMS[0], std::vector<std::vector<int> >(LUTDIMS[1], std::vector<int>(LUTDIMS[2], 0)));

    std::vector<int> localdims = {LUTDIMS[0], LUTDIMS[1], LUTDIMS[2]};

    // lut has the form [y,x,z]
    for (int i = 0; i < LUTDIMS[0]; i++) {
        for (int j = 0; j < LUTDIMS[1]; j++) {
            for (int k = 0; k < LUTDIMS[2]; k++) {
                // calculate the index of the lut
                std::vector<int> subscript = {i, j, k};
                int ind = s2i(subscript, localdims);
                lutdata[i][j][k] = lut[ind];
            }
        }
    }

    // create a vector of cubes to draw
    for (int i = 0; i < LUTDIMS[0]; i++) {
        for (int j = 0; j < LUTDIMS[1]; j++) {
            for (int k = 0; k < LUTDIMS[2]; k++) {
                if (lutdata[i][j][k] > 0) {
                    cubes.push_back(Vector3((float) i, (float) j, (float) k));
                }
            }
        }
    }

    // set the first connections parent to 1
    data[0][5] = 1;

    // add each connection to the global variables gstart and gend
    for (int i = 0; i < data.size(); i++) {
        Radii.push_back(data[i][4]);
        // get the curr x y z r id parent
        float cx = data[i][1];
        float cy = data[i][2];
        float cz = data[i][3];
        float cr = data[i][4];
        int cid = (int) data[i][0];
        int cparent = (int) data[i][5] - 1;

        // get the parent x y z r
        float px = data[cparent][1];
        float py = data[cparent][2];
        float pz = data[cparent][3];
        float pr = data[cparent][4];

        // create the start and end vectors
        Vector3 start(cx, cy, cz);
        Vector3 end(px, py, pz);

        // add the start and end to the global variables
        gstart.push_back(start);
        gend.push_back(end);

        // normalize the radius
        float r = cr;
        float g = 0;
        float b = (1.0 - r) / 1.5;

        Vector3 color(r, g, b);
        colors.push_back(color);
    }

    // calculate the center of the data
    float cx = 0;
    float cy = 0;
    float cz = 0;
    for (int i = 0; i < gstart.size(); i++) {
        cx += gstart[i].x;
        cy += gstart[i].y;
        cz += gstart[i].z;
    }
    cx /= gstart.size();
    cy /= gstart.size();
    cz /= gstart.size();
    center = Vector3(cx, cy, cz);
    std::cout << "center: " << center.x << " " << center.y << " " << center.z << std::endl;

    // translate the data to the center
    for (int i = 0; i < gstart.size(); i++) {
        gstart[i].x -= center.x;
        gstart[i].y -= center.y;
        gstart[i].z -= center.z;
        gend[i].x -= center.x;
        gend[i].y -= center.y;
        gend[i].z -= center.z;
    }

    // translate the cubes to the center
    for (int i = 0; i < cubes.size(); i++) {
        cubes[i].x -= center.x;
        cubes[i].y -= center.y;
        cubes[i].z -= center.z;
    }


}

#pragma clang diagnostic pop