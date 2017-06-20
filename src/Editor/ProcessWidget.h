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

	bool initialized() const;
	void setInitialized(bool init);
	virtual void activate() = 0;
	virtual void deactivate() = 0;
	virtual void onImport(GeomInfo* info) = 0;
	virtual void onPicked(const PickingInfo& info);

public Q_SLOTS:
	void onInitializeScene(Scene* scene);

protected:
	GLWidget* _glWidget;
	Scene* _scene;
	bool _initialized = false;
};
