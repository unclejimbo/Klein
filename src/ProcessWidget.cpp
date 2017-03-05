#include "ProcessWidget.h"

ProcessWidget::ProcessWidget(QWidget* parent, GLWidget* glWidget)
	: QWidget(parent), _glWidget(glWidget)
{
}

ProcessWidget::~ProcessWidget() = default;

void ProcessWidget::onInitializeScene(Scene* scene)
{
	_scene = scene;
}
