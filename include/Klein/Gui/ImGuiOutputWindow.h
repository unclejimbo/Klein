#ifndef KLEIN_IMGUIOUTPUTWINDOW_H
#define KLEIN_IMGUIOUTPUTWINDOW_H

#include <Klein/Core/Export.h>

#include <imgui.h>
#include <QColor>
#include <QList>
#include <QObject>

namespace Klein
{

class KLEIN_API ImGuiOutputWindow : public QObject
{
    Q_OBJECT

public:
    explicit ImGuiOutputWindow(QObject* parent = nullptr);

    virtual ~ImGuiOutputWindow() = default;

    void update();

    bool isEnabled() const { return m_open; }

    int maximumHistory() const { return m_maxHistory; }

public slots:
    void setEnabled(bool value) { m_open = m_openLast = value; }

    void println(QString str)
    {
        str.append("\n");
        m_strs.push_back(str);
        m_colors.push_back(ImGui::GetStyleColorVec4(ImGuiCol_Text));
        if (m_strs.size() > m_maxHistory) {
            m_strs.pop_front();
            m_colors.pop_front();
        }
        m_dirty = true;
    }

    void println(QString str, QColor color)
    {
        str.append("\n");
        m_strs.push_back(str);
        m_colors.push_back(ImVec4(
            color.redF(), color.greenF(), color.blueF(), color.alphaF()));
        if (m_strs.size() > m_maxHistory) {
            m_strs.pop_front();
            m_colors.pop_front();
        }
        m_dirty = true;
    }

    void clear()
    {
        m_strs.clear();
        m_colors.clear();
    }

    void setMaximumHistory(int num) { m_maxHistory = num; }

signals:
    void enabledChanged(bool value);

private:
    bool m_open = true;
    bool m_openLast = true;
    bool m_autoScroll = true;
    bool m_dirty = false;
    int m_maxHistory = 1e5;
    QList<QString> m_strs;
    QList<ImVec4> m_colors;
};

class KLEIN_API ImGuiMessageHandler
{
public:
    static void qMsgHandler(QtMsgType type,
                            const QMessageLogContext& context,
                            const QString& msg);

    static ImGuiOutputWindow* window;
};

} // namespace Klein

#endif
