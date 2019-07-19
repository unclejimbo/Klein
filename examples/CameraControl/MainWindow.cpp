#include "MainWindow.h"
#include <Klein/Gui/ImGuiManager.h>
#include <Klein/Input/TrackballCameraController.h>
#include <imgui.h>
#include <QColor>
#include <QVector3D>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QMetalRoughMaterial>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>

MainWindow::MainWindow(QWindow* parent) : Klein::AbstractQt3DWindow(parent)
{
    m_imguiManager = new Klein::ImGuiManager(this);
    m_imguiManager->setInputEventSource(this);
    auto draw = [this](bool wantInput) {
        // If imgui want input, then disable the camera controller
        m_cameraController->setEnabled(!wantInput);
        _renderGui();
    };
    m_imguiManager->setFrameFunc(draw);
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    if (m_camera) {
        m_camera->setAspectRatio(this->width() / (float)this->height());
    }
    if (m_cameraController) { m_cameraController->setWindowSize(this->size()); }
    if (m_imguiManager) {
        m_imguiManager->resize(this->size(), this->devicePixelRatio());
    }
}

Qt3DCore::QEntity* MainWindow::createSceneGraph()
{
    auto rootEntity = new Qt3DCore::QEntity;

    // Create a cube entity
    auto material = new Qt3DExtras::QMetalRoughMaterial(rootEntity);
    material->setRoughness(1.0f);
    material->setMetalness(0.0f);
    material->setBaseColor(QColor(255, 0, 0));
    auto mesh = new Qt3DExtras::QCuboidMesh(rootEntity);
    auto visual = new Qt3DCore::QEntity(rootEntity);
    visual->addComponent(material);
    visual->addComponent(mesh);

    // Add some lighting
    auto light = new Qt3DRender::QDirectionalLight(rootEntity);
    light->setWorldDirection(QVector3D(1.0f, 1.0f, 1.0f));

    // Set up a camera
    m_camera = new Qt3DRender::QCamera(rootEntity);
    m_camera->setPosition(QVector3D(4.0f, 0.0f, 4.0f));
    m_camera->setViewCenter(QVector3D(0, 0, 0));
    auto aspect = (this->width() + 0.0f) / this->height();
    m_camera->lens()->setPerspectiveProjection(60.0f, aspect, 0.1f, 100.0f);

    // Set up a controller
    m_cameraController = new Klein::TrackballCameraController(rootEntity);
    m_cameraController->setCamera(m_camera);
    m_cameraController->setWindowSize(this->size());

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

    auto cameraSelctor = new Qt3DRender::QCameraSelector(viewport);
    cameraSelctor->setCamera(m_camera);

    m_imguiManager->attachGuiPassTo(viewport);

    // Use this framegraph
    auto settings = new Qt3DRender::QRenderSettings(root);
    settings->setActiveFrameGraph(rootNode);
    return settings;
}

void MainWindow::_renderGui()
{
    ImVec2 pos(10.0f, 10.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.3f);
    if (ImGui::Begin(
            "Tip",
            nullptr,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
        ImGui::Text("Hold left mouse and drag to rotate");
        ImGui::Text("Hold shift and drag to pan");
        ImGui::Text("Scroll wheel to zoom");
    }
    ImGui::End();
}
