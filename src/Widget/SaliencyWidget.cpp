#include "Widget/SaliencyWidget.h"
#include "Core/ResourceManager.h"

#include <Euclid/Analysis/Descriptor.h>
#include <Euclid/Geometry/MeshProperties.h>
#include <Euclid/Geometry/Surface_mesh.h>
#include <boost/iterator/zip_iterator.hpp>
#include <vector>

SaliencyWidget::SaliencyWidget(QWidget* parent, GLWidget* glWidget)
	: ProcessWidget(parent, glWidget)
{
	auto layout = new QVBoxLayout(this);
	this->setLayout(layout);

	auto btn = new QPushButton("Go", this);
	layout->addWidget(btn);

	connect(btn, &QPushButton::clicked, this, &SaliencyWidget::spinImage);
}

SaliencyWidget::~SaliencyWidget() = default;

void SaliencyWidget::activate()
{
}

void SaliencyWidget::deactivate()
{
}

void SaliencyWidget::onImport(GeomInfo* info)
{
	_spinImages.clear();

	if (info != nullptr) {
		_geomID = info->id;
		_geomType = info->type;
	}
}

void SaliencyWidget::spinImage()
{
	auto mesh = ResourceManager::instance().mesh(_geomID)->surfaceMesh();

	std::vector<Vector_3> faceNormals;
	for (const auto& f : mesh->faces()) {
		auto n = Euclid::face_normal(f, *mesh);
		faceNormals.push_back(n);
	}
	
	Euclid::SM_PropertyMap<Vector_3> vertexNormalMap(faceNormals);
	std::vector<Vector_3> vertexNormals;
	for (const auto& v : mesh->vertices()) {
		auto n = Euclid::vertex_normal(v, *mesh, vertexNormalMap);
	}
}
