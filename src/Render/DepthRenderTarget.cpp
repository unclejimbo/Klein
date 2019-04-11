#include <Klein/Render/DepthRenderTarget.h>

namespace Klein
{

DepthRenderTarget::DepthRenderTarget(const QSize& size, Qt3DCore::QNode* parent)
    : DepthRenderTarget(
          size,
          Qt3DRender::QRenderTargetOutput::AttachmentPoint::Color0,
          parent)
{}

DepthRenderTarget::DepthRenderTarget(
    const QSize& size,
    Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint,
    Qt3DCore::QNode* parent)
    : Qt3DRender::QRenderTarget(parent), m_size(size)
{
    m_depthTexture = new Qt3DRender::QTexture2D();
    m_depthTexture->setSize(size.width(), size.height());
    m_depthTexture->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
    m_depthTexture->setGenerateMipMaps(false);
    m_depthTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_depthTexture->setMagnificationFilter(
        Qt3DRender::QAbstractTexture::Linear);
    m_depthTexture->setComparisonFunction(
        Qt3DRender::QAbstractTexture::CompareLessEqual);
    m_depthTexture->setComparisonMode(
        Qt3DRender::QAbstractTexture::CompareRefToTexture);
    m_depthOutput = new Qt3DRender::QRenderTargetOutput(this);
    m_depthOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);
    m_depthOutput->setTexture(m_depthTexture);
    this->addOutput(m_depthOutput);
}

void DepthRenderTarget::setSize(const QSize& size)
{
    m_size = size;
    m_depthTexture->setSize(size.width(), size.height());
}

} // namespace Klein
