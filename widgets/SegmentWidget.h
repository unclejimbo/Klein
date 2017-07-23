#pragma once

#include "Editor/ProcessWidget.h"

class SegmentWidget : public ProcessWidget
{
	Q_OBJECT

public:
	SegmentWidget(Scene* scene, QWidget* parent, GLWidget* glWidget);

	void activate() override;
	void deactivate() override;
	void onImport(GeomInfo* info) override;
	void onPicked(const PickingInfo& info) override;

public Q_SLOTS:
	void sample();
	void seeding();
	void segment();

private:
	QLineEdit* _lineEdit;
	std::vector<int> _picked;
	GeomType _type = GEOM_TYPE_NONE;
	unsigned _id = 0;
};
