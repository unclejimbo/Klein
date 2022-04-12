#include <Klein/Render/BasePBRMaterial.h>

#include <Klein/Render/DumbTextureImage.h>
#include <Klein/Render/ResourceManager.h>
#include <Klein/Render/ShaderProgram.h>
#include <QString>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QSeamlessCubemap>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QTechniqueFilter>

namespace Klein
{

BasePBRMaterial::BasePBRMaterial(Qt3DCore::QNode* parent)
    : Qt3DRender::QMaterial(parent)
{
    // need to fill texture maps with dumb data even if they are not used
    auto dumbTexture = gResourceManager().get<Qt3DRender::QAbstractTexture>(
        "KLEIN_DUMB_TEXTURE");
    if (dumbTexture == nullptr) {
        dumbTexture = new Qt3DRender::QTexture2D;
        auto image = new Klein::DumbTextureImage(dumbTexture);
        dumbTexture->addTextureImage(image);
        gResourceManager().put(dumbTexture);
    }

    m_baseColor = new Qt3DRender::QParameter(
        QStringLiteral("baseColor"), QColor("white"), this);
    m_baseColorMap = new Qt3DRender::QParameter(
        QStringLiteral("baseColorMap"), dumbTexture, this);
    m_clearCoat =
        new Qt3DRender::QParameter(QStringLiteral("clearcoat"), 0.0f, this);
    m_clearCoatRoughness = new Qt3DRender::QParameter(
        QStringLiteral("clearcoatRoughness"), 0.0f, this);
    m_envLightBrdf = new Qt3DRender::QParameter(
        QStringLiteral("envLight.brdf"), dumbTexture, this);
    m_envLightIntensity = new Qt3DRender::QParameter(
        QStringLiteral("envLight.intensity"), 0.0f, this);
    m_envLightMipLevels = new Qt3DRender::QParameter(
        QStringLiteral("envLight.mipLevels"), 8, this);
    m_envLightMipOffset = new Qt3DRender::QParameter(
        QStringLiteral("envLight.mipOffset"), 0, this);
    m_metalness =
        new Qt3DRender::QParameter(QStringLiteral("metalness"), 0.0f, this);
    m_metalnessMap = new Qt3DRender::QParameter(
        QStringLiteral("metalnessMap"), dumbTexture, this);
    m_normalMap = new Qt3DRender::QParameter(
        QStringLiteral("normalMap"), dumbTexture, this);
    m_renderMode = new Qt3DRender::QParameter(
        QStringLiteral("renderMode"), RENDER_MODE_BASE_COLOR, this);
    m_receiveShadow = new Qt3DRender::QParameter(
        QStringLiteral("receiveShadow"), false, this);
    m_roughness =
        new Qt3DRender::QParameter(QStringLiteral("roughness"), 0.0f, this);
    m_roughnessMap = new Qt3DRender::QParameter(
        QStringLiteral("roughnessMap"), dumbTexture, this);
    m_shift = new Qt3DRender::QParameter(QStringLiteral("shift"), 0.0f, this);
    m_specular =
        new Qt3DRender::QParameter(QStringLiteral("specular"), 0.5f, this);
    m_texCoordOffset = new Qt3DRender::QParameter(
        QStringLiteral("texCoordOffset"), 0.0f, this);
    m_texCoordScale =
        new Qt3DRender::QParameter(QStringLiteral("texCoordScale"), 1.0f, this);
    this->addParameter(m_baseColor);
    this->addParameter(m_baseColorMap);
    this->addParameter(m_clearCoat);
    this->addParameter(m_clearCoatRoughness);
    this->addParameter(m_envLightBrdf);
    this->addParameter(m_envLightIntensity);
    this->addParameter(m_metalness);
    this->addParameter(m_metalnessMap);
    this->addParameter(m_envLightMipLevels);
    this->addParameter(m_envLightMipOffset);
    this->addParameter(m_normalMap);
    this->addParameter(m_receiveShadow);
    this->addParameter(m_renderMode);
    this->addParameter(m_roughness);
    this->addParameter(m_roughnessMap);
    this->addParameter(m_shift);
    this->addParameter(m_specular);
    this->addParameter(m_texCoordOffset);
    this->addParameter(m_texCoordScale);
}

Qt3DRender::QFrameGraphNode* BasePBRMaterial::attachRenderPassTo(
    Qt3DRender::QFrameGraphNode* parent)
{
    auto tfilter = new Qt3DRender::QTechniqueFilter(parent);
    auto renderingStyle = new Qt3DRender::QFilterKey;
    renderingStyle->setName(QStringLiteral("renderingStyle"));
    renderingStyle->setValue(QStringLiteral("forward"));
    tfilter->addMatch(renderingStyle);
    auto renderingTechnique = new Qt3DRender::QFilterKey;
    renderingTechnique->setName(QStringLiteral("renderingTechnique"));
    renderingTechnique->setValue(QStringLiteral("pbr"));
    tfilter->addMatch(renderingTechnique);

    auto pfilter = new Qt3DRender::QRenderPassFilter(tfilter);
    auto pfilterkey = new Qt3DRender::QFilterKey;
    pfilterkey->setName(QStringLiteral("renderPass"));
    pfilterkey->setValue(QStringLiteral("lighting"));
    pfilter->addMatch(pfilterkey);

    auto dumbTexture = gResourceManager().get<Qt3DRender::QAbstractTexture>(
        "KLEIN_DUMB_TEXTURE");
    auto paramShadowMap = new Qt3DRender::QParameter(pfilter);
    paramShadowMap->setName(QStringLiteral("shadowMap"));
    paramShadowMap->setValue(QVariant::fromValue(dumbTexture));

    auto renderStateSet = new Qt3DRender::QRenderStateSet(pfilter);
    auto depthTest = new Qt3DRender::QDepthTest(renderStateSet);
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    auto seamlessCubeMap = new Qt3DRender::QSeamlessCubemap(renderStateSet);
    renderStateSet->addRenderState(depthTest);
    renderStateSet->addRenderState(seamlessCubeMap);
    return renderStateSet;
}

Qt3DRender::QFrameGraphNode* BasePBRMaterial::attachRenderPassTo(
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
    auto renderingTechnique = new Qt3DRender::QFilterKey;
    renderingTechnique->setName(QStringLiteral("renderingTechnique"));
    renderingTechnique->setValue(QStringLiteral("pbr"));
    tfilter->addMatch(renderingTechnique);

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
    auto seamlessCubeMap = new Qt3DRender::QSeamlessCubemap(renderStateSet);
    renderStateSet->addRenderState(depthTest);
    renderStateSet->addRenderState(seamlessCubeMap);
    return renderStateSet;
}

Qt3DRender::QFrameGraphNode* BasePBRMaterial::attachShadowPassTo(
    Qt3DRender::QFrameGraphNode* parent)
{
    auto tfilter = new Qt3DRender::QTechniqueFilter(parent);
    auto renderingStyle = new Qt3DRender::QFilterKey;
    renderingStyle->setName(QStringLiteral("renderingStyle"));
    renderingStyle->setValue(QStringLiteral("forward"));
    tfilter->addMatch(renderingStyle);
    auto renderingTechnique = new Qt3DRender::QFilterKey;
    renderingTechnique->setName(QStringLiteral("renderingTechnique"));
    renderingTechnique->setValue(QStringLiteral("pbr"));
    tfilter->addMatch(renderingTechnique);

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

Qt3DRender::QEffect* BasePBRMaterial::createEffectImpl(
    Qt3DRender::QShaderProgram* shader,
    bool castShadow)
{
    auto technique = new Qt3DRender::QTechnique;

    if (castShadow) {
        QString shaderPath("data/shader/");
        auto depthShader =
            createShader(shaderPath + QStringLiteral("Depth.vert"),
                         shaderPath + QStringLiteral("Depth.frag"));

        auto shadowPass = new Qt3DRender::QRenderPass;
        shadowPass->setShaderProgram(depthShader);

        auto shadowPassFK = new Qt3DRender::QFilterKey;
        shadowPassFK->setName(QStringLiteral("renderPass"));
        shadowPassFK->setValue(QStringLiteral("shadow"));
        shadowPass->addFilterKey(shadowPassFK);

        technique->addRenderPass(shadowPass);
    }

    auto lightingPass = new Qt3DRender::QRenderPass;
    lightingPass->setShaderProgram(shader);

    auto lightingPassFK = new Qt3DRender::QFilterKey;
    lightingPassFK->setName(QStringLiteral("renderPass"));
    lightingPassFK->setValue(QStringLiteral("lighting"));
    lightingPass->addFilterKey(lightingPassFK);

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
    auto renderingTechnique = new Qt3DRender::QFilterKey;
    renderingTechnique->setName(QStringLiteral("renderingTechnique"));
    renderingTechnique->setValue(QStringLiteral("pbr"));
    technique->addFilterKey(renderingTechnique);

    auto effect = new Qt3DRender::QEffect;
    effect->addTechnique(technique);
    return effect;
}

} // namespace Klein
