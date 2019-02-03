#include <Klein/Render/MeshGeometry.h>

#include <Qt3DRender/QBuffer>

namespace Klein
{

MeshGeometry::MeshGeometry(RequiredBuffers requiredBuffers,
                           Qt3DCore::QNode* parent)
    : Qt3DRender::QGeometry(parent)
{
    m_positions = new Qt3DRender::QAttribute(this);
    m_positions->setName(
        Qt3DRender::QAttribute::defaultPositionAttributeName());
    m_positions->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_positions->setVertexSize(3);
    this->addAttribute(m_positions);
    m_normals = new Qt3DRender::QAttribute(this);
    m_normals->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
    m_normals->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_normals->setVertexSize(3);
    this->addAttribute(m_normals);
    m_colors = nullptr;
    m_texCoords = nullptr;
    if (requiredBuffers == WITH_TEXCOORD) {
        m_texCoords = new Qt3DRender::QAttribute(this);
        m_texCoords->setName(
            Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName());
        m_texCoords->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        m_texCoords->setVertexSize(2);
        this->addAttribute(m_texCoords);
    }
    else if (requiredBuffers == WITH_COLOR) {
        m_colors = new Qt3DRender::QAttribute(this);
        m_colors->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
        m_colors->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        m_colors->setVertexSize(3);
        this->addAttribute(m_colors);
    }
}

void MeshGeometry::setBuffer(Qt3DRender::QBuffer* buffer,
                             bool interleaved,
                             uint count,
                             Qt3DRender::QAttribute::VertexBaseType dataType,
                             uint dataTypeSize)
{
    m_positions->setBuffer(buffer);
    m_positions->setVertexBaseType(dataType);
    m_positions->setCount(count);
    m_normals->setBuffer(buffer);
    m_normals->setVertexBaseType(dataType);
    m_normals->setCount(count);
    if (m_texCoords != nullptr) {
        m_texCoords->setBuffer(buffer);
        m_texCoords->setVertexBaseType(dataType);
        m_texCoords->setCount(count);
    }
    else if (m_colors != nullptr) {
        m_colors->setBuffer(buffer);
        m_colors->setVertexBaseType(dataType);
        m_colors->setCount(count);
    }
    if (interleaved) {
        auto stride = 6;
        if (m_texCoords != nullptr) { stride = 8; }
        else if (m_colors != nullptr) {
            stride = 9;
        }
        stride *= dataTypeSize;
        m_positions->setByteOffset(0);
        m_positions->setByteStride(stride);
        m_normals->setByteOffset(3 * dataTypeSize);
        m_normals->setByteStride(stride);
        if (m_texCoords != nullptr) {
            m_texCoords->setByteOffset(6 * dataTypeSize);
            m_texCoords->setByteStride(stride);
        }
        else if (m_colors != nullptr) {
            m_colors->setByteOffset(6 * dataTypeSize);
            m_colors->setByteStride(stride);
        }
    }
    else {
        m_positions->setByteOffset(0);
        m_positions->setByteStride(3 * dataTypeSize);
        m_normals->setByteOffset(3 * count * dataTypeSize);
        m_normals->setByteStride(3 * dataTypeSize);
        if (m_texCoords != nullptr) {
            m_texCoords->setByteOffset(6 * count * dataTypeSize);
            m_texCoords->setByteStride(2 * dataTypeSize);
        }
        else if (m_colors != nullptr) {
            m_colors->setByteOffset(6 * count * dataTypeSize);
            m_colors->setByteStride(3 * dataTypeSize);
        }
    }
}

void MeshGeometry::setCount(uint count)
{
    if (m_positions != nullptr) { m_positions->setCount(count); }
    if (m_normals != nullptr) { m_normals->setCount(count); }
    if (m_texCoords != nullptr) { m_texCoords->setCount(count); }
    if (m_colors != nullptr) { m_colors->setCount(count); }
}

void MeshGeometry::setPositionBuffer(
    Qt3DRender::QBuffer* buffer,
    uint count,
    uint byteOffset,
    uint byteStride,
    Qt3DRender::QAttribute::VertexBaseType dataType)
{
    m_positions->setBuffer(buffer);
    m_positions->setVertexBaseType(dataType);
    m_positions->setCount(count);
    m_positions->setByteOffset(byteOffset);
    m_positions->setByteStride(byteStride);
}

void MeshGeometry::setNormalBuffer(
    Qt3DRender::QBuffer* buffer,
    uint count,
    uint byteOffset,
    uint byteStride,
    Qt3DRender::QAttribute::VertexBaseType dataType)
{
    m_normals->setBuffer(buffer);
    m_normals->setVertexBaseType(dataType);
    m_normals->setCount(count);
    m_normals->setByteOffset(byteOffset);
    m_normals->setByteStride(byteStride);
}

void MeshGeometry::setTexCoordBuffer(
    Qt3DRender::QBuffer* buffer,
    uint count,
    uint byteOffset,
    uint byteStride,
    Qt3DRender::QAttribute::VertexBaseType dataType)
{
    if (m_texCoords != nullptr) {
        m_texCoords->setBuffer(buffer);
        m_texCoords->setVertexBaseType(dataType);
        m_texCoords->setCount(count);
        m_texCoords->setByteOffset(byteOffset);
        m_texCoords->setByteStride(byteStride);
    }
}

void MeshGeometry::setColorBuffer(
    Qt3DRender::QBuffer* buffer,
    uint count,
    uint byteOffset,
    uint byteStride,
    Qt3DRender::QAttribute::VertexBaseType dataType)
{
    if (m_colors != nullptr) {
        m_colors->setBuffer(buffer);
        m_colors->setVertexBaseType(dataType);
        m_colors->setCount(count);
        m_colors->setByteOffset(byteOffset);
        m_colors->setByteStride(byteStride);
    }
}

} // namespace Klein
