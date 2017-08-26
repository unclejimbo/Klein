#include "Editor/ProcessWidget.h"

ProcessWidget::ProcessWidget(Scene* scene, QWidget* parent, GLWidget* glWidget, bool requireGeoemtry)
	: QWidget(parent), _scene(scene), _glWidget(glWidget), _requireGeometry(requireGeoemtry)
{
}

ProcessWidget::~ProcessWidget() = default;

bool ProcessWidget::initialized()
{
	return !_requireGeometry || _initialized;
}

bool ProcessWidget::activated()
{
	return _activated;
}

void ProcessWidget::onImport(const GeomInfo& info)
{
	if (_requireGeometry) {
		if (_initialized) {
			_onClear();
		}
		_onImport(info);
		_initialized = true;
	}
}

void ProcessWidget::onActivated()
{
	if (!_requireGeometry || _initialized) {
		_onActivated();
	}
	_activated = true;
}

void ProcessWidget::onDeactivated()
{
	if (!_requireGeometry || _initialized) {
		_onDeactivated();
	}
	_activated = false;
}

void ProcessWidget::onPicked(const PickingInfo& info)
{
	if (!_requireGeometry || _initialized) {
		_onPicked(info);
	}
}

void ProcessWidget::onClear()
{
	if (!_requireGeometry || _initialized) {
		_onClear();
		_initialized = false;
	}
}

void ProcessWidget::_onImport(const GeomInfo& info)
{
}

void ProcessWidget::_onActivated()
{
}

void ProcessWidget::_onDeactivated()
{
}

void ProcessWidget::_onClear()
{
}

void ProcessWidget::_onPicked(const PickingInfo& info)
{
}
