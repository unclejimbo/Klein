#include <Klein/Render/WBOITMaterial.h>

#include <Klein/Render/DumbTextureImage.h>
#include <Klein/Render/ResourceManager.h>
#include <Klein/Render/ShaderProgram.h>
#include <QString>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QTechniqueFilter>
#include <Qt3DRender/QTextureWrapMode>
#include <Qt3DExtras/QPlaneMesh>

namespace Klein
{

WBOITCompositor::WBOITCompositor(Qt3DCore::QNode* parent)
{
    createTarget(parent);
    createEntity(parent);
}

Qt3DRender::QFrameGraphNode* WBOITCompositor::attachRenderTargetTo(
    Qt3DRender::QFrameGraphNode* parent)
{
    auto targetSel = new Qt3DRender::QRenderTargetSelector(parent);
    targetSel->setTarget(m_renderTarget);

    auto clearBuffer0 = new Qt3DRender::QClearBuffers(targetSel);
    clearBuffer0->setBuffers(Qt3DRender::QClearBuffers::ColorBuffer);
    clearBuffer0->setColorBuffer(m_accumOutput);
    clearBuffer0->setClearColor(QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f));

    auto clearBuffer1 = new Qt3DRender::QClearBuffers(clearBuffer0);
    clearBuffer1->setBuffers(Qt3DRender::QClearBuffers::ColorBuffer);
    clearBuffer1->setColorBuffer(m_revealageOutput);
    clearBuffer1->setClearColor(QColor::fromRgbF(1.0f, 0.0f, 0.0f, 0.0f));

    auto clearBuffer2 = new Qt3DRender::QClearBuffers(clearBuffer1);
    clearBuffer2->setBuffers(Qt3DRender::QClearBuffers::DepthStencilBuffer);
    clearBuffer2->setClearDepthValue(1.0);
    clearBuffer2->setClearStencilValue(0.0);
    new Qt3DRender::QNoDraw(clearBuffer2);

    return targetSel;
}

Qt3DRender::QFrameGraphNode* WBOITCompositor::attachCompositionPassTo(
    Qt3DRender::QFrameGraphNode* parent)
{
    auto tfilter = new Qt3DRender::QTechniqueFilter(parent);
    auto renderingStyleFK = new Qt3DRender::QFilterKey;
    renderingStyleFK->setName(QStringLiteral("renderingStyle"));
    renderingStyleFK->setValue(QStringLiteral("forward"));
    tfilter->addMatch(renderingStyleFK);
    auto renderingTechnique = new Qt3DRender::QFilterKey;
    renderingTechnique->setName(QStringLiteral("renderingTechnique"));
    renderingTechnique->setValue(QStringLiteral("wboit"));
    tfilter->addMatch(renderingTechnique);

    auto pfilter = new Qt3DRender::QRenderPassFilter(tfilter);
    auto passFK = new Qt3DRender::QFilterKey;
    passFK->setName(QStringLiteral("renderPass"));
    passFK->setValue(QStringLiteral("composition"));
    pfilter->addMatch(passFK);
    auto amap = new Qt3DRender::QParameter(pfilter);
    amap->setName(QStringLiteral("accumMap"));
    amap->setValue(QVariant::fromValue(m_accumTexture));
    pfilter->addParameter(amap);
    auto rmap = new Qt3DRender::QParameter(pfilter);
    rmap->setName(QStringLiteral("revealageMap"));
    rmap->setValue(QVariant::fromValue(m_revealageTexture));
    pfilter->addParameter(rmap);

    auto renderStateSet = new Qt3DRender::QRenderStateSet(pfilter);
    auto depthTest = new Qt3DRender::QDepthTest(renderStateSet);
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    renderStateSet->addRenderState(depthTest);
    auto blendEqn = new Qt3DRender::QBlendEquation(renderStateSet);
    blendEqn->setBlendFunction(Qt3DRender::QBlendEquation::Add);
    renderStateSet->addRenderState(blendEqn);
    auto blendEqnArgs = new Qt3DRender::QBlendEquationArguments(renderStateSet);
    blendEqnArgs->setSourceRgba(
        Qt3DRender::QBlendEquationArguments::SourceAlpha);
    blendEqnArgs->setDestinationRgba(
        Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    renderStateSet->addRenderState(blendEqnArgs);

    return renderStateSet;
}

void WBOITCompositor::createTarget(Qt3DCore::QNode* parent)
{
    Qt3DRender::QTextureWrapMode wrapMode;
    wrapMode.setX(Qt3DRender::QTextureWrapMode::ClampToEdge);
    wrapMode.setY(Qt3DRender::QTextureWrapMode::ClampToEdge);

    m_renderTarget = new Qt3DRender::QRenderTarget(parent);

    m_accumOutput = new Qt3DRender::QRenderTargetOutput(m_renderTarget);
    m_accumOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    m_renderTarget->addOutput(m_accumOutput);

    m_revealageOutput = new Qt3DRender::QRenderTargetOutput(m_renderTarget);
    m_revealageOutput->setAttachmentPoint(
        Qt3DRender::QRenderTargetOutput::Color1);
    m_renderTarget->addOutput(m_revealageOutput);

    m_depthOutput = new Qt3DRender::QRenderTargetOutput(m_renderTarget);
    m_depthOutput->setAttachmentPoint(
        Qt3DRender::QRenderTargetOutput::DepthStencil);
    m_renderTarget->addOutput(m_depthOutput);

    m_accumTexture = new Qt3DRender::QTexture2D(m_accumOutput);
    m_accumTexture->setFormat(Qt3DRender::QAbstractTexture::RGBA32F);
    m_accumTexture->setWrapMode(wrapMode);
    m_accumTexture->setGenerateMipMaps(false);
    m_accumTexture->setMinificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);
    m_accumTexture->setMagnificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);
    m_accumOutput->setTexture(m_accumTexture);

    m_revealageTexture = new Qt3DRender::QTexture2D(m_revealageOutput);
    m_revealageTexture->setFormat(Qt3DRender::QAbstractTexture::R8_UNorm);
    m_revealageTexture->setWrapMode(wrapMode);
    m_revealageTexture->setGenerateMipMaps(false);
    m_revealageTexture->setMinificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);
    m_revealageTexture->setMagnificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);
    m_revealageOutput->setTexture(m_revealageTexture);

    m_depthTexture = new Qt3DRender::QTexture2D(m_depthOutput);
    m_depthTexture->setFormat(Qt3DRender::QAbstractTexture::D24S8);
    m_depthTexture->setWrapMode(wrapMode);
    m_depthTexture->setGenerateMipMaps(false);
    m_depthTexture->setMinificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);
    m_depthTexture->setMagnificationFilter(
        Qt3DRender::QAbstractTexture::Nearest);
    m_depthOutput->setTexture(m_depthTexture);
}

void WBOITCompositor::createEntity(Qt3DCore::QNode* parent)
{
    m_entity = new Qt3DCore::QEntity(parent);


    auto mesh = new Qt3DExtras::QPlaneMesh(m_entity);
    mesh->setWidth(2.0f);
    mesh->setHeight(2.0f);
    mesh->setMeshResolution(QSize(2, 2));
    m_entity->addComponent(mesh);

    auto transform = new Qt3DCore::QTransform(m_entity);
    transform->setRotationX(90.0f);
    m_entity->addComponent(transform);

    auto material = new Qt3DRender::QMaterial(m_entity);
    m_entity->addComponent(material);

    auto effect = new Qt3DRender::QEffect(material);
    material->setEffect(effect);

    auto technique = new Qt3DRender::QTechnique(effect);
    technique->graphicsApiFilter()->setApi(
        Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(3);
    technique->graphicsApiFilter()->setProfile(
        Qt3DRender::QGraphicsApiFilter::CoreProfile);
    effect->addTechnique(technique);

    // usually, only one compositor is used in a scene so we choose not to share
    // the effect for it
    const QString shaderPath("data/shader/");
    const auto shader =
        createShader(shaderPath + QStringLiteral("NoProj.vert"),
                     shaderPath + QStringLiteral("WBOITComposition.frag"));

    auto compositionPass = new Qt3DRender::QRenderPass(technique);
    compositionPass->setShaderProgram(shader);
    auto compositionPassFK = new Qt3DRender::QFilterKey(compositionPass);
    compositionPassFK->setName(QStringLiteral("renderPass"));
    compositionPassFK->setValue(QStringLiteral("composition"));
    compositionPass->addFilterKey(compositionPassFK);
    technique->addRenderPass(compositionPass);

    auto renderingStyle = new Qt3DRender::QFilterKey(technique);
    renderingStyle->setName(QStringLiteral("renderingStyle"));
    renderingStyle->setValue(QStringLiteral("forward"));
    technique->addFilterKey(renderingStyle);

    auto renderingTechnique = new Qt3DRender::QFilterKey(technique);
    renderingTechnique->setName(QStringLiteral("renderingTechnique"));
    renderingTechnique->setValue(QStringLiteral("wboit"));
    technique->addFilterKey(renderingTechnique);
}

WBOITMaterial::WBOITMaterial(Qt3DCore::QNode* parent)
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
    m_depthScale =
        new Qt3DRender::QParameter(QStringLiteral("depthScale"), 200.0f, this);
    m_renderMode = new Qt3DRender::QParameter(
        QStringLiteral("renderMode"), RENDER_MODE_BASE_COLOR, this);
    m_texCoordOffset = new Qt3DRender::QParameter(
        QStringLiteral("texCoordOffset"), 0.0f, this);
    m_texCoordScale =
        new Qt3DRender::QParameter(QStringLiteral("texCoordScale"), 1.0f, this);
    this->addParameter(m_baseColor);
    this->addParameter(m_baseColorMap);
    this->addParameter(m_depthScale);
    this->addParameter(m_renderMode);
    this->addParameter(m_texCoordOffset);
    this->addParameter(m_texCoordScale);
    auto effect = gResourceManager().get<Qt3DRender::QEffect>(effectName);
    if (effect == nullptr) {
        effect = createEffect();
        gResourceManager().put(effectName, effect);
    }
    this->setEffect(effect);
}

Qt3DRender::QEffect* WBOITMaterial::createEffect()
{
    auto effect = new Qt3DRender::QEffect;

    auto technique = new Qt3DRender::QTechnique(effect);
    effect->addTechnique(technique);
    technique->graphicsApiFilter()->setApi(
        Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(3);
    technique->graphicsApiFilter()->setProfile(
        Qt3DRender::QGraphicsApiFilter::CoreProfile);

    const QString shaderPath("data/shader/");
    const auto shader =
        createShader(shaderPath + QStringLiteral("Shading.vert"),
                     shaderPath + QStringLiteral("WBOITTransparent.frag"));

    auto transparentPass = new Qt3DRender::QRenderPass(technique);
    technique->addRenderPass(transparentPass);
    transparentPass->setShaderProgram(shader);
    auto transparentPassFK = new Qt3DRender::QFilterKey(transparentPass);
    transparentPassFK->setName(QStringLiteral("renderPass"));
    transparentPassFK->setValue(QStringLiteral("transparent"));
    transparentPass->addFilterKey(transparentPassFK);

    auto renderingStyle = new Qt3DRender::QFilterKey(technique);
    renderingStyle->setName(QStringLiteral("renderingStyle"));
    renderingStyle->setValue(QStringLiteral("forward"));
    technique->addFilterKey(renderingStyle);

    auto renderingTechnique = new Qt3DRender::QFilterKey(technique);
    renderingTechnique->setName(QStringLiteral("renderingTechnique"));
    renderingTechnique->setValue(QStringLiteral("wboit"));
    technique->addFilterKey(renderingTechnique);

    return effect;
}

Qt3DRender::QFrameGraphNode* WBOITMaterial::attachTranparentPassTo(
    Qt3DRender::QFrameGraphNode* parent)
{
    auto tfilter = new Qt3DRender::QTechniqueFilter(parent);
    auto renderingStyleFK = new Qt3DRender::QFilterKey(tfilter);
    renderingStyleFK->setName(QStringLiteral("renderingStyle"));
    renderingStyleFK->setValue(QStringLiteral("forward"));
    tfilter->addMatch(renderingStyleFK);
    auto renderingTechnique = new Qt3DRender::QFilterKey(tfilter);
    renderingTechnique->setName(QStringLiteral("renderingTechnique"));
    renderingTechnique->setValue(QStringLiteral("wboit"));
    tfilter->addMatch(renderingTechnique);

    auto pfilter = new Qt3DRender::QRenderPassFilter(tfilter);
    auto passFK = new Qt3DRender::QFilterKey(pfilter);
    passFK->setName(QStringLiteral("renderPass"));
    passFK->setValue(QStringLiteral("transparent"));
    pfilter->addMatch(passFK);

    auto renderStateSet = new Qt3DRender::QRenderStateSet(pfilter);
    auto depthTest = new Qt3DRender::QDepthTest(renderStateSet);
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    renderStateSet->addRenderState(depthTest);
    auto noDepthMask = new Qt3DRender::QNoDepthMask(renderStateSet);
    renderStateSet->addRenderState(noDepthMask);
    auto blendEqn = new Qt3DRender::QBlendEquation(renderStateSet);
    blendEqn->setBlendFunction(Qt3DRender::QBlendEquation::Add);
    renderStateSet->addRenderState(blendEqn);
    auto blendEqnArgs0 =
        new Qt3DRender::QBlendEquationArguments(renderStateSet);
    blendEqnArgs0->setSourceRgba(Qt3DRender::QBlendEquationArguments::One);
    blendEqnArgs0->setDestinationRgba(Qt3DRender::QBlendEquationArguments::One);
    blendEqnArgs0->setBufferIndex(Qt3DRender::QRenderTargetOutput::Color0);
    renderStateSet->addRenderState(blendEqnArgs0);
    auto blendEqnArgs1 =
        new Qt3DRender::QBlendEquationArguments(renderStateSet);
    blendEqnArgs1->setSourceRgba(Qt3DRender::QBlendEquationArguments::Zero);
    blendEqnArgs1->setDestinationRgba(
        Qt3DRender::QBlendEquationArguments::OneMinusSourceColor);
    blendEqnArgs1->setBufferIndex(Qt3DRender::QRenderTargetOutput::Color1);
    renderStateSet->addRenderState(blendEqnArgs1);

    return renderStateSet;
}

} // namespace Klein
