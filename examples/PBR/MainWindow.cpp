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
#include <Qt3DRender/QPointLight>
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
    texture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
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
    baseColorMap->setFormat(Qt3DRender::QAbstractTexture::SRGB8);
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
                    Qt3DRender::QAbstractTexture::LinearMipMapLinear);

    // Add PBR material
    m_cerberusMaterial = new Klein::PBRMaterial(rootEntity);
    m_cerberusMaterial->setRenderMode(
        Klein::BasePBRMaterial::RENDER_MODE_BASE_COLOR_MAP |
        Klein::BasePBRMaterial::RENDER_MODE_NORMAL_MAP |
        Klein::BasePBRMaterial::RENDER_MODE_MATERIAL_MAP);
    m_cerberusMaterial->setBaseColorMap(baseColorMap);
    m_cerberusMaterial->setNormalMap(normalMap);
    m_cerberusMaterial->setRoughnessMap(roughnessMap);
    m_cerberusMaterial->setMetalnessMap(metalnessMap);
    m_cerberusMaterial->setEnvLightBrdf(
        brdfMap); // some ibl properties are set here
    m_cerberusMaterial->setEnvLightIntensity(1.0f);

    // The mesh entity
    m_cerberus = new Qt3DCore::QEntity(rootEntity);
    m_cerberus->addComponent(m_cerberusMaterial);
    m_cerberus->addComponent(mesh);

    // This node acts as a parent with no visuals
    m_spheres = new Qt3DCore::QEntity(rootEntity);
    m_spheres->setEnabled(false);
    auto sphereMesh = new Qt3DExtras::QSphereMesh(m_spheres);
    sphereMesh->setRings(50);
    sphereMesh->setSlices(100);
    sphereMesh->setShareable(true);
    for (int i = 0; i <= 10; ++i) {
        auto sphere = new Qt3DCore::QEntity(m_spheres);
        auto material = new Klein::PBRMaterial(sphere);
        material->setBaseColor(Qt::red);
        material->setRoughness(i / 10.0f);
        material->setMetalness(0.0f);
        material->setEnvLightBrdf(brdfMap);
        material->setEnvLightIntensity(1.0f);
        material->setEnvLightMipLevels(7);
        m_sphereMaterials.push_back(material);
        auto transform = new Qt3DCore::QTransform(sphere);
        transform->setScale(0.15f);
        transform->setTranslation(QVector3D(0.0f, 0.0f, 2.0f - i * 0.4f));
        sphere->addComponent(sphereMesh);
        sphere->addComponent(material);
        sphere->addComponent(transform);
    }

    // Add direct lighting
    QVector<QVector3D> lightPositions{
        QVector3D(2.0f, 1.0f, 1.0f),
        QVector3D(2.0f, 1.0f, -1.0f),
        QVector3D(2.0f, -1.0f, 1.0f),
        QVector3D(2.0f, -1.0f, -1.0f),
    };
    for (auto pos : lightPositions) {
        auto light = new Qt3DCore::QEntity(rootEntity);
        auto lighting = new Qt3DRender::QPointLight(light);
        lighting->setIntensity(m_dlIntensity);
        m_directLights.push_back(lighting);
        auto transform = new Qt3DCore::QTransform(light);
        transform->setTranslation(pos);
        light->addComponent(lighting);
        light->addComponent(transform);
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
        if (m_activeScene == 1) {
            ImGui::TextUnformatted("Material:");
            ImGui::Indent();
            const char* params[] = { "Roughness", "Metalness", "Specular" };
            if (ImGui::Combo(
                    "Varying", &m_varyingParam, params, IM_ARRAYSIZE(params))) {
                for (int i = 0; i < m_sphereMaterials.size(); ++i) {
                    switch (m_varyingParam) {
                    case 0:
                        m_sphereMaterials[i]->setRoughness(
                            (i + 0.0f) / (m_sphereMaterials.size() - 1));
                        m_sphereMaterials[i]->setMetalness(m_metalness);
                        m_sphereMaterials[i]->setSpecular(m_specular);
                        break;
                    case 1:
                        m_sphereMaterials[i]->setMetalness(
                            (i + 0.0f) / (m_sphereMaterials.size() - 1));
                        m_sphereMaterials[i]->setRoughness(m_metalness);
                        m_sphereMaterials[i]->setSpecular(m_specular);
                        break;
                    case 2:
                        m_sphereMaterials[i]->setSpecular(
                            (i + 0.0f) / (m_sphereMaterials.size() - 1));
                        m_sphereMaterials[i]->setRoughness(m_specular);
                        m_sphereMaterials[i]->setMetalness(m_metalness);
                        break;
                    }
                }
            }
            if (m_varyingParam == 0) {
                ImGui::SliderFloat("Roughness",
                                   &m_roughness,
                                   m_roughness,
                                   m_roughness,
                                   "%.2f");
            }
            else {
                if (ImGui::SliderFloat(
                        "Roughness", &m_roughness, 0.0f, 1.0f, "%.2f")) {
                    for (auto& mat : m_sphereMaterials) {
                        mat->setRoughness(m_roughness);
                    }
                }
            }
            if (m_varyingParam == 1) {
                ImGui::SliderFloat("Metalness",
                                   &m_metalness,
                                   m_metalness,
                                   m_metalness,
                                   "%.2f");
            }
            else {
                if (ImGui::SliderFloat(
                        "Metalness", &m_metalness, 0.0f, 1.0f, "%.2f")) {
                    for (auto& mat : m_sphereMaterials) {
                        mat->setMetalness(m_metalness);
                    }
                }
            }
            if (m_varyingParam == 2) {
                ImGui::SliderFloat(
                    "Specular", &m_specular, m_specular, m_specular, "%.2f");
            }
            else {
                if (ImGui::SliderFloat(
                        "Specular", &m_specular, 0.0f, 1.0f, "%.2f")) {
                    for (auto& mat : m_sphereMaterials) {
                        mat->setSpecular(m_specular);
                    }
                }
            }
            if (ImGui::SliderFloat(
                    "Clearcoat", &m_clearcoat, 0.0f, 1.0f, "%.2f")) {
                for (auto& mat : m_sphereMaterials) {
                    mat->setClearCoat(m_clearcoat);
                }
            }
            if (ImGui::SliderFloat("Clearcoat roughness",
                                   &m_clearcoatRoughness,
                                   0.0f,
                                   1.0f,
                                   "%.2f")) {
                for (auto& mat : m_sphereMaterials) {
                    mat->setClearCoatRoughness(m_clearcoatRoughness);
                }
            }
            if (ImGui::ColorEdit3("Base color", m_baseColor)) {
                for (auto& mat : m_sphereMaterials) {
                    mat->setBaseColor(QColor::fromRgbF(
                        m_baseColor[0], m_baseColor[1], m_baseColor[2]));
                }
            }
            ImGui::Unindent();
        }
        ImGui::TextUnformatted("Lighting:");
        ImGui::Indent();
        if (ImGui::SliderFloat("Direct lighting intensity",
                               &m_dlIntensity,
                               0.0f,
                               1.0f,
                               "%.2f")) {
            for (auto& light : m_directLights) {
                light->setIntensity(m_dlIntensity);
            }
        }
        if (ImGui::SliderFloat("Image-based lighting intensity",
                               &m_iblIntensity,
                               0.0f,
                               1.0f,
                               "%.2f")) {
            m_cerberusMaterial->setEnvLightIntensity(m_iblIntensity);
            for (auto& mat : m_sphereMaterials) {
                mat->setEnvLightIntensity(m_iblIntensity);
            }
        }
        ImGui::Unindent();
    }
    ImGui::End();
}
