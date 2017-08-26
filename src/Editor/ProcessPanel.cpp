#include "Editor/ProcessPanel.h"
#include "Editor/MainWindow.h"
#include "Editor/GLWidget.h"
#include "Editor/PropertyWidget.h"
#include "Core/Message.h"

#include <QString>
#include <QtWidgets>

ProcessPanel::ProcessPanel(Scene* scene, MainWindow* parent, GLWidget* glWidget)
	: QDockWidget(parent), _scene(scene)
{
	_initializePanel(parent, glWidget);
}

ProcessPanel::ProcessPanel(Scene* scene, const QString& title, MainWindow* parent, GLWidget* glWidget)
	: QDockWidget(title, parent), _scene(scene)
{
	_initializePanel(parent, glWidget);
}

void ProcessPanel::addWidget(ProcessWidget* widget, const QString& name)
{
	_comboBox->addItem(name);
	_stackedWidget->addWidget(widget);
}

void ProcessPanel::onCurrentWidgetChanged(int id)
{
	auto curWidget = dynamic_cast<ProcessWidget*>(_stackedWidget->currentWidget());
	_activatedWidget->onDeactivated();
	curWidget->onActivated();
	_activatedWidget = curWidget;
}

void ProcessPanel::onImport(const GeomInfo& info)
{
	for (auto i = 0; i < _stackedWidget->count(); ++i) {
		auto processWidget = dynamic_cast<ProcessWidget*>(_stackedWidget->widget(i));
		processWidget->onImport(info);
	}
}

void ProcessPanel::onClear()
{
	for (auto i = 0; i < _stackedWidget->count(); ++i) {
		auto processWidget = dynamic_cast<ProcessWidget*>(_stackedWidget->widget(i));
		processWidget->onClear();
	}
}

void ProcessPanel::onPicked(const PickingInfo& info)
{
	_activatedWidget->onPicked(info);
}

void ProcessPanel::_initializePanel(MainWindow* parent, GLWidget* glWidget)
{
	_parent = parent;
	_glWidget = glWidget;

	auto container = new QWidget(this);
	auto layout = new QVBoxLayout(container);
	container->setLayout(layout);
	this->setWidget(container);

	_comboBox = new QComboBox(container);
	layout->addWidget(_comboBox);

	_stackedWidget = new QStackedWidget(container);
	layout->addWidget(_stackedWidget);

	auto propertyWidget = new PropertyWidget(_scene, _stackedWidget, glWidget);
	addWidget(propertyWidget, "Properties");;
	_activatedWidget = propertyWidget;

	// Add your own widgets here using addWidget function

	connect(_comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		_stackedWidget, &QStackedWidget::setCurrentIndex);
	connect(_comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this, &ProcessPanel::onCurrentWidgetChanged);
	connect(parent, &MainWindow::geomImported, this, &ProcessPanel::onImport);
	connect(glWidget, &GLWidget::picked, this, &ProcessPanel::onPicked);
}
