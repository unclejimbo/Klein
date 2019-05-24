#include <Klein/Render/PBRSolidWireframeMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <QString>

namespace Klein
{

PBRSolidWireframeMaterial::PBRSolidWireframeMaterial(Qt3DCore::QNode* parent)
    : PBRSolidWireframeMaterial(BASECOLOR_MODE, parent)
{}

PBRSolidWireframeMaterial::PBRSolidWireframeMaterial(ColorMode mode,
                                                     Qt3DCore::QNode* parent)
    : BasePBRMaterial(mode, parent)
{
    m_lineColor = new Qt3DRender::QParameter(
        QStringLiteral("lineColor"), QColor("black"), this);
    m_lineWidth =
        new Qt3DRender::QParameter(QStringLiteral("lineWidth"), 1.0f, this);
    this->addParameter(m_lineColor);
    this->addParameter(m_lineWidth);
    auto effect =
        createEffect(gResourceManager().get<Qt3DRender::QShaderProgram>(
            BUILTIN_SHADER_PBR_SOLIDWIREFRAME));
    this->setEffect(effect);
}

} // namespace Klein
