#include <Klein/Util/Gizmo.h>

namespace Klein
{

Gizmo::Gizmo(QObject* parent) : QObject(parent) {}

Gizmo::~Gizmo() = default;

void Gizmo::update()
{
    if (m_enabled) {
        ImGuizmo::BeginFrame();
        auto& io = ImGui::GetIO();
        auto viewMat = m_camera->viewMatrix();
        auto view = viewMat.constData();
        auto projMat = m_camera->projectionMatrix();
        auto proj = projMat.constData();
        auto tranMat = m_transform->matrix();
        auto tran = tranMat.data();
        QMatrix4x4 idMat;
        idMat.setToIdentity();
        ImGuizmo::SetRect(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(view, proj, m_opration, m_mode, tran);
        if (ImGuizmo::IsUsing()) {
            m_transform->setMatrix(QMatrix4x4(tran).transposed());
        }
    }
}

void Gizmo::setEnabled(bool value)
{
    m_enabled = value;
    ImGuizmo::Enable(m_enabled);
    emit enabledChanged(value);
}

void Gizmo::setOperation(ImGuizmo::OPERATION operation)
{
    m_opration = operation;
}

void Gizmo::setMode(ImGuizmo::MODE mode)
{
    m_mode = mode;
}

void Gizmo::setTarget(Qt3DCore::QTransform* transform)
{
    m_transform = transform;
}

void Gizmo::setCamera(Qt3DRender::QCamera* camera)
{
    m_camera = camera;
    if (m_camera) {
        if (m_camera->projectionType() ==
            Qt3DRender::QCameraLens::OrthographicProjection) {
            ImGuizmo::SetOrthographic(true);
        }
        else {
            ImGuizmo::SetOrthographic(false);
        }
    }
}

} // namespace Klein
