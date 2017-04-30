#include "Editor/ProcessWidget.h"

ProcessWidget::ProcessWidget(QWidget* parent, QOpenGLWidget* glWidget)
	: QWidget(parent), _glWidget(glWidget)
{
}

ProcessWidget::~ProcessWidget() = default;

void ProcessWidget::onInitializeScene(Scene* scene)
{
	_scene = scene;
}
