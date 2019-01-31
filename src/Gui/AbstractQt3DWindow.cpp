#include <Klein/Gui/AbstractQt3DWindow.h>

#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QEntity>
#include <Qt3DInput/QInputAspect>
#include <Qt3DInput/QInputSettings>
#include <Qt3DLogic/QLogicAspect>
#include <Qt3DRender/QRenderAspect>

namespace Klein
{

AbstractQt3DWindow::AbstractQt3DWindow(QWindow* parent) : QWindow(parent)
{
    this->setSurfaceType(QSurface::OpenGLSurface);
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#ifdef QT_OPENGL_ES_2
    format.setRenderableType(QSurfaceFormat::OpenGLES);
#else
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
#endif
    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    this->setFormat(format);
    QSurfaceFormat::setDefaultFormat(format);

    m_rootEntity = new Qt3DCore::QEntity;

    m_aspectEngine = new Qt3DCore::QAspectEngine(this);
    m_aspectEngine->registerAspect(new Qt3DInput::QInputAspect);
    m_aspectEngine->registerAspect(new Qt3DLogic::QLogicAspect);
    m_aspectEngine->registerAspect(new Qt3DRender::QRenderAspect);
}

void AbstractQt3DWindow::exposeEvent(QExposeEvent*)
{
    if (!m_initialized && isExposed()) {
        createSceneGraph();
        createRenderSettings();
        createInputSettings();
        m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(m_rootEntity));
        m_initialized = true;
    }
}

void AbstractQt3DWindow::createInputSettings()
{
    auto settings = new Qt3DInput::QInputSettings;
    settings->setEventSource(this);
    m_rootEntity->addComponent(settings);
}

} // namespace Klein
