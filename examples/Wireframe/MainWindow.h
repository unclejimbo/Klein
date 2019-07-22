#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Klein/Gui/AbstractQt3DWindow.h>

namespace Qt3DRender
{
class QCamera;
}

namespace Klein
{
class ImGuiManager;
class PBRSolidWireframeMaterial;
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
    float m_lineWidth = 1.0f;
    float m_lineColor[3] = { 0, 0, 0 };
    Klein::PBRSolidWireframeMaterial* m_material;
    Qt3DRender::QCamera* m_camera = nullptr;
    Klein::TrackballCameraController* m_cameraController = nullptr;
    Klein::ImGuiManager* m_imguiManager = nullptr;
};

#endif
