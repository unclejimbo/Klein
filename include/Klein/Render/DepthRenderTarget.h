#ifndef KLEIN_DEPTHRENDERTARGET_H
#define KLEIN_DEPTHRENDERTARGET_H

#include <Klein/Core/Export.h>

#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QTexture>

namespace Klein
{

class KLEIN_API DepthRenderTarget : public Qt3DRender::QRenderTarget
{
    Q_OBJECT

public:
    DepthRenderTarget(const QSize& size, Qt3DCore::QNode* parent = nullptr);

    DepthRenderTarget(
        const QSize& size,
        Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint,
        Qt3DCore::QNode* parent = nullptr);

    virtual ~DepthRenderTarget() = default;

    const Qt3DRender::QTexture2D* depthTexture() const
    {
        return m_depthTexture;
    }

    QSize size() const { return m_size; }

public slots:
    void setSize(const QSize& size)
    {
        m_size = size;
        m_depthTexture->setSize(size.width(), size.height());
    }

private:
    QSize m_size;
    Qt3DRender::QRenderTargetOutput* m_depthOutput;
    Qt3DRender::QTexture2D* m_depthTexture;
};

} // namespace Klein

#endif
