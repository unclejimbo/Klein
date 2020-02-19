#ifndef KLEIN_SHADERPROGRAM_H
#define KLEIN_SHADERPROGRAM_H

#include <Klein/Core/Export.h>

#include <QUrl>
#include <Qt3DRender/QShaderProgram>

namespace Klein
{

inline KLEIN_API Qt3DRender::QShaderProgram* createShader(const QString& fvert,
                                                          const QString& ffrag)

{
    auto shader = new Qt3DRender::QShaderProgram();
    shader->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(fvert)));
    shader->setFragmentShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(ffrag)));
    return shader;
}

inline KLEIN_API Qt3DRender::QShaderProgram* createShader(const QString& fvert,
                                                          const QString& fgeom,
                                                          const QString& ffrag)
{
    auto shader = new Qt3DRender::QShaderProgram();
    shader->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(fvert)));
    shader->setGeometryShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(fgeom)));
    shader->setFragmentShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(ffrag)));
    return shader;
}

} // namespace Klein

#endif
