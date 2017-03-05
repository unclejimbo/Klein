#include "UprightWidget.h"

#include "Color.h"
#include "Logger.h"
#include "ResourceManager.h"
#include "Util.h"
#include "VertexColorMeshNode.h"
#include <euclid/Polyhedron_3.h>
#include <euclid/OBB.h>
#include <euclid/ViewSelection.h>

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QImage>
#include <QOpenGLFrameBufferObject>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_length_cost.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>
#include <algorithm>
#include <utility>
#include <vector>
#include <array>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#ifdef _WIN32
#pragma warning(pop)
#endif

UprightWidget::UprightWidget(QWidget* parent, GLWidget* glWidget)
	: ProcessWidget(parent, glWidget)
{
	auto algoGroup = new QGroupBox("Algorithm", this);
	auto runAlgo = new QGroupBox("Run Algorithm", algoGroup);
	auto go = new QPushButton("Go", runAlgo);
	auto step1 = new QGroupBox("Step1: Computing ConvHull", algoGroup);
	auto convHull = new QPushButton("Go", step1);
	auto step2 = new QGroupBox("Step2: ConvHull Simplification", algoGroup);
	_iterations = new QSpinBox(step2);
	_iterations->setMinimum(1);
	auto convHullSimp = new QPushButton("Go", step2);
	auto step3 = new QGroupBox("Step3: Eliminating Small ConvHull Faces", algoGroup);
	_faceAreaThreshold = new QSpinBox(step3);
	_faceAreaThreshold->setMinimum(0);
	_faceAreaThreshold->setMaximum(100);
	_faceAreaThreshold->setValue(10);
	_faceAreaThreshold->setSuffix("%");
	auto elimSmallFace = new QPushButton("Go", step3);
	auto step4 = new QGroupBox("Step4: Clustering ConvHull Faces", algoGroup);
	_bandwidth = new QDoubleSpinBox(step4);
	auto cluster = new QPushButton("Go", step4);
	auto step5 = new QGroupBox("Step5: Scoring Candidate Bases", algoGroup);
	auto score = new QPushButton("Go", step5);

	auto resultGroup = new QGroupBox("Results", this);
	_scoreTable = new QTableWidget(0, 5, resultGroup);
	_scoreTable->setHorizontalHeaderLabels(QStringList({ "View", "Reflection", "Steady", "Area", "Score" }));
	_scoreTable->resizeColumnsToContents();
	auto scoreTableHHeader = _scoreTable->horizontalHeader();
	auto scoreTableVHeader = _scoreTable->verticalHeader();
	auto accuracy = new QLabel("Accuracy: ", resultGroup);
	_accuracy = new QLabel("N/A", resultGroup);

	auto visGroup = new QGroupBox("Visualisation", this);
	_meshCheck = new QCheckBox("Hide Mesh", visGroup);
	_convHullCheck = new QCheckBox("Hide ConvHull", visGroup);
	_viewSphereCheck = new QCheckBox("Hide ViewSphere", visGroup);

	auto layout = new QVBoxLayout(this);
	{
		layout->addWidget(algoGroup);
		auto algoLayout = new QVBoxLayout(algoGroup);
		{
			algoLayout->addWidget(runAlgo);
			auto runLayout = new QVBoxLayout(runAlgo);
			{
				runLayout->addWidget(go);
			}
		}
		{
			algoLayout->addWidget(step1);
			auto step1Layout = new QVBoxLayout(step1);
			{
				step1Layout->addWidget(convHull);
			}
		}
		{
			algoLayout->addWidget(step2);
			auto step2Layout = new QVBoxLayout(step2);
			{
				auto iterLayout = new QFormLayout();
				step2Layout->addLayout(iterLayout);
				{
					iterLayout->addRow("Iterations", _iterations);
				}
			}
			{
				step2Layout->addWidget(convHullSimp);
			}
		}
		{
			algoLayout->addWidget(step3);
			auto step3Layout = new QVBoxLayout(step3);
			{
				auto thresLayout = new QFormLayout();
				step3Layout->addLayout(thresLayout);
				{
					thresLayout->addRow("Threshold", _faceAreaThreshold);
				}
			}
			{
				step3Layout->addWidget(elimSmallFace);
			}
		}
		{
			algoLayout->addWidget(step4);
			auto step4Layout = new QVBoxLayout(step4);
			{
				auto bandLayout = new QFormLayout();
				step4Layout->addLayout(bandLayout);
				{
					bandLayout->addRow("Bandwidth", _bandwidth);
				}
			}
			{
				step4Layout->addWidget(cluster);
			}
		}
		{
			algoLayout->addWidget(step5);
			auto step5Layout = new QVBoxLayout(step5);
			{
				step5Layout->addWidget(score);
			}
		}
	}
	{
		layout->addWidget(resultGroup);
		auto resultLayout = new QVBoxLayout(resultGroup);
		{
			resultLayout->addWidget(_scoreTable);
		}
		{
			auto accuLayout = new QHBoxLayout();
			resultLayout->addLayout(accuLayout);
			{
				accuLayout->addWidget(accuracy);
			}
			{
				accuLayout->addWidget(_accuracy);
			}
		}
	}
	{
		layout->addWidget(visGroup);
		auto visLayout = new QGridLayout(visGroup);
		{
			visLayout->addWidget(_meshCheck, 0, 0);
		}
		{
			visLayout->addWidget(_convHullCheck, 0, 1);
		}
		{
			visLayout->addWidget(_viewSphereCheck, 0, 2);
		}
	}

	connect(convHull, &QPushButton::clicked, this, &UprightWidget::_step1);
	connect(convHullSimp, &QPushButton::clicked, this, &UprightWidget::_step2);
	connect(elimSmallFace, &QPushButton::clicked, this, &UprightWidget::_step3);
	connect(cluster, &QPushButton::clicked, this, &UprightWidget::_step4);
	connect(score, &QPushButton::clicked, this, &UprightWidget::_step5);
	connect(_meshCheck, &QCheckBox::stateChanged, this, &UprightWidget::_hideMesh);
	connect(_convHullCheck, &QCheckBox::stateChanged, this, &UprightWidget::_hideConvHull);
	connect(_viewSphereCheck, &QCheckBox::stateChanged, this, &UprightWidget::_hideViewSphere);
	connect(scoreTableHHeader, &QHeaderView::sectionClicked, this, &UprightWidget::_onScoreTableHHeaderClicked);
	connect(scoreTableVHeader, &QHeaderView::sectionClicked, this, &UprightWidget::_onScoreTableVHeaderClicked);
	connect(_scoreTable, &QTableWidget::cellClicked, this, &UprightWidget::_onScoreTableCellClicked);
}

UprightWidget::~UprightWidget() = default;

void UprightWidget::onImportMesh(const MeshInfo& info)
{
	_meshCheck->setChecked(false);
	_convHullCheck->setChecked(false);
	_viewSphereCheck->setChecked(false);

	ResourceManager::instance().removeGLBuffer("UprightConvHull_VeretexBuffer");
	ResourceManager::instance().removeGLBuffer("UprightConvHull_NormalBuffer");
	ResourceManager::instance().removeGLBuffer("UprightConvHull_ColorBuffer");
	ResourceManager::instance().removeGLBuffer("UprightViewSphere_VeretexBuffer");
	ResourceManager::instance().removeGLBuffer("UprightViewSphere_ColorBuffer");

	_scene->node("MainMesh")->addRenderPass(RenderPass::offscreen);
	_scene->removeNode("UprightConvHull");
	_scene->removeNode("UprightViewSphere");

	_scoreTable->setRowCount(0);
}

void UprightWidget::_step1()
{
	if (auto cMesh = ResourceManager::instance().mesh("MainMesh").first->cMesh()) {
		CGAL::convex_hull_3(cMesh->points_begin(), cMesh->points_end(), _hull);
	}
	else {
		auto& vertices = ResourceManager::instance().mesh("MainMesh").first->vertices();
		std::vector<Kernel::Point_3> cVertices(vertices.size());
		std::transform(vertices.begin(), vertices.end(), cVertices.begin(), eigenToCgal<Kernel>);
		CGAL::convex_hull_3(cVertices.begin(), cVertices.end(), _hull);
	}

	addGLBuffer("UprightConvHull", _hull);
	std::vector<QVector4D> colors(_hull.size_of_facets() * 3, QVector4D(0.0f, 0.0f, 1.0f, 0.5f));
	ResourceManager::instance().addGLBuffer("UprightConvHull_ColorBuffer", colors);

	auto hullNode = dynamic_cast<VertexColorMeshNode*>(_scene->addNode(_scene->rootNode(),
		SceneNodeType::alphaVertexColorMeshNode, "UprightConvHull", _scene->node("MainMesh")->transform()));
	hullNode->attachMesh("UprightConvHull_VertexBuffer", "UprightConvHull_ColorBuffer");

	_glWidget->update();
}

void UprightWidget::_step2()
{
	using namespace CGAL::Surface_mesh_simplification;

	Count_stop_predicate<CMesh> stop(100);
	edge_collapse(_hull, stop,
		CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index, _hull)).
		halfedge_index_map(get(CGAL::halfedge_external_index, _hull)).
		get_cost(Edge_length_cost<CMesh>()).
		get_placement(Midpoint_placement<CMesh>()));

	addGLBuffer("UprightConvHull", _hull);
	std::vector<QVector4D> colors(_hull.size_of_facets() * 3, QVector4D(0.0f, 0.0f, 1.0f, 0.5f));
	ResourceManager::instance().addGLBuffer("UprightConvHull_ColorBuffer", colors);

	_glWidget->update();
}

void UprightWidget::_step3()
{
	size_t i = 0;
	for (auto f = _hull.facets_begin(); f != _hull.facets_end(); ++f) {
		f->id() = i++;
	}

	std::vector<std::pair<float, CMesh::Facet_handle::value_type>> hullFacetAreas(_hull.size_of_facets());
	std::transform(_hull.facets_begin(), _hull.facets_end(), hullFacetAreas.begin(),
		[](const CMesh::Facet_handle::value_type& f) {
			auto area = euclid::compute_facet_area<CMesh>(f);
			return std::make_pair(area, f);
		}
	);
	auto mm = std::minmax_element(hullFacetAreas.begin(), hullFacetAreas.end(),
		[](const decltype(hullFacetAreas.begin())::value_type& area1, const decltype(hullFacetAreas.begin())::value_type& area2) {
			return area1.first < area2.first;
		}
	);
	auto smallArea = mm.first->first + (mm.second->first - mm.first->first) * _faceAreaThreshold->value() / 100.0f;
	auto largeEnd = std::partition(hullFacetAreas.begin(), hullFacetAreas.end(),
		[smallArea](const decltype(hullFacetAreas.begin())::value_type& area) {
			return area.first > smallArea;
		}
	);
	_largeHullFacets.clear();
	std::transform(hullFacetAreas.begin(), largeEnd, std::back_inserter(_largeHullFacets),
		[](const decltype(hullFacetAreas.begin())::value_type& area) {
			return area.second;
		}
	);

	std::vector<QVector4D> colors(_hull.size_of_facets() * 3, QVector4D(0.0f, 0.0f, 1.0f, 0.5f));
	for (auto f = largeEnd; f != hullFacetAreas.end(); ++f) {
		auto idx = f->second.id();
		colors[idx * 3 + 0] = QVector4D(0.0f, 0.0f, 1.0f, 0.0f);
		colors[idx * 3 + 1] = QVector4D(0.0f, 0.0f, 1.0f, 0.0f);
		colors[idx * 3 + 2] = QVector4D(0.0f, 0.0f, 1.0f, 0.0f);
	}
	ResourceManager::instance().addGLBuffer("UprightConvHull_ColorBuffer", colors);

	_glWidget->update();
}

void UprightWidget::_step4()
{
	size_t nClusters = 0;
	_labels.clear();
	_labels.resize(_largeHullFacets.size());
	std::vector<QVector3D> hullNormals(_largeHullFacets.size());
	std::transform(_largeHullFacets.begin(), _largeHullFacets.end(), hullNormals.begin(),
		[](const CMesh::Face_handle::value_type& f) {
			auto n = euclid::compute_facet_normal<CMesh>(f);
			return eigenToQt(n);
		}
	);

	auto scriptName = PyUnicode_DecodeFSDefault("cluster");
	auto cluster = PyImport_Import(scriptName);
	Py_DECREF(scriptName);
	if (cluster == nullptr) {
		PyErr_Print();
		return;
	}

	auto meanshift = PyObject_GetAttrString(cluster, "meanshift");
	if (meanshift != nullptr && PyCallable_Check(meanshift)) {
		// Feed data into python
		auto norms = PyList_New(hullNormals.size());
		for (size_t i = 0; i < hullNormals.size(); ++i) {
			auto& n = hullNormals[i];
			auto vec = PyList_New(3);
			for (size_t j = 0; j < 3; ++j) {
				auto val = PyFloat_FromDouble(n[j]);
				PyList_SetItem(vec, j, val);
			}
			PyList_SetItem(norms, i, vec);
		}
		auto args = PyTuple_New(1);
		PyTuple_SetItem(args, 0, norms);

		auto results = PyObject_CallObject(meanshift, args);
		Py_DECREF(args);

		// Read data out of python
		if (results != nullptr) {
			auto nClusters_py = PyTuple_GetItem(results, 0);
			auto labels_py = PyTuple_GetItem(results, 1);
			auto centers_py = PyTuple_GetItem(results, 2);

			nClusters = PyLong_AsSize_t(nClusters_py);

			for (size_t i = 0; i < _labels.size(); ++i) {
				auto l = PyList_GetItem(labels_py, i);
				if (l == nullptr) {
					PyErr_Print();
					return;
				}
				else {
					_labels[i] = PyLong_AsSize_t(l);
				}
			}

			for (size_t i = 0; i < nClusters; ++i) {
				auto c = PyList_GetItem(centers_py, i);
				if (c == nullptr) {
					PyErr_Print();
					return;
				}
				else {
					auto cx = PyList_GetItem(c, 0);
					auto cy = PyList_GetItem(c, 1);
					auto cz = PyList_GetItem(c, 2);
					auto x = static_cast<float>(PyFloat_AsDouble(cx));
					auto y = static_cast<float>(PyFloat_AsDouble(cy));
					auto z = static_cast<float>(PyFloat_AsDouble(cz));
					_candidateBaseNormals.push_back(QVector3D(x, y, z).normalized());
				}
			}
			Py_DECREF(results);
		}
		else {
			PyErr_Print();
			return;
		}
		Py_DECREF(meanshift);
	}
	Py_DECREF(cluster);

	_scoreTable->setRowCount(nClusters);

	std::vector<QVector4D> colors(_hull.size_of_facets() * 3, QVector4D(0.0f, 0.0f, 0.0f, 0.0f));
	for (size_t i = 0; i < _largeHullFacets.size(); ++i) {
		auto color = Color::get(_labels[i]);
		colors[_largeHullFacets[i].id() * 3 + 0] = QVector4D(color, 0.5f);
		colors[_largeHullFacets[i].id() * 3 + 1] = QVector4D(color, 0.5f);
		colors[_largeHullFacets[i].id() * 3 + 2] = QVector4D(color, 0.5f);
	}
	ResourceManager::instance().addGLBuffer("UprightConvHull_ColorBuffer", colors);

	_glWidget->update();
}

void UprightWidget::_step5()
{
	_viewSelection();
	_symmetryDetection();
	_evalSteadiness();
	_evalArea();
}

void UprightWidget::_hideMesh(int state)
{
	if (state == Qt::CheckState::Checked) {
		_scene->node("MainMesh")->setVisible(false);
	}
	else {
		_scene->node("MainMesh")->setVisible(true);
	}
	_glWidget->update();
}

void UprightWidget::_hideConvHull(int state)
{
	if (state == Qt::CheckState::Checked) {
		_scene->node("UprightConvHull")->setVisible(false);
	}
	else {
		_scene->node("UprightConvHull")->setVisible(true);
	}
	_glWidget->update();
}

void UprightWidget::_hideViewSphere(int state)
{
	if (state == Qt::CheckState::Checked) {
		_scene->node("UprightViewSphere")->setVisible(false);
	}
	else {
		_scene->node("UprightViewSphere")->setVisible(true);
	}
	_glWidget->update();
}

void UprightWidget::_onScoreTableHHeaderClicked(int idx)
{
}

void UprightWidget::_onScoreTableVHeaderClicked(int idx)
{
}

void UprightWidget::_onScoreTableCellClicked(int row, int col)
{
}

void UprightWidget::_viewSelection()
{
	std::unique_ptr<euclid::OBB<CMesh>> obb = nullptr;
	auto mesh = ResourceManager::instance().mesh("MainMesh").first;
	if (mesh->cMesh() != nullptr) {
		obb = std::make_unique<euclid::OBB<CMesh>>(*mesh->cMesh());
	}
	else {
		obb = std::make_unique<euclid::OBB<CMesh>>(mesh->vertices());
	}

	Eigen::Vector3f dir1 = obb->directions()[0] * 3.0f;
	Eigen::Vector3f dir2 = obb->directions()[1] * 3.0f;
	Eigen::Vector3f dir3 = obb->directions()[2] * 3.0f;
	std::vector<QVector3D> eyes{ eigenToQt(dir1), eigenToQt(dir2), eigenToQt(dir3), 
		-eigenToQt(dir1), -eigenToQt(dir2), -eigenToQt(dir3) };
	std::vector<QVector3D> ups{ eigenToQt(dir2), eigenToQt(dir3), eigenToQt(dir1),
		-eigenToQt(dir2), -eigenToQt(dir3), -eigenToQt(dir1) };
	std::array<Eigen::MatrixXf, 6> zbuffers;
	for (size_t i = 0; i < eyes.size(); ++i) {
		std::vector<float> depth;
		_scene->camera()->lookAt(eyes[i], QVector3D(0.0f, 0.0f, 0.0f), ups[i]);
		_glWidget->readDepthBuffer(depth);
		Eigen::Map<Eigen::MatrixXf> depthBuffer(depth.data(), _glWidget->height(), _glWidget->width());
		zbuffers[i] = depthBuffer;
	}

	std::vector<Eigen::Vector3f> viewPoints;
	std::vector<float> viewScores;
	std::vector<size_t> sphereIndices;
	if (mesh->cMesh() != nullptr) {
		euclid::proxy_view_selection(*mesh->cMesh(), zbuffers, viewPoints, viewScores, 3, &sphereIndices, obb.get());
		auto bestViewIter = std::max_element(viewScores.begin(), viewScores.end());
		auto bestViewOffset = bestViewIter - viewScores.begin();
		Eigen::Vector3f bestView = viewPoints[bestViewOffset] / obb->radius();
		_scene->camera()->lookAt(eigenToQt(bestView), _scene->camera()->focus(), _scene->camera()->worldUp());
	}
	else {

	}

	std::vector<QVector3D> sphereVertices(sphereIndices.size());
	std::transform(sphereIndices.begin(), sphereIndices.end(), sphereVertices.begin(),
		[&viewPoints](size_t idx) { return eigenToQt(viewPoints[idx]); });
	ResourceManager::instance().addGLBuffer("UprightViewSphere_VertexBuffer", sphereVertices);
	std::vector<float> sphereScores(sphereIndices.size());
	std::transform(sphereIndices.begin(), sphereIndices.end(), sphereScores.begin(),
		[&viewScores](size_t idx) { return viewScores[idx]; });
	auto colors = temperature(sphereScores, QVector4D(1.0f, 0.0f, 0.0f, 0.7f), QVector4D(0.0f, 0.0f, 1.0f, 0.7f));
	ResourceManager::instance().addGLBuffer("UprightViewSphere_ColorBuffer", colors);
	auto sphereNode = dynamic_cast<VertexColorMeshNode*>(_scene->addNode(
		_scene->rootNode(), SceneNodeType::vertexColorMeshNode, "UprightViewSphere", _scene->node("MainMesh")->transform()));
	sphereNode->attachMesh("UprightViewSphere_VertexBuffer", "UprightViewSphere_ColorBuffer");
	sphereNode->setVisible(false);
	_viewSphereCheck->setChecked(true);
	
	_glWidget->update();
}

void UprightWidget::_symmetryDetection()
{
}

void UprightWidget::_evalSteadiness()
{
}

void UprightWidget::_evalArea()
{
}
