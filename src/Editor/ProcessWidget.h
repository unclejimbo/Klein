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
	ProcessWidget(Scene* scene, QWidget* parent, GLWidget* glWidget);
	virtual ~ProcessWidget();

	bool initialized() const;
	void setInitialized(bool init);
	virtual void activate() = 0;
	virtual void deactivate() = 0;
	virtual void onImport(GeomInfo* info) = 0;
	virtual void onPicked(const PickingInfo& info);

protected:
	Scene* _scene;
	GLWidget* _glWidget;
	bool _initialized = false;
};
