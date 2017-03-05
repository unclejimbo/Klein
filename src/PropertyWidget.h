#pragma once

#include "ProcessWidget.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <memory>

#ifdef _WIN32
#pragma warning(pop)
#endif

class PropertyWidget : public ProcessWidget
{
	Q_OBJECT

public:
	explicit PropertyWidget(QWidget* parent, GLWidget* glWidget);
	~PropertyWidget();

public Q_SLOTS:
	void onImportMesh(MeshInfo* info) override;
	void showAABB(int state);
	void showOBB(int state);
	void showSphere(int state);
	void onColorChanged(int state);
	 
private:
	bool _valid = false;
	QLabel* _fileName;
	QLabel* _nVertices;
	QLabel* _nFaces;
	QLabel* _euler;
	QLabel* _center;
	QLabel* _radius;
	QLabel* _minX;
	QLabel* _maxX;
	QLabel* _minY;
	QLabel* _maxY;
	QLabel* _minZ;
	QLabel* _maxZ;
	QCheckBox* _aabb;
	QCheckBox* _obb;
	QCheckBox* _sphere;
	QComboBox* _color;
};
