TEMPLATE = app
CONFIG += c++14 console no_keywords
QT += core gui widgets

CGAL_INCLUDE = E:/_dependencies/include
EIGEN_INCLUDE = E:/_dependencies/include
PYTHON_INCLUDE = E:/Anaconda3/include
EUCLID_INCLUDE = ../../euclid/include

INCLUDEPATH += $$CGAL_INCLUDE $$EIGEN_INCLUDE $$PYTHON_INCLUDE $$EUCLID_INCLUDE

PYTHON_LIB = E:/_dependencies/lib/python35.lib
CGAL_RELEASE = E:/_dependencies/lib/CGAL-vc140-mt-4.9.lib
CGAL_CORE_RELEASE = E:/_dependencies/lib/CGAL_Core-vc140-mt-4.9.lib
CGAL_DEBUG = E:/_dependencies/lib/CGAL-vc140-mt-gd-4.9.lib
CGAL_CORE_DEBUG = E:/_dependencies/lib/CGAL_Core-vc140-mt-gd-4.9.lib

LIBS += $$PYTHON_LIB
CONFIG(debug, debug|release) {
	LIBS += $$CGAL_DEBUG $$CGAL_CORE_DEBUG
	DESTDIR = ../bin/debug
	TARGET = Klein_d
}
else {
	LIBS += $$CGAL_RELEASE $$CGAL_CORE_RELEASE
	DESTDIR = ../bin/release
	TARGET = Klein
}

HEADERS += ../src/Common.h \
	../src/MainWindow.h \
	../src/GLWidget.h \
	../src/ProcessPanel.h \
	../src/ProcessWidget.h \
	../src/PropertyWidget.h \
	../src/ResourceManager.h \
	../src/Mesh.h \
	../src/Material.h \
	../src/Light.h \
	../src/MeshIO.h \
	../src/OBJMeshIO.h \
	../src/OFFMeshIO.h \
	../src/Camera.h \
	../src/CameraController.h \
	../src/ArcballController.h \
	../src/Scene.h \
	../src/SceneNode.h \
	../src/RenderMeshNode.h \
	../src/VertexColorMeshNode.h \
	../src/PrimitiveNode.h \
	../src/Logger.h \
	../src/Util.h \
	../src/Color.h

SOURCES += ../src/Main.cpp \
	../src/MainWindow.cpp \
	../src/GLWidget.cpp \
	../src/ProcessPanel.cpp \
	../src/ProcessWidget.cpp \
	../src/PropertyWidget.cpp \
	../src/ResourceManager.cpp \
	../src/Mesh.cpp \
	../src/MeshIO.cpp \
	../src/OBJMeshIO.cpp \
	../src/OFFMeshIO.cpp \
	../src/Camera.cpp \
	../src/CameraController.cpp \
	../src/ArcballController.cpp \
	../src/Scene.cpp \
	../src/SceneNode.cpp \
	../src/RenderMeshNode.cpp \
	../src/VertexColorMeshNode.cpp \
	../src/PrimitiveNode.cpp \
	../src/Logger.cpp \
	../src/Color.cpp

SHADERS += ../contents/shaders/Mesh.vert \
	../contents/shaders/ColorMesh.frag \
	../contents/shaders/CookTorrance.frag \
	../contents/shaders/VertexColorMesh.vert \
	../contents/shaders/VertexColorMesh.frag \
	../contents/shaders/Primitive.vert \
	../contents/shaders/Primitive.frag \
	../contents/shaders/VertexColorPrimitive.vert \
	../contents/shaders/VertexColorPrimitive.frag

SCRIPTS += ../contents/scripts/cluster.py

RESOURCES += $$SHADERS $$SCRIPTS
