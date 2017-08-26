#pragma once

#include "Core/Scene.h"
#include "Core/SceneNode.h"
#include "Core/Message.h"
#include "Editor/GLWidget.h"

#include <QtWidgets>

class ProcessWidget : public QWidget
{
	Q_OBJECT

public:
	ProcessWidget(Scene* scene, QWidget* parent, GLWidget* glWidget, bool requireGeometry = true);
	virtual ~ProcessWidget();

	bool initialized();
	bool activated();
	void onImport(const GeomInfo& info);
	void onActivated();
	void onDeactivated();
	void onClear();
	void onPicked(const PickingInfo& info);

protected:
	virtual void _onImport(const GeomInfo& info);
	virtual void _onActivated();
	virtual void _onDeactivated();
	virtual void _onClear();
	virtual void _onPicked(const PickingInfo& info);

protected:
	Scene* _scene;
	GLWidget* _glWidget;
	bool _requireGeometry = true; // False when mesh or pc are not required for this widget to operate
	bool _initialized = false; // Set true onImport, set false onClear
	bool _activated = false; // Set true onActivated, set false onDeactivated
};
