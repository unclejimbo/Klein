#include "Editor/MainWindow.h"
#include "Core/Logger.h"
#include "Core/ObjIO.h"
#include "Core/OffIO.h"
#include "Core/PlyIO.h"
#include "Core/XyzIO.h"
#include "Core/ResourceManager.h"
#include "Core/Scene.h"
#include "Core/SceneNode.h"
#include "Core/PBRMeshGraphics.h"
#include "Core/PrimitiveGraphics.h"
#include "Editor/GLWidget.h"
#include "Editor/ProcessPanel.h"

#include <QMatrix4x4>
#include <QVector3D>
#include <fstream>

MainWindow::MainWindow(Scene* scene, QWidget* parent)
	: QMainWindow(parent), _scene(scene)
{
	this->setWindowTitle(KLEIN_TITLE);
	_createCentralWidget();
	_createDockWidgets();
	_createActions();
	_createMenuBar();
	_createToolBar();
	_createStatusBar();
}

void MainWindow::_createCentralWidget()
{
	_glWidget = new GLWidget(_scene, this);
	_glWidget->setMinimumSize(KLEIN_MIN_WIDTH, KLEIN_MIN_HEIGHT);
	this->setCentralWidget(_glWidget);
	ResourceManager::instance().initialize(_glWidget);
}

void MainWindow::_createDockWidgets()
{
	_processPanel = new ProcessPanel(_scene, "ProcessPanel", this, _glWidget);
	_processPanel->setAllowedAreas(Qt::RightDockWidgetArea);
	_processPanel->setFeatures(QDockWidget::NoDockWidgetFeatures);
	_processPanel->setMinimumWidth(400);
	this->addDockWidget(Qt::RightDockWidgetArea, _processPanel);
}

void MainWindow::_createActions()
{
	_aQuit = new QAction("Quit", this);
	connect(_aQuit, &QAction::triggered, this, &MainWindow::close);

	_aImportMesh = new QAction("Import Mesh", this);
	connect(_aImportMesh, &QAction::triggered, this, &MainWindow::_importMesh);

	_aImportPointCloud = new QAction("Import Point Cloud", this);
	connect(_aImportPointCloud, &QAction::triggered, this, &MainWindow::_importPointCloud);

	_aClear = new QAction("Clear All", this);
	connect(_aClear, &QAction::triggered, this, &MainWindow::_clearAll);

	_aScreenShot = new QAction("Screen Shot", this);
	connect(_aScreenShot, &QAction::triggered, this, &MainWindow::_screenShot);

	auto meshShading = new QActionGroup(this);

	_aShaded = new QAction("Shaded Mesh", meshShading);
	_aShaded->setCheckable(true);
	_aShaded->setChecked(true);
	connect(_aShaded, &QAction::triggered, this, &MainWindow::_shaded);

	_aWireframe = new QAction("Wireframe Mesh", meshShading);
	_aWireframe->setCheckable(true);
	connect(_aWireframe, &QAction::triggered, this, &MainWindow::_wireframe);

	_aHiddenline = new QAction("Hiddenline Mesh", meshShading);
	_aHiddenline->setCheckable(true);
	connect(_aHiddenline, &QAction::triggered, this, &MainWindow::_hiddenline);

	auto primitiveShading = new QActionGroup(this);

	_aPrimitive = new QAction("Basic Primitive", primitiveShading);
	_aPrimitive->setCheckable(true);
	_aPrimitive->setChecked(true);
	connect(_aPrimitive, &QAction::triggered, this, &MainWindow::_primitive);

	_aSolid = new QAction("Solid Primitive", primitiveShading);
	_aSolid->setCheckable(true);
	connect(_aSolid, &QAction::triggered, this, &MainWindow::_solid);

	_aBackgroundWall = new QAction("Background Wall", this);
	_aBackgroundWall->setCheckable(true);
	connect(_aBackgroundWall, &QAction::triggered, this, &MainWindow::_backgroundWall);
		
	_aUnlit = new QAction("Light Off", this);
	_aUnlit->setCheckable(true);
	_aUnlit->setChecked(false);
	connect(_aUnlit, &QAction::triggered, this, &MainWindow::_unlit);

	_aShadow = new QAction("Shadow On", this);
	_aShadow->setCheckable(true);
	_aShadow->setChecked(false);
	connect(_aShadow, &QAction::triggered, this, &MainWindow::_shadow);

	_aAdvancedRender = new QAction("Advanced Options", this);
	connect(_aAdvancedRender, &QAction::triggered, this, &MainWindow::_advancedRender);
}

void MainWindow::_createMenuBar()
{
	_menuBar = new QMenuBar(this);
	this->setMenuBar(_menuBar);

	auto file = _menuBar->addMenu("File");
	file->addAction(_aImportMesh);
	file->addAction(_aImportPointCloud);
	file->addAction(_aClear);
	file->addSeparator();
	file->addAction(_aScreenShot);
	file->addSeparator();
	file->addAction(_aQuit);

	auto render = _menuBar->addMenu("Render");
	render->addAction(_aShaded);
	render->addAction(_aWireframe);
	render->addAction(_aHiddenline);
	render->addSeparator();
	render->addAction(_aPrimitive);
	render->addAction(_aSolid);
	render->addSeparator();
	render->addAction(_aBackgroundWall);
	render->addSeparator();
	render->addAction(_aUnlit);
	render->addAction(_aShadow);
	render->addSeparator();
	render->addAction(_aAdvancedRender);
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
		_lastOpenFile, "Mesh Files(*.off *.obj *.ply)");
	if (path.length() != 0) {
		_statusBar->showMessage("Reading mesh...");

		std::unique_ptr<GeomIO> geomIO = nullptr;
		if (QFileInfo(path).suffix() == "off") {
			geomIO = std::make_unique<OffIO>();
		}
		else if (QFileInfo(path).suffix() == "obj") {
			geomIO = std::make_unique<ObjIO>();
		}
		else if (QFileInfo(path).suffix() == "ply") {
			geomIO = std::make_unique<PlyIO>();
		}
		else {
			// Empty
		}

		unsigned posBufID;
		unsigned normBufID;
		GeomInfo geomInfo;
		if (geomIO->readMesh(path, posBufID, normBufID, &geomInfo)) {
			ResourceManager::instance().removeMesh(_geomInfo.id);
			_geomInfo = geomInfo;
			_scene->removeNode("MainMesh");
			auto node = _scene->addNode(_scene->rootNode(), "MainMesh");

			QMatrix4x4 transform;
			auto diag = QVector3D(geomInfo.maxX - geomInfo.minX,
				geomInfo.maxY - geomInfo.minY, geomInfo.maxZ - geomInfo.minZ);
			diag *= 0.5f;
			transform.scale(1.0f / diag.length());
			transform.translate(-geomInfo.center);
			node->setTransform(transform);

			auto graphics = std::make_unique<PBRMeshGraphics>(*_glWidget);
			graphics->setShaderLit("KLEIN_CookTorrance");
			graphics->setShaderUnlit("KLEIN_Unlit");
			graphics->setPositionBuffer(posBufID);
			graphics->setNormalBuffer(normBufID);
			graphics->setMaterial("KLEIN_PBR_Default");
			graphics->addRenderPass(RENDER_PICK);
			node->addGraphicsComponent(std::move(graphics));

			_scene->camera()->lookAt(QVector3D(2.0f, 2.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

			_lastOpenFile = QFileInfo(path).path();
			_changeTitle(QFileInfo(path).fileName());
			_updateStatusLabel(geomInfo.nVertices, geomInfo.nFaces);
			Q_EMIT geomImported(&_geomInfo);
		}

		_glWidget->update();
		_statusBar->clearMessage();
	}
}

void MainWindow::_importPointCloud()
{
	auto path = QFileDialog::getOpenFileName(nullptr, "Import PointCloud",
		_lastOpenFile, "PointCloud Files(*.off *.obj *.ply *xyz)");
	if (path.length() != 0) {
		_statusBar->showMessage("Reading point cloud...");

		std::unique_ptr<GeomIO> geomIO = nullptr;
		if (QFileInfo(path).suffix() == "off") {
			geomIO = std::make_unique<OffIO>();
		}
		else if (QFileInfo(path).suffix() == "obj") {
			geomIO = std::make_unique<ObjIO>();
		}
		else if (QFileInfo(path).suffix() == "ply") {
			geomIO = std::make_unique<PlyIO>();
		}
		else if (QFileInfo(path).suffix() == "xyz") {
			geomIO = std::make_unique<XyzIO>();
		}
		else {
			// Empty
		}

		unsigned posBufID;
		GeomInfo geomInfo;
		if (geomIO->readPointCloud(path, posBufID, &geomInfo)) {
			ResourceManager::instance().removePointCloud(_geomInfo.id);
			_geomInfo = geomInfo;
			_scene->removeNode("MainMesh");
			auto node = _scene->addNode(_scene->rootNode(), "MainMesh");

			QMatrix4x4 transform;
			auto diag = QVector3D(_geomInfo.maxX - _geomInfo.minX,
				_geomInfo.maxY - _geomInfo.minY, _geomInfo.maxZ - _geomInfo.minZ);
			diag *= 0.5f;
			transform.scale(1.0f / diag.length());
			transform.translate(-_geomInfo.center);
			node->setTransform(transform);

			auto graphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
			graphics->setPointPositionBuffer(posBufID);
			graphics->setColor(QVector3D(1.0f, 1.0f, 1.0f));
			graphics->addRenderPass(RENDER_PICK);
			node->addGraphicsComponent(std::move(graphics));

			_scene->camera()->lookAt(QVector3D(2.0f, 2.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

			_lastOpenFile = QFileInfo(path).path();
			_changeTitle(QFileInfo(path).fileName());
			_updateStatusLabel(_geomInfo.nVertices, _geomInfo.nFaces);
			Q_EMIT geomImported(&_geomInfo);
		}

		_glWidget->update();
		_statusBar->clearMessage();
	}
}

void MainWindow::_clearAll()
{
	_scene->removeNode("MainMesh");
	Q_EMIT geomImported(nullptr);
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
	_scene->setGlobalMeshRenderMode(MeshRenderMode::shaded);
	_glWidget->update();
}

void MainWindow::_wireframe()
{
	_scene->setGlobalMeshRenderMode(MeshRenderMode::wireframe);
	_glWidget->update();
}

void MainWindow::_hiddenline()
{
	_scene->setGlobalMeshRenderMode(MeshRenderMode::hiddenLine);
	_glWidget->update();
}

void MainWindow::_primitive()
{
	_scene->setGlobalPrimitiveRenderMode(PrimitiveRenderMode::basic);
	_glWidget->update();
}

void MainWindow::_solid()
{
	_scene->setGlobalPrimitiveRenderMode(PrimitiveRenderMode::solid);
	_glWidget->update();
}

void MainWindow::_backgroundWall()
{
}

void MainWindow::_unlit()
{
	if (_aUnlit->isChecked()) {
		_scene->setUnlit(true);
	}
	else {
		_scene->setUnlit(false);
	}
	_glWidget->update();
}

void MainWindow::_shadow()
{
}

void MainWindow::_advancedRender()
{
}
