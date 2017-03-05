Klein is a mesh viewer as well as a coding framework to help visualize and test your geometry processing algorithms.

# Dependencies
- Qt 5.8
- CGAL 4.9
- Eigen 3.3
- Python 3.5
- [euclid](https://github.com/unclejimbo/euclid)
- C++14 compatable compiler

# Installation
Please change the include and lib path in "build/Klein.pro".

Use Qt Creator to open this project file and build, or type

```
qmake -tp vc Klein.pro
```

to generate a msvc solution file on windows.

# Using Klein

## Open Klein
![](https://github.com/unclejimbo/Klein/raw/master/contents/images/Klein.PNG)

## File formats supported
.obj, .off

## Camera control
- Hold middle mouse to rotate.
- Hold shift + middle mouse to pan.
- Scroll wheel to zoom.
- Hold ctrl/alt and scroll to pan horizontally/vertically.

# Extending Klein
The widget concerning your algorithm should derive from class ProcessWidget. PropertyWidget serves as an example to write your own widget and use the facilities provided by Klein to visualize your algorithm.
