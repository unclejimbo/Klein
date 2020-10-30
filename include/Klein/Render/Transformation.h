#ifndef KLEIN_TRANSFORMATION_H
#define KLEIN_TRANSFORMATION_H

#include <Klein/Core/Export.h>

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
    QMatrix4x4 scaling, rotation, translation;
    scaling.scale(1.0f, (p0 - p1).length(), 1.0f);
    rotation.rotate(
        QQuaternion::rotationTo(QVector3D(0.0f, 1.0f, 0.0f), (p1 - p0)));
    translation.translate((p0 + p1) / 2);
    return translation * rotation * scaling;
}

} // namespace Klein

#endif
