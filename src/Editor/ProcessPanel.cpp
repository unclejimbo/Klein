#include "Editor/ProcessPanel.h"
#include "Editor/MainWindow.h"
#include "Editor/GLWidget.h"
#include "Editor/PropertyWidget.h"

#include <QtWidgets>

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

void ProcessPanel::onCurrentWidgetChanged(int id)
{
	// Only the current widget can take control of the picking events
	auto prevWidget = dynamic_cast<ProcessWidget*>(_stackedWidget->widget(_prevWidgetID));
	auto curWidget = dynamic_cast<ProcessWidget*>(_stackedWidget->currentWidget());
	prevWidget->enablePicking(false);
	curWidget->enablePicking(true);
}

void ProcessPanel::_initializePanel(MainWindow* parent, GLWidget* glWidget)
{
	_glWidget = glWidget;

	auto container = new QWidget(this);
	auto layout = new QVBoxLayout(container);
	container->setLayout(layout);
	this->setWidget(container);

	auto comboBox = new QComboBox(container);
	comboBox->addItem("Mesh Properties");
	layout->addWidget(comboBox);

	_stackedWidget = new QStackedWidget(container);
	auto propertyWidget = new PropertyWidget(_stackedWidget, glWidget);
	_stackedWidget->addWidget(propertyWidget);
	layout->addWidget(_stackedWidget);

	connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		_stackedWidget, &QStackedWidget::setCurrentIndex);
	connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this, &ProcessPanel::onCurrentWidgetChanged);
	connect(glWidget, &GLWidget::picked, propertyWidget, &PropertyWidget::onPicked);
	connect(parent, &MainWindow::geomImported, propertyWidget, &PropertyWidget::onImport);
	connect(parent, &MainWindow::sceneInitialized, propertyWidget, &PropertyWidget::onInitializeScene);
}
