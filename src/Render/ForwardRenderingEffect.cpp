#include <Klein/Render/ForwardRenderingEffect.h>

#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>

namespace Klein
{

QSharedPointer<Qt3DRender::QEffect> createForwardRenderingEffect(
    Qt3DRender::QShaderProgram* shader,
    int glMajorVersion,
    int glMinorVersion)
{
    QSharedPointer<Qt3DRender::QEffect> effect(new Qt3DRender::QEffect);
    auto technique = new Qt3DRender::QTechnique;
    auto pass = new Qt3DRender::QRenderPass;
    pass->setShaderProgram(shader);
    technique->addRenderPass(pass);
    auto filterkey = new Qt3DRender::QFilterKey;
    filterkey->setName(QLatin1String("renderingStyle"));
    filterkey->setValue(QLatin1String("forward"));
    technique->addFilterKey(filterkey);
    technique->graphicsApiFilter()->setApi(
        Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(glMajorVersion);
    technique->graphicsApiFilter()->setMinorVersion(glMinorVersion);
    technique->graphicsApiFilter()->setProfile(
        Qt3DRender::QGraphicsApiFilter::CoreProfile);
    effect->addTechnique(technique);
    return effect;
}

} // namespace Klein
