#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Klein/Gui/AbstractQt3DWindow.h>

namespace Qt3DRender
{
class QCamera;
}

class MainWindow : public Klein::AbstractQt3DWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWindow* parent = nullptr)
        : Klein::AbstractQt3DWindow(parent)
    {}

    virtual ~MainWindow() = default;

protected:
    Qt3DCore::QEntity* createSceneGraph() override;

    Qt3DRender::QRenderSettings* createRenderSettings(
        Qt3DCore::QEntity* root) override;

private:
    Qt3DRender::QCamera* m_camera;
};

#endif
