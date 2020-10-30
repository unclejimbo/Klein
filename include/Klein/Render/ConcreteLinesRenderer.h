#ifndef KLEIN_CONCRETELINESRENDERER_H
#define KLEIN_CONCRETELINESRENDERER_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QVector>
#include <QVector3D>
#include <Qt3DRender/QGeometryRenderer>

namespace Qt3DRender
{
class QAttribute;
class QBuffer;
class QGeometry;
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

    enum LineStyle
    {
        CYLINDER,
        CONE
    };

public:
    explicit ConcreteLinesRenderer(Qt3DCore::QNode* parent = nullptr);

    ConcreteLinesRenderer(AdditionalAttributes attributes,
                          Qt3DCore::QNode* parent = nullptr);

    ConcreteLinesRenderer(AdditionalAttributes attributes,
                          LineStyle style,
                          Qt3DCore::QNode* parent = nullptr);

public slots:
    void setLineStyle(LineStyle style);

    void setRadius(float radius);

    void setPositions(const QVector<QVector3D>& positions,
                      ConcreteLinesRenderer::LineType type = LINES);

    void setColors(const QVector<QColor>& colors);

private:
    LineStyle m_style = CYLINDER;
    float m_radius = 1.0f;
    Qt3DRender::QGeometry* m_geometry = nullptr;
    Qt3DRender::QBuffer* m_modelBuffer = nullptr;
    Qt3DRender::QBuffer* m_colorBuffer = nullptr;
    Qt3DRender::QAttribute* m_instanceModel = nullptr;
    Qt3DRender::QAttribute* m_instanceColor = nullptr;
};

} // namespace Klein

#endif
