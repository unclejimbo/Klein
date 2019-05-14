#ifndef KLEIN_IMGUIOUTPUTWINDOW_H
#define KLEIN_IMGUIOUTPUTWINDOW_H

#include <Klein/Core/Export.h>

#include <imgui.h>
#include <QColor>
#include <QObject>
#include <QVector>

namespace Klein
{

class KLEIN_API ImGuiOutputWindow : public QObject
{
    Q_OBJECT

public:
    explicit ImGuiOutputWindow(QObject* parent = nullptr);

    void update();

    bool isEnabled() const { return m_open; }

public slots:
    void setEnabled(bool value) { m_open = value; }

    void println(QString str)
    {
        str.append("\n");
        m_strs.push_back(str);
        m_colors.push_back(ImGui::GetStyleColorVec4(ImGuiCol_Text));
    }

    void println(QString str, QColor color)
    {
        str.append("\n");
        m_strs.push_back(str);
        m_colors.push_back(ImVec4(
            color.redF(), color.greenF(), color.blueF(), color.alphaF()));
    }

private:
    bool m_open = true;
    QVector<QString> m_strs;
    QVector<ImVec4> m_colors;
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
