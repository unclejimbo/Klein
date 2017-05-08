#pragma once

#include "Core/Scene.h"
#include "Core/Common.h"
#include "Core/GeomIO.h"
#include "Editor/GLWidget.h"
#include "Editor/ProcessPanel.h"

#include <QMainWindow>
#include <QtWidgets>
#include <memory>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = 0);
	~MainWindow();

	void initializeScene();

Q_SIGNALS:
	void geomImported(GeomInfo*);
	void sceneInitialized(Scene*);

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
	void _pickNothing();
	void _pickVertex();
	void _pickLine();
	void _pickFace();
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
	QAction* _aPickNothing;
	QAction* _aPickVertex;
	QAction* _aPickLine;
	QAction* _aPickFace;
	QAction* _aShaded;
	QAction* _aWireframe;
	QAction* _aHiddenline;
	QAction* _aUnlit;

	Scene _scene;
	GeomInfo _geomInfo;
	QString _lastOpenFile = KLEIN_IMPORT_PATH;
};
