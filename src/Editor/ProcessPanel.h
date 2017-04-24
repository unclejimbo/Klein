#pragma once

#include <QDockWidget>
#include <QString>

class MainWindow;
class GLWidget;

class ProcessPanel : public QDockWidget
{
	Q_OBJECT

public:
	ProcessPanel(MainWindow* parent, GLWidget* glWidget);
	ProcessPanel(const QString& title, MainWindow* parent, GLWidget* glWidget);
	~ProcessPanel();

private:
	void _initializePanel(MainWindow* parent, GLWidget* glWidget);
};
