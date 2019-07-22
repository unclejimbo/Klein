#include "MainWindow.h"
#include <Klein/Input/TrackballCameraController.h>
#include <Klein/Render/CheckerboardTextureImage.h>
#include <Klein/Render/DirectionalShadowCaster.h>
#include <Klein/Render/PBRMaterial.h>
#include <imgui.h>
#include <QColor>
#include <QVector3D>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QDirectionalLight>
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

MainWindow::MainWindow(QWindow* parent) : Klein::AbstractQt3DWindow(parent) {}

void MainWindow::resizeEvent(QResizeEvent*)
{
    if (m_camera) {
        m_camera->setAspectRatio(this->width() / (float)this->height());
    }
    if (m_cameraController) { m_cameraController->setWindowSize(this->size()); }
}

Qt3DCore::QEntity* MainWindow::createSceneGraph()
{
    auto rootEntity = new Qt3DCore::QEntity;

    auto bunnyMesh = new Qt3DRender::QMesh(rootEntity);
    bunnyMesh->setSource(
        QUrl::fromLocalFile(QStringLiteral("./data/mesh/bunny.obj")));
    auto planeMesh = new Qt3DExtras::QPlaneMesh(rootEntity);
    planeMesh->setWidth(5.0f);
    planeMesh->setHeight(5.0f);

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
    auto texture = new Qt3DRender::QTexture2D(rootEntity);
    auto image = new Klein::CheckerboardTextureImage(texture);
    texture->addTextureImage(image);

    // This material both casts and recieves shadow
    auto bunnyMaterial0 = new Klein::PBRMaterial(rootEntity);
    bunnyMaterial0->setBaseColor(QColor("teal"));
    bunnyMaterial0->setRoughness(1.0f);
    bunnyMaterial0->setMetalness(0.0f);
    bunnyMaterial0->setShadowCastingEnabled(true);
    bunnyMaterial0->setShadowReceivingEnabled(true);
    bunnyMaterial0->setEnvLightBrdf(brdfMap);
    bunnyMaterial0->setEnvLightIntensity(0.2f);
    bunnyMaterial0->setBaseColorMap(texture);
    bunnyMaterial0->setRoughnessMap(texture);
    bunnyMaterial0->setMetalnessMap(texture);
    bunnyMaterial0->setNormalMap(texture);

    // This material only casts shadow
    auto bunnyMaterial1 = new Klein::PBRMaterial(rootEntity);
    bunnyMaterial1->setBaseColor(QColor("orange"));
    bunnyMaterial1->setRoughness(1.0f);
    bunnyMaterial1->setMetalness(0.0f);
    bunnyMaterial1->setShadowCastingEnabled(true);
    bunnyMaterial1->setEnvLightBrdf(brdfMap);
    bunnyMaterial1->setEnvLightIntensity(0.2f);
    bunnyMaterial1->setBaseColorMap(texture);
    bunnyMaterial1->setRoughnessMap(texture);
    bunnyMaterial1->setMetalnessMap(texture);
    bunnyMaterial1->setNormalMap(texture);

    // This material only recieves shadow
    auto planeMaterial = new Klein::PBRMaterial(rootEntity);
    planeMaterial->setBaseColor(QColor("white"));
    planeMaterial->setRoughness(1.0f);
    planeMaterial->setMetalness(0.0f);
    planeMaterial->setShadowReceivingEnabled(true);
    planeMaterial->setEnvLightBrdf(brdfMap);
    planeMaterial->setEnvLightIntensity(0.2f);

    auto bunnyTransform0 = new Qt3DCore::QTransform(rootEntity);
    bunnyTransform0->setScale(0.5f);
    bunnyTransform0->setTranslation(QVector3D(0.8f, 0.5f, 0.8f));

    auto bunnyTransform1 = new Qt3DCore::QTransform(rootEntity);
    bunnyTransform1->setScale(0.5f);
    bunnyTransform1->setTranslation(QVector3D(-0.0f, 0.5f, -0.0f));

    auto bunnyTransform2 = new Qt3DCore::QTransform(rootEntity);
    bunnyTransform2->setScale(0.5f);
    bunnyTransform2->setTranslation(QVector3D(-0.8f, 0.5f, -0.8f));

    auto bunny0 = new Qt3DCore::QEntity(rootEntity);
    bunny0->addComponent(bunnyMesh);
    bunny0->addComponent(bunnyMaterial0);
    bunny0->addComponent(bunnyTransform0);

    auto bunny1 = new Qt3DCore::QEntity(rootEntity);
    bunny1->addComponent(bunnyMesh);
    bunny1->addComponent(bunnyMaterial1);
    bunny1->addComponent(bunnyTransform1);

    auto bunny2 = new Qt3DCore::QEntity(rootEntity);
    bunny2->addComponent(bunnyMesh);
    bunny2->addComponent(bunnyMaterial0);
    bunny2->addComponent(bunnyTransform2);

    auto plane = new Qt3DCore::QEntity(rootEntity);
    plane->addComponent(planeMesh);
    plane->addComponent(planeMaterial);

    // Direct lighting creates shadow
    auto dlightLighting = new Qt3DRender::QDirectionalLight(rootEntity);
    dlightLighting->setWorldDirection(QVector3D(-1.0f, -1.0f, -1.0f));
    dlightLighting->setIntensity(2.0f);
    dlightLighting->setColor(Qt::white);
    auto dlight = new Qt3DCore::QEntity(rootEntity);
    dlight->addComponent(dlightLighting);

    // Set up the shadow map
    m_dcaster = new Klein::DirectionalShadowCaster(rootEntity);
    m_dcaster->setSize(QSize(2048, 2048));
    m_dcaster->lookAt(QVector3D(3.0f, 3.0f, 3.0f),
                      QVector3D(0.0f, 0.0f, 0.0f),
                      QVector3D(0.0f, 1.0f, 0.0f));
    m_dcaster->setFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 20.0f);

    // Image-based lighting doesn't create shadow
    auto ibl = new Qt3DCore::QEntity(rootEntity);
    auto envMap = new Qt3DRender::QEnvironmentLight(ibl);
    envMap->setIrradiance(irradianceMap);
    envMap->setSpecular(specularMap);
    ibl->addComponent(envMap);

    m_camera = new Qt3DRender::QCamera(rootEntity);
    m_camera->setPosition(QVector3D(2.0f, 2.0f, 2.0f));
    m_camera->setViewCenter(QVector3D(0, 0, 0));
    auto aspect = (this->width() + 0.0f) / this->height();
    m_camera->lens()->setPerspectiveProjection(60.0f, aspect, 0.1f, 100.0f);

    m_cameraController = new Klein::TrackballCameraController(rootEntity);
    m_cameraController->setCamera(m_camera);
    m_cameraController->setWindowSize(this->size());

    return rootEntity;
}

Qt3DRender::QRenderSettings* MainWindow::createRenderSettings(
    Qt3DCore::QEntity* root)
{
    auto rootNode = new Qt3DRender::QFrameGraphNode(root);

    // Shadow map generation
    {
        auto surfaceSelector = new Qt3DRender::QRenderSurfaceSelector(rootNode);
        surfaceSelector->setSurface(this);
        surfaceSelector->setExternalRenderTargetSize(m_dcaster->size());

        auto viewport = new Qt3DRender::QViewport(surfaceSelector);
        viewport->setNormalizedRect(QRect(0, 0, 1, 1));

        // Set up framegraph for shadow map
        auto shadowCaster = m_dcaster->attachTo(viewport);

        // Set up framegraph for shadow casting material
        Klein::BasePBRMaterial::attachShadowPassTo(shadowCaster);
    }

    // Forward pass
    {
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

        Klein::BasePBRMaterial::attachRenderPassTo(
            cameraSelctor,
            m_dcaster->shadowMap(),
            m_dcaster->lightSpaceMatrix(),
            m_dcaster->lightDirection(),
            100.0f);
    }

    auto settings = new Qt3DRender::QRenderSettings(root);
    settings->setActiveFrameGraph(rootNode);
    return settings;
}
