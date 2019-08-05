#include <Klein/Render/MeshGeometry.h>

#include <QtGlobal>
#include <Qt3DRender/QBuffer>

namespace Klein
{

MeshGeometry::MeshGeometry(AdditionalAttributes attributes,
                           Qt3DCore::QNode* parent)
    : Qt3DRender::QGeometry(parent)
{
    m_positions = new Qt3DRender::QAttribute(this);
    m_positions->setName(
        Qt3DRender::QAttribute::defaultPositionAttributeName());
    m_positions->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_positions->setVertexSize(3);
    this->addAttribute(m_positions);

    if (attributes & ADDITIONAL_ATTRIBUTE_NORMAL) {
        m_normals = new Qt3DRender::QAttribute(this);
        m_normals->setName(
            Qt3DRender::QAttribute::defaultNormalAttributeName());
        m_normals->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        m_normals->setVertexSize(3);
        this->addAttribute(m_normals);
    }

    if (attributes & ADDITIONAL_ATTRIBUTE_TEXCOORD) {
        m_texCoords = new Qt3DRender::QAttribute(this);
        m_texCoords->setName(
            Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName());
        m_texCoords->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        m_texCoords->setVertexSize(2);
        this->addAttribute(m_texCoords);
    }

    if (attributes & ADDITIONAL_ATTRIBUTE_COLOR) {
        m_colors = new Qt3DRender::QAttribute(this);
        m_colors->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
        m_colors->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        m_colors->setVertexSize(3);
        this->addAttribute(m_colors);
    }
}

void MeshGeometry::setCount(uint count)
{
    m_positions->setCount(count);
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
    if (m_normals != nullptr) {
        m_normals->setBuffer(buffer);
        m_normals->setVertexBaseType(dataType);
        m_normals->setCount(count);
        m_normals->setByteOffset(byteOffset);
        m_normals->setByteStride(byteStride);
    }
    else {
        qWarning()
            << "[Warning] MeshGeometry::setNormalBuffer called on an object "
               "without normal attribute.";
    }
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
    else {
        qWarning()
            << "[Warning] MeshGeometry::setTexCoordBuffer called on an object "
               "without texCoord attribute.";
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
    else {
        qWarning()
            << "[Warning] MeshGeometry::setColorBuffer called on an object "
               "without color attribute.";
    }
}

} // namespace Klein
