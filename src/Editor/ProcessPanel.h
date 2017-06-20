#pragma once

#include <QDockWidget>
#include <QString>

class MainWindow;
class GLWidget;
class ProcessWidget;
class QStackedWidget;

class ProcessPanel : public QDockWidget
{
	Q_OBJECT

public:
	ProcessPanel(MainWindow* parent, GLWidget* glWidget);
	ProcessPanel(const QString& title, MainWindow* parent, GLWidget* glWidget);
	~ProcessPanel();

	void addWidget(ProcessWidget* widget);

public Q_SLOTS:
	void onCurrentWidgetChanged(int id);

private:
	void _initializePanel(MainWindow* parent, GLWidget* glWidget);

private:
	MainWindow* _parent;
	GLWidget* _glWidget;
	QStackedWidget* _stackedWidget;
	int _prevWidgetID = 0;
};
