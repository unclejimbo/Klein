Klein is a mesh viewer as well as a coding framework to help visualize and test your geometry processing algorithms.

# Dependencies
- Qt 5.9
- CGAL 4.10
- Eigen 3.3
- OpenCV 3.2
- [Euclid](https://github.com/unclejimbo/Euclid)
- C++17 compatable compiler

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
The widget concerning your algorithm should derive from class ProcessWidget. PropertyWidget serves as an example to write your own widget and use the facilities provided by Klein to visualize your algorithm.
