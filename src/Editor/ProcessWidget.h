#pragma once

#include "Core/MeshIO.h"
#include "Core/Scene.h"
#include "Editor/GLWidget.h"

#include <QtWidgets>

class ProcessWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ProcessWidget(QWidget* parent, GLWidget* glWidget);
	virtual ~ProcessWidget();

public Q_SLOTS:
	virtual void onImportMesh(MeshInfo* info) = 0; // Clear all states and info
	void onInitializeScene(Scene* scene);

protected:
	GLWidget* _glWidget;
	Scene* _scene;
};
