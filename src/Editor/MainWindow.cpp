#include "Editor/MainWindow.h"
#include "Core/Logger.h"
#include "Core/OBJMeshIO.h"
#include "Core/OFFMeshIO.h"
#include "Core/ResourceManager.h"
#include "Core/Scene.h"
#include "Core/SceneNode.h"
#include "Core/PBRMeshGraphics.h"

#include <QMatrix4x4>
#include <memory>
#include <fstream>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	this->setWindowTitle(KLEIN_TITLE);
	_createCentralWidget();
	_createDockWidgets();
	_createActions();
	_createMenuBar();
	_createToolBar();
	_createStatusBar();
}

MainWindow::~MainWindow() = default;

void MainWindow::initializeScene()
{
	// Default scene setup
	_scene.setCamera(QVector3D(2.0f, 2.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), 
		QVector3D(0.0f, 1.0f, 0.0f), 45.0f, _glWidget->width() / (_glWidget->height() + 0.00001f));
	_scene.setLight(0, QVector3D(3.0f, 3.0f, 3.0f), QVector3D(300.0f, 300.0f, 300.0f));
	_scene.setLight(1, QVector3D(-3.0f, -3.0f, -3.0f), QVector3D(300.0f, 300.0f, 300.0f));

	// bindScene should be called at the end of scene initialization
	_glWidget->bindScene(&_scene);

	Q_EMIT sceneInitialized(&_scene);
}

void MainWindow::_createCentralWidget()
{
	_glWidget = new GLWidget(this);
	_glWidget->setMinimumSize(KLEIN_MIN_WIDTH, KLEIN_MIN_HEIGHT);
	this->setCentralWidget(_glWidget);
	ResourceManager::instance().initialize(_glWidget);
}

void MainWindow::_createDockWidgets()
{
	_processPanel = new ProcessPanel("ProcessPanel", this, _glWidget);
	_processPanel->setAllowedAreas(Qt::RightDockWidgetArea);
	_processPanel->setFeatures(QDockWidget::NoDockWidgetFeatures);
	_processPanel->setMinimumWidth(400);
	this->addDockWidget(Qt::RightDockWidgetArea, _processPanel);
}

void MainWindow::_createActions()
{
	_aQuit = new QAction("Quit", this);
	connect(_aQuit, &QAction::triggered, this, &MainWindow::close);

	_aOpen = new QAction("Open", this);
	connect(_aOpen, &QAction::triggered, this, &MainWindow::_importMesh);

	_aClear = new QAction("Clear All", this);
	connect(_aClear, &QAction::triggered, this, &MainWindow::_clearAll);

	_aScreenShot = new QAction("Screen Shot", this);
	connect(_aScreenShot, &QAction::triggered, this, &MainWindow::_screenShot);

	auto shadingActions = new QActionGroup(this);

	_aShaded = new QAction("Shaded", shadingActions);
	_aShaded->setCheckable(true);
	_aShaded->setChecked(true);
	connect(_aShaded, &QAction::triggered, this, &MainWindow::_shaded);

	_aWireframe = new QAction("Wireframe", shadingActions);
	_aWireframe->setCheckable(true);
	connect(_aWireframe, &QAction::triggered, this, &MainWindow::_wireframe);

	_aHiddenline = new QAction("Hiddenline", shadingActions);
	_aHiddenline->setCheckable(true);
	connect(_aHiddenline, &QAction::triggered, this, &MainWindow::_hiddenline);

	_aUnlit = new QAction("Unlit", this);
	_aUnlit->setCheckable(true);
	_aUnlit->setChecked(false);
	connect(_aUnlit, &QAction::triggered, this, &MainWindow::_unlit);
}

void MainWindow::_createMenuBar()
{
	_menuBar = new QMenuBar(this);
	this->setMenuBar(_menuBar);

	auto file = _menuBar->addMenu("File");
	file->addAction(_aOpen);
	file->addAction(_aClear);
	file->addSeparator();
	file->addAction(_aScreenShot);
	file->addSeparator();
	file->addAction(_aQuit);
	
	auto view = _menuBar->addMenu("View");
	view->addAction(_aShaded);
	view->addAction(_aWireframe);
	view->addAction(_aHiddenline);
	view->addSeparator();
	view->addAction(_aUnlit);
}

void MainWindow::_createToolBar()
{
}

void MainWindow::_createStatusBar()
{
	_statusBar = new QStatusBar(this);

	auto msglabel = new QLabel(_statusBar);
	msglabel->setText("Ready");
	_statusBar->addWidget(msglabel);

	_statusLabel = new QLabel(_statusBar);
	QString info = QString("#V: %1  #F: %2")
		.arg("N/A", -5).arg("N/A", -5);
	_statusLabel->setText(info);
	_statusBar->addPermanentWidget(_statusLabel);

	_statusBar->setStyleSheet(QString("QStatusBar::item{border: 0px}"));
	this->setStatusBar(_statusBar);
}

void MainWindow::_changeTitle(const QString& title)
{
	this->setWindowTitle(QString(KLEIN_TITLE).append(" -- ").append(title));
}

void MainWindow::_updateStatusLabel(size_t nVertices, size_t nFaces)
{
	QString info = QString("#V: %1  #F: %2")
		.arg(nVertices, -5).arg(nFaces, -5);
	_statusLabel->setText(info);
}

void MainWindow::_importMesh()
{
	auto path = QFileDialog::getOpenFileName(nullptr, "Import Mesh",
		_lastOpenFile, "Mesh Files(*.off *.obj)");
	if (path.length() != 0) {
		_statusBar->showMessage("Reading mesh...");

		std::unique_ptr<MeshIO> meshIO = nullptr;
		if (QFileInfo(path).suffix() == "off") {
			meshIO = std::make_unique<OFFMeshIO>();
		}
		if (QFileInfo(path).suffix() == "obj") {
			meshIO = std::make_unique<OBJMeshIO>();
		}

		if (meshIO->readMesh(path, "MainMesh", true, &_meshInfo)) {
			_scene.removeNode("MainMesh");
			auto node = _scene.addNode(_scene.rootNode(), "MainMesh");

			QMatrix4x4 transform;
			transform.scale(1.0f / _meshInfo.radius);
			transform.translate(-_meshInfo.center);
			node->setTransform(transform);

			auto graphics = std::make_unique<PBRMeshGraphics>(*_glWidget);
			graphics->setShaderLit("KLEIN_CookTorrance");
			graphics->setShaderUnlit("KLEIN_Unlit");
			graphics->setPositionBuffer("MainMesh_VertexBuffer");
			graphics->setNormalBuffer("MainMesh_NormalBuffer");
			graphics->setMaterial("KLEIN_PBR_Default");
			node->addGraphicsComponent(std::move(graphics));

			_scene.camera()->lookAt(QVector3D(2.0f, 2.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

			_lastOpenFile = QFileInfo(path).path();
			_changeTitle(QFileInfo(path).fileName());
			_updateStatusLabel(_meshInfo.nVertices, _meshInfo.nFaces);
			Q_EMIT meshImported(&_meshInfo);
		}

		_glWidget->update();
		_statusBar->clearMessage();
	}
}

void MainWindow::_clearAll()
{
	_scene.removeNode("MainMesh");
	Q_EMIT meshImported(nullptr);
	_glWidget->update();
}

void MainWindow::_screenShot()
{
	QImage img;
	_glWidget->renderToTexture(img);
	auto fileName = QString(KLEIN_SCREEN_SHOT_PATH);
	auto curTime = QDateTime::currentDateTime();
	fileName.append(curTime.toString("yyyymmddhhmmsszzz")).append(".png");
	KLEIN_LOG_DEBUG(fileName);
	img.save(fileName);
}

void MainWindow::_shaded()
{
	_scene.setShadingMethod(ShadingMethod::shaded);
	_glWidget->update();
}

void MainWindow::_wireframe()
{
	_scene.setShadingMethod(ShadingMethod::wireframe);
	_glWidget->update();
}

void MainWindow::_hiddenline()
{
	_scene.setShadingMethod(ShadingMethod::hiddenLine);
	_glWidget->update();
}

void MainWindow::_unlit()
{
	if (_aUnlit->isChecked()) {
		_scene.setUnlit(true);
	}
	else {
		_scene.setUnlit(false);
	}
	_glWidget->update();
}
