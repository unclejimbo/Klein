#include "Editor/ProcessWidget.h"

ProcessWidget::ProcessWidget(Scene* scene, QWidget* parent, GLWidget* glWidget)
	: QWidget(parent), _scene(scene), _glWidget(glWidget)
{
}

ProcessWidget::~ProcessWidget() = default;

bool ProcessWidget::initialized() const
{
	return _initialized;
}

void ProcessWidget::setInitialized(bool init)
{
	_initialized = init;
}

void ProcessWidget::onPicked(const PickingInfo& info)
{
}
