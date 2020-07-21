#ifndef KLEIN_ABSTRACTQT3DWINDOW_H
#define KLEIN_ABSTRACTQT3DWINDOW_H

#include <Klein/Core/Export.h>

#include <QWindow>

namespace Qt3DCore
{
class QAbstractAspect;
class QAspectEngine;
class QEntity;
} // namespace Qt3DCore

namespace Qt3DRender
{
class QRenderSettings;
}

namespace Qt3DInput
{
class QInputSettings;
}

namespace Klein
{

class KLEIN_API AbstractQt3DWindow : public QWindow
{
    Q_OBJECT

public:
    explicit AbstractQt3DWindow(QWindow* parent = nullptr);

protected:
    void exposeEvent(QExposeEvent*) override;

    virtual Qt3DCore::QEntity* createSceneGraph() = 0;

    virtual Qt3DRender::QRenderSettings* createRenderSettings(
        Qt3DCore::QEntity* root) = 0;

    virtual Qt3DInput::QInputSettings* createInputSettings(
        Qt3DCore::QEntity* root);

protected:
    Qt3DCore::QAspectEngine* m_aspectEngine;

private:
    bool m_initialized = false;
};

} // namespace Klein
#endif
