#include "MainWindow.h"
#include <imgui.h>
#include <Klein/Gui/ImGuiManager.h>
#include <Klein/Input/TrackballCameraController.h>
#include <Klein/Util/Gizmo.h>
#include <QColor>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QGoochMaterial>
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
    m_gizmo = new Klein::Gizmo(this);
    m_imguiManager = new Klein::ImGuiManager(this);
    m_imguiManager->setInputEventSource(this);
    // FrameFunc is where the imgui code goes
    auto drawGui = [this](bool) {
        this->updateGui();
        m_gizmo->update();
    };
    m_imguiManager->setFrameFunc(drawGui);
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    // Need to resize
    m_imguiManager->resize(this->size(), this->devicePixelRatio());
    if (m_camera) {
        m_camera->setAspectRatio(this->width() / (this->height() + 1e-5f));
    }
    if (m_cameraController) { m_cameraController->setWindowSize(this->size()); }
}

Qt3DCore::QEntity* MainWindow::createSceneGraph()
{
    auto rootEntity = new Qt3DCore::QEntity;
    // Call initialize on the root entity for imgui contents to show
    m_imguiManager->initialize(rootEntity);

    auto cubeRenderer = new Qt3DExtras::QCuboidMesh(rootEntity);
    cubeRenderer->setShareable(true);

    auto cubeEntity1 = new Qt3DCore::QEntity(rootEntity);
    cubeEntity1->addComponent(cubeRenderer);
    auto cubeMaterial1 = new Qt3DExtras::QGoochMaterial(rootEntity);
    cubeMaterial1->setWarm(QColor(Qt::yellow));
    cubeMaterial1->setCool(QColor(Qt::darkRed));
    cubeEntity1->addComponent(cubeMaterial1);
    m_cubeTransform1 = new Qt3DCore::QTransform(rootEntity);
    m_cubeTransform1->setTranslation(QVector3D(-1.0f, 0.0f, 0.0f));
    cubeEntity1->addComponent(m_cubeTransform1);

    auto cubeEntity2 = new Qt3DCore::QEntity(rootEntity);
    cubeEntity2->addComponent(cubeRenderer);
    auto cubeMaterial2 = new Qt3DExtras::QGoochMaterial(rootEntity);
    cubeMaterial2->setWarm(QColor(Qt::green));
    cubeMaterial2->setCool(QColor(Qt::darkCyan));
    cubeEntity2->addComponent(cubeMaterial2);
    m_cubeTransform2 = new Qt3DCore::QTransform(rootEntity);
    m_cubeTransform2->setTranslation(QVector3D(1.0f, 0.0f, 0.0f));
    cubeEntity2->addComponent(m_cubeTransform2);

    auto lightEntity = new Qt3DRender::QDirectionalLight(rootEntity);
    lightEntity->setWorldDirection(QVector3D(-1.0f, -1.0f, -1.0f));
    lightEntity->setIntensity(1.0f);

    m_camera = new Qt3DRender::QCamera(rootEntity);
    m_camera->setPosition(QVector3D(2.0, 2.0f, 2.0f));
    m_camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    m_camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    auto aspect = this->width() / (this->height() + 1e-5f);
    m_camera->lens()->setPerspectiveProjection(60.0f, aspect, 0.1f, 100.0f);
    m_cameraController = new Klein::TrackballCameraController(rootEntity);
    m_cameraController->setCamera(m_camera);
    m_cameraController->setWindowSize(this->size());
    m_gizmo->setCamera(m_camera);
    m_gizmo->setTarget(m_cubeTransform1);

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

    auto cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    cameraSelector->setCamera(m_camera);

    m_imguiManager->attachSkipGuiPassTo(cameraSelector);

    // Attach related framegraph nodes to the graph
    m_imguiManager->attachGuiPassTo(viewport);

    // Use this framegraph
    auto settings = new Qt3DRender::QRenderSettings(root);
    settings->setActiveFrameGraph(rootNode);
    return settings;
}

void MainWindow::updateGui()
{
    ImGui::Begin("Gizmo");
    if (ImGui::RadioButton("Cube yellow", &m_active, 0)) {
        m_gizmo->setEnabled(true);
        m_gizmo->setTarget(m_cubeTransform1);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Cube green", &m_active, 1)) {
        m_gizmo->setEnabled(true);
        m_gizmo->setTarget(m_cubeTransform2);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("None", &m_active, 2)) {
        m_gizmo->setEnabled(false);
    }
    if (ImGui::RadioButton("Translate", &m_operation, 0)) {
        m_gizmo->setOperation(ImGuizmo::TRANSLATE);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", &m_operation, 1)) {
        m_gizmo->setOperation(ImGuizmo::ROTATE);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", &m_operation, 2)) {
        m_gizmo->setOperation(ImGuizmo::SCALE);
    }
    if (ImGui::RadioButton("World", &m_mode, 0)) {
        m_gizmo->setMode(ImGuizmo::WORLD);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Local", &m_mode, 1)) {
        m_gizmo->setMode(ImGuizmo::LOCAL);
    }
    ImGui::End();
}
