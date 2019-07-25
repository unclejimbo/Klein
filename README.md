# Klein

Klein provides various rendering techniques and utilities based on Qt3D, including:

- Physically based material
- Percentage-closer soft shadows
- Solid wireframe rendering
- Weighted-blended order independt transparency
- Instancing
- Dear ImGui integration
- Per-vertex color rendering
- Trackball camera controller
- And more...

![](https://i.loli.net/2019/07/25/5d392fc43fb0a93634.jpg)

## Dependencies

- Qt5(>=5.12.0)

## Getting started

This project uses CMake.

```
mkdir build
cd build
cmake ..
make/msbuild
```

Build examples by adding `-DBUILD_EXAMPLES=ON`.

Use Klein in your own CMake project by

```
target_link_libraries(your-target Klein::Klein)
```

The `examples\BasicWindow` folder is a good starting point to get to play with Klein.
