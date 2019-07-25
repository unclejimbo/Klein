#include "MainWindow.h"
#include <Klein/Input/TrackballCameraController.h>
#include <Klein/Render/UnlitMaterial.h>
#include <Klein/Render/WBOITMaterial.h>
#include <imgui.h>
#include <QColor>
#include <QVector3D>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QColorMask>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>

MainWindow::MainWindow(QWindow* parent) : Klein::AbstractQt3DWindow(parent) {}

void MainWindow::resizeEvent(QResizeEvent*)
{
    if (m_camera) {
        m_camera->setAspectRatio(this->width() / (float)this->height());
    }
    if (m_cameraController) { m_cameraController->setWindowSize(this->size()); }
    if (m_compositor) { m_compositor->setSize(this->size()); }
}

Qt3DCore::QEntity* MainWindow::createSceneGraph()
{
    auto rootEntity = new Qt3DCore::QEntity;

    auto meshRenderer = new Qt3DRender::QMesh(rootEntity);
    meshRenderer->setSource(
        QUrl::fromLocalFile(QStringLiteral("./data/mesh/bunny.obj")));

    auto transparentMaterial0 = new Klein::WBOITMaterial(rootEntity);
    transparentMaterial0->setBaseColor(QColor("orange"));
    auto transparentMaterial1 = new Klein::WBOITMaterial(rootEntity);
    transparentMaterial1->setBaseColor(QColor("gray"));
    auto transparentMaterial2 = new Klein::WBOITMaterial(rootEntity);
    transparentMaterial2->setBaseColor(QColor("lightskyblue"));
    auto transparentMaterial3 = new Klein::WBOITMaterial(rootEntity);
    transparentMaterial3->setBaseColor(QColor("orchid"));
    auto opaqueMaterial = new Klein::UnlitMaterial(rootEntity);
    opaqueMaterial->setBaseColor(QColor("teal"));

    auto transform0 = new Qt3DCore::QTransform(rootEntity);
    transform0->setTranslation(QVector3D(1.0f, 0.0f, 1.0f));
    transform0->setScale(0.5f);
    auto transform1 = new Qt3DCore::QTransform(rootEntity);
    transform1->setTranslation(QVector3D(1.0f, 0.0f, -1.0f));
    transform1->setScale(0.5f);
    auto transform2 = new Qt3DCore::QTransform(rootEntity);
    transform2->setTranslation(QVector3D(-1.0f, 0.0f, -1.0f));
    transform2->setScale(0.5f);
    auto transform3 = new Qt3DCore::QTransform(rootEntity);
    transform3->setTranslation(QVector3D(-1.0f, 0.0f, 1.0f));
    transform3->setScale(0.5f);
    auto transform4 = new Qt3DCore::QTransform(rootEntity);
    transform4->setScale(0.5f);

    // WBOIT material also needs an offscreen render target
    m_compositor = new Klein::WBOITCompositor(rootEntity);
    m_compositor->setSize(this->size());

    auto transparentMesh0 = new Qt3DCore::QEntity(rootEntity);
    transparentMesh0->addComponent(transparentMaterial0);
    transparentMesh0->addComponent(meshRenderer);
    transparentMesh0->addComponent(transform0);
    auto transparentMesh1 = new Qt3DCore::QEntity(rootEntity);
    transparentMesh1->addComponent(transparentMaterial1);
    transparentMesh1->addComponent(meshRenderer);
    transparentMesh1->addComponent(transform1);
    auto transparentMesh2 = new Qt3DCore::QEntity(rootEntity);
    transparentMesh2->addComponent(transparentMaterial2);
    transparentMesh2->addComponent(meshRenderer);
    transparentMesh2->addComponent(transform2);
    auto transparentMesh3 = new Qt3DCore::QEntity(rootEntity);
    transparentMesh3->addComponent(transparentMaterial3);
    transparentMesh3->addComponent(meshRenderer);
    transparentMesh3->addComponent(transform3);
    auto opaqueMesh = new Qt3DCore::QEntity(rootEntity);
    opaqueMesh->addComponent(opaqueMaterial);
    opaqueMesh->addComponent(meshRenderer);
    opaqueMesh->addComponent(transform4);

    m_camera = new Qt3DRender::QCamera(rootEntity);
    m_camera->setPosition(QVector3D(2.0f, 0.0f, 0.0f));
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

    auto surfaceSelector = new Qt3DRender::QRenderSurfaceSelector(rootNode);
    surfaceSelector->setSurface(this);

    auto viewport = new Qt3DRender::QViewport(surfaceSelector);
    viewport->setNormalizedRect(QRect(0, 0, 1, 1));

    // Clear default framebuffer
    {
        auto clearBuffers = new Qt3DRender::QClearBuffers(viewport);
        clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
        clearBuffers->setClearColor(QColor(255, 255, 255));

        new Qt3DRender::QNoDraw(clearBuffers);
    }

    // Render opaque entities first
    {
        auto cameraSelector = new Qt3DRender::QCameraSelector(viewport);
        cameraSelector->setCamera(m_camera);

        Klein::BaseUnlitMaterial::attachRenderPassTo(cameraSelector);
    }

    // Render transparent entities
    {
        auto cameraSelector = new Qt3DRender::QCameraSelector(viewport);
        cameraSelector->setCamera(m_camera);

        // Render transparency to a composition target
        {
            auto target = m_compositor->attachRenderTargetTo(cameraSelector);

            // FIXME:
            // Qt3D doesn't support blitting depth buffer
            // Here we render opaque objects again to get depth buffer
            // Could render to a fbo instead and copy to the default one
            auto colorMask = new Qt3DRender::QColorMask;
            colorMask->setRedMasked(false);
            colorMask->setGreenMasked(false);
            colorMask->setBlueMasked(false);
            colorMask->setAlphaMasked(false);
            auto renderStateSet = new Qt3DRender::QRenderStateSet(target);
            renderStateSet->addRenderState(colorMask);
            Klein::BaseUnlitMaterial::attachRenderPassTo(renderStateSet);

            Klein::WBOITMaterial::attachTranparentPassTo(target);
        }

        // Composite transparency to the on-screen target
        {
            m_compositor->attachCompositionPassTo(cameraSelector);
        }
    }

    auto settings = new Qt3DRender::QRenderSettings(root);
    settings->setActiveFrameGraph(rootNode);
    return settings;
}
