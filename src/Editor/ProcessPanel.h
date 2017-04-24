#pragma once

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QDockWidget>
#include <QString>

#ifdef _WIN32
#pragma warning(pop)
#endif

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
