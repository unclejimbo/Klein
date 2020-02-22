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

    bool isEntityOnly() const { return m_entityOnly; }

    Qt3DCore::QNode* rootNode() { return m_rootNode; }

public slots:
    void setEnabled(bool value)
    {
        if (m_enabled != value) {
            m_enabled = value;
            emit enabledChanged(value);
        }
    }

    void setEmbedded(bool value)
    {
        if (m_embedded != value) {
            m_embedded = value;
            emit embeddedChanged(value);
        }
    }

    void setEntityOnly(bool value)
    {
        if (m_entityOnly != value) {
            m_entityOnly = value;
            emit entityOnlyChanged(value);
        }
    }

    void setRootNode(Qt3DCore::QNode* rootNode)
    {
        if (m_rootNode != rootNode) {
            m_rootNode = rootNode;
            emit rootNodeChanged(rootNode);
        }
    }

signals:
    void enabledChanged(bool value);

    void embeddedChanged(bool value);

    void entityOnlyChanged(bool value);

    void rootNodeChanged(Qt3DCore::QNode* node);

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
