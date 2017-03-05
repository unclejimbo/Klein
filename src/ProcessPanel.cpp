#include "ProcessPanel.h"

#include "MainWindow.h"
#include "GLWidget.h"
#include "PropertyWidget.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QtWidgets>

#ifdef _WIN32
#pragma warning(pop)
#endif

ProcessPanel::ProcessPanel(MainWindow* parent, GLWidget* glWidget)
	: QDockWidget(parent)
{
	_initializePanel(parent, glWidget);
}

ProcessPanel::ProcessPanel(const QString& title, MainWindow* parent, GLWidget* glWidget)
	: QDockWidget(title, parent)
{
	_initializePanel(parent, glWidget);
}

ProcessPanel::~ProcessPanel() = default;

void ProcessPanel::_initializePanel(MainWindow* parent, GLWidget* glWidget)
{
	auto container = new QWidget(this);
	auto layout = new QVBoxLayout(container);
	container->setLayout(layout);
	this->setWidget(container);

	auto comboBox = new QComboBox(container);
	comboBox->addItem("Mesh Properties");
	layout->addWidget(comboBox);

	auto stackedWidget = new QStackedWidget(container);
	auto propertyWidget = new PropertyWidget(stackedWidget, glWidget);
	stackedWidget->addWidget(propertyWidget);
	layout->addWidget(stackedWidget);

	connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		stackedWidget, &QStackedWidget::setCurrentIndex);
	connect(parent, &MainWindow::meshImported, propertyWidget, &PropertyWidget::onImportMesh);
	connect(parent, &MainWindow::sceneInitialized, propertyWidget, &PropertyWidget::onInitializeScene);
}
