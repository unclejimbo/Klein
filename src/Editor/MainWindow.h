#pragma once

#include "Core/Common.h"
#include "Core/MeshIO.h"
#include "Editor/GLWidget.h"
#include "Editor/ProcessPanel.h"

#include <QMainWindow>
#include <QtWidgets>
#include <memory>

class Scene;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = 0);
	~MainWindow();

	void initializeScene();

Q_SIGNALS:
	void meshImported(MeshInfo*);
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
	void _clearAll();
	void _screenShot();
	void _shaded();
	void _wireframe();
	void _hiddenline();

private:
	GLWidget* _glWidget;
	ProcessPanel* _processPanel;
	QMenuBar* _menuBar;
	QStatusBar* _statusBar;
	QLabel* _statusLabel;

	QAction* _aQuit;
	QAction* _aOpen;
	QAction* _aClear;
	QAction* _aScreenShot;
	QAction* _aShaded;
	QAction* _aWireframe;
	QAction* _aHiddenline;

	std::unique_ptr<Scene> _scene;
	MeshInfo _meshInfo;
	QString _lastOpenFile = KLEIN_IMPORT_PATH;
};
