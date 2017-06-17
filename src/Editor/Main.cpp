#include "Core/Common.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"
#include "Editor/MainWindow.h"

#include <QApplication>
#include <QColor>
#include <QString>
#include <QSurfaceFormat>

//#ifdef _DEBUG
//#undef _DEBUG
//#include <Python.h>
//#define _DEBUG
//#else
//#include <Python.h>
//#endif

void initializeResourceSystem()
{
	// Load default contents
	ResourceManager::instance().addPBRMaterial("KLEIN_PBR_Default",
		QVector3D(1.0f, 1.0f, 1.0f), 0.8f, 0.4f, 10.0f);
	ResourceManager::instance().addPBRMaterial("KLEIN_PBR_Red",
		QVector3D(1.0f, 0.0f, 0.0f), 0.8f, 0.4f, 10.0f);

	ResourceManager::instance().addShaderProgram("KLEIN_CookTorrance",
		QString(KLEIN_SHADER_PATH).append("Mesh.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("CookTorrance.frag").toStdString());
	ResourceManager::instance().addShaderProgram("KLEIN_CookTorrance_VColor",
		QString(KLEIN_SHADER_PATH).append("MeshVColor.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("CookTorranceVColor.frag").toStdString());
	ResourceManager::instance().addShaderProgram("KLEIN_Unlit",
		QString(KLEIN_SHADER_PATH).append("Unlit.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("Unlit.frag").toStdString());
	ResourceManager::instance().addShaderProgram("KLEIN_Unlit_VColor",
		QString(KLEIN_SHADER_PATH).append("UnlitVColor.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("UnlitVColor.frag").toStdString());
	ResourceManager::instance().addShaderProgram("KLEIN_Picking",
		QString(KLEIN_SHADER_PATH).append("Unlit.vert").toStdString(),
		QString(KLEIN_SHADER_PATH).append("Picking.frag").toStdString());
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

	//Py_Initialize();
	//PyRun_SimpleString("import sys");
	//PyRun_SimpleString("sys.path.append(\"../contents/scripts\")");

	app.exec();

	//Py_Finalize();
	//Logger::cleanup();
	return 0;
}
