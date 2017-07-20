#include "Editor/PropertyWidget.h"
#include "Core/ResourceManager.h"
#include "Core/PrimitiveGraphics.h"
#include "Core/PBRMeshVColorGraphics.h"
#include "Core/Color.h"
#include "Core/Util.h"

#include <Euclid/Geometry/MeshProperties.h>
#include <Euclid/Analysis/OBB.h>
#include <Eigen/Dense>
#include <vector>
#include <memory>

PropertyWidget::PropertyWidget(Scene* scene, QWidget* parent, GLWidget* glWidget)
	: ProcessWidget(scene, parent, glWidget)
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
	pickLayout->addWidget(new QLabel("AreaType:"), 3, 0);
	_areaType = new QComboBox(pickGroup);
	_areaType->addItem("Barycentric");
	_areaType->addItem("Voronoi");
	_areaType->addItem("Mixed");
	pickLayout->addWidget(_areaType, 3, 1);
	pickLayout->addWidget(new QLabel("Area    :"), 4, 0);
	_primArea = new QLabel(pickGroup);
	pickLayout->addWidget(_primArea, 4, 1);

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
	visLayout->addWidget(new QLabel("Color: "), 1, 0);
	_color = new QComboBox(visGroup);
	_color->addItem("Material");
	visLayout->addWidget(_color, 1, 1, 1, 2);

	connect(_nothingBtn, &QRadioButton::clicked,
		[this, glWidget](bool) 
		{ 
			_scene->setPickingPrimitive(PICKING_PRIMITIVE_NONE);
			_scene->removeNode("PropertyPick");
			_scene->removeNode("BarycentricArea");
			_scene->removeNode("VoronoiArea");
			_scene->removeNode("MixedArea");
			glWidget->update();
		}
	);
	connect(_vertexBtn, &QRadioButton::clicked,
		[this, glWidget](bool)
		{
			_scene->setPickingPrimitive(PICKING_PRIMITIVE_VERTEX);
			_scene->removeNode("PropertyPick");
			_scene->removeNode("BarycentricArea");
			_scene->removeNode("VoronoiArea");
			_scene->removeNode("MixedArea");
			glWidget->update();
		}
	);
	connect(_faceBtn, &QRadioButton::clicked,
		[this, glWidget](bool)
		{
			_scene->setPickingPrimitive(PICKING_PRIMITIVE_FACE);
			_scene->removeNode("PropertyPick");
			_scene->removeNode("BarycentricArea");
			_scene->removeNode("VoronoiArea");
			_scene->removeNode("MixedArea");
			glWidget->update();
		}
	);
	connect(_areaType, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this, &PropertyWidget::onAreaTypeChanged);
	connect(_aabb, &QCheckBox::stateChanged, this, &PropertyWidget::showAABB);
	connect(_obb, &QCheckBox::stateChanged, this, &PropertyWidget::showOBB);
	connect(_color, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this, &PropertyWidget::onColorChanged);
}

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
	_scene->removeNode("BarycentricArea");
	_scene->removeNode("VoronoiArea");
	_scene->removeNode("MixedArea");
	_scene->removeNode("MainMeshValence");
	_scene->removeNode("MainMeshGaussian");
	_scene->removeNode("MainMeshMean");
	_scene->removeNode("PropertyPick");
	_clearResources();

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
				auto lbb = cgalToQt(obb.lbb());
				auto lbf = cgalToQt(obb.lbf());
				auto ltb = cgalToQt(obb.ltb());
				auto ltf = cgalToQt(obb.ltf());
				auto rbb = cgalToQt(obb.rbb());
				auto rbf = cgalToQt(obb.rbf());
				auto rtb = cgalToQt(obb.rtb());
				auto rtf = cgalToQt(obb.rtf());
				obbGraphics->addBox(lbb, lbf, ltb, ltf, rbb, rbf, rtb, rtf);
			}
			else {
				Euclid::OBB<float>
					obb(ResourceManager::instance().mesh(_id)->points());
				auto lbb = cgalToQt(obb.lbb());
				auto lbf = cgalToQt(obb.lbf());
				auto ltb = cgalToQt(obb.ltb());
				auto ltf = cgalToQt(obb.ltf());
				auto rbb = cgalToQt(obb.rbb());
				auto rbf = cgalToQt(obb.rbf());
				auto rtb = cgalToQt(obb.rtb());
				auto rtf = cgalToQt(obb.rtf());
				obbGraphics->addBox(lbb, lbf, ltb, ltf, rbb, rbf, rtb, rtf);
			}
			obbGraphics->setVisible(false);
			obbNode->addGraphicsComponent(std::move(obbGraphics));

			_fileName->setText(info->fileName);
			_nVertices->setNum(static_cast<int>(info->nVertices));
			_nFaces->setNum(static_cast<int>(info->nFaces));
			_center->setText(QString("(%1, %2, %3)").arg(info->center.x()).
				arg(info->center.y()).arg(info->center.z()));
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

			_color->setCurrentIndex(0);
			if (!ResourceManager::instance().mesh(_id)->isManifold()) {
				_color->removeItem(1);
				_color->removeItem(1);
				_color->removeItem(1);
			}
			else {
				if (_color->count() == 1) {
					_color->addItem("Valence");
					_color->addItem("Gaussian Curvature");
					_color->addItem("Mean Curvature");
				}
			}

			_areaType->setEnabled(true);
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
				obb(ResourceManager::instance().pointCloud(_id)->pointSet());
			auto lbb = cgalToQt(obb.lbb());
			auto lbf = cgalToQt(obb.lbf());
			auto ltb = cgalToQt(obb.ltb());
			auto ltf = cgalToQt(obb.ltf());
			auto rbb = cgalToQt(obb.rbb());
			auto rbf = cgalToQt(obb.rbf());
			auto rtb = cgalToQt(obb.rtb());
			auto rtf = cgalToQt(obb.rtf());
			obbGraphics->addBox(lbb, lbf, ltb, ltf, rbb, rbf, rtb, rtf);

			obbGraphics->setVisible(false);
			obbNode->addGraphicsComponent(std::move(obbGraphics));

			_fileName->setText(info->fileName);
			_nVertices->setNum(static_cast<int>(info->nVertices));
			_nFaces->setNum(static_cast<int>(info->nFaces));
			_center->setText(QString("(%1, %2, %3)").arg(info->center.x()).
				arg(info->center.y()).arg(info->center.z()));
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
			_color->setCurrentIndex(0);
			_color->removeItem(1);
			_color->removeItem(1);
			_color->removeItem(1);
			_areaType->setEnabled(false);
		}
	}
	else { // if (info != nullptr)
		_valid = false;

		_fileName->setText("");
		_nVertices->setText("");
		_nFaces->setText("");
		_center->setText("");
		_minX->setText("");
		_maxX->setText("");
		_minY->setText("");
		_maxY->setText("");
		_minZ->setText("");
		_maxZ->setText("");
		_aabb->setCheckable(false);
		_obb->setCheckable(false);
		_color->setCurrentIndex(0);
		_color->removeItem(1);
		_color->removeItem(1);
		_color->removeItem(1);
		_areaType->setEnabled(false);
	}
}

void PropertyWidget::onAreaTypeChanged(int state)
{
	// If this function get called, it means the input is already a mesh
	if (!_primID->text().isEmpty()) {
		_drawVertexArea();
		_glWidget->update();
	}
}

void PropertyWidget::onPicked(const PickingInfo& info)
{
	_primID->setNum(static_cast<int>(info.primitiveID));
	if (info.primitiveType == PICKING_PRIMITIVE_VERTEX) {
		_primType->setText("Vertex");
		if (_areaType->isEnabled()) {
			_scene->removeNode("BarycentricArea");
			_scene->removeNode("VoronoiArea");
			_scene->removeNode("MixedArea");
			_drawVertexArea();
		}
	}
	if (info.primitiveType == PICKING_PRIMITIVE_LINE) {
		_primType->setText("Line");
	}
	if (info.primitiveType == PICKING_PRIMITIVE_FACE) {
		_primType->setText("Face");
	}
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

void PropertyWidget::onColorChanged(int state)
{
	_hideAllColorNodes();
	if (state == 0) { // Material
		_scene->node("MainMesh")->graphicsComponent()->setVisible(true);
	}
	else if (state == 1) { // Valence
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

			auto node = _scene->addNode(_scene->rootNode(), "MainMeshValence", _scene->node("MainMesh")->transform());
			auto graphics = std::make_unique<PBRMeshVColorGraphics>(*_glWidget);
			graphics->setPositionBuffer(ResourceManager::instance().mesh(_id)->pointBufferID());
			graphics->setNormalBuffer(ResourceManager::instance().mesh(_id)->normalBufferID());
			graphics->setColorBuffer(_valenceBuffer);
			node->addGraphicsComponent(std::move(graphics));
		}
	}
	else if (state == 2) { // Gaussian Curvature
		if (_scene->node("MainMeshGaussian") != nullptr) {
			_scene->node("MainMeshGaussian")->graphicsComponent()->setVisible(true);
		}
		else {
			auto mesh = ResourceManager::instance().mesh(_id)->surfaceMesh();
			std::vector<double> gaussian;
			gaussian.reserve(mesh->num_faces() * 3);
			for (const auto& f : mesh->faces()) {
				auto he = mesh->halfedge(f);
				for (const auto& v : mesh->vertices_around_face(he)) {
					gaussian.push_back(Euclid::gaussian_curvature(v, *mesh));
				}
			}
			auto colors = temperature(gaussian, QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));
			_gaussianBuffer = ResourceManager::instance().addGLBuffer(colors, GL_TRIANGLES);

			auto node = _scene->addNode(_scene->rootNode(), "MainMeshGaussian", _scene->node("MainMesh")->transform());
			auto graphics = std::make_unique<PBRMeshVColorGraphics>(*_glWidget);
			graphics->setPositionBuffer(ResourceManager::instance().mesh(_id)->pointBufferID());
			graphics->setNormalBuffer(ResourceManager::instance().mesh(_id)->normalBufferID());
			graphics->setColorBuffer(_gaussianBuffer);
			node->addGraphicsComponent(std::move(graphics));
		}
	}
	else { // Mean Curvature
		if (_scene->node("MainMeshMean") != nullptr) {
			_scene->node("MainMeshMean")->graphicsComponent()->setVisible(true);
		}
		else {
			auto mesh = ResourceManager::instance().mesh(_id)->surfaceMesh();
			std::vector<double> mean;
			mean.reserve(mesh->num_faces() * 3);
			for (const auto& f : mesh->faces()) {
				auto he = mesh->halfedge(f);
				for (const auto& v : mesh->vertices_around_face(he)) {
					mean.push_back(Euclid::mean_curvature(v, *mesh));
				}
			}
			auto colors = temperature(mean, QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));
			_meanBuffer = ResourceManager::instance().addGLBuffer(colors, GL_TRIANGLES);

			auto node = _scene->addNode(_scene->rootNode(), "MainMeshMean", _scene->node("MainMesh")->transform());
			auto graphics = std::make_unique<PBRMeshVColorGraphics>(*_glWidget);
			graphics->setPositionBuffer(ResourceManager::instance().mesh(_id)->pointBufferID());
			graphics->setNormalBuffer(ResourceManager::instance().mesh(_id)->normalBufferID());
			graphics->setColorBuffer(_meanBuffer);
			node->addGraphicsComponent(std::move(graphics));
		}
	}

	_glWidget->update();
}

void PropertyWidget::_clearResources()
{
	ResourceManager::instance().removeGLBuffer(_valenceBuffer);
	ResourceManager::instance().removeGLBuffer(_gaussianBuffer);
	ResourceManager::instance().removeGLBuffer(_meanBuffer);
}

void PropertyWidget::_hideAllAreaNodes()
{
	if (_scene->node("BarycentricArea") != nullptr) {
		_scene->node("BarycentricArea")->graphicsComponent()->setVisible(false);
	}
	if (_scene->node("VoronoiArea") != nullptr) {
		_scene->node("VoronoiArea")->graphicsComponent()->setVisible(false);
	}
	if (_scene->node("MixedArea") != nullptr) {
		_scene->node("MixedArea")->graphicsComponent()->setVisible(false);
	}
}

void PropertyWidget::_hideAllColorNodes()
{
	_scene->node("MainMesh")->graphicsComponent()->setVisible(false);
	if (_scene->node("MainMeshValence") != nullptr) {
		_scene->node("MainMeshValence")->graphicsComponent()->setVisible(false);
	}
	if (_scene->node("MainMeshGaussian") != nullptr) {
		_scene->node("MainMeshGaussian")->graphicsComponent()->setVisible(false);
	}
	if (_scene->node("MainMeshMean") != nullptr) {
		_scene->node("MainMeshMean")->graphicsComponent()->setVisible(false);
	}
}

void PropertyWidget::_drawVertexArea()
{
	_hideAllAreaNodes();
	auto mesh = ResourceManager::instance().mesh(_id);
	auto surfaceMesh = mesh->surfaceMesh();
	auto picked = _primID->text().toUInt();
	auto pointID = mesh->pointIndex(picked);
	Surface_mesh::Vertex_index v(pointID);
	QMatrix4x4 transform;
	transform.scale(1.001f);
	if (_areaType->currentIndex() == 0) { // Barycentric
		if (_scene->node("BarycentricArea") != nullptr) {
			_scene->node("BarycentricArea")->graphicsComponent()->setVisible(true);
		}
		else {
			const double one_third = 1.0 / 3.0;
			auto node = _scene->addNode("MainMesh", "BarycentricArea", transform);
			auto graphics = std::make_unique<PrimitiveGraphics>(*_glWidget, true);
			for (const auto& he : surfaceMesh->halfedges_around_target(surfaceMesh->halfedge(v))) {
				auto p1 = surfaceMesh->point(surfaceMesh->source(he));
				auto p2 = surfaceMesh->point(surfaceMesh->target(he));
				auto p3 = surfaceMesh->point(surfaceMesh->target(surfaceMesh->next(he)));
				auto mid1 = CGAL::midpoint(p2, p1);
				auto mid2 = CGAL::midpoint(p2, p3);
				auto center = CGAL::barycenter(p1, one_third, p2, one_third, p3);
				graphics->addTriangleFill(cgalToQt(mid1), cgalToQt(p2), cgalToQt(center));
				graphics->addTriangleFill(cgalToQt(mid2), cgalToQt(center), cgalToQt(p2));
			}
			graphics->setColor(QVector3D(0.0f, 0.0f, 1.0f));
			graphics->setTransparency(0.35f);
			node->addGraphicsComponent(std::move(graphics));
		}
	}
	else if (_areaType->currentIndex() == 1) { // Voronoi
		if (_scene->node("VoronoiArea") != nullptr) {
			_scene->node("VoronoiArea")->graphicsComponent()->setVisible(true);
		}
		else {
			auto node = _scene->addNode("MainMesh", "VoronoiArea", transform);
			auto graphics = std::make_unique<PrimitiveGraphics>(*_glWidget, true);
			for (const auto& he : surfaceMesh->halfedges_around_target(surfaceMesh->halfedge(v))) {
				auto p1 = surfaceMesh->point(surfaceMesh->source(he));
				auto p2 = surfaceMesh->point(surfaceMesh->target(he));
				auto p3 = surfaceMesh->point(surfaceMesh->target(surfaceMesh->next(he)));
				auto mid1 = CGAL::midpoint(p2, p1);
				auto mid2 = CGAL::midpoint(p2, p3);
				auto center = CGAL::circumcenter(p1, p2, p3);
				graphics->addTriangleFill(cgalToQt(mid1), cgalToQt(p2), cgalToQt(center));
				graphics->addTriangleFill(cgalToQt(mid2), cgalToQt(center), cgalToQt(p2));
			}
			graphics->setColor(QVector3D(0.0f, 0.0f, 1.0f));
			graphics->setTransparency(0.35f);
			node->addGraphicsComponent(std::move(graphics));
		}
	}
	else { // Mixed
		if (_scene->node("MixedArea") != nullptr) {
			_scene->node("MixedArea")->graphicsComponent()->setVisible(true);
		}
		else {
			auto node = _scene->addNode("MainMesh", "MixedArea", transform);
			auto graphics = std::make_unique<PrimitiveGraphics>(*_glWidget, true);
			for (const auto& he : surfaceMesh->halfedges_around_target(surfaceMesh->halfedge(v))) {
				auto p1 = surfaceMesh->point(surfaceMesh->source(he));
				auto p2 = surfaceMesh->point(surfaceMesh->target(he));
				auto p3 = surfaceMesh->point(surfaceMesh->target(surfaceMesh->next(he)));
				auto mid1 = CGAL::midpoint(p2, p1);
				auto mid2 = CGAL::midpoint(p2, p3);
				Point_3 center;
				if (CGAL::angle(p1, p2, p3) == CGAL::OBTUSE) {
					center = CGAL::midpoint(p1, p3);
					graphics->addTriangleFill(cgalToQt(mid1), cgalToQt(p2), cgalToQt(center));
					graphics->addTriangleFill(cgalToQt(mid2), cgalToQt(center), cgalToQt(p2));
				}
				else if (CGAL::angle(p2, p3, p1) == CGAL::OBTUSE ||
					CGAL::angle(p3, p1, p2) == CGAL::OBTUSE) {
					graphics->addTriangleFill(cgalToQt(mid2), cgalToQt(mid1), cgalToQt(p2));
				}
				else { // Triangle is acute or right
					center = CGAL::circumcenter(p1, p2, p3);
					graphics->addTriangleFill(cgalToQt(mid1), cgalToQt(p2), cgalToQt(center));
					graphics->addTriangleFill(cgalToQt(mid2), cgalToQt(center), cgalToQt(p2));
				}
			}
			graphics->setColor(QVector3D(0.0f, 0.0f, 1.0f));
			graphics->setTransparency(0.35f);
			node->addGraphicsComponent(std::move(graphics));
		}
	}
}
