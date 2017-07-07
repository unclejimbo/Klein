#include "Core/Common.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"
#include "Core/Scene.h"
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

	Scene scene;
	scene.setCamera(QVector3D(2.0f, 2.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f),
		QVector3D(0.0f, 1.0f, 0.0f), 45.0f);
	scene.setLight(0, QVector3D(3.0f, 3.0f, 3.0f), QVector3D(300.0f, 300.0f, 300.0f));
	scene.setLight(1, QVector3D(-3.0f, -3.0f, -3.0f), QVector3D(300.0f, 300.0f, 300.0f));

	MainWindow mainWindow(&scene);
	mainWindow.move(200, 100);
	mainWindow.show();

	//Py_Initialize();
	//PyRun_SimpleString("import sys");
	//PyRun_SimpleString("sys.path.append(\"../contents/scripts\")");

	app.exec();

	//Py_Finalize();
	//Logger::cleanup();
	return 0;
}
