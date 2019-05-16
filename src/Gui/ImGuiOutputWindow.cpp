#include <Klein/Gui/ImGuiOutputWindow.h>

#include <functional>

namespace Klein
{

ImGuiOutputWindow::ImGuiOutputWindow(QObject* parent) : QObject(parent) {}

void ImGuiOutputWindow::update()
{
    if (m_open) {
        ImGui::Begin("Output", &m_open);
        for (int i = 0; i < m_strs.size(); ++i) {
            ImGui::TextColored(m_colors[i], m_strs[i].toStdString().c_str());
        }
        ImGui::SetScrollHereY(1.0f);
        ImGui::End();
    }
}

ImGuiOutputWindow* ImGuiMessageHandler::window = nullptr;

void ImGuiMessageHandler::qMsgHandler(QtMsgType type,
                                      const QMessageLogContext& context,
                                      const QString& msg)
{
    Q_UNUSED(context);
    if (window != nullptr) {
        auto c = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        auto color = QColor::fromRgbF(c.x, c.y, c.z, c.w);
        switch (type) {
        case QtWarningMsg: color = QColor(Qt::yellow); break;
        case QtCriticalMsg:
        case QtFatalMsg: color = QColor(Qt::red); break;
        default: break;
        }
        window->println(msg, color);
    }
}

} // namespace Klein