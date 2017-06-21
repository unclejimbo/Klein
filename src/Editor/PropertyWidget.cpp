#include "Editor/PropertyWidget.h"
#include "Core/ResourceManager.h"
#include "Core/PrimitiveGraphics.h"
#include "Core/PBRMeshVColorGraphics.h"
#include "Core/Color.h"
#include "Core/Util.h"

#include <Euclid/Analysis/OBB.h>
#include <Eigen/Dense>
#include <vector>
#include <memory>

PropertyWidget::PropertyWidget(QWidget* parent, GLWidget* glWidget)
	: ProcessWidget(parent, glWidget)
{
	auto layout = new QVBoxLayout(this);
	this->setLayout(layout);

	auto infoGroup = new QGroupBox("Mesh Info", this);
	layout->addWidget(infoGroup);
	auto infoLayout = new QGridLayout(infoGroup);
	infoGroup->setLayout(infoLayout);
	infoLayout->addWidget(new QLabel("File: ", infoGroup), 0, 0);
	_fileName = new QLabel(infoGroup);
	infoLayout->addWidget(_fileName, 0, 1, 1, 3);
	infoLayout->addWidget(new QLabel("#V: "), 1, 0);
	_nVertices = new QLabel(infoGroup);
	infoLayout->addWidget(_nVertices, 1, 1);
	infoLayout->addWidget(new QLabel("#F: "), 1, 2);
	_nFaces = new QLabel(infoGroup);
	infoLayout->addWidget(_nFaces, 1, 3);
	infoLayout->addWidget(new QLabel("Center: "), 2, 0);
	_center = new QLabel(infoGroup);
	infoLayout->addWidget(_center, 2, 1);
	infoLayout->addWidget(new QLabel("Radius: "), 2, 2);
	_radius = new QLabel(infoGroup);
	infoLayout->addWidget(_radius, 2, 3);
	infoLayout->addWidget(new QLabel("minX:"), 3, 0);
	_minX = new QLabel(infoGroup);
	infoLayout->addWidget(_minX, 3, 1);
	infoLayout->addWidget(new QLabel("maxX:"), 3, 2);
	_maxX = new QLabel(infoGroup);
	infoLayout->addWidget(_maxX, 3, 3);
	infoLayout->addWidget(new QLabel("minY:"), 4, 0);
	_minY = new QLabel(infoGroup);
	infoLayout->addWidget(_minY, 4, 1);
	infoLayout->addWidget(new QLabel("maxY:"), 4, 2);
	_maxY = new QLabel(infoGroup);
	infoLayout->addWidget(_maxY, 4, 3);
	infoLayout->addWidget(new QLabel("minZ:"), 5, 0);
	_minZ = new QLabel(infoGroup);
	infoLayout->addWidget(_minZ, 5, 1);
	infoLayout->addWidget(new QLabel("maxZ:"), 5, 2);
	_maxZ = new QLabel(infoGroup);
	infoLayout->addWidget(_maxZ, 5, 3);

	auto pickGroup = new QGroupBox("Picked Primitive", this);
	layout->addWidget(pickGroup);
	auto pickLayout = new QGridLayout(pickGroup);
	pickGroup->setLayout(pickLayout);
	_nothingBtn = new QRadioButton("Don't Pick", pickGroup);
	_nothingBtn->setChecked(true);
	pickLayout->addWidget(_nothingBtn, 0, 0);
	_vertexBtn = new QRadioButton("Pick Vertex", pickGroup);
	pickLayout->addWidget(_vertexBtn, 0, 1);
	_faceBtn = new QRadioButton("Pick Face", pickGroup);
	pickLayout->addWidget(_faceBtn, 0, 2);
	pickLayout->addWidget(new QLabel("PrimType:"), 1, 0);
	_primType = new QLabel(pickGroup);
	pickLayout->addWidget(_primType, 1, 1);
	pickLayout->addWidget(new QLabel("PrimID  :"), 2, 0);
	_primID = new QLabel(pickGroup);
	pickLayout->addWidget(_primID, 2, 1);

	auto visGroup = new QGroupBox("Visualization", this);
	layout->addWidget(visGroup);
	auto visLayout = new QGridLayout(visGroup);
	visGroup->setLayout(visLayout);
	_aabb = new QCheckBox("AABB", visGroup);
	_aabb->setCheckable(false);
	visLayout->addWidget(_aabb, 0, 0);
	_obb = new QCheckBox("OBB", visGroup);
	_obb->setCheckable(false);
	visLayout->addWidget(_obb, 0, 1);
	_sphere = new QCheckBox("Sphere", visGroup);
	_sphere->setCheckable(false);
	visLayout->addWidget(_sphere, 0, 2);
	visLayout->addWidget(new QLabel("Color: "), 1, 0);
	_color = new QComboBox(visGroup);
	_color->addItem("Material");
	visLayout->addWidget(_color, 1, 1, 1, 2);

	connect(_nothingBtn, &QRadioButton::clicked,
		[this](bool) { _scene->setPickingPrimitive(PICKING_PRIMITIVE_NONE); });
	connect(_vertexBtn, &QRadioButton::clicked,
		[this](bool) { _scene->setPickingPrimitive(PICKING_PRIMITIVE_VERTEX); });
	connect(_faceBtn, &QRadioButton::clicked,
		[this](bool) { _scene->setPickingPrimitive(PICKING_PRIMITIVE_FACE); });
	connect(_aabb, &QCheckBox::stateChanged, this, &PropertyWidget::showAABB);
	connect(_obb, &QCheckBox::stateChanged, this, &PropertyWidget::showOBB);
	connect(_sphere, &QCheckBox::stateChanged, this, &PropertyWidget::showSphere);
	connect(_color, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this, &PropertyWidget::onColorChanged);
}

PropertyWidget::~PropertyWidget() = default;

void PropertyWidget::activate()
{
	auto node = _scene->node("PropertyPick");
	if (node != nullptr) {
		node->graphicsComponent()->setVisible(true);
	}
	if (_nothingBtn->isChecked()) {
		_scene->setPickingPrimitive(PICKING_PRIMITIVE_NONE);
	}
	if (_vertexBtn->isChecked()) {
		_scene->setPickingPrimitive(PICKING_PRIMITIVE_VERTEX);
	}
	if (_faceBtn->isChecked()) {
		_scene->setPickingPrimitive(PICKING_PRIMITIVE_FACE);
	}
	_glWidget->update();
}

void PropertyWidget::deactivate()
{
	auto node = _scene->node("PropertyPick");
	if (node != nullptr) {
		node->graphicsComponent()->setVisible(false);
	}
	_scene->setPickingPrimitive(PICKING_PRIMITIVE_NONE);
	_glWidget->update();
}

void PropertyWidget::onImport(GeomInfo* info)
{
	_scene->removeNode("AABB");
	_scene->removeNode("OBB");
	_scene->removeNode("Sphere");
	_scene->removeNode("MainMeshValence");
	ResourceManager::instance().removeGLBuffer(_valenceBuffer);

	if (info != nullptr) {
		_valid = true;
		_id = info->id;

		if (info->type == GEOM_TYPE_MESH) {
			auto aabbNode = _scene->addNode("MainMesh", "AABB");
			auto aabbGraphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
			aabbGraphics->addBox(QVector3D(info->minX, info->minY, info->minZ),
				info->maxX - info->minX, info->maxY - info->minY, info->maxZ - info->minZ);
			aabbGraphics->setVisible(false);
			aabbNode->addGraphicsComponent(std::move(aabbGraphics));

			auto obbNode = _scene->addNode("MainMesh", "OBB");
			auto obbGraphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
			auto cMesh = ResourceManager::instance().mesh(_id)->surfaceMesh();
			if (cMesh != nullptr) {
				Euclid::OBB<Kernel::FT> obb(*cMesh);
				auto lbb = eigenToQt(obb.lbb());
				auto lbf = eigenToQt(obb.lbf());
				auto ltb = eigenToQt(obb.ltb());
				auto ltf = eigenToQt(obb.ltf());
				auto rbb = eigenToQt(obb.rbb());
				auto rbf = eigenToQt(obb.rbf());
				auto rtb = eigenToQt(obb.rtb());
				auto rtf = eigenToQt(obb.rtf());
				obbGraphics->addBox(lbb, lbf, ltb, ltf, rbb, rbf, rtb, rtf);
			}
			else {
				Euclid::OBB<float>
					obb(ResourceManager::instance().mesh(_id)->vertices());
				auto lbb = eigenToQt(obb.lbb());
				auto lbf = eigenToQt(obb.lbf());
				auto ltb = eigenToQt(obb.ltb());
				auto ltf = eigenToQt(obb.ltf());
				auto rbb = eigenToQt(obb.rbb());
				auto rbf = eigenToQt(obb.rbf());
				auto rtb = eigenToQt(obb.rtb());
				auto rtf = eigenToQt(obb.rtf());
				obbGraphics->addBox(lbb, lbf, ltb, ltf, rbb, rbf, rtb, rtf);
			}
			obbGraphics->setVisible(false);
			obbNode->addGraphicsComponent(std::move(obbGraphics));

			auto sphereNode = _scene->addNode("MainMesh", "Sphere");
			auto sphereGraphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
			sphereGraphics->addSphere(info->center, info->radius);
			sphereGraphics->setVisible(false);
			sphereNode->addGraphicsComponent(std::move(sphereGraphics));

			_fileName->setText(info->fileName);
			_nVertices->setNum(static_cast<int>(info->nVertices));
			_nFaces->setNum(static_cast<int>(info->nFaces));
			_center->setText(QString("(%1, %2, %3)").arg(info->center.x()).
				arg(info->center.y()).arg(info->center.z()));
			_radius->setNum(info->radius);
			_minX->setNum(info->minX);
			_maxX->setNum(info->maxX);
			_minY->setNum(info->minY);
			_maxY->setNum(info->maxY);
			_minZ->setNum(info->minZ);
			_maxZ->setNum(info->maxZ);
			_aabb->setChecked(false);
			_aabb->setCheckable(true);
			_obb->setChecked(false);
			_obb->setCheckable(true);
			_sphere->setChecked(false);
			_sphere->setCheckable(true);
			_color->setCurrentIndex(0);
			if (!ResourceManager::instance().mesh(_id)->isManifold()) {
				_color->removeItem(1);
			}
			else {
				if (_color->count() == 1) {
					_color->addItem("Valence");
				}
			}
		}

		if (info->type == GEOM_TYPE_POINTCLOUD) {
			auto aabbNode = _scene->addNode("MainMesh", "AABB");
			auto aabbGraphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
			aabbGraphics->addBox(QVector3D(info->minX, info->minY, info->minZ),
				info->maxX - info->minX, info->maxY - info->minY, info->maxZ - info->minZ);
			aabbGraphics->setVisible(false);
			aabbNode->addGraphicsComponent(std::move(aabbGraphics));

			auto obbNode = _scene->addNode("MainMesh", "OBB");
			auto obbGraphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
			Euclid::OBB<Kernel::FT>
				obb(ResourceManager::instance().pointCloud(_id)->vertices);
			auto lbb = eigenToQt(obb.lbb());
			auto lbf = eigenToQt(obb.lbf());
			auto ltb = eigenToQt(obb.ltb());
			auto ltf = eigenToQt(obb.ltf());
			auto rbb = eigenToQt(obb.rbb());
			auto rbf = eigenToQt(obb.rbf());
			auto rtb = eigenToQt(obb.rtb());
			auto rtf = eigenToQt(obb.rtf());
			obbGraphics->addBox(lbb, lbf, ltb, ltf, rbb, rbf, rtb, rtf);

			obbGraphics->setVisible(false);
			obbNode->addGraphicsComponent(std::move(obbGraphics));

			auto sphereNode = _scene->addNode("MainMesh", "Sphere");
			auto sphereGraphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
			sphereGraphics->addSphere(info->center, info->radius);
			sphereGraphics->setVisible(false);
			sphereNode->addGraphicsComponent(std::move(sphereGraphics));

			_fileName->setText(info->fileName);
			_nVertices->setNum(static_cast<int>(info->nVertices));
			_nFaces->setNum(static_cast<int>(info->nFaces));
			_center->setText(QString("(%1, %2, %3)").arg(info->center.x()).
				arg(info->center.y()).arg(info->center.z()));
			_radius->setNum(info->radius);
			_minX->setNum(info->minX);
			_maxX->setNum(info->maxX);
			_minY->setNum(info->minY);
			_maxY->setNum(info->maxY);
			_minZ->setNum(info->minZ);
			_maxZ->setNum(info->maxZ);
			_aabb->setChecked(false);
			_aabb->setCheckable(true);
			_obb->setChecked(false);
			_obb->setCheckable(true);
			_sphere->setChecked(false);
			_sphere->setCheckable(true);
			_color->setCurrentIndex(0);
			_color->removeItem(1);
		}
	}
	else { // if (info != nullptr)
		_valid = false;

		_fileName->setText("");
		_nVertices->setText("");
		_nFaces->setText("");
		_center->setText("");
		_radius->setText("");
		_minX->setText("");
		_maxX->setText("");
		_minY->setText("");
		_maxY->setText("");
		_minZ->setText("");
		_maxZ->setText("");
		_aabb->setCheckable(false);
		_obb->setCheckable(false);
		_sphere->setCheckable(false);
		_color->setCurrentIndex(0);
		_color->removeItem(1);
	}
}

void PropertyWidget::onPicked(const PickingInfo& info)
{
	if (info.primitiveType == PICKING_PRIMITIVE_VERTEX) {
		_primType->setText("Vertex");
	}
	if (info.primitiveType == PICKING_PRIMITIVE_LINE) {
		_primType->setText("Line");
	}
	if (info.primitiveType == PICKING_PRIMITIVE_FACE) {
		_primType->setText("Face");
	}
	_primID->setNum(static_cast<int>(info.primitiveID));
	_glWidget->renderPicked(info, "PropertyPick");
}

void PropertyWidget::showAABB(int state)
{
	if (_valid) {
		if (state == Qt::Checked) {
			_scene->node("AABB")->graphicsComponent()->setVisible(true);
		}
		else {
			_scene->node("AABB")->graphicsComponent()->setVisible(false);
		}
		_glWidget->update();
	}
}

void PropertyWidget::showOBB(int state)
{
	if (_valid) {
		if (state == Qt::Checked) {
			_scene->node("OBB")->graphicsComponent()->setVisible(true);
		} 
		else {
			_scene->node("OBB")->graphicsComponent()->setVisible(false);
		}
		_glWidget->update();
	}
}

void PropertyWidget::showSphere(int state)
{
	if (_valid) {
		if (state == Qt::Checked) {
			_scene->node("Sphere")->graphicsComponent()->setVisible(true);
		} 
		else {
			_scene->node("Sphere")->graphicsComponent()->setVisible(false);
		}
		_glWidget->update();
	}
}

void PropertyWidget::onColorChanged(int state)
{
	if (state == 0) { // Material
		_scene->node("MainMesh")->graphicsComponent()->setVisible(true);
		if (_scene->node("MainMeshValence") != nullptr) {
			_scene->node("MainMeshValence")->graphicsComponent()->setVisible(false);
		}
	}
	
	if (state == 1) { // Valence
		_scene->node("MainMesh")->graphicsComponent()->setVisible(false);
		if (_scene->node("MainMeshValence") != nullptr) {
			_scene->node("MainMeshValence")->graphicsComponent()->setVisible(true);
		}
		else {
			auto cMesh = ResourceManager::instance().mesh(_id)->polyhedron();
			std::vector<unsigned> valences;
			valences.reserve(cMesh->size_of_facets() * 3);
			for (auto f = cMesh->facets_begin(); f != cMesh->facets_end(); ++f) {
				auto v = f->facet_begin();
				do {
					valences.push_back(static_cast<unsigned>(v->vertex_degree()));
				} while (++v != f->facet_begin());
			}
			auto colors = temperature(valences, QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));
			_valenceBuffer = ResourceManager::instance().addGLBuffer(colors, GL_TRIANGLES);

			auto valenceNode = _scene->addNode(_scene->rootNode(), "MainMeshValence", _scene->node("MainMesh")->transform());
			auto graphics = std::make_unique<PBRMeshVColorGraphics>(*_glWidget);
			graphics->setPositionBuffer(ResourceManager::instance().mesh(_id)->positionBufferID());
			graphics->setNormalBuffer(ResourceManager::instance().mesh(_id)->normalBufferID());
			graphics->setColorBuffer(_valenceBuffer);
			valenceNode->addGraphicsComponent(std::move(graphics));
		}
	}

	_glWidget->update();
}
