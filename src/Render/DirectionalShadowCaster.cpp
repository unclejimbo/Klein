#include <Klein/Render/DirectionalShadowCaster.h>

#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QTextureWrapMode>

namespace Klein
{

DirectionalShadowCaster::DirectionalShadowCaster(Qt3DCore::QNode* parent)
    : Qt3DCore::QNode(parent)
{
    m_camera = new Qt3DRender::QCamera(this);
    m_camera->setProjectionType(
        Qt3DRender::QCameraLens::OrthographicProjection);

    m_depthTarget = new Qt3DRender::QRenderTarget(this);
    m_depthOutput = new Qt3DRender::QRenderTargetOutput(m_depthTarget);
    m_depthTexture = new Qt3DRender::QTexture2D(m_depthOutput);

    // FIXME:
    // Should set to ClampToBorder,
    // but Qt3D doesn't support setting border color
    Qt3DRender::QTextureWrapMode wrapmode;
    wrapmode.setX(Qt3DRender::QTextureWrapMode::ClampToEdge);
    wrapmode.setY(Qt3DRender::QTextureWrapMode::ClampToEdge);
    m_depthTexture->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
    m_depthTexture->setWrapMode(wrapmode);
    m_depthTexture->setGenerateMipMaps(false);
    m_depthTexture->setMinificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);
    m_depthTexture->setMagnificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);

    m_depthOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);
    m_depthOutput->setTexture(m_depthTexture);

    m_depthTarget->addOutput(m_depthOutput);
}

Qt3DRender::QFrameGraphNode* DirectionalShadowCaster::attachTo(
    Qt3DRender::QFrameGraphNode* parent)
{

    auto target = new Qt3DRender::QRenderTargetSelector(parent);
    target->setTarget(m_depthTarget);

    auto clearBuffers = new Qt3DRender::QClearBuffers(target);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    new Qt3DRender::QNoDraw(clearBuffers);

    auto camera = new Qt3DRender::QCameraSelector(target);
    camera->setCamera(m_camera);

    return camera;
}

} // namespace Klein
