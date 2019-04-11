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

    virtual ~ColorRenderTarget() = default;

    QSize size() const { return m_size; }

public slots:
    void setSize(const QSize& size);

private:
    QSize m_size;
    Qt3DRender::QRenderTargetOutput* m_colorOutput;
    Qt3DRender::QRenderTargetOutput* m_depthOutput;
    Qt3DRender::QTexture2D* m_colorTexture;
    Qt3DRender::QTexture2D* m_depthTexture;
};

} // namespace Klein

#endif
