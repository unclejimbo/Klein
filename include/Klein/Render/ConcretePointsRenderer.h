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
    enum AdditionalAttributeBits : char
    {
        ADDITIONAL_ATTRIBUTE_NONE = 0x0,
        ADDITIONAL_ATTRIBUTE_COLOR = 0x1
    };
    using AdditionalAttributes = char;

public:
    ConcretePointsRenderer(
        AdditionalAttributes attributes = ADDITIONAL_ATTRIBUTE_NONE,
        Qt3DCore::QNode* parent = nullptr);

public slots:
    void setRadius(float radius) { m_sphere->setRadius(radius); }

    void setPositions(const QVector<QVector3D>& positions);

    void setColors(const QVector<QColor>& colors);

private:
    Qt3DExtras::QSphereGeometry* m_sphere = nullptr;
    Qt3DRender::QBuffer* m_modelBuffer = nullptr;
    Qt3DRender::QBuffer* m_colorBuffer = nullptr;
    Qt3DRender::QAttribute* m_instanceModel = nullptr;
    Qt3DRender::QAttribute* m_instanceColor = nullptr;
};

} // namespace Klein

#endif
