#include "MainWindow.h"
#include "../external/tiny_obj_loader.h"
#include <Klein/Input/TrackballCameraController.h>
#include <Klein/Render/ConcreteLinesRenderer.h>
#include <Klein/Render/ConcretePointsRenderer.h>
#include <Klein/Render/PBRInstancedMaterial.h>
#include <Klein/Util/ColorPalette.h>
#include <imgui.h>
#include <QColor>
#include <QVector>
#include <QVector3D>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QEnvironmentLight>
#include <Qt3DRender/QFrameGraphNode>
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

    // Load mesh geometry
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    tinyobj::LoadObj(
        &attrib, &shapes, nullptr, nullptr, nullptr, "./data/mesh/bunny.obj");
    const auto& shape = shapes[0];

    // Points representing vertices
    QVector<QVector3D> pPositions;
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        auto x = attrib.vertices[i + 0];
        auto y = attrib.vertices[i + 1];
        auto z = attrib.vertices[i + 2];
        pPositions.push_back(QVector3D(x, y, z));
    }
    auto pointsRenderer = new Klein::ConcretePointsRenderer(
        Klein::ConcretePointsRenderer::ADDITIONAL_ATTRIBUTE_COLOR, rootEntity);
    pointsRenderer->setRadius(0.01f);
    pointsRenderer->setPositions(pPositions);
    // Set random colors for each instance
    Klein::ColorPalette palette(pPositions.size());
    pointsRenderer->setColors(palette.palette());

    // Lines representing edges
    QVector<QVector3D> lPositions;
    size_t offset = 0;
    for (auto fv : shape.mesh.num_face_vertices) {
        std::vector<size_t> indices;
        for (size_t v = 0; v < (size_t)fv; ++v) {
            auto idx = shape.mesh.indices[offset + v];
            indices.push_back(idx.vertex_index);
        }
        for (size_t i = 0; i < indices.size(); ++i) {
            auto i0 = indices[i];
            auto x0 = (attrib.vertices[3 * i0 + 0]);
            auto y0 = (attrib.vertices[3 * i0 + 1]);
            auto z0 = (attrib.vertices[3 * i0 + 2]);
            lPositions.push_back(QVector3D(x0, y0, z0));
            auto i1 = indices[(i + 1) % indices.size()];
            auto x1 = (attrib.vertices[3 * i1 + 0]);
            auto y1 = (attrib.vertices[3 * i1 + 1]);
            auto z1 = (attrib.vertices[3 * i1 + 2]);
            lPositions.push_back(QVector3D(x1, y1, z1));
        }
        offset += fv;
    }
    auto linesRenderer = new Klein::ConcreteLinesRenderer(
        Klein::ConcreteLinesRenderer::ADDITIONAL_ATTRIBUTE_NONE, rootEntity);
    linesRenderer->setRadius(0.003f);
    linesRenderer->setPositions(lPositions);

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
    auto pointsMaterial = new Klein::PBRInstancedMaterial(rootEntity);
    pointsMaterial->setUseInstanceColor(true);
    pointsMaterial->setRoughness(1.0f);
    pointsMaterial->setMetalness(0.0f);
    pointsMaterial->setEnvLightBrdf(brdfMap);
    pointsMaterial->setEnvLightIntensity(1.0f);

    auto linesMaterial = new Klein::PBRInstancedMaterial(rootEntity);
    linesMaterial->setBaseColor(Qt::gray);
    linesMaterial->setRoughness(0.0f);
    linesMaterial->setMetalness(1.0f);
    linesMaterial->setEnvLightBrdf(brdfMap);
    linesMaterial->setEnvLightIntensity(1.0f);

    // The mesh entity
    m_points = new Qt3DCore::QEntity(rootEntity);
    m_points->addComponent(pointsRenderer);
    m_points->addComponent(pointsMaterial);

    m_lines = new Qt3DCore::QEntity(rootEntity);
    m_lines->addComponent(linesRenderer);
    m_lines->addComponent(linesMaterial);

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

    // Use this framegraph
    auto settings = new Qt3DRender::QRenderSettings(root);
    settings->setActiveFrameGraph(rootNode);
    return settings;
}
