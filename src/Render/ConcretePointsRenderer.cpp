#include <Klein/Render/ConcretePointsRenderer.h>

#include <Klein/Render/BufferData.h>
#include <Klein/Render/Transformation.h>

#include <algorithm>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>

namespace Klein
{

ConcretePointsRenderer::ConcretePointsRenderer(RequiredBuffers requiredBuffers,
                                               Qt3DCore::QNode* parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    qRegisterMetaType<QVector<QVector3D>>("QVector<QVector3D>");
    qRegisterMetaType<QVector<QColor>>("QVector<QColor>");

    m_sphere = new Qt3DExtras::QSphereGeometry(this);
    m_sphere->setRadius(1.0f);
    m_sphere->setRings(10);
    m_sphere->setSlices(10);
    this->setGeometry(m_sphere);

    m_modelBuffer = new Qt3DRender::QBuffer(this);
    m_instanceModel = new Qt3DRender::QAttribute(this);
    m_instanceModel->setName(QStringLiteral("instanceModel"));
    m_instanceModel->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_instanceModel->setVertexSize(16);
    m_instanceModel->setDivisor(1);
    m_instanceModel->setBuffer(m_modelBuffer);
    m_instanceModel->setVertexBaseType(Qt3DRender::QAttribute::Float);
    m_instanceModel->setByteOffset(0);
    m_instanceModel->setByteStride(16 * sizeof(float));
    m_sphere->addAttribute(m_instanceModel);

    m_colorBuffer = nullptr;
    m_instanceColor = nullptr;
    if (requiredBuffers == WITH_COLOR) {
        m_colorBuffer = new Qt3DRender::QBuffer(this);
        m_instanceColor = new Qt3DRender::QAttribute(this);
        m_instanceColor->setName(QStringLiteral("instanceColor"));
        m_instanceColor->setAttributeType(
            Qt3DRender::QAttribute::VertexAttribute);
        m_instanceColor->setVertexSize(3);
        m_instanceColor->setDivisor(1);
        m_instanceColor->setBuffer(m_colorBuffer);
        m_instanceColor->setVertexBaseType(Qt3DRender::QAttribute::Float);
        m_instanceColor->setByteOffset(0);
        m_instanceColor->setByteStride(3 * sizeof(float));
        m_sphere->addAttribute(m_instanceColor);
    }
}

void ConcretePointsRenderer::setPositions(const QVector<QVector3D>& positions)
{
    QVector<float> modelMatrices;
    modelMatrices.reserve(positions.size() * 16);
    for (const auto& p : positions) {
        auto mat = pointTransformation(p);
        std::copy(
            mat.data(), mat.data() + 16, std::back_inserter(modelMatrices));
    }
    auto bytes = createByteArray(modelMatrices.begin(), modelMatrices.end());
    m_modelBuffer->setData(bytes);
    m_instanceModel->setCount(positions.size());
    this->setInstanceCount(positions.size());
}

void ConcretePointsRenderer::setColors(const QVector<QColor>& colors)
{
    if (m_colorBuffer != nullptr) {
        auto bytes = createByteArray(colors.begin(), colors.end());
        m_colorBuffer->setData(bytes);
        m_instanceColor->setCount(colors.size());
    }
}

} // namespace Klein
