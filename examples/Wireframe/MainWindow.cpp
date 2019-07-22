#include "MainWindow.h"
#include <Klein/Gui/ImGuiManager.h>
#include <Klein/Input/TrackballCameraController.h>
#include <Klein/Render/PBRSolidWireframeMaterial.h>
#include <imgui.h>
#include <QColor>
#include <QVector3D>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QEnvironmentLight>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>
#include <Qt3DRender/QViewport>

Qt3DRender::QAbstractTexture* loadTexture(
    Qt3DCore::QNode* parent,
    const QString& file,
    bool genMipMaps,
    Qt3DRender::QAbstractTexture::Filter filter)
{
    Qt3DRender::QTextureWrapMode wrapMode;
    wrapMode.setX(Qt3DRender::QTextureWrapMode::ClampToEdge);
    wrapMode.setY(Qt3DRender::QTextureWrapMode::ClampToEdge);
    auto texture = new Qt3DRender::QTextureLoader(parent);
    texture->setSource(QUrl::fromLocalFile(file));
    texture->setWrapMode(wrapMode);
    texture->setGenerateMipMaps(genMipMaps);
    texture->setMagnificationFilter(filter);
    texture->setMinificationFilter(filter);
    return texture;
}

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

    // Load mesh from file
    auto mesh = new Qt3DRender::QMesh(rootEntity);
    mesh->setSource(
        QUrl::fromLocalFile(QStringLiteral("./data/mesh/bunny.obj")));

    // Load textures from file
    auto brdfMap = loadTexture(rootEntity,
                               QStringLiteral("./data/envmap/brdfSmith.dds"),
                               false,
                               Qt3DRender::QAbstractTexture::Linear);
    auto irradianceMap =
        loadTexture(rootEntity,
                    QStringLiteral("./data/envmap/outdoorDiffuseHDR.dds"),
                    false,
                    Qt3DRender::QAbstractTexture::Linear);
    auto specularMap =
        loadTexture(rootEntity,
                    QStringLiteral("./data/envmap/outdoorSpecularHDR.dds"),
                    false,
                    Qt3DRender::QAbstractTexture::Linear);

    // Add PBR material
    m_material = new Klein::PBRSolidWireframeMaterial(rootEntity);
    m_material->setBaseColor(QColor("teal"));
    m_material->setRoughness(1.0f);
    m_material->setMetalness(0.0f);
    m_material->setEnvLightBrdf(brdfMap);
    m_material->setEnvLightIntensity(1.0f);
    m_material->setLineWidth(m_lineWidth);
    m_material->setLineColor(
        QColor::fromRgbF(m_lineColor[0], m_lineColor[1], m_lineColor[2]));

    // The mesh entity
    auto visual = new Qt3DCore::QEntity(rootEntity);
    visual->addComponent(m_material);
    visual->addComponent(mesh);

    // Add image-based lighting
    auto ibl = new Qt3DCore::QEntity(rootEntity);
    auto envMap = new Qt3DRender::QEnvironmentLight(ibl);
    envMap->setIrradiance(irradianceMap);
    envMap->setSpecular(specularMap);
    ibl->addComponent(envMap);

    // Set up a camera
    m_camera = new Qt3DRender::QCamera(rootEntity);
    m_camera->setPosition(QVector3D(0.0f, 0.0f, 3.0f));
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
        if (ImGui::SliderFloat(
                "Line width", &m_lineWidth, 0.1f, 2.0f, "%.1f")) {
            // Change line width
            m_material->setLineWidth(m_lineWidth);
        }
        if (ImGui::ColorPicker3("Line color", m_lineColor)) {
            // Change line color
            m_material->setLineColor(QColor::fromRgbF(
                m_lineColor[0], m_lineColor[1], m_lineColor[2]));
        }
    }
    ImGui::End();
}
