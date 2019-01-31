#include <Klein/Render/OffscreenRenderTarget.h>

namespace Klein
{

OffscreenRenderTarget::OffscreenRenderTarget(const QSize& size,
                                             Qt3DCore::QNode* parent)
    : OffscreenRenderTarget(
          size,
          Qt3DRender::QRenderTargetOutput::AttachmentPoint::Color0,
          parent)
{}

OffscreenRenderTarget::OffscreenRenderTarget(
    const QSize& size,
    Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint,
    Qt3DCore::QNode* parent)
    : Qt3DRender::QRenderTarget(parent), m_size(size)
{
    m_output = new Qt3DRender::QRenderTargetOutput(this);
    m_output->setAttachmentPoint(attatchmentPoint);
    m_texture = new Qt3DRender::QTexture2D(m_output);
    m_texture->setSize(size.width(), size.height());
    m_texture->setFormat(Qt3DRender::QAbstractTexture::RGB8_UNorm);
    m_texture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_texture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_output->setTexture(m_texture);
    this->addOutput(m_output);

    m_depthTextureOutput = new Qt3DRender::QRenderTargetOutput(this);
    m_depthTextureOutput->setAttachmentPoint(
        Qt3DRender::QRenderTargetOutput::Depth);
    m_depthTexture = new Qt3DRender::QTexture2D(m_depthTextureOutput);
    m_depthTexture->setSize(size.width(), size.height());
    m_depthTexture->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
    m_depthTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_depthTexture->setMagnificationFilter(
        Qt3DRender::QAbstractTexture::Linear);
    m_depthTexture->setComparisonFunction(
        Qt3DRender::QAbstractTexture::CompareLessEqual);
    m_depthTexture->setComparisonMode(
        Qt3DRender::QAbstractTexture::CompareRefToTexture);
    m_depthTextureOutput->setTexture(m_depthTexture);
    this->addOutput(m_depthTextureOutput);
}

Qt3DRender::QTexture2D* OffscreenRenderTarget::getTexture()
{
    return m_texture;
}

void OffscreenRenderTarget::setSize(const QSize& size)
{
    m_size = size;
    m_texture->setSize(size.width(), size.height());
    m_depthTexture->setSize(size.width(), size.height());
}

} // namespace Klein
