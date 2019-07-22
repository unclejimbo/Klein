#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Klein/Gui/AbstractQt3DWindow.h>

namespace Qt3DRender
{
class QCamera;
}

namespace Klein
{
class DirectionalShadowCaster;
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
    Klein::DirectionalShadowCaster* m_dcaster;
    Qt3DRender::QCamera* m_camera = nullptr;
    Klein::TrackballCameraController* m_cameraController = nullptr;
};

#endif
