#pragma once

#include <QDockWidget>
#include <memory>

class QString;
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
	ProcessPanel(MainWindow* parent, GLWidget* glWidget);
	ProcessPanel(const QString& title, MainWindow* parent, GLWidget* glWidget);

	void addWidget(ProcessWidget* widget, const QString& name);

public Q_SLOTS:
	void onCurrentWidgetChanged(int id);
	void onImport(GeomInfo* info);
	void onPicked(const PickingInfo& info);

private:
	void _initializePanel(MainWindow* parent, GLWidget* glWidget);

private:
	MainWindow* _parent;
	GLWidget* _glWidget;
	QStackedWidget* _stackedWidget;
	QComboBox* _comboBox;
	std::unique_ptr<GeomInfo> _geomInfo = nullptr;
	ProcessWidget* _activatedWidget = nullptr;
};
