#pragma once

#include "GLWidget.h"
#include "MeshIO.h"
#include "Scene.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QtWidgets>

#ifdef _WIN32
#pragma warning(pop)
#endif

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
