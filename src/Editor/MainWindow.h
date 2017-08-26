#pragma once

#include "Core/Common.h"
#include "Core/GeomIO.h"
#include "Core/Message.h"

#include <QMainWindow>
#include <QtWidgets>

class Scene;
class GLWidget;
class ProcessPanel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(Scene* scene, QWidget* parent = 0);

Q_SIGNALS:
	void geomImported(const GeomInfo& info);
	void clearAll();

private:
	void _createCentralWidget();
	void _createDockWidgets();
	void _createActions();
	void _createMenuBar();
	void _createToolBar();
	void _createStatusBar();
	void _changeTitle(const QString& title);
	void _updateStatusLabel(size_t nVertices, size_t nFaces);
	void _createMeshNode(const Geometry& geometry,
		unsigned& pointBuffer, unsigned& normalBuffer);
	void _createPointCloudNode(const Geometry& geometry,
		unsigned& pointBuffer);
	unsigned _createMesh(const Geometry& geometry,
		unsigned pointBuffer, unsigned normalBuffer);
	unsigned _createPointCloud(const Geometry& geometry, unsigned pointBuffer);

private Q_SLOTS:
	void _importMesh();
	void _importPointCloud();
	void _clearAll();
	void _screenShot();
	void _shaded();
	void _wireframe();
	void _hiddenline();
	void _primitive();
	void _solid();
	void _backgroundWall();
	void _unlit();
	void _shadow();
	void _advancedRender();

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
	QAction* _aPrimitive;
	QAction* _aSolid;
	QAction* _aBackgroundWall;
	QAction* _aUnlit;
	QAction* _aShadow;
	QAction* _aAdvancedRender;

	Scene* _scene;
	QString _lastOpenFile = KLEIN_IMPORT_PATH;
};
