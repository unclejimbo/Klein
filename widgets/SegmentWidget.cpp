#include "Widgets/SegmentWidget.h"
#include "Core/ResourceManager.h"
#include "Core/PrimitiveGraphics.h"
#include "Core/PrimitiveVColorGraphics.h"
#include "Core/PBRMeshVColorGraphics.h"
#include "Core/Color.h"
#include "Core/Util.h"
#include "Core/Logger.h"

#include <Euclid/Analysis/Segmentation.h>
#include <unordered_map>
#include <random>

SegmentWidget::SegmentWidget(Scene* scene, QWidget* parent, GLWidget* glWidget)
	: ProcessWidget(scene, parent, glWidget)
{
	auto layout = new QGridLayout(this);
	this->setLayout(layout);

	auto startBtn = new QPushButton("Start Seeding", this);
	layout->addWidget(startBtn, 0, 0);

	auto label = new QLabel("Sample Rate:", this);
	layout->addWidget(label, 1, 0);

	_lineEdit = new QLineEdit(this);
	_lineEdit->setValidator(new QDoubleValidator(0.0, 1.0, 2, _lineEdit));
	layout->addWidget(_lineEdit, 1, 1);

	auto sampleBtn = new QPushButton("Sample", this);
	layout->addWidget(sampleBtn, 1, 2);

	auto goBtn = new QPushButton("Go", this);
	layout->addWidget(goBtn, 2, 0);

	connect(startBtn, &QPushButton::clicked, this, &SegmentWidget::seeding);
	connect(sampleBtn, &QPushButton::clicked, this, &SegmentWidget::sample);
	connect(goBtn, &QPushButton::clicked, this, &SegmentWidget::segment);
}

void SegmentWidget::activate()
{
	switch (_type) {
	case GEOM_TYPE_MESH:
		_scene->setPickingPrimitive(PICKING_PRIMITIVE_FACE);
		break;
	case GEOM_TYPE_POINTCLOUD:
		_scene->setPickingPrimitive(PICKING_PRIMITIVE_VERTEX);
		break;
	}

	auto node = _scene->node("Seeds");
	if (node != nullptr) {
		node->graphicsComponent()->setVisible(true);
	}

	_glWidget->update();
}

void SegmentWidget::deactivate()
{
	if (_type != GEOM_TYPE_NONE) {
		_scene->setPickingPrimitive(PICKING_PRIMITIVE_NONE);
	}

	auto node = _scene->node("Seeds");
	if (node != nullptr) {
		node->graphicsComponent()->setVisible(false);
	}

	_glWidget->update();
}

void SegmentWidget::onImport(GeomInfo* info)
{
	this->_scene->removeNode("Seeds");
	_picked.clear();
	_scene->node("MainMesh")->graphicsComponent()->setVisible(true);
	_id = info->id;

	if (info->type == GEOM_TYPE_MESH) {
		_type = GEOM_TYPE_MESH;
		this->_scene->setPickingPrimitive(PICKING_PRIMITIVE_FACE);
	}
	if (info->type == GEOM_TYPE_POINTCLOUD) {
		_type = GEOM_TYPE_POINTCLOUD;
		this->_scene->setPickingPrimitive(PICKING_PRIMITIVE_VERTEX);
	}
}

void SegmentWidget::sample()
{
	// Sample input point cloud to reduce complexity
	if (_type != GEOM_TYPE_POINTCLOUD) {
		return;
	}

	auto ratio = _lineEdit->text().toFloat();
	std::default_random_engine dre;
	std::uniform_real_distribution<float> urd(0.0f, 1.0f);
	auto pc = ResourceManager::instance().pointCloud(_id);
	auto& ps = pc->pointSet();

	auto iter = ps.begin();
	while (iter != ps.end()) {
		if (urd(dre) > ratio) {
			ps.remove(iter);
		}
		else {
			++iter;
		}
	}
	ps.collect_garbage();

	pc->updateGLBuffer();
	_glWidget->update();
}

void SegmentWidget::onPicked(const PickingInfo& info)
{
	_picked.push_back(info.primitiveID);

	if (info.bufferSpec == GL_TRIANGLES) {
		auto mesh = ResourceManager::instance().mesh(_id);

		auto node = this->_scene->node("Seeds");
		if (node != nullptr) {
			auto graphics = dynamic_cast<PrimitiveGraphics*>(node->graphicsComponent());
			graphics->setPointSize(10);
			graphics->addPoint(cgalToQt(mesh->pointAt(info.primitiveID * 3 + 0)));
			graphics->addPoint(cgalToQt(mesh->pointAt(info.primitiveID * 3 + 1)));
			graphics->addPoint(cgalToQt(mesh->pointAt(info.primitiveID * 3 + 2)));
		}
		else {
			node = this->_scene->addNode("MainMesh", "Seeds");
			auto graphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
			graphics->setPointSize(10);
			graphics->addPoint(cgalToQt(mesh->pointAt(info.primitiveID * 3 + 0)));
			graphics->addPoint(cgalToQt(mesh->pointAt(info.primitiveID * 3 + 1)));
			graphics->addPoint(cgalToQt(mesh->pointAt(info.primitiveID * 3 + 2)));
			graphics->addRenderPass(RENDER_PICK);
			node->addGraphicsComponent(std::move(graphics));
		}
	}

	if (info.bufferSpec == GL_POINTS) {
		auto pc = ResourceManager::instance().pointCloud(_id)->pointSet();

		auto node = this->_scene->node("Seeds");
		if (node != nullptr) {
			auto graphics = dynamic_cast<PrimitiveGraphics*>(node->graphicsComponent());
			graphics->setPointSize(10);
			graphics->addPoint(cgalToQt(pc.point(info.primitiveID)));
		}
		else {
			node = this->_scene->addNode("MainMesh", "Seeds");
			auto graphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
			graphics->setPointSize(10);
			graphics->addPoint(cgalToQt(pc.point(info.primitiveID)));
			node->addGraphicsComponent(std::move(graphics));
		}
	}

	_glWidget->update();
}

void SegmentWidget::seeding()
{
	_picked.clear();
	_scene->removeNode("Seeds");
	_glWidget->update();
}

void SegmentWidget::segment()
{
	if (_type == GEOM_TYPE_MESH) {
		auto mesh = ResourceManager::instance().mesh(_id);
		std::vector<int> segmentation;
		if (!mesh->isManifold()) {
			KLEIN_LOG_CRITICAL("Can't segment non-manifold mesh");
		}
		else {
			Euclid::random_walk_segmentation(*mesh->surfaceMesh(), _picked, segmentation);

			// Construct the color buffer
			std::vector<QVector3D> colors;
			std::unordered_map<int, QVector3D> color_map;
			for (auto i = 0; i < _picked.size(); ++i) {
				color_map.emplace(_picked[i], Color::nextColor());
			}
			colors.reserve(segmentation.size() * 3);
			for (auto i = 0; i < segmentation.size(); ++i) {
				colors.push_back(color_map[segmentation[i]]);
				colors.push_back(color_map[segmentation[i]]);
				colors.push_back(color_map[segmentation[i]]);
			}
			auto colorBuf = ResourceManager::instance().addGLBuffer(colors, GL_TRIANGLES);

			// Visualisation
			_scene->node("MainMesh")->graphicsComponent()->setVisible(false);
			_scene->removeNode("Segmentation");
			auto segNode = _scene->addNode("MainMesh", "Segmentation");
			auto graphics = std::make_unique<PBRMeshVColorGraphics>(*_glWidget);
			graphics->setPositionBuffer(mesh->pointBufferID());
			graphics->setNormalBuffer(mesh->normalBufferID());
			graphics->setColorBuffer(colorBuf);
			graphics->setMaterial("KLEIN_PBR_Default");
			segNode->addGraphicsComponent(std::move(graphics));
			_glWidget->update();
		}
	}

	if (_type == GEOM_TYPE_POINTCLOUD) {
		auto pc = ResourceManager::instance().pointCloud(_id);
		auto ps = pc->pointSet();
		std::vector<int> segmentation;
		if (ps.has_normal_map()) {
			Euclid::random_walk_segmentation(ps.begin(), ps.end(), ps.point_map(), ps.normal_map(), _picked, segmentation);
		}

		std::vector<QVector3D> colors;
		std::unordered_map<int, QVector3D> color_map;
		for (auto i = 0; i < _picked.size(); ++i) {
			color_map.emplace(_picked[i], Color::nextColor());
		}
		colors.reserve(segmentation.size());
		for (auto i = 0; i < segmentation.size(); ++i) {
			colors.push_back(color_map[segmentation[i]]);
		}
		auto colorBuf = ResourceManager::instance().addGLBuffer(colors, GL_POINTS);

		// Visualisation
		_scene->node("MainMesh")->graphicsComponent()->setVisible(false);
		_scene->removeNode("Segmentation");
		auto segNode = _scene->addNode("MainMesh", "Segmentation");
		auto graphics = std::make_unique<PrimitiveVColorGraphics>(*_glWidget);
		graphics->setPointPositionBuffer(pc->pointBufferID());
		graphics->setPointColorBuffer(colorBuf);
		graphics->setShaderLit("KLEIN_Unlit_VColor");
		graphics->setShaderUnlit("KLEIN_Unlit_VColor");
		graphics->setPointSize(3);
		segNode->addGraphicsComponent(std::move(graphics));
		_glWidget->update();
	}
}
