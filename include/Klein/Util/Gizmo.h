#ifndef KLEIN_GIZMO_H
#define KLEIN_GIZMO_H

#include <Klein/Core/Export.h>

#include <imgui.h>
#include <ImGuizmo.h>
#include <QObject>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>

namespace Klein
{

class KLEIN_API Gizmo : public QObject
{
    Q_OBJECT

public:
    explicit Gizmo(QObject* parent = nullptr);

    void update();

    bool isEnabled() const noexcept { return m_enabled; }

public slots:
    void setEnabled(bool value);

    void setOperation(ImGuizmo::OPERATION operation);

    void setMode(ImGuizmo::MODE mode);

    void setTarget(Qt3DCore::QTransform* transform);

    void setCamera(Qt3DRender::QCamera* camera);

signals:
    void enabledChanged(bool value);

private:
    bool m_enabled = true;
    ImGuizmo::OPERATION m_opration = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE m_mode = ImGuizmo::WORLD;
    Qt3DCore::QTransform* m_transform = nullptr;
    Qt3DRender::QCamera* m_camera = nullptr;
};

} // namespace Klein

#endif
