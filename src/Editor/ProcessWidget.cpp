#include "Editor/ProcessWidget.h"

ProcessWidget::ProcessWidget(QWidget* parent, GLWidget* glWidget)
	: QWidget(parent), _glWidget(glWidget)
{
}

ProcessWidget::~ProcessWidget() = default;

void ProcessWidget::enablePicking(bool picking)
{
	_picking = picking;
}

void ProcessWidget::onPicked(const PickingInfo& info)
{
	if (_picking) {
		_onPickedImp(info);
	}
}

void ProcessWidget::onInitializeScene(Scene* scene)
{
	_scene = scene;
}

void ProcessWidget::_onPickedImp(const PickingInfo& info)
{
}
