#ifndef KLEIN_FORWARDRENDERINGEFFECT_H
#define KLEIN_FORWARDRENDERINGEFFECT_H

#include <Klein/Core/Export.h>

#include <QSharedPointer>
#include <Qt3DRender/QEffect>

namespace Qt3DRender
{
class QShaderProgram;
}

namespace Klein
{

KLEIN_API QSharedPointer<Qt3DRender::QEffect> createForwardRenderingEffect(
    Qt3DRender::QShaderProgram* shader,
    int glMajorVersion = 3,
    int glMinorVersion = 3);

} // namespace Klein

#endif
