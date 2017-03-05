#pragma once

#include "ProcessWidget.h"
#include "Mesh.h"

class UprightWidget : public ProcessWidget
{
	Q_OBJECT

public:
	explicit UprightWidget(QWidget* parent, GLWidget* glWidget);
	~UprightWidget();

public Q_SLOTS:
	void onImportMesh(const MeshInfo& info) override;

private Q_SLOTS:
	void _step1();
	void _step2();
	void _step3();
	void _step4();
	void _step5();
	void _hideMesh(int state);
	void _hideConvHull(int state);
	void _hideViewSphere(int state);
	void _onScoreTableHHeaderClicked(int idx);
	void _onScoreTableVHeaderClicked(int idx);
	void _onScoreTableCellClicked(int row, int col);

private:
	void _viewSelection();
	void _symmetryDetection();
	void _evalSteadiness();
	void _evalArea();

private:
	QSpinBox* _iterations;
	QSpinBox* _faceAreaThreshold;
	QDoubleSpinBox* _bandwidth;
	QTableWidget* _scoreTable;
	QLabel* _accuracy;
	QCheckBox* _meshCheck;
	QCheckBox* _convHullCheck;
	QCheckBox* _viewSphereCheck;

	CMesh _hull;
	std::vector<CMesh::Facet_handle::value_type> _largeHullFacets;
	std::vector<size_t> _labels;
	std::vector<QVector3D> _candidateBaseNormals;
};
