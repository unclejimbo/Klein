#ifndef KLEIN_DIRECTIONALSHADOWCASTER_H
#define KLEIN_DIRECTIONALSHADOWCASTER_H

#include <Klein/Core/Export.h>

#include <QMatrix4x4>
#include <QVector3D>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QTexture>

namespace Qt3DRender
{
class QFrameGraphNode;
class QRenderTarget;
class QRenderTargetOutput;
} // namespace Qt3DRender

namespace Klein
{

class KLEIN_API DirectionalShadowCaster : public Qt3DCore::QNode
{
    Q_OBJECT

public:
    explicit DirectionalShadowCaster(Qt3DCore::QNode* parent = nullptr);

    Qt3DRender::QCamera* camera() { return m_camera; }

    QVector3D lightDirection() const { return -m_camera->viewVector(); }

    QVector3D lightPosition() const { return m_camera->position(); }

    QMatrix4x4 lightSpaceMatrix() const
    {
        return m_camera->projectionMatrix() * m_camera->viewMatrix();
    }

    Qt3DRender::QRenderTarget* renderTarget() { return m_depthTarget; }

    Qt3DRender::QTexture2D* shadowMap() { return m_depthTexture; }

    QSize size() const { return m_size; }

    Qt3DRender::QFrameGraphNode* attachTo(Qt3DRender::QFrameGraphNode* parent);

public slots:
    void lookAt(const QVector3D& position,
                const QVector3D& focus,
                const QVector3D& up)
    {
        m_camera->setPosition(position);
        m_camera->setViewCenter(focus);
        m_camera->setUpVector(up);
    }

    void setFrustum(float left,
                    float right,
                    float bottom,
                    float top,
                    float nearplane,
                    float farplane)
    {
        m_camera->setLeft(left);
        m_camera->setRight(right);
        m_camera->setBottom(bottom);
        m_camera->setTop(top);
        m_camera->setNearPlane(nearplane);
        m_camera->setFarPlane(farplane);
    }

    void setSize(const QSize& size)
    {
        m_size = size;
        m_depthTexture->setSize(size.width(), size.height());
    }

private:
    QSize m_size;
    Qt3DRender::QCamera* m_camera = nullptr;
    Qt3DRender::QRenderTarget* m_depthTarget = nullptr;
    Qt3DRender::QRenderTargetOutput* m_depthOutput = nullptr;
    Qt3DRender::QTexture2D* m_depthTexture = nullptr;
};

} // namespace Klein

#endif
