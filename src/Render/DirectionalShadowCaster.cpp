#include <Klein/Render/DirectionalShadowCaster.h>

#include <Qt3DRender/QCameraLens>
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
    m_depthTexture->setComparisonFunction(
        Qt3DRender::QAbstractTexture::CompareLess);

    m_depthOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);
    m_depthOutput->setTexture(m_depthTexture);

    m_depthTarget->addOutput(m_depthOutput);
}

} // namespace Klein
