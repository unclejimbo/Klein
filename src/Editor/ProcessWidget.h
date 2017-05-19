#pragma once

#include "Core/GeomIO.h"
#include "Core/Scene.h"
#include "Core/SceneNode.h"
#include "Editor/GLWidget.h"

#include <QtWidgets>

class ProcessWidget : public QWidget
{
	Q_OBJECT

public:
	ProcessWidget(QWidget* parent, GLWidget* glWidget);
	virtual ~ProcessWidget();

public Q_SLOTS:
	virtual void onImport(GeomInfo* info) = 0; // Clear all states and info
	void onInitializeScene(Scene* scene);

protected:
	GLWidget* _glWidget;
	Scene* _scene;
};
