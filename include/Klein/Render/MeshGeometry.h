#ifndef KLEIN_MESHGEOMETRY_H
#define KLEIN_MESHGEOMETRY_H

#include <Klein/Core/Export.h>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometry>

namespace Qt3DRender
{
class QBuffer;
} // namespace Qt3DRender

namespace Klein
{

class KLEIN_API MeshGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT

public:
    enum AdditionalAttributeBits : char
    {
        ADDITIONAL_ATTRIBUTE_NONE = 0x0,
        ADDITIONAL_ATTRIBUTE_NORMAL = 0x1,
        ADDITIONAL_ATTRIBUTE_TEXCOORD = 0x2,
        ADDITIONAL_ATTRIBUTE_COLOR = 0x4
    };
    using AdditionalAttributes = char;

public:
    MeshGeometry(AdditionalAttributes attributes = ADDITIONAL_ATTRIBUTE_NONE,
                 Qt3DCore::QNode* parent = nullptr);

    virtual ~MeshGeometry() = default;

public slots:
    void setCount(uint count);

    void setPositionBuffer(Qt3DRender::QBuffer* buffer,
                           uint count = 0,
                           uint byteOffset = 0,
                           uint byteStride = 3 * sizeof(float),
                           Qt3DRender::QAttribute::VertexBaseType dataType =
                               Qt3DRender::QAttribute::Float);

    void setNormalBuffer(Qt3DRender::QBuffer* buffer,
                         uint count = 0,
                         uint byteOffset = 0,
                         uint byteStride = 3 * sizeof(float),
                         Qt3DRender::QAttribute::VertexBaseType dataType =
                             Qt3DRender::QAttribute::Float);

    void setTexCoordBuffer(Qt3DRender::QBuffer* buffer,
                           uint count = 0,
                           uint byteOffset = 0,
                           uint byteStride = 2 * sizeof(float),
                           Qt3DRender::QAttribute::VertexBaseType dataType =
                               Qt3DRender::QAttribute::Float);

    void setColorBuffer(Qt3DRender::QBuffer* buffer,
                        uint count = 0,
                        uint byteOffset = 0,
                        uint byteStride = 3 * sizeof(float),
                        Qt3DRender::QAttribute::VertexBaseType dataType =
                            Qt3DRender::QAttribute::Float);

protected:
    Qt3DRender::QAttribute* m_positions = nullptr;
    Qt3DRender::QAttribute* m_normals = nullptr;
    Qt3DRender::QAttribute* m_texCoords = nullptr;
    Qt3DRender::QAttribute* m_colors = nullptr;
};

} // namespace Klein

#endif
