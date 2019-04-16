#include <Klein/Render/BasePBRMaterial.h>

#include <QString>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QTechniqueFilter>

namespace Klein
{

BasePBRMaterial::BasePBRMaterial(Qt3DCore::QNode* parent)
    : BasePBRMaterial(BASECOLOR_MODE, parent)
{}

BasePBRMaterial::BasePBRMaterial(ColorMode mode, Qt3DCore::QNode* parent)
    : Qt3DRender::QMaterial(parent)
{
    m_baseColor = new Qt3DRender::QParameter(
        QStringLiteral("baseColor"), QColor("white"), this);
    m_colorMode =
        new Qt3DRender::QParameter(QStringLiteral("colorMode"), mode, this);
    m_envLightIntensity = new Qt3DRender::QParameter(
        QStringLiteral("envLight.intensity"), 0.0f, this);
    m_metalness =
        new Qt3DRender::QParameter(QStringLiteral("metalness"), 0.0f, this);
    m_roughness =
        new Qt3DRender::QParameter(QStringLiteral("roughness"), 0.0f, this);
    m_shift = new Qt3DRender::QParameter(QStringLiteral("shift"), 0.0f, this);
    m_texCoordOffset = new Qt3DRender::QParameter(
        QStringLiteral("texCoordOffset"), 0.0f, this);
    m_texCoordScale =
        new Qt3DRender::QParameter(QStringLiteral("texCoordScale"), 1.0f, this);
    this->addParameter(m_baseColor);
    this->addParameter(m_colorMode);
    this->addParameter(m_envLightIntensity);
    this->addParameter(m_metalness);
    this->addParameter(m_roughness);
    this->addParameter(m_shift);
    this->addParameter(m_texCoordOffset);
    this->addParameter(m_texCoordScale);
}

Qt3DRender::QFrameGraphNode* BasePBRMaterial::attachRenderPassTo(
    Qt3DRender::QFrameGraphNode* parent)
{
    // render materials tagged "forward" (default)
    auto tfilter = new Qt3DRender::QTechniqueFilter(parent);
    auto tfilterkey = new Qt3DRender::QFilterKey;
    tfilterkey->setName(QLatin1String("renderingStyle"));
    tfilterkey->setValue(QLatin1String("forward"));
    tfilter->addMatch(tfilterkey);

    auto renderStateSet = new Qt3DRender::QRenderStateSet(tfilter);
    auto depthTest = new Qt3DRender::QDepthTest(renderStateSet);
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Less);
    renderStateSet->addRenderState(depthTest);
    return renderStateSet;
}

Qt3DRender::QEffect* BasePBRMaterial::createEffect(
    Qt3DRender::QShaderProgram* shader)
{
    auto lightingPass = new Qt3DRender::QRenderPass;
    lightingPass->setShaderProgram(shader);

    auto lightingPassFK = new Qt3DRender::QFilterKey;
    lightingPassFK->setName(QLatin1String("renderPass"));
    lightingPassFK->setValue(QLatin1String("lighting"));
    lightingPass->addFilterKey(lightingPassFK);

    auto technique = new Qt3DRender::QTechnique;
    technique->addRenderPass(lightingPass);
    technique->graphicsApiFilter()->setApi(
        Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(3);
    technique->graphicsApiFilter()->setProfile(
        Qt3DRender::QGraphicsApiFilter::CoreProfile);

    auto techniqueFK = new Qt3DRender::QFilterKey;
    techniqueFK->setName(QLatin1String("renderingStyle"));
    techniqueFK->setValue(QLatin1String("forward"));
    technique->addFilterKey(techniqueFK);

    auto effect = new Qt3DRender::QEffect;
    effect->addTechnique(technique);
    return effect;
}

} // namespace Klein
