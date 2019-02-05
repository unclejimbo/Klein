#include <Klein/Render/InstancedMesh.h>

namespace Klein
{

InstancedMesh::InstancedMesh(RequiredBuffers requiredBuffers,
                             Qt3DCore::QNode* parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    m_instanceModel = new Qt3DRender::QAttribute(this);
    m_instanceModel->setName(QStringLiteral("instanceModel"));
    m_instanceModel->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_instanceModel->setVertexSize(16);
    m_instanceModel->setDivisor(1);
    m_instanceColor == nullptr;
    if (requiredBuffers == WITH_COLOR) {
        m_instanceColor = new Qt3DRender::QAttribute(this);
        m_instanceColor->setName(QStringLiteral("instanceColor"));
        m_instanceColor->setAttributeType(
            Qt3DRender::QAttribute::VertexAttribute);
        m_instanceColor->setVertexSize(3);
        m_instanceColor->setDivisor(1);
    }

    connect(this,
            &Qt3DRender::QGeometryRenderer::geometryChanged,
            this,
            [this](Qt3DRender::QGeometry* geometry) {
                geometry->addAttribute(m_instanceModel);
                if (m_instanceColor != nullptr) {
                    geometry->addAttribute(m_instanceColor);
                }
            });
    connect(this,
            &Qt3DRender::QGeometryRenderer::instanceCountChanged,
            this,
            [this](int instanceCount) {
                m_instanceModel->setCount(instanceCount);
                if (m_instanceColor != nullptr) {
                    m_instanceColor->setCount(instanceCount);
                }
            });
}

void InstancedMesh::setBuffer(Qt3DRender::QBuffer* buffer,
                              bool interleaved,
                              uint count,
                              Qt3DRender::QAttribute::VertexBaseType dataType,
                              uint dataTypeSize)
{
    m_instanceModel->setBuffer(buffer);
    m_instanceModel->setVertexBaseType(dataType);
    m_instanceModel->setCount(count);
    if (m_instanceColor != nullptr) {
        m_instanceColor->setBuffer(buffer);
        m_instanceColor->setVertexBaseType(dataType);
        m_instanceColor->setCount(count);
    }
    if (interleaved) {
        auto stride = 16;
        if (m_instanceColor != nullptr) { stride = 19; }
        stride *= dataTypeSize;
        m_instanceModel->setByteOffset(0);
        m_instanceModel->setByteStride(stride);
        if (m_instanceColor != nullptr) {
            m_instanceColor->setByteOffset(16 * dataTypeSize);
            m_instanceColor->setByteStride(stride);
        }
    }
    else {
        m_instanceModel->setByteOffset(0);
        m_instanceModel->setByteStride(16 * dataTypeSize);
        if (m_instanceColor != nullptr) {
            m_instanceColor->setByteOffset(16 * count * dataTypeSize);
            m_instanceColor->setByteOffset(3 * dataTypeSize);
        }
    }
}

void InstancedMesh::setInstanceModelMatrixBuffer(
    Qt3DRender::QBuffer* buffer,
    uint count,
    uint byteOffset,
    uint byteStride,
    Qt3DRender::QAttribute::VertexBaseType dataType)
{
    m_instanceModel->setBuffer(buffer);
    m_instanceModel->setVertexBaseType(dataType);
    m_instanceModel->setCount(count);
    m_instanceModel->setByteOffset(byteOffset);
    m_instanceModel->setByteStride(byteStride);
}

void InstancedMesh::setInstanceColorBuffer(
    Qt3DRender::QBuffer* buffer,
    uint count,
    uint byteOffset,
    uint byteStride,
    Qt3DRender::QAttribute::VertexBaseType dataType)
{
    if (m_instanceColor != nullptr) {
        m_instanceColor->setBuffer(buffer);
        m_instanceColor->setVertexBaseType(dataType);
        m_instanceColor->setCount(count);
        m_instanceColor->setByteOffset(byteOffset);
        m_instanceColor->setByteStride(byteStride);
    }
}

} // namespace Klein
