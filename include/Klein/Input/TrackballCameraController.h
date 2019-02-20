#ifndef KLEIN_TRACKBALLCAMERACONTROLLER_H
#define KLEIN_TRACKBALLCAMERACONTROLLER_H

#include <Klein/Core/Export.h>

#include <QPoint>
#include <QSize>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QAbstractCameraController>

namespace Klein
{

class KLEIN_API TrackballCameraController
    : public Qt3DExtras::QAbstractCameraController
{
    Q_OBJECT

    Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize NOTIFY
                   windowSizeChanged)
    Q_PROPERTY(float trackballSize READ trackballSize WRITE setTrackballSize
                   NOTIFY trackballSizeChanged)
    Q_PROPERTY(float rotationSpeed READ rotationSpeed WRITE setRotationSpeed
                   NOTIFY rotationSpeedChanged)
    Q_PROPERTY(
        float panSpeed READ panSpeed WRITE setPanSpeed NOTIFY panSpeedChanged)
    Q_PROPERTY(float zoomSpeed READ zoomSpeed WRITE setZoomSpeed NOTIFY
                   zoomSpeedChanged)

public:
    explicit TrackballCameraController(Qt3DCore::QNode* parent = nullptr);

    virtual ~TrackballCameraController() = default;

    QSize windowSize() const { return m_windowSize; }

    float trackballSize() const { return m_trackballSize; }

    float rotationSpeed() const { return m_rotationSpeed; }

    float panSpeed() const { return m_panSpeed; }

    float zoomSpeed() const { return m_zoomSpeed; }

public slots:
    void setWindowSize(QSize windowSize)
    {
        if (m_windowSize == windowSize) return;

        m_windowSize = windowSize;
        emit windowSizeChanged(m_windowSize);
    }

    void setTrackballSize(float trackballSize)
    {
        if (qFuzzyCompare(m_trackballSize, trackballSize)) return;

        m_trackballSize = trackballSize;
        emit trackballSizeChanged(m_trackballSize);
    }

    void setRotationSpeed(float rotationSpeed)
    {
        if (qFuzzyCompare(m_rotationSpeed, rotationSpeed)) return;

        m_rotationSpeed = rotationSpeed;
        emit rotationSpeedChanged(m_rotationSpeed);
    }

    void setPanSpeed(float panSpeed)
    {
        if (qFuzzyCompare(m_panSpeed, panSpeed)) return;

        m_panSpeed = panSpeed;
        emit panSpeedChanged(m_panSpeed);
    }

    void setZoomSpeed(float zoomSpeed)
    {
        if (qFuzzyCompare(m_zoomSpeed, zoomSpeed)) return;

        m_zoomSpeed = zoomSpeed;
        emit zoomSpeedChanged(m_zoomSpeed);
    }

    void reset()
    {
        m_rotationSpeed = 2.0f;
        m_panSpeed = 1.0f;
        m_zoomSpeed = 5.0f;
    }

signals:
    void windowSizeChanged(QSize windowSize);

    void trackballSizeChanged(float trackballSize);

    void rotationSpeedChanged(float rotationSpeed);

    void panSpeedChanged(float panSpeed);

    void zoomSpeedChanged(float zoomSpeed);

private:
    void moveCamera(
        const Qt3DExtras::QAbstractCameraController::InputState& state,
        float dt) override;

    QVector3D projectToTrackball(const QPoint& screenCoords) const;

    void createRotation(const QPoint& firstPoint,
                        const QPoint& nextPoint,
                        QVector3D& dir,
                        float& angle);

private:
    QPoint m_mouseLastPosition;
    QPoint m_mouseCurrentPosition;
    QSize m_windowSize;
    float m_rotationSpeed = 2.0f;
    float m_panSpeed = 1.0f;
    float m_zoomSpeed = 5.0f;
    float m_trackballSize = 1.0f;
};

} // namespace Klein

#endif // TRACKBALLCAMERACONTROLLER_H
