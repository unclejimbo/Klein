#ifndef KLEIN_MESHGEOMETRY_H
#define KLEIN_MESHGEOMETRY_H

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometry>

namespace Qt3DRender
{
class QBuffer;
} // namespace Qt3DRender

namespace Klein
{

class MeshGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT

public:
    enum RequiredBuffers
    {
        BASIC,
        WITH_TEXCOORD,
        WITH_COLOR
    };

public:
    MeshGeometry(RequiredBuffers requiredBuffers = BASIC,
                 Qt3DCore::QNode* parent = nullptr);

public slots:
    void setBuffer(Qt3DRender::QBuffer* buffer,
                   uint count = 0,
                   bool interleaved = false,
                   Qt3DRender::QAttribute::VertexBaseType dataType =
                       Qt3DRender::QAttribute::Float,
                   uint dataTypeSize = sizeof(float));

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
    Qt3DRender::QAttribute* m_positions;
    Qt3DRender::QAttribute* m_normals;
    Qt3DRender::QAttribute* m_texCoords;
    Qt3DRender::QAttribute* m_colors;
};

} // namespace Klein

#endif
