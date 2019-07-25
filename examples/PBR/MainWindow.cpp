#include "MainWindow.h"
#include <Klein/Gui/ImGuiManager.h>
#include <Klein/Input/TrackballCameraController.h>
#include <Klein/Render/PBRMaterial.h>
#include <imgui.h>
#include <QColor>
#include <QVector3D>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSkyboxEntity>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QEnvironmentLight>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
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
        QUrl::fromLocalFile(QStringLiteral("./data/mesh/cerberus.obj")));

    // Load textures from file
    auto baseColorMap =
        loadTexture(rootEntity,
                    QStringLiteral("./data/texture/cerberus_A.png"),
                    true,
                    Qt3DRender::QAbstractTexture::LinearMipMapLinear);
    auto normalMap =
        loadTexture(rootEntity,
                    QStringLiteral("./data/texture/cerberus_N.png"),
                    true,
                    Qt3DRender::QAbstractTexture::LinearMipMapLinear);
    auto roughnessMap =
        loadTexture(rootEntity,
                    QStringLiteral("./data/texture/cerberus_R.png"),
                    true,
                    Qt3DRender::QAbstractTexture::LinearMipMapLinear);
    auto metalnessMap =
        loadTexture(rootEntity,
                    QStringLiteral("./data/texture/cerberus_M.png"),
                    true,
                    Qt3DRender::QAbstractTexture::LinearMipMapLinear);
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
    auto material = new Klein::PBRMaterial(rootEntity);
    material->setRenderMode(Klein::BasePBRMaterial::RENDER_MODE_TEXTURE |
                            Klein::BasePBRMaterial::RENDER_MODE_NORMAL_MAP |
                            Klein::BasePBRMaterial::RENDER_MODE_MATERIAL_MAP);
    material->setBaseColorMap(baseColorMap);
    material->setNormalMap(normalMap);
    material->setRoughnessMap(roughnessMap);
    material->setMetalnessMap(metalnessMap);
    material->setEnvLightBrdf(brdfMap); // some ibl properties are set here
    material->setEnvLightIntensity(1.0f);

    // The mesh entity
    m_cerberus = new Qt3DCore::QEntity(rootEntity);
    m_cerberus->addComponent(material);
    m_cerberus->addComponent(mesh);

    // This node acts as a parent with no visuals
    m_spheres = new Qt3DCore::QEntity(rootEntity);
    m_spheres->setEnabled(false);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            auto sphere = new Qt3DCore::QEntity(m_spheres);
            auto renderer = new Qt3DExtras::QSphereMesh(sphere);
            auto material = new Klein::PBRMaterial(sphere);
            material->setBaseColor(Qt::red);
            material->setRoughness(i / 10.0f);
            material->setMetalness(j / 10.0f);
            material->setEnvLightBrdf(brdfMap);
            material->setEnvLightIntensity(1.0f);
            auto transform = new Qt3DCore::QTransform(sphere);
            transform->setScale(0.04f);
            transform->setTranslation(
                QVector3D(0.0f, 0.5f - i / 10.0f, 0.5f - j / 10.0f));
            sphere->addComponent(renderer);
            sphere->addComponent(material);
            sphere->addComponent(transform);
        }
    }

    // Add image-based lighting
    auto ibl = new Qt3DCore::QEntity(rootEntity);
    auto envMap = new Qt3DRender::QEnvironmentLight(ibl);
    envMap->setIrradiance(irradianceMap);
    envMap->setSpecular(specularMap);
    ibl->addComponent(envMap);

    // Add skybox
    auto skybox = new Qt3DExtras::QSkyboxEntity(rootEntity);
    skybox->setBaseName(QStringLiteral("file:./data/envmap/outdoorEnvHDR"));
    skybox->setExtension(QStringLiteral(".dds"));
    m_skyboxLayer = new Qt3DRender::QLayer(skybox);
    skybox->addComponent(m_skyboxLayer);

    // Set up a camera
    m_camera = new Qt3DRender::QCamera(rootEntity);
    m_camera->setPosition(QVector3D(2.0f, 0.0f, 0.0f));
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
    auto rootNode = new Qt3DRender::QFrameGraphNode(root);

    auto surfaceSelector = new Qt3DRender::QRenderSurfaceSelector(rootNode);
    surfaceSelector->setSurface(this);

    auto viewport = new Qt3DRender::QViewport(surfaceSelector);
    viewport->setNormalizedRect(QRect(0, 0, 1, 1));

    // Clear buffers
    {
        auto clearBuffers = new Qt3DRender::QClearBuffers(viewport);
        clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
        clearBuffers->setClearColor(QColor(255, 255, 255));
        new Qt3DRender::QNoDraw(clearBuffers);
    }

    // Forward pass
    {
        auto cameraSelctor = new Qt3DRender::QCameraSelector(viewport);
        cameraSelctor->setCamera(m_camera);

        // PBR materials
        {
            Klein::BasePBRMaterial::attachRenderPassTo(cameraSelctor);
        }

        // Skybox
        {
            auto layerFilter = new Qt3DRender::QLayerFilter(cameraSelctor);
            layerFilter->addLayer(m_skyboxLayer);
        }
    }

    // Gui pass
    {
        m_imguiManager->attachGuiPassTo(viewport);
    }

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
        ImGui::Text("Scene: ");
        ImGui::SameLine();
        if (ImGui::RadioButton("cerberus", &m_activeScene, 0)) {
            m_cerberus->setEnabled(true);
            m_spheres->setEnabled(false);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("spheres", &m_activeScene, 1)) {
            m_cerberus->setEnabled(false);
            m_spheres->setEnabled(true);
        }
    }
    ImGui::End();
}
