[Work in Progress]
Klein is a mesh viewer as well as a coding framework to help visualize and test your geometry processing algorithms.

# Introduction
The purpose of Klein is to ease the process to develop 3D geometry processing algorithms, which often involves lots of visualization, user interaction, resource management and data conversion. Although those parts are usually not part of the algortihm, but it's often impossible to get it right when you can't see the results directly. Thus Klein serves as a starting point to make life easier when developing 3D geometry algorithms.

# What does Klein provide?
- Basic mesh viewer
- Different mesh and point set data structures
- A bunch of visualization methods to draw meshes and primitives
- Primitive picking for user interaction
- Automatic resource management

# Dependencies
- Qt 5.9
- CGAL 4.10
- Eigen 3.3
- OpenCV 3.2
- [Euclid](https://github.com/unclejimbo/Euclid)
- C++17 compatible compiler

# Installation
Use CMake to build.

# Using Klein

## Open Klein
![](https://github.com/unclejimbo/Klein/raw/master/contents/images/Klein.PNG)

## File formats supported
.obj, .off, .ply, .xyz

## Camera control
- Hold middle mouse to rotate.
- Hold shift + middle mouse to pan.
- Scroll wheel to zoom.
- Hold ctrl/alt and scroll to pan horizontally/vertically.

# Extending Klein
When you open Klein you'll see a PropertyWidget on the right side of the window. This serves as a basic example to extend Klein when developing your own algorithm. The basic idea is to put all the bits and pieces related to your algorithm, such as visualization, interactions, results, etc, into a class derived from ProcessWidget. This class provides an interface which all widgets should obey in order for it to integrate into Klein and handle resources correctly. After that you should use the addWidget method from ProcessPanel class to add your widget into Klein. Then hit compile and you're up running.

There're more widgets I use to test some algorithms in Euclid which lies in the widgets folder. You could try to compile with those widgets and see the results.

In the future I might write a plugin framework to extend Klein more easily without recompiling the core program.
