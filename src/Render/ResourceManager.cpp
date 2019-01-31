#include <Klein/Render/ResourceManager.h>

#include <QString>
#include <QUrl>
#include <Qt3DRender/QShaderProgram>

namespace Klein
{

inline Qt3DRender::QShaderProgram* createShader(const QString& fvert,
                                                const QString& ffrag)

{
    auto shader = new Qt3DRender::QShaderProgram;
    shader->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(fvert)));
    shader->setFragmentShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(ffrag)));
    return shader;
}

inline Qt3DRender::QShaderProgram* createShader(const QString& fvert,
                                                const QString& fgeom,
                                                const QString& ffrag)
{
    auto shader = new Qt3DRender::QShaderProgram;
    shader->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(fvert)));
    shader->setGeometryShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(fgeom)));
    shader->setFragmentShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(ffrag)));
    return shader;
}

Qt3DRender::QShaderProgram* ResourceManager::get(BuiltinShader bs)
{
    QString shaderPath("data/shader/");
    if (!m_builtinShaders.contains(bs)) {
        switch (bs) {
        case KLEIN_SHADER_PBR:
            m_builtinShaders.insert(
                bs,
                createShader(shaderPath + "pbr.vert", shaderPath + "pbr.frag"));
            break;
        default: break;
        }
    }
    return m_builtinShaders[bs];
}

} // namespace Klein
