#include <Klein/Render/ColorRenderTarget.h>

namespace Klein
{

ColorRenderTarget::ColorRenderTarget(const QSize& size, Qt3DCore::QNode* parent)
    : ColorRenderTarget(
          size,
          Qt3DRender::QRenderTargetOutput::AttachmentPoint::Color0,
          parent)
{}

ColorRenderTarget::ColorRenderTarget(
    const QSize& size,
    Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint,
    Qt3DCore::QNode* parent)
    : Qt3DRender::QRenderTarget(parent), m_size(size)
{
    m_colorOutput = new Qt3DRender::QRenderTargetOutput(this);
    m_colorOutput->setAttachmentPoint(attatchmentPoint);
    this->addOutput(m_colorOutput);

    m_colorTexture = new Qt3DRender::QTexture2D(m_colorOutput);
    m_colorTexture->setSize(size.width(), size.height());
    m_colorTexture->setFormat(Qt3DRender::QAbstractTexture::RGB8_UNorm);
    m_colorTexture->setGenerateMipMaps(false);
    m_colorTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_colorTexture->setMagnificationFilter(
        Qt3DRender::QAbstractTexture::Linear);
    m_colorOutput->setTexture(m_colorTexture);

    m_depthOutput = new Qt3DRender::QRenderTargetOutput(this);
    m_depthOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);
    this->addOutput(m_depthOutput);

    m_depthTexture = new Qt3DRender::QTexture2D(m_depthOutput);
    m_depthTexture->setSize(size.width(), size.height());
    m_depthTexture->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
    m_depthTexture->setGenerateMipMaps(false);
    m_depthTexture->setMinificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);
    m_depthTexture->setMagnificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);
    m_depthOutput->setTexture(m_depthTexture);
}

} // namespace Klein
