#include <Klein/Gui/ImGuiSceneGraphWindow.h>

#include <Qt3DCore/QEntity>

namespace Klein
{

ImGuiSceneGraphWindow::ImGuiSceneGraphWindow(QObject* parent) : QObject(parent)
{}

void ImGuiSceneGraphWindow::update()
{
    if (m_enabled) {
        if (!m_embedded) { ImGui::Begin("Scene Graph", &m_enabled); }

        ImGui::Checkbox("Entity only", &m_entityOnly);

        if (ImGui::TreeNodeEx("Scene Root", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto pos = ImGui::GetWindowContentRegionWidth() - 10.0f;
            const auto& nodes = m_rootNode->childNodes();
            for (const auto& child : nodes) {
                printNode(child, pos);
            }
            ImGui::TreePop();
        }

        if (!m_embedded) { ImGui::End(); }

        if (!m_enabled) { emit enabledChanged(m_enabled); }
    }
}

void ImGuiSceneGraphWindow::printNode(Qt3DCore::QNode* node, float pos)
{
    Qt3DCore::QEntity entity;
    auto meta = node->metaObject();
    if (m_entityOnly && !meta->inherits(entity.metaObject())) { return; }

    auto label = QString("%1#%2").arg(meta->className(),
                                      QString::number(node->id().id()));
    auto name = node->objectName();
    if (!name.isEmpty()) { label.append(QString(" (%1)").arg(name)); }

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (m_selectedId == node->id().id()) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    auto isOpen = ImGui::TreeNodeEx(label.toStdString().c_str(), nodeFlags);

    if (ImGui::IsItemClicked()) {
        if (m_selectedId == node->id().id()) {
            m_selectedId = 0;
            emit selectedNodeChanged(nullptr);
        }
        else {
            m_selectedId = node->id().id();
            emit selectedNodeChanged(node);
        }
    }

    ImGui::SameLine(pos);
    bool visible = node->isEnabled();
    if (ImGui::Checkbox(QString("cb").append(label).toStdString().c_str(),
                        &visible)) {
        node->setEnabled(visible);
    }

    if (isOpen) {
        const auto& nodes = node->childNodes();
        for (const auto& child : nodes) {
            printNode(child, pos);
        }
        ImGui::TreePop();
    }
}

} // namespace Klein
