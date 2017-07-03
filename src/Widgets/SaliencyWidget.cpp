#include "Widgets/SaliencyWidget.h"
#include "Core/ResourceManager.h"
#include "Core/PrimitiveGraphics.h"
#include "Core/Util.h"

#include <Euclid/Analysis/Descriptor.h>
#include <Euclid/Geometry/MeshProperties.h>
#include <Euclid/Geometry/Surface_mesh.h>
#include <QPixmap>
#include <QImage>
#include <memory>
#include <ostream>

SaliencyWidget::SaliencyWidget(QWidget* parent, GLWidget* glWidget)
	: ProcessWidget(parent, glWidget)
{
	auto layout = new QVBoxLayout(this);
	this->setLayout(layout);

	auto vdGroup = new QGroupBox("Vertex Distinctness", this);
	layout->addWidget(vdGroup);
	auto vdLayout = new QGridLayout(vdGroup);
	vdGroup->setLayout(vdLayout);
	auto binSizeLabel = new QLabel("Bin Size:", vdGroup);
	vdLayout->addWidget(binSizeLabel, 0, 0);
	_binSize = new QLineEdit("1", vdGroup);
	vdLayout->addWidget(_binSize, 0, 1);
	auto imageWidthLabel = new QLabel("Image Width:", vdGroup);
	vdLayout->addWidget(imageWidthLabel, 1, 0);
	_imageWidth = new QLineEdit("15", vdGroup);
	vdLayout->addWidget(_imageWidth, 1, 1);
	auto supportAngleLabel = new QLabel("Support Angle:", vdGroup);
	vdLayout->addWidget(supportAngleLabel, 2, 0);
	_supportAngle = new QLineEdit("60", vdGroup);
	vdLayout->addWidget(_supportAngle, 2, 1);
	auto computeSpin = new QPushButton("Compute SpinImages", vdGroup);
	vdLayout->addWidget(computeSpin, 3, 0);
	auto spinIDLabel = new QLabel("SpinImage ID to View:", vdGroup);
	vdLayout->addWidget(spinIDLabel, 4, 0);
	_spinID = new QLineEdit("0", vdGroup);
	vdLayout->addWidget(_spinID, 4, 1);
	auto viewSpin = new QPushButton("View SpinImages", vdGroup);
	vdLayout->addWidget(viewSpin, 5, 0);
	_spinImage = new QLabel(vdGroup);
	vdLayout->addWidget(_spinImage, 5, 1);

	auto seGroup = new QGroupBox("Shape Extremities", this);
	layout->addWidget(seGroup);
	auto seLayout = new QGridLayout(seGroup);
	seGroup->setLayout(seLayout);

	auto paGroup = new QGroupBox("Patch Association", this);
	layout->addWidget(paGroup);

	connect(computeSpin, &QPushButton::clicked, this, &SaliencyWidget::spinImage);
	connect(viewSpin, &QPushButton::clicked, this, &SaliencyWidget::viewSpinImages);
}

SaliencyWidget::~SaliencyWidget() = default;

void SaliencyWidget::activate()
{
	auto node = _scene->node("SpinImageVertex");
	if (node != nullptr) {
		node->graphicsComponent()->setVisible(false);
	}
	_scene->node("MainMesh")->setTransform(_trans);
	_glWidget->update();
}

void SaliencyWidget::deactivate()
{
	auto node = _scene->node("SpinImageVertex");
	if (node != nullptr) {
		node->graphicsComponent()->setVisible(true);
	}
	_scene->node("MainMesh")->setTransform(_originalTrans);
	_glWidget->update();
}

void SaliencyWidget::onImport(GeomInfo* info)
{
	_spinImages.clear();
	_scene->removeNode("SpinImageVertex");
	_spinImage->clear();
	_originalTrans = _scene->node("MainMesh")->transform();
	_trans = _originalTrans;

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

	Euclid::SM_PropertyMap<Vector_3> faceNormalMap(faceNormals);
	_vertexNormals.clear();
	_vertexNormals.reserve(mesh->number_of_faces());
	for (const auto& v : mesh->vertices()) {
		auto n = Euclid::vertex_normal(v, *mesh, faceNormalMap, Euclid::NormalWeighting::face_area);
		_vertexNormals.push_back(n);
	}

	Euclid::SM_PropertyMap<Vector_3> vertexNormalMap(_vertexNormals);
	auto binSize = _binSize->text().toUInt();
	auto imgWidth = _imageWidth->text().toUInt();
	auto supportAngle = _supportAngle->text().toFloat();
	Euclid::spin_image(*mesh, vertexNormalMap, _spinImages, binSize, imgWidth, supportAngle);

	_glWidget->update();
}

void SaliencyWidget::viewSpinImages()
{
	auto spinID = _spinID->text().toUInt();
	if (spinID >= _spinImages.size()) {
		std::cerr << "Invalid SpinImageID" << std::endl;
		return;
	}
	auto imageWidth = _imageWidth->text().toInt();

	_imgData.clear();
	_imgData.reserve(_spinImages[spinID].size());
	for (const auto& pixel : _spinImages[spinID]) {
		_imgData.push_back(static_cast<uchar>(pixel * 255));
	}
	QImage img(_imgData.data(), imageWidth, imageWidth, QImage::Format_Grayscale8);
	auto display = img.scaled(128, 128);
	auto pixmap = QPixmap::fromImage(display);
	_spinImage->setPixmap(pixmap);

	// Draw the vertex
	auto mesh = ResourceManager::instance().mesh(_geomID);
	auto vertex = mesh->point(spinID);
	auto node = _scene->node("SpinImageVertex");
	if (node == nullptr) {
		node = _scene->addNode("MainMesh", "SpinImageVertex");
	}
	auto graphics = std::make_unique<PrimitiveGraphics>(*_glWidget);
	graphics->addPoint(vertex.x(), vertex.y(), vertex.z());
	graphics->setPointSize(10);
	auto p1 = mesh->point(spinID);
	auto p2 = p1 + _vertexNormals[spinID] * 0.05;
	graphics->addLine(cgalToQt(p1), cgalToQt(p2));
	node->addGraphicsComponent(std::move(graphics));

	// Transform the object
	auto from_o = vertex;
	auto normal = _vertexNormals[spinID];
	CGAL::Plane_3<Kernel> plane(vertex, normal);
	auto from_x = from_o + Euclid::normalized(plane.base1());
	auto from_y = from_o + normal;
	
	CGAL::Aff_transformation_3<Kernel> transform;
	Euclid::transform_between_2_coord_systems(
		from_o, from_x, from_y,
		Point_3(0, 0, 0), Point_3(1, 0, 0), Point_3(0, 1, 0),
		transform);
	_trans = QMatrix4x4(transform.m(0, 0), transform.m(0, 1), transform.m(0, 2), transform.m(0, 3),
		transform.m(1, 0), transform.m(1, 1), transform.m(1, 2), transform.m(1, 3),
		transform.m(2, 0), transform.m(2, 1), transform.m(2, 2), transform.m(2, 3),
		0, 0, 0, 1);
	auto parent = _scene->node("MainMesh");
	parent->setTransform(_trans);

	_glWidget->update();
}
