#ifndef KLEIN_CONCRETEPOINTSRENDERER_H
#define KLEIN_CONCRETEPOINTSRENDERER_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QVector>
#include <QVector3D>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/QSphereGeometry>

namespace Qt3DRender
{
class QAttribute;
class QBuffer;
} // namespace Qt3DRender

namespace Klein
{

class KLEIN_API ConcretePointsRenderer : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT

public:
    enum RequiredBuffers
    {
        BASIC,
        WITH_COLOR
    };

public:
    ConcretePointsRenderer(RequiredBuffers requiredBuffers = BASIC,
                           Qt3DCore::QNode* parent = nullptr);

    virtual ~ConcretePointsRenderer() = default;

public slots:
    void setRadius(float radius) { m_sphere->setRadius(radius); }

    void setPositions(const QVector<QVector3D>& positions);

    void setColors(const QVector<QColor>& colors);

private:
    Qt3DExtras::QSphereGeometry* m_sphere;
    Qt3DRender::QBuffer* m_modelBuffer;
    Qt3DRender::QBuffer* m_colorBuffer;
    Qt3DRender::QAttribute* m_instanceModel;
    Qt3DRender::QAttribute* m_instanceColor;
};

} // namespace Klein

#endif
