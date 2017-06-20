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

void ProcessPanel::addWidget(ProcessWidget* widget)
{
	_stackedWidget->addWidget(widget);
	connect(_parent, SIGNAL(sceneInitialized(Scene*)), widget, SLOT(onInitializeScene(Scene*)));
	connect(_parent, SIGNAL(geomImported(GeomInfo*)), widget, SLOT(onImport(GeomInfo*)));
	connect(_glWidget, SIGNAL(picked(const PickingInfo&)), widget, SLOT(onPicked(const PickingInfo&)));
}

void ProcessPanel::onCurrentWidgetChanged(int id)
{
	// Only the current widget can take control of the picking events
	auto prevWidget = dynamic_cast<ProcessWidget*>(_stackedWidget->widget(_prevWidgetID));
	auto curWidget = dynamic_cast<ProcessWidget*>(_stackedWidget->currentWidget());
	prevWidget->enablePicking(false);
	curWidget->enablePicking(true);
	_prevWidgetID = id;
}

void ProcessPanel::_initializePanel(MainWindow* parent, GLWidget* glWidget)
{
	_parent = parent;
	_glWidget = glWidget;

	auto container = new QWidget(this);
	auto layout = new QVBoxLayout(container);
	container->setLayout(layout);
	this->setWidget(container);

	auto comboBox = new QComboBox(container);
	comboBox->addItem("Mesh Properties");
	layout->addWidget(comboBox);

	_stackedWidget = new QStackedWidget(container);
	layout->addWidget(_stackedWidget);

	auto propertyWidget = new PropertyWidget(_stackedWidget, glWidget);
	addWidget(propertyWidget);
	propertyWidget->enablePicking(true);
	_prevWidgetID = _stackedWidget->currentIndex();

	connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		_stackedWidget, &QStackedWidget::setCurrentIndex);
	connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this, &ProcessPanel::onCurrentWidgetChanged);
}
