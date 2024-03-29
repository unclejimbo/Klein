#ifndef KLEIN_WBOITMATERIAL_H
#define KLEIN_WBOITMATERIAL_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QVariant>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QTexture>

namespace Qt3DRender
{
class QEffect;
class QFrameGraphNode;
} // namespace Qt3DRender

namespace Klein
{

class KLEIN_API WBOITCompositor : public Qt3DCore::QNode
{
    Q_OBJECT

public:
    explicit WBOITCompositor(Qt3DCore::QNode* parent = nullptr);

    Qt3DRender::QFrameGraphNode* attachRenderTargetTo(
        Qt3DRender::QFrameGraphNode* parent);

    Qt3DRender::QFrameGraphNode* attachCompositionPassTo(
        Qt3DRender::QFrameGraphNode* parent);

    Qt3DCore::QEntity* entity() { return m_entity; }

    QSize size() const { return m_size; }

public slots:
    void setSize(const QSize& size)
    {
        m_size = size;
        m_accumTexture->setSize(size.width(), size.height());
        m_revealageTexture->setSize(size.width(), size.height());
        m_depthTexture->setSize(size.width(), size.height());
    }

private:
    void createTarget(Qt3DCore::QNode* parent);

    void createEntity(Qt3DCore::QNode* parent);

private:
    QSize m_size;
    Qt3DRender::QRenderTarget* m_renderTarget = nullptr;
    Qt3DRender::QRenderTargetOutput* m_accumOutput = nullptr;
    Qt3DRender::QRenderTargetOutput* m_revealageOutput = nullptr;
    Qt3DRender::QRenderTargetOutput* m_depthOutput = nullptr;
    Qt3DRender::QTexture2D* m_accumTexture = nullptr;
    Qt3DRender::QTexture2D* m_revealageTexture = nullptr;
    Qt3DRender::QTexture2D* m_depthTexture = nullptr;
    Qt3DCore::QEntity* m_entity = nullptr;
};

class KLEIN_API WBOITMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT

public:
    enum RenderModeBits : int
    {
        RENDER_MODE_BASE_COLOR = 0x0,
        RENDER_MODE_TEXTURE = 0x1,
        RENDER_MODE_VCOLOR = 0x3
    };
    using RenderMode = int;

public:
    explicit WBOITMaterial(Qt3DCore::QNode* parent = nullptr);

    QColor baseColor() const { return m_baseColor->value().value<int>(); }

    const Qt3DRender::QAbstractTexture* baseColorMap() const
    {
        return m_baseColorMap->value().value<Qt3DRender::QAbstractTexture*>();
    }

    float depthScale() const { return m_depthScale->value().value<float>(); }

    RenderMode renderMode() const
    {
        return m_renderMode->value().value<RenderMode>();
    }

    float texCoordOffset() const
    {
        return m_texCoordOffset->value().value<float>();
    }

    float texCoordScale() const
    {
        return m_texCoordScale->value().value<float>();
    }

public slots:
    void setBaseColor(const QColor& value) { m_baseColor->setValue(value); }

    void setBaseColorMap(Qt3DRender::QAbstractTexture* value)
    {
        m_baseColorMap->setValue(QVariant::fromValue(value));
    }

    void setDepthScale(float value) { m_depthScale->setValue(value); }

    void setRenderMode(WBOITMaterial::RenderMode value)
    {
        m_renderMode->setValue(value);
    }

    void setTexCoordOffset(float value) { m_texCoordOffset->setValue(value); }

    void setTexCoordScale(float value) { m_texCoordScale->setValue(value); }

public:
    static constexpr const char* effectName{ "KLEIN_EFFECT_WBOIT" };

    static Qt3DRender::QEffect* createEffect();

    static Qt3DRender::QFrameGraphNode* attachTranparentPassTo(
        Qt3DRender::QFrameGraphNode* parent);

protected:
    Qt3DRender::QParameter* m_baseColor = nullptr;
    Qt3DRender::QParameter* m_baseColorMap = nullptr;
    Qt3DRender::QParameter* m_depthScale = nullptr;
    Qt3DRender::QParameter* m_renderMode = nullptr;
    Qt3DRender::QParameter* m_texCoordOffset = nullptr;
    Qt3DRender::QParameter* m_texCoordScale = nullptr;
};

} // namespace Klein

Q_DECLARE_METATYPE(Klein::WBOITMaterial::RenderModeBits)

#endif
