#include "MainWindow.h"
#include <imgui.h>
#include <Klein/Gui/ImGuiManager.h>
#include <QColor>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>

MainWindow::MainWindow(QWindow* parent) : Klein::AbstractQt3DWindow(parent)
{
    m_imguiManager = new Klein::ImGuiManager(this);
    m_imguiManager->setInputEventSource(this);
    // FrameFunc is where the imgui code goes
    auto drawGui = [](bool) { ImGui::ShowDemoWindow(); };
    m_imguiManager->setFrameFunc(drawGui);
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    // Need to resize
    m_imguiManager->resize(this->size(), this->devicePixelRatio());
}

Qt3DCore::QEntity* MainWindow::createSceneGraph()
{
    auto rootEntity = new Qt3DCore::QEntity;
    // Call initialize on the root entity for imgui contents to show
    m_imguiManager->initialize(rootEntity);
    return rootEntity;
}

Qt3DRender::QRenderSettings* MainWindow::createRenderSettings(
    Qt3DCore::QEntity* root)
{
    // Add a basic framegraph
    auto rootNode = new Qt3DRender::QFrameGraphNode(root);

    auto surfaceSelector = new Qt3DRender::QRenderSurfaceSelector(rootNode);
    surfaceSelector->setSurface(this);

    auto viewport = new Qt3DRender::QViewport(surfaceSelector);
    viewport->setNormalizedRect(QRect(0, 0, 1, 1));

    auto clearBuffers = new Qt3DRender::QClearBuffers(viewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    clearBuffers->setClearColor(QColor(255, 255, 255));

    new Qt3DRender::QNoDraw(clearBuffers);

    // Attach related framegraph nodes to the graph
    m_imguiManager->attachGuiPassTo(viewport);

    // Use this framegraph
    auto settings = new Qt3DRender::QRenderSettings(root);
    settings->setActiveFrameGraph(rootNode);
    return settings;
}
