#pragma once

#include "Editor/ProcessWidget.h"

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

private:
	std::vector<std::vector<float>> _spinImages;
	unsigned _geomID;
	GeomType _geomType;
};