#pragma once

#include "Core/Common.h"
#include "Core/GeomIO.h"

#include <QMainWindow>
#include <QtWidgets>
#include <memory>

class Scene;
class GLWidget;
class ProcessPanel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(Scene* scene, QWidget* parent = 0);

Q_SIGNALS:
	void geomImported(GeomInfo*);

private:
	void _createCentralWidget();
	void _createDockWidgets();
	void _createActions();
	void _createMenuBar();
	void _createToolBar();
	void _createStatusBar();
	void _changeTitle(const QString& title);
	void _updateStatusLabel(size_t nVertices, size_t nFaces);

private Q_SLOTS:
	void _importMesh();
	void _importPointCloud();
	void _clearAll();
	void _screenShot();
	void _shaded();
	void _wireframe();
	void _hiddenline();
	void _unlit();

private:
	GLWidget* _glWidget;
	ProcessPanel* _processPanel;
	QMenuBar* _menuBar;
	QStatusBar* _statusBar;
	QLabel* _statusLabel;

	QAction* _aQuit;
	QAction* _aImportMesh;
	QAction* _aImportPointCloud;
	QAction* _aClear;
	QAction* _aScreenShot;
	QAction* _aShaded;
	QAction* _aWireframe;
	QAction* _aHiddenline;
	QAction* _aUnlit;

	Scene* _scene;
	GeomInfo _geomInfo;
	QString _lastOpenFile = KLEIN_IMPORT_PATH;
};
