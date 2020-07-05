#include <Klein/Render/ConcreteLinesRenderer.h>

#include <Klein/Render/BufferData.h>
#include <Klein/Render/Transformation.h>

#include <algorithm>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>

namespace Klein
{

ConcreteLinesRenderer::ConcreteLinesRenderer(AdditionalAttributes attributes,
                                             Qt3DCore::QNode* parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    qRegisterMetaType<QVector<QVector3D>>("QVector<QVector3D>");
    qRegisterMetaType<QVector<QColor>>("QVector<QColor>");

    m_cylinder = new Qt3DExtras::QCylinderGeometry(this);
    m_cylinder->setRadius(1.0f);
    m_cylinder->setRings(10);
    m_cylinder->setSlices(10);
    m_cylinder->setLength(1.0f);
    this->setGeometry(m_cylinder);

    m_modelBuffer = new Qt3DRender::QBuffer(this);
    m_modelBuffer->setUsage(Qt3DRender::QBuffer::StaticDraw);
    m_modelBuffer->setAccessType(Qt3DRender::QBuffer::Write);
    // FIXME: Empty data causes failure on Intel's Graphics cards
    QByteArray dummy;
    dummy.resize(1);
    m_modelBuffer->setData(dummy);

    m_instanceModel = new Qt3DRender::QAttribute(this);
    m_instanceModel->setName(QStringLiteral("instanceModel"));
    m_instanceModel->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_instanceModel->setVertexSize(16);
    m_instanceModel->setDivisor(1);
    m_instanceModel->setBuffer(m_modelBuffer);
    m_instanceModel->setVertexBaseType(Qt3DRender::QAttribute::Float);
    m_instanceModel->setByteOffset(0);
    m_instanceModel->setByteStride(16 * sizeof(float));
    m_instanceModel->setCount(0);
    m_cylinder->addAttribute(m_instanceModel);

    if (attributes & ADDITIONAL_ATTRIBUTE_COLOR) {
        m_colorBuffer = new Qt3DRender::QBuffer(this);
        m_colorBuffer->setUsage(Qt3DRender::QBuffer::StaticDraw);
        m_colorBuffer->setAccessType(Qt3DRender::QBuffer::Write);
        // FIXME: Empty data causes failure on Intel's Graphics cards
        QByteArray dummy;
        dummy.resize(1);
        m_colorBuffer->setData(dummy);

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
        m_instanceColor->setCount(0);
        m_cylinder->addAttribute(m_instanceColor);
    }

    this->setInstanceCount(0);
} // namespace Klein

void ConcreteLinesRenderer::setPositions(const QVector<QVector3D>& positions,
                                         LineType type)
{
    if (!positions.isEmpty()) {
        QVector<float> modelMatrices;
        int cnt;
        if (type == LINES) {
            cnt = positions.size() / 2;
            modelMatrices.reserve(cnt * 16);
            for (int i = 0; i < positions.size(); i += 2) {
                auto mat = lineTransformation(positions[i], positions[i + 1]);
                std::copy(mat.data(),
                          mat.data() + 16,
                          std::back_inserter(modelMatrices));
            }
        }
        else if (type == LINE_STRIP) {
            cnt = positions.size() - 1;
            modelMatrices.reserve(cnt * 16);
            for (int i = 0; i < positions.size() - 1; ++i) {
                auto mat = lineTransformation(positions[i], positions[i + 1]);
                std::copy(mat.data(),
                          mat.data() + 16,
                          std::back_inserter(modelMatrices));
            }
        }
        else if (type == LINE_LOOP) {
            cnt = positions.size();
            modelMatrices.reserve(cnt * 16);
            for (int i = 0; i < positions.size() - 1; ++i) {
                auto mat = lineTransformation(positions[i], positions[i + 1]);
                std::copy(mat.data(),
                          mat.data() + 16,
                          std::back_inserter(modelMatrices));
            }
            auto mat = lineTransformation(positions.back(), positions.front());
            std::copy(
                mat.data(), mat.data() + 16, std::back_inserter(modelMatrices));
        }
        auto bytes =
            createByteArray(modelMatrices.begin(), modelMatrices.end());
        m_modelBuffer->setData(bytes);
        m_instanceModel->setCount(cnt);
        this->setInstanceCount(cnt);
    }
}

void ConcreteLinesRenderer::setColors(const QVector<QColor>& colors)
{
    if (!colors.isEmpty()) {
        if (m_colorBuffer != nullptr) {
            auto bytes = createByteArray(colors.begin(), colors.end());
            m_colorBuffer->setData(bytes);
            m_instanceColor->setCount(colors.size());
        }
        else {
            qWarning() << "[Warning] ConcreteLinesRenderer::setColors called "
                          "on an object "
                          "without color attribute.";
        }
    }
}

} // namespace Klein
