#ifndef KLEIN_IMGUISCENEGRAPHWINDOW_H
#define KLEIN_IMGUISCENEGRAPHWINDOW_H

#include <Klein/Core/Export.h>

#include <imgui.h>
#include <QObject>
#include <Qt3DCore/QNode>

namespace Klein
{

class KLEIN_API ImGuiSceneGraphWindow : public QObject
{
    Q_OBJECT

public:
    explicit ImGuiSceneGraphWindow(QObject* parent = nullptr);

    virtual ~ImGuiSceneGraphWindow() = default;

    void update();

    bool isEnabled() const { return m_enabled; }

    bool isEmbedded() const { return m_embedded; }

    Qt3DCore::QNode* rootNode() { return m_rootNode; }

public slots:
    void setEnabled(bool value) { m_enabled = value; }

    void setEmbedded(bool value) { m_embedded = value; }

    void setRootNode(Qt3DCore::QNode* rootNode) { m_rootNode = rootNode; }

private:
    void printNode(Qt3DCore::QNode* node, float pos);

private:
    bool m_enabled = true;
    bool m_embedded = false;
    bool m_entityOnly = true;
    Qt3DCore::QNode* m_rootNode = nullptr;
};

} // namespace Klein

#endif
