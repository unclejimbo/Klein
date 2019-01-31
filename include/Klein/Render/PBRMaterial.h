#ifndef KLEIN_PBRMATERIAL_H
#define KLEIN_PBRMATERIAL_H

#include <Klein/Core/Export.h>

#include <QSharedPointer>
#include <Qt3DRender/QMaterial>

namespace Qt3DRender
{
class QEffect;
class QParameter;
} // namespace Qt3DRender

namespace Klein
{

struct KLEIN_API PBRMaterialParams
{
    Qt3DRender::QParameter* ambientness = nullptr;
    Qt3DRender::QParameter* baseColor = nullptr;
    Qt3DRender::QParameter* baseColorMap = nullptr;
    Qt3DRender::QParameter* metalness = nullptr;
    Qt3DRender::QParameter* roughness = nullptr;
    Qt3DRender::QParameter* texCoordOffset = nullptr;
    Qt3DRender::QParameter* texCoordScale = nullptr;
    Qt3DRender::QParameter* useTexture = nullptr;
};

class KLEIN_API PBRMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT

public:
    explicit PBRMaterial(Qt3DCore::QNode* parent = nullptr);

    PBRMaterial(PBRMaterialParams params, Qt3DCore::QNode* parent = nullptr);

public:
    static QSharedPointer<Qt3DRender::QEffect> pbrEffect;

private:
    void build();

private:
    Qt3DRender::QParameter* m_ambientness;
    Qt3DRender::QParameter* m_baseColor;
    Qt3DRender::QParameter* m_baseColorMap;
    Qt3DRender::QParameter* m_metalness;
    Qt3DRender::QParameter* m_roughness;
    Qt3DRender::QParameter* m_texCoordOffset;
    Qt3DRender::QParameter* m_texCoordScale;
    Qt3DRender::QParameter* m_useTexture;
};

} // namespace Klein

#endif
