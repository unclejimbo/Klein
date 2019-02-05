#ifndef KLEIN_INSTANCEDMESH_H
#define KLEIN_INSTANCEDMESH_H

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometryRenderer>

namespace Qt3DRender
{
class QBuffer;
}

namespace Klein
{
class InstancedMesh : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT

public:
    enum RequiredBuffers
    {
        BASIC,
        WITH_COLOR
    };

public:
    InstancedMesh(RequiredBuffers requiredBuffers = BASIC,
                  Qt3DCore::QNode* parent = nullptr);

    void setBuffer(Qt3DRender::QBuffer* buffer,
                   bool interleaved = true,
                   uint count = 0,
                   Qt3DRender::QAttribute::VertexBaseType dataType =
                       Qt3DRender::QAttribute::Float,
                   uint dataTypeSize = sizeof(float));

    void setInstanceModelMatrixBuffer(
        Qt3DRender::QBuffer* buffer,
        uint count = 0,
        uint byteOffset = 0,
        uint byteStride = 16 * sizeof(float),
        Qt3DRender::QAttribute::VertexBaseType dataType =
            Qt3DRender::QAttribute::Float);

    void setInstanceColorBuffer(Qt3DRender::QBuffer* buffer,
                                uint count = 0,
                                uint byteOffset = 0,
                                uint byteStride = 3 * sizeof(float),
                                Qt3DRender::QAttribute::VertexBaseType
                                    dataType = Qt3DRender::QAttribute::Float);

private:
    Qt3DRender::QAttribute* m_instanceModel;
    Qt3DRender::QAttribute* m_instanceColor;
};

} // namespace Klein

#endif
