#include "MainWindow.h"
#include <QColor>
#include <QVector3D>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QMetalRoughMaterial>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>

void MainWindow::resizeEvent(QResizeEvent*)
{
    if (m_camera) {
        m_camera->setAspectRatio(this->width() / (float)this->height());
    }
}

Qt3DCore::QEntity* MainWindow::createSceneGraph()
{
    auto rootEntity = new Qt3DCore::QEntity;

    // Create a sphere entity
    auto material = new Qt3DExtras::QMetalRoughMaterial(rootEntity);
    material->setRoughness(1.0f);
    material->setMetalness(0.0f);
    material->setBaseColor(QColor(255, 0, 0));
    auto mesh = new Qt3DExtras::QSphereMesh(rootEntity);
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
