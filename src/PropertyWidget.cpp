#include "PropertyWidget.h"

#include "Color.h"
#include "PrimitiveNode.h"
#include "ResourceManager.h"
#include "Util.h"
#include "VertexColorMeshNode.h"
#include <Euclid/Analysis/OBB.h>

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <Eigen/Dense>
#include <vector>

#ifdef _WIN32
#pragma warning(pop)
#endif

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

	connect(_aabb, &QCheckBox::stateChanged, this, &PropertyWidget::showAABB);
	connect(_obb, &QCheckBox::stateChanged, this, &PropertyWidget::showOBB);
	connect(_sphere, &QCheckBox::stateChanged, this, &PropertyWidget::showSphere);
	connect(_color, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this, &PropertyWidget::onColorChanged);
}

PropertyWidget::~PropertyWidget() = default;

void PropertyWidget::onImportMesh(MeshInfo* info)
{
	_scene->removeNode("AABB");
	_scene->removeNode("OBB");
	_scene->removeNode("Sphere");
	_scene->removeNode("MainMeshValence");
	ResourceManager::instance().removeGLBuffer("MainMeshValence");

	if (info != nullptr) {
		_valid = true;

		auto aabbNode = dynamic_cast<PrimitiveNode*>(
			_scene->addNode(_scene->node("MainMesh"), SceneNodeType::primitiveNode, "AABB"));
		aabbNode->addBox(QVector3D(info->minX, info->minY, info->minZ),
			info->maxX - info->minX, info->maxY - info->minY, info->maxZ - info->minZ);
		aabbNode->setVisible(false);

		auto obbNode = dynamic_cast<PrimitiveNode*>(
			_scene->addNode(_scene->node("MainMesh"), SceneNodeType::primitiveNode, "OBB"));
		if (auto cMesh = ResourceManager::instance().mesh("MainMesh").first->cMesh()) {
			Euclid::OBB<CMesh> obb(*cMesh);
			auto lbb = eigenToQt(obb.lbb());
			auto lbf = eigenToQt(obb.lbf());
			auto ltb = eigenToQt(obb.ltb());
			auto ltf = eigenToQt(obb.ltf());
			auto rbb = eigenToQt(obb.rbb());
			auto rbf = eigenToQt(obb.rbf());
			auto rtb = eigenToQt(obb.rtb());
			auto rtf = eigenToQt(obb.rtf());
			obbNode->addBox(lbb, lbf, ltb, ltf, rbb, rbf, rtb, rtf);
		}
		else {
			Euclid::OBB<CMesh>
				obb(ResourceManager::instance().mesh("MainMesh").first->vertices());
			auto lbb = eigenToQt(obb.lbb());
			auto lbf = eigenToQt(obb.lbf());
			auto ltb = eigenToQt(obb.ltb());
			auto ltf = eigenToQt(obb.ltf());
			auto rbb = eigenToQt(obb.rbb());
			auto rbf = eigenToQt(obb.rbf());
			auto rtb = eigenToQt(obb.rtb());
			auto rtf = eigenToQt(obb.rtf());
			obbNode->addBox(lbb, lbf, ltb, ltf, rbb, rbf, rtb, rtf);
		}
		obbNode->setVisible(false);

		auto sphereNode = dynamic_cast<PrimitiveNode*>(
			_scene->addNode(_scene->node("MainMesh"), SceneNodeType::primitiveNode, "Sphere"));
		sphereNode->addSphere(info->center, info->radius);
		sphereNode->setVisible(false);

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
		if (ResourceManager::instance().mesh("MainMesh").first->cMesh() == nullptr) {
			_color->removeItem(1);
		}
		else {
			if (_color->count() == 1) {
				_color->addItem("Valence");
			}
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

void PropertyWidget::showAABB(int state)
{
	if (_valid) {
		if (state == Qt::Checked) {
			_scene->node("AABB")->setVisible(true);
		}
		else {
			_scene->node("AABB")->setVisible(false);
		}
		_glWidget->update();
	}
}

void PropertyWidget::showOBB(int state)
{
	if (_valid) {
		if (state == Qt::Checked) {
			_scene->node("OBB")->setVisible(true);
		} 
		else {
			_scene->node("OBB")->setVisible(false);
		}
		_glWidget->update();
	}
}

void PropertyWidget::showSphere(int state)
{
	if (_valid) {
		if (state == Qt::Checked) {
			_scene->node("Sphere")->setVisible(true);
		} 
		else {
			_scene->node("Sphere")->setVisible(false);
		}
		_glWidget->update();
	}
}

void PropertyWidget::onColorChanged(int state)
{
	if (state == 0) { // Material
		_scene->node("MainMesh")->setVisible(true);
		if (_scene->node("MainMeshValence") != nullptr) {
			_scene->node("MainMeshValence")->setVisible(false);
		}
	}
	
	if (state == 1) { // Valence
		_scene->node("MainMesh")->setVisible(false);
		if (_scene->node("MainMeshValence") != nullptr) {
			_scene->node("MainMeshValence")->setVisible(true);
		}
		else {
			auto cMesh = ResourceManager::instance().mesh("MainMesh").first->cMesh();
			std::vector<unsigned> valences;
			valences.reserve(cMesh->size_of_facets() * 3);
			for (auto f = cMesh->facets_begin(); f != cMesh->facets_end(); ++f) {
				auto v = f->facet_begin();
				do {
					valences.push_back(static_cast<unsigned>(v->vertex_degree()));
				} while (++v != f->facet_begin());
			}
			auto colors = temperature(valences, QVector4D(1.0f, 0.0f, 0.0f, 1.0f), QVector4D(0.0f, 0.0f, 1.0f, 1.0f));
			ResourceManager::instance().addGLBuffer("MainMeshValence", colors);

			auto valenceNode = dynamic_cast<VertexColorMeshNode*>(_scene->addNode(_scene->rootNode(),
				SceneNodeType::vertexColorMeshNode, "MainMeshValence", _scene->node("MainMesh")->transform()));
			valenceNode->attachMesh("MainMesh_VertexBuffer", "MainMeshValence");
		}
	}

	_glWidget->update();
}
