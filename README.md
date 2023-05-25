# LUTViewer

## Description

Simple OpenGL Application to visualize LUTs (Look Up Tables) in 3D.

## Dependencies

- [GLFW](https://www.glfw.org/)
- [OpenGL](https://www.opengl.org/)
- [GLM](https://glm.g-truc.net/0.9.9/index.html)

I'd recommend using a package manager like [Homebrew](https://brew.sh/) to install the dependencies.

```
brew install glfw
brew install glm
brew install opengl
```

## Installation

```
git clone <this-repo>
cd LUTViewer
```

## Build

```
mkdir build
cd build
cmake ..
make
```

## Usage

```
./LUTViewer <path-to-data-dir>
```

## Sample Data

You can find some sample data in the `data` directory. By default, the application will load this data if no path is
specified.


