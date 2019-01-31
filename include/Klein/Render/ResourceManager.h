#ifndef KLEIN_RESOURCEMANAGER_H
#define KLEIN_RESOURCEMANAGER_H

#include <Klein/Core/Export.h>

#include <QHash>

namespace Qt3DRender
{
class QShaderProgram;
}

namespace Klein
{

enum BuiltinShader : int
{
    KLEIN_SHADER_PBR
};

class KLEIN_API ResourceManager
{
public:
    static ResourceManager& instance()
    {
        static ResourceManager m;
        return m;
    }

public:
    ResourceManager(const ResourceManager&) = delete;

    void operator=(const ResourceManager&) = delete;

    Qt3DRender::QShaderProgram* get(BuiltinShader bs);

private:
    ResourceManager() {}

private:
    QHash<BuiltinShader, Qt3DRender::QShaderProgram*> m_builtinShaders;
};

inline KLEIN_API ResourceManager& gResourceManager()
{
    return ResourceManager::instance();
}

} // namespace Klein

#endif
