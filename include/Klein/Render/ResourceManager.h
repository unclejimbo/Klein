#ifndef KLEIN_RESOURCEMANAGER_H
#define KLEIN_RESOURCEMANAGER_H

#include <Klein/Core/Export.h>

#include <QHash>
#include <QString>
#include <Qt3DCore/QNode>

namespace Klein
{

class KLEIN_API ResourceManager
{
public:
    static ResourceManager& instance() noexcept 
    {
        static ResourceManager m;
        return m;
    }

public:
    ResourceManager(const ResourceManager&) = delete;

    ResourceManager(ResourceManager&&) = delete;

    ResourceManager& operator=(const ResourceManager&) = delete;

    ResourceManager& operator=(ResourceManager&&) = delete;

    void setRoot(Qt3DCore::QNode* root) noexcept { m_root = root; }

    Qt3DCore::QNode* root() noexcept { return m_root; }

    template<typename T>
    T* get(int key)
    {
        return qobject_cast<T*>(m_idNodeMap.value(key));
    }

    template<typename T>
    T* get(const QString& key)
    {
        if (m_nameIdMap.contains(key)) {
            const int id = m_nameIdMap.value(key);
            return get<T>(id);
        }
        else {
            return nullptr;
        }
    }

    void put(Qt3DCore::QNode* node)
    {
        if (node == nullptr) { return; }
        m_idNodeMap.insert(node->id().id(), node);
        node->setParent(m_root);
    }

    void put(const QString& key, Qt3DCore::QNode* node)
    {
        if (node == nullptr) { return; }
        m_nameIdMap.insert(key, node->id().id());
        put(node);
    }

private:
    ResourceManager() noexcept {}

    ~ResourceManager() {}

private:
    Qt3DCore::QNode* m_root = nullptr;
    QHash<QString, int> m_nameIdMap;          // optional for user
    QHash<int, Qt3DCore::QNode*> m_idNodeMap; // data goes here
};

inline KLEIN_API ResourceManager& gResourceManager() noexcept
{
    return ResourceManager::instance();
}

} // namespace Klein

#endif
