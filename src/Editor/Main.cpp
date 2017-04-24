#include "Core/Common.h"
#include "Core/Logger.h"
#include "Core/OBJMeshIO.h"
#include "Core/OFFMeshIO.h"
#include "Core/ResourceManager.h"
#include "Editor/MainWindow.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QApplication>
#include <QColor>
#include <QString>
#include <QSurfaceFormat>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#ifdef _WIN32
#pragma warning(pop)
#endif

void initializeResourceSystem()
{
	// Load default contents
	OFFMeshIO offIO;
	OBJMeshIO objIO;
	offIO.readMesh(QString(KLEIN_MESH_PATH).append("Cube.off"), "KLEIN_Cube", false, nullptr);
	objIO.readMesh(QString(KLEIN_MESH_PATH).append("UVSphere.obj"), "KLEIN_UVSphere", false, nullptr);
		
	auto color = QColor(255, 0, 0);
	ResourceManager::instance().addMaterial("KLEIN_Red",
		QVector3D(static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF())),
		0.8f, 0.4f, 10.0f);

	ResourceManager::instance().addShaderProgram("KLEIN_ColorMesh",
		QString(KLEIN_SHADER_PATH).append("Mesh.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("ColorMesh.frag").toStdString());
	ResourceManager::instance().addShaderProgram("KLEIN_CookTorrance",
		QString(KLEIN_SHADER_PATH).append("Mesh.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("CookTorrance.frag").toStdString());
	ResourceManager::instance().addShaderProgram("KLEIN_VertexColorMesh",
		QString(KLEIN_SHADER_PATH).append("VertexColorMesh.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("VertexColorMesh.frag").toStdString());
	ResourceManager::instance().addShaderProgram("KLEIN_Primitive",
		QString(KLEIN_SHADER_PATH).append("Primitive.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("Primitive.frag").toStdString());
	ResourceManager::instance().addShaderProgram("KLEIN_VertexColorPrimitive",
		QString(KLEIN_SHADER_PATH).append("VertexColorPrimitive.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("VertexColorPrimitive.frag").toStdString());
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setVersion(4, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setOption(QSurfaceFormat::DebugContext);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setSamples(4);
	QSurfaceFormat::setDefaultFormat(format);

	Logger::initialize();
	qInstallMessageHandler(&Logger::log);

	auto version = QString("Klein %1.%2.%3").arg(KLEIN_VERSION_MAJOR).
		arg(KLEIN_VERSION_MINOR).arg(KLEIN_VERSION_PATCH);
	KLEIN_LOG_INFO(version);

	MainWindow mainWindow;
	mainWindow.show();
	initializeResourceSystem();
	mainWindow.initializeScene();

	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append(\"../contents/scripts\")");

	app.exec();

	Py_Finalize();
	Logger::cleanup();
	return 0;
}
