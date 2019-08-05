#include "MainWindow.h"
#include <Klein/Render/BufferData.h>
#include <Klein/Render/MeshGeometry.h>
#include <Klein/Render/UnlitMaterial.h>
#include <QByteArray>
#include <QVector>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>

MainWindow::MainWindow(QWindow* parent) : Klein::AbstractQt3DWindow(parent) {}

void MainWindow::resizeEvent(QResizeEvent*)
{
    if (m_camera) {
        m_camera->setAspectRatio(this->width() / (float)this->height());
    }
}

Qt3DCore::QEntity* MainWindow::createSceneGraph()
{
    auto rootEntity = new Qt3DCore::QEntity;

    // Create a triangle with vertex color attribute
    auto mesh = new Qt3DRender::QGeometryRenderer(rootEntity);
    auto geometry = new Klein::MeshGeometry(
        Klein::MeshGeometry::ADDITIONAL_ATTRIBUTE_COLOR, mesh);
    auto pbuffer = new Qt3DRender::QBuffer(geometry);
    auto cbuffer = new Qt3DRender::QBuffer(geometry);
    QVector<float> positions{ -1.0f, 0.0f, -1.0, 1.0f, 0.0f,
                              -1.0f, 0.0f, 0.0f, 1.0f };
    QVector<float> colors{
        1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    auto pbytes = Klein::createByteArray(positions.begin(), positions.end());
    auto cbytes = Klein::createByteArray(colors.begin(), colors.end());
    pbuffer->setData(pbytes);
    cbuffer->setData(cbytes);
    geometry->setPositionBuffer(pbuffer, 3);
    geometry->setColorBuffer(cbuffer, 3);
    mesh->setGeometry(geometry);
    mesh->setVertexCount(3);

    // Add unlit material
    auto material = new Klein::UnlitMaterial(rootEntity);
    material->setRenderMode(Klein::BaseUnlitMaterial::RENDER_MODE_VCOLOR);

    // The mesh entity
    auto visual = new Qt3DCore::QEntity(rootEntity);
    visual->addComponent(material);
    visual->addComponent(mesh);

    // Set up a camera
    m_camera = new Qt3DRender::QCamera(rootEntity);
    m_camera->setPosition(QVector3D(0.0f, -2.0f, 0.0f));
    m_camera->setViewCenter(QVector3D(0, 0, 0));
    auto aspect = (this->width() + 0.0f) / this->height();
    m_camera->lens()->setPerspectiveProjection(60.0f, aspect, 0.1f, 100.0f);

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
