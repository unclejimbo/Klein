#pragma once

#include "Editor/ProcessWidget.h"

class PropertyWidget : public ProcessWidget
{
	Q_OBJECT

public:
	PropertyWidget(Scene* scene, QWidget* parent, GLWidget* glWidget);

	void activate() override;
	void deactivate() override;
	void onImport(GeomInfo* info) override;
	void onPicked(const PickingInfo& info) override;

public Q_SLOTS:
	void showAABB(int state);
	void showOBB(int state);
	void onColorChanged(int state);
	 
private:
	bool _valid = false;
	QLabel* _fileName;
	QLabel* _nVertices;
	QLabel* _nFaces;
	QLabel* _euler;
	QLabel* _center;
	QLabel* _minX;
	QLabel* _maxX;
	QLabel* _minY;
	QLabel* _maxY;
	QLabel* _minZ;
	QLabel* _maxZ;
	QRadioButton* _nothingBtn;
	QRadioButton* _vertexBtn;
	QRadioButton* _faceBtn;
	QLabel* _geomType;
	QLabel* _geomID;
	QLabel* _primType;
	QLabel* _primID;
	QCheckBox* _aabb;
	QCheckBox* _obb;
	QComboBox* _color;

	unsigned _id;
	unsigned _valenceBuffer;
};
