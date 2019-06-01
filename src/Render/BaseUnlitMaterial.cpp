#include <Klein/Render/BaseUnlitMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <QString>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QTechniqueFilter>

namespace Klein
{

BaseUnlitMaterial::BaseUnlitMaterial(Qt3DCore::QNode* parent)
    : BaseUnlitMaterial(BASECOLOR_MODE, parent)
{}

BaseUnlitMaterial::BaseUnlitMaterial(ColorMode mode, Qt3DCore::QNode* parent)
    : Qt3DRender::QMaterial(parent)
{
    m_baseColor = new Qt3DRender::QParameter(
        QStringLiteral("baseColor"), QColor("white"), this);
    m_colorMode =
        new Qt3DRender::QParameter(QStringLiteral("colorMode"), mode, this);
    m_shift = new Qt3DRender::QParameter(QStringLiteral("shift"), 0.0f, this);
    m_texCoordOffset = new Qt3DRender::QParameter(
        QStringLiteral("texCoordOffset"), 0.0f, this);
    m_texCoordScale =
        new Qt3DRender::QParameter(QStringLiteral("texCoordScale"), 1.0f, this);
    this->addParameter(m_baseColor);
    this->addParameter(m_colorMode);
    this->addParameter(m_shift);
    this->addParameter(m_texCoordOffset);
    this->addParameter(m_texCoordScale);
}

Qt3DRender::QFrameGraphNode* BaseUnlitMaterial::attachRenderPassTo(
    Qt3DRender::QFrameGraphNode* parent,
    Qt3DRender::QAbstractTexture* shadowMap,
    const QMatrix4x4& lightSpaceMatrix,
    const QVector3D& lightDir,
    float shadowFilterScale)
{
    auto tfilter = new Qt3DRender::QTechniqueFilter(parent);
    auto renderingStyle = new Qt3DRender::QFilterKey;
    renderingStyle->setName(QStringLiteral("renderingStyle"));
    renderingStyle->setValue(QStringLiteral("forward"));
    tfilter->addMatch(renderingStyle);
    auto transparency = new Qt3DRender::QFilterKey;
    transparency->setName(QStringLiteral("transparency"));
    transparency->setValue(QStringLiteral("opaque"));
    tfilter->addMatch(transparency);

    auto pfilter = new Qt3DRender::QRenderPassFilter(tfilter);
    auto pfilterkey = new Qt3DRender::QFilterKey;
    pfilterkey->setName(QStringLiteral("renderPass"));
    pfilterkey->setValue(QStringLiteral("lighting"));
    pfilter->addMatch(pfilterkey);
    auto paramShadowMap = new Qt3DRender::QParameter(pfilter);
    paramShadowMap->setName(QStringLiteral("shadowMap"));
    paramShadowMap->setValue(QVariant::fromValue(shadowMap));
    pfilter->addParameter(paramShadowMap);
    auto paramLightSpaceMatrix = new Qt3DRender::QParameter(pfilter);
    paramLightSpaceMatrix->setName(QStringLiteral("lightSpaceMatrix"));
    paramLightSpaceMatrix->setValue(QVariant::fromValue(lightSpaceMatrix));
    pfilter->addParameter(paramLightSpaceMatrix);
    auto paramLightDir = new Qt3DRender::QParameter(pfilter);
    paramLightDir->setName(QStringLiteral("lightDir"));
    paramLightDir->setValue(QVariant::fromValue(lightDir));
    pfilter->addParameter(paramLightDir);
    auto paramShadowFilterScale = new Qt3DRender::QParameter(pfilter);
    paramShadowFilterScale->setName(QStringLiteral("shadowFilterScale"));
    paramShadowFilterScale->setValue(shadowFilterScale);
    pfilter->addParameter(paramShadowFilterScale);

    auto renderStateSet = new Qt3DRender::QRenderStateSet(pfilter);
    auto depthTest = new Qt3DRender::QDepthTest(renderStateSet);
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    renderStateSet->addRenderState(depthTest);
    return renderStateSet;
}

Qt3DRender::QFrameGraphNode* BaseUnlitMaterial::attachShadowPassTo(
    Qt3DRender::QFrameGraphNode* parent)
{
    auto tfilter = new Qt3DRender::QTechniqueFilter(parent);
    auto renderingStyle = new Qt3DRender::QFilterKey;
    renderingStyle->setName(QStringLiteral("renderingStyle"));
    renderingStyle->setValue(QStringLiteral("forward"));
    tfilter->addMatch(renderingStyle);
    auto transparency = new Qt3DRender::QFilterKey;
    transparency->setName(QStringLiteral("transparency"));
    transparency->setValue(QStringLiteral("opaque"));
    tfilter->addMatch(transparency);

    auto pfilter = new Qt3DRender::QRenderPassFilter(tfilter);
    auto pfilterkey = new Qt3DRender::QFilterKey;
    pfilterkey->setName(QStringLiteral("renderPass"));
    pfilterkey->setValue(QStringLiteral("shadow"));
    pfilter->addMatch(pfilterkey);

    auto renderStateSet = new Qt3DRender::QRenderStateSet(pfilter);
    auto depthTest = new Qt3DRender::QDepthTest(renderStateSet);
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    renderStateSet->addRenderState(depthTest);
    return renderStateSet;
}

Qt3DRender::QEffect* BaseUnlitMaterial::createEffect(
    Qt3DRender::QShaderProgram* shader)
{
    auto lightingPass = new Qt3DRender::QRenderPass;
    lightingPass->setShaderProgram(shader);

    auto lightingPassFK = new Qt3DRender::QFilterKey;
    lightingPassFK->setName(QStringLiteral("renderPass"));
    lightingPassFK->setValue(QStringLiteral("lighting"));
    lightingPass->addFilterKey(lightingPassFK);

    auto technique = new Qt3DRender::QTechnique;
    technique->addRenderPass(lightingPass);
    technique->graphicsApiFilter()->setApi(
        Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(3);
    technique->graphicsApiFilter()->setProfile(
        Qt3DRender::QGraphicsApiFilter::CoreProfile);

    auto renderingStyle = new Qt3DRender::QFilterKey;
    renderingStyle->setName(QStringLiteral("renderingStyle"));
    renderingStyle->setValue(QStringLiteral("forward"));
    technique->addFilterKey(renderingStyle);
    auto transparency = new Qt3DRender::QFilterKey;
    transparency->setName(QStringLiteral("transparency"));
    transparency->setValue(QStringLiteral("opaque"));
    technique->addFilterKey(transparency);

    auto effect = new Qt3DRender::QEffect;
    effect->addTechnique(technique);
    return effect;
}

} // namespace Klein
