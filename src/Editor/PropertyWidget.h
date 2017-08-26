#pragma once

#include "Editor/ProcessWidget.h"

class PropertyWidget : public ProcessWidget
{
	Q_OBJECT

public:
	PropertyWidget(Scene* scene, QWidget* parent, GLWidget* glWidget);	

public Q_SLOTS:
	void onAreaTypeChanged(int state);
	void showAABB(int state);
	void showOBB(int state);
	void onColorChanged(int state);

protected:	
	void _onImport(const GeomInfo& info) override;
	void _onActivated() override;
	void _onDeactivated() override;
	void _onClear() override;
	void _onPicked(const PickingInfo& info) override;

private:
	void _clearResources();
	void _hideAllAreaNodes();
	void _hideAllColorNodes();
	void _drawVertexArea();
	 
private:
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
	QLabel* _primType;
	QLabel* _primID;
	QComboBox* _areaType;
	QLabel* _primArea;
	QCheckBox* _aabb;
	QCheckBox* _obb;
	QComboBox* _color;

	unsigned _id;
	unsigned _valenceBuffer;
	unsigned _gaussianBuffer;
	unsigned _meanBuffer;
};
