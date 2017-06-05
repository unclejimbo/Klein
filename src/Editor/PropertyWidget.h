#pragma once

#include "Editor/ProcessWidget.h"

class PropertyWidget : public ProcessWidget
{
	Q_OBJECT

public:
	PropertyWidget(QWidget* parent, GLWidget* glWidget);
	~PropertyWidget();

public Q_SLOTS:
	void onImport(GeomInfo* info) override;
	void showAABB(int state);
	void showOBB(int state);
	void showSphere(int state);
	void onColorChanged(int state);

private:
	void _onPickedImp(const PickingInfo& info) override;
	 
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
	QLabel* _geomType;
	QLabel* _geomID;
	QLabel* _primType;
	QLabel* _primID;
	QCheckBox* _aabb;
	QCheckBox* _obb;
	QCheckBox* _sphere;
	QComboBox* _color;

	unsigned _id;
	unsigned _valenceBuffer;
};
