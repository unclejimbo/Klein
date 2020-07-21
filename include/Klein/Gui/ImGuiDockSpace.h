#ifndef KLEIN_IMGUIDOCKSPACE_H
#define KLEIN_IMGUIDOCKSPACE_H

#include <Klein/Core/Export.h>

#include <imgui.h>
#include <QObject>

namespace Klein
{

class KLEIN_API ImGuiDockSpace : public QObject
{
    Q_OBJECT

public:
    explicit ImGuiDockSpace(QObject* parent = nullptr) : QObject(parent) {}

    void update();

    bool isEnabled() const { return m_open; }

public slots:
    void setEnabled(bool value)
    {
        m_open = value;
        emit enabledChanged(value);
    }

signals:
    void enabledChanged(bool value);

private:
    bool m_open = true;
};

} // namespace Klein

#endif
