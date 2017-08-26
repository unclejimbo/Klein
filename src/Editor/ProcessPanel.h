#pragma once

#include "Core/GeomIO.h"

#include <QDockWidget>
#include <QString>
#include <memory>

class Scene;
class MainWindow;
class GLWidget;
class ProcessWidget;
class QStackedWidget;
class QComboBox;
struct GeomInfo;
struct PickingInfo;

class ProcessPanel : public QDockWidget
{
	Q_OBJECT

public:
	ProcessPanel(Scene* scene, MainWindow* parent, GLWidget* glWidget);
	ProcessPanel(Scene* scene, const QString& title, MainWindow* parent, GLWidget* glWidget);

	void addWidget(ProcessWidget* widget, const QString& name);

public Q_SLOTS:
	void onCurrentWidgetChanged(int id);
	void onImport(const GeomInfo& info);
	void onClear();
	void onPicked(const PickingInfo& info);

private:
	void _initializePanel(MainWindow* parent, GLWidget* glWidget);

private:
	Scene* _scene;
	MainWindow* _parent;
	GLWidget* _glWidget;
	QStackedWidget* _stackedWidget;
	QComboBox* _comboBox;
	ProcessWidget* _activatedWidget = nullptr;
};
