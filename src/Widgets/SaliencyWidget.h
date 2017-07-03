#pragma once

#include "Editor/ProcessWidget.h"
#include "Core/GeomBase.h"

#include <QMatrix4x4>
#include <vector>

class SaliencyWidget : public ProcessWidget
{
	Q_OBJECT

public:
	SaliencyWidget(QWidget* parent, GLWidget* glWidget);
	~SaliencyWidget();

	void activate() override;
	void deactivate() override;
	void onImport(GeomInfo* info) override;

public Q_SLOTS:
	void spinImage();
	void viewSpinImages();

private:
	std::vector<Vector_3> _vertexNormals;
	std::vector<std::vector<float>> _spinImages;
	unsigned _geomID;
	GeomType _geomType;
	QMatrix4x4 _originalTrans;
	QMatrix4x4 _trans;

	QLineEdit* _binSize;
	QLineEdit* _imageWidth;
	QLineEdit* _supportAngle;
	QLineEdit* _spinID;
	QLabel* _spinImage;
	std::vector<uchar> _imgData;
};