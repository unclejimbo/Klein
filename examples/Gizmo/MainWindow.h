#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Klein/Gui/AbstractQt3DWindow.h>

namespace Qt3DCore
{
class QTransform;
} // namespace Qt3DCore

namespace Qt3DRender
{
class QCamera;
} // namespace Qt3DRender

namespace Klein
{
class Gizmo;
class ImGuiManager;
class TrackballCameraController;
} // namespace Klein

class MainWindow : public Klein::AbstractQt3DWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWindow* parent = nullptr);

    virtual ~MainWindow() = default;

protected:
    void resizeEvent(QResizeEvent*) override;

    Qt3DCore::QEntity* createSceneGraph() override;

    Qt3DRender::QRenderSettings* createRenderSettings(
        Qt3DCore::QEntity* root) override;

private:
    void updateGui();

private:
    Klein::ImGuiManager* m_imguiManager;
    Klein::Gizmo* m_gizmo;
    Qt3DRender::QCamera* m_camera = nullptr;
    Klein::TrackballCameraController* m_cameraController = nullptr;
    Qt3DCore::QTransform* m_cubeTransform1 = nullptr;
    Qt3DCore::QTransform* m_cubeTransform2 = nullptr;
    int m_active = 0;
    int m_operation = 0;
    int m_mode = 0;
};

#endif
