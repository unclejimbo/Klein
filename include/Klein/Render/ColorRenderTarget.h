#ifndef KLEIN_COLORRENDERTARGET_H
#define KLEIN_COLORRENDERTARGET_H

#include <Klein/Core/Export.h>

#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QTexture>

namespace Klein
{

class KLEIN_API ColorRenderTarget : public Qt3DRender::QRenderTarget
{
    Q_OBJECT

public:
    ColorRenderTarget(const QSize& size, Qt3DCore::QNode* parent = nullptr);

    ColorRenderTarget(
        const QSize& size,
        Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint,
        Qt3DCore::QNode* parent = nullptr);

    const Qt3DRender::QTexture2D* colorTexture() const
    {
        return m_colorTexture;
    }

    const Qt3DRender::QTexture2D* depthTexture() const
    {
        return m_depthTexture;
    }

    QSize size() const { return m_size; }

public slots:
    void setSize(const QSize& size)
    {
        m_size = size;
        m_colorTexture->setSize(size.width(), size.height());
        m_depthTexture->setSize(size.width(), size.height());
    }

private:
    QSize m_size;
    Qt3DRender::QRenderTargetOutput* m_colorOutput = nullptr;
    Qt3DRender::QRenderTargetOutput* m_depthOutput = nullptr;
    Qt3DRender::QTexture2D* m_colorTexture = nullptr;
    Qt3DRender::QTexture2D* m_depthTexture = nullptr;
};

} // namespace Klein

#endif
