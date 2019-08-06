#ifndef KLEIN_CONCRETELINESRENDERER_H
#define KLEIN_CONCRETELINESRENDERER_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QVector>
#include <QVector3D>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/QCylinderGeometry>

namespace Qt3DRender
{
class QAttribute;
class QBuffer;
} // namespace Qt3DRender

namespace Klein
{

class KLEIN_API ConcreteLinesRenderer : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT

public:
    enum AdditionalAttributeBits : char
    {
        ADDITIONAL_ATTRIBUTE_NONE = 0x0,
        ADDITIONAL_ATTRIBUTE_COLOR = 0x1
    };
    using AdditionalAttributes = char;

    enum LineType
    {
        LINES,
        LINE_STRIP,
        LINE_LOOP
    };

public:
    ConcreteLinesRenderer(
        AdditionalAttributes attributes = ADDITIONAL_ATTRIBUTE_NONE,
        Qt3DCore::QNode* parent = nullptr);

    virtual ~ConcreteLinesRenderer() = default;

public slots:
    void setRadius(float radius) { m_cylinder->setRadius(radius); }

    void setPositions(const QVector<QVector3D>& positions,
                      LineType type = LINES);

    void setColors(const QVector<QColor>& colors);

private:
    Qt3DExtras::QCylinderGeometry* m_cylinder = nullptr;
    Qt3DRender::QBuffer* m_modelBuffer = nullptr;
    Qt3DRender::QBuffer* m_colorBuffer = nullptr;
    Qt3DRender::QAttribute* m_instanceModel = nullptr;
    Qt3DRender::QAttribute* m_instanceColor = nullptr;
};

} // namespace Klein

#endif
