#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Klein/Gui/AbstractQt3DWindow.h>

namespace Klein
{
class ImGuiManager;
}

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
    Klein::ImGuiManager* m_imguiManager;
};

#endif
