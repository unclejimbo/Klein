#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Klein/Gui/AbstractQt3DWindow.h>

namespace Qt3DCore
{
class QEntity;
}

namespace Qt3DRender
{
class QCamera;
}

namespace Klein
{
class ImGuiManager;
class TrackballCameraController;
} // namespace Klein

class MainWindow : public Klein::AbstractQt3DWindow
{
    Q_OBJECT

public:
    MainWindow(QWindow* parent = nullptr);

    virtual ~MainWindow() = default;

protected:
    void resizeEvent(QResizeEvent*) override;

    Qt3DCore::QEntity* createSceneGraph() override;

    Qt3DRender::QRenderSettings* createRenderSettings(
        Qt3DCore::QEntity* root) override;

private:
    void _renderGui();

private:
    Qt3DCore::QEntity* m_cerberus = nullptr;
    Qt3DCore::QEntity* m_spheres = nullptr;
    Qt3DRender::QCamera* m_camera = nullptr;
    Klein::TrackballCameraController* m_cameraController = nullptr;
    Klein::ImGuiManager* m_imguiManager = nullptr;
    int m_activeScene = 0;
};

#endif
