#pragma once

#include "Core/Common.h"
#include "Core/MeshIO.h"
#include "Editor/GLWidget.h"
#include "Editor/ProcessPanel.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QMainWindow>
#include <QtWidgets>
#include <memory>

#ifdef _WIN32
#pragma warning(pop)
#endif

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
