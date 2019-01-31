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

namespace Klein
{

class KLEIN_API AbstractQt3DWindow : public QWindow
{
    Q_OBJECT

public:
    explicit AbstractQt3DWindow(QWindow* parent = nullptr);

    virtual ~AbstractQt3DWindow() {}

protected:
    void exposeEvent(QExposeEvent*) override;

    virtual void createSceneGraph() = 0;

    virtual void createRenderSettings() = 0;

    virtual void createInputSettings();

protected:
    Qt3DCore::QAspectEngine* m_aspectEngine;
    Qt3DCore::QEntity* m_rootEntity;

private:
    bool m_initialized = false;
};

} // namespace Klein
#endif
