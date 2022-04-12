#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Klein/Gui/AbstractQt3DWindow.h>

namespace Qt3DCore
{
class QEntity;
}

namespace Qt3DRender
{
class QAbstractLight;
class QCamera;
class QLayer;
} // namespace Qt3DRender

namespace Klein
{
class BasePBRMaterial;
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
    Klein::BasePBRMaterial* m_cerberusMaterial = nullptr;
    QVector<Klein::BasePBRMaterial*> m_sphereMaterials;
    QVector<Qt3DRender::QAbstractLight*> m_directLights;
    Qt3DRender::QLayer* m_skyboxLayer = nullptr;
    Qt3DRender::QCamera* m_camera = nullptr;
    Klein::TrackballCameraController* m_cameraController = nullptr;
    Klein::ImGuiManager* m_imguiManager = nullptr;
    int m_activeScene = 0;
    int m_varyingParam = 0;
    float m_roughness = 0.0f;
    float m_metalness = 0.0f;
    float m_specular = 0.5f;
    float m_clearcoat = 0.0f;
    float m_clearcoatRoughness = 0.0f;
    float m_baseColor[3] = { 1.0f, 0.0f, 0.0f };
    float m_dlIntensity = 1.0f;
    float m_iblIntensity = 1.0f;
};

#endif
