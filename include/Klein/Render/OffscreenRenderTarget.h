#ifndef KLEIN_OFFSCREENRENDERTARGET_H
#define KLEIN_OFFSCREENRENDERTARGET_H

#include <Klein/Core/Export.h>

#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QTexture>

namespace Klein
{

class KLEIN_API OffscreenRenderTarget : public Qt3DRender::QRenderTarget
{
    Q_OBJECT

public:
    OffscreenRenderTarget(const QSize& size, Qt3DCore::QNode* parent = nullptr);

    OffscreenRenderTarget(
        const QSize& size,
        Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint,
        Qt3DCore::QNode* parent = nullptr);

    virtual ~OffscreenRenderTarget() = default;

    QSize getSize() const { return m_size; }

    Qt3DRender::QTexture2D* getTexture();

public slots:
    void setSize(const QSize& size);

private:
    QSize m_size;
    Qt3DRender::QRenderTargetOutput* m_output;
    Qt3DRender::QRenderTargetOutput* m_depthTextureOutput;
    Qt3DRender::QTexture2D* m_texture;
    Qt3DRender::QTexture2D* m_depthTexture;
};

} // namespace Klein

#endif
