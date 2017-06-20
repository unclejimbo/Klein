#include "Editor/ProcessWidget.h"

ProcessWidget::ProcessWidget(QWidget* parent, GLWidget* glWidget)
	: QWidget(parent), _glWidget(glWidget)
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

void ProcessWidget::onInitializeScene(Scene* scene)
{
	_scene = scene;
}
