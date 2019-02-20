#ifndef KLEIN_TRANSFORMATION_H
#define KLEIN_TRANSFORMATION_H

#include <Foliation/Core/Export.h>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>

namespace Klein
{

inline KLEIN_API QMatrix4x4 pointTransformation(const QVector3D& p)
{
    QMatrix4x4 translation;
    translation.translate(p);
    return translation;
}

inline KLEIN_API QMatrix4x4 lineTransformation(const QVector3D& p0,
                                               const QVector3D& p1)
{
    QMatrix4x4 rotation, translation;
    rotation.rotate(
        QQuaternion::rotationTo(QVector3D(0.0f, 1.0f, 0.0f), (p0 - p1)));
    translation.translate((p0 + p1) / 2);
    return translation * rotation;
}

} // namespace Klein

#endif
