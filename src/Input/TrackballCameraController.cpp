#include <Klein/Input/TrackballCameraController.h>

#include <QtMath>
#include <QVector2D>
#include <Qt3DCore/QTransform>
#include <Qt3DInput/QAction>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QKeyboardHandler>
#include <Qt3DRender/QCamera>

namespace Klein
{

TrackballCameraController::TrackballCameraController(Qt3DCore::QNode* parent)
    : Qt3DExtras::QAbstractCameraController(parent)
{
    Qt3DInput::QMouseHandler* mouseHandler = new Qt3DInput::QMouseHandler(this);
    mouseHandler->setSourceDevice(mouseDevice());

    QObject::connect(mouseHandler,
                     &Qt3DInput::QMouseHandler::pressed,
                     [this](Qt3DInput::QMouseEvent* pressedEvent) {
                         pressedEvent->setAccepted(true);
                         m_mouseLastPosition =
                             QPoint(pressedEvent->x(), pressedEvent->y());
                         m_mouseCurrentPosition = m_mouseLastPosition;
                     });

    QObject::connect(mouseHandler,
                     &Qt3DInput::QMouseHandler::positionChanged,
                     [this](Qt3DInput::QMouseEvent* positionChangedEvent) {
                         positionChangedEvent->setAccepted(true);
                         m_mouseCurrentPosition =
                             QPoint(positionChangedEvent->x(),
                                    positionChangedEvent->y());
                     });
}

QVector3D TrackballCameraController::projectToTrackball(
    const QPoint& screenCoords) const
{
    float sx = screenCoords.x(), sy = m_windowSize.height() - screenCoords.y();

    QVector2D p2d(sx / m_windowSize.width() - 0.5f,
                  sy / m_windowSize.height() - 0.5f);

    float z = 0.0f;
    float r2 = m_trackballSize * m_trackballSize;
    if (p2d.lengthSquared() <= r2 * 0.5f) {
        z = qSqrt(r2 - p2d.lengthSquared());
    }
    else {
        z = r2 * 0.5f / p2d.length();
    }

    QVector3D p3d(p2d, z);
    return p3d;
}

static inline float clamp(float x)
{
    return x > 1 ? 1 : (x < -1 ? -1 : x);
}

void TrackballCameraController::createRotation(const QPoint& firstPoint,
                                               const QPoint& nextPoint,
                                               QVector3D& dir,
                                               float& angle)
{
    auto lastPos3D = projectToTrackball(firstPoint).normalized();
    auto currentPos3D = projectToTrackball(nextPoint).normalized();

    // Compute axis of rotation:
    dir = QVector3D::crossProduct(currentPos3D, lastPos3D);

    // Approximate rotation angle:
    angle = qAcos(clamp(QVector3D::dotProduct(currentPos3D, lastPos3D)));
}

void TrackballCameraController::moveCamera(
    const Qt3DExtras::QAbstractCameraController::InputState& state,
    float dt)
{
    auto theCamera = this->camera();

    if (theCamera == nullptr) return;

    if (state.leftMouseButtonActive) {
        if (state.shiftKeyActive) { // pan
            auto dist =
                (theCamera->position() - theCamera->viewCenter()).length();
            auto d = dt * m_panSpeed * dist;
            theCamera->translate(
                QVector3D(-state.rxAxisValue, -state.ryAxisValue, 0.0f) * d);
        }
        else { // rotate
            QVector3D dir;
            float angle;
            createRotation(
                m_mouseLastPosition, m_mouseCurrentPosition, dir, angle);

            auto currentRotation = theCamera->transform()->rotation();

            auto rotatedAxis = currentRotation.rotatedVector(dir);
            angle *= m_rotationSpeed;

            theCamera->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle(
                rotatedAxis, angle * M_1_PI * 180));
            m_mouseLastPosition = m_mouseCurrentPosition;
        }
    }

    if (state.tzAxisValue != 0.0f) { // zoom
        theCamera->translate(
            QVector3D(0.0f, 0.0f, state.tzAxisValue * m_zoomSpeed * dt),
            Qt3DRender::QCamera::DontTranslateViewCenter);
    }
}

} // namespace Klein