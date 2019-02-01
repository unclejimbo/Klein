#ifndef KLEIN_FORWARDRENDERINGEFFECT_H
#define KLEIN_FORWARDRENDERINGEFFECT_H

#include <Klein/Core/Export.h>

namespace Qt3DCore
{
class QNode;
}

namespace Qt3DRender
{
class QEffect;
class QShaderProgram;
} // namespace Qt3DRender

namespace Klein
{

KLEIN_API Qt3DRender::QEffect* createForwardRenderingEffect(
    Qt3DRender::QShaderProgram* shader,
    int glMajorVersion = 3,
    int glMinorVersion = 3,
    Qt3DCore::QNode* parent = nullptr);

} // namespace Klein

#endif
