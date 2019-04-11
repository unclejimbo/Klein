#ifndef KLEIN_PBRMATERIAL_H
#define KLEIN_PBRMATERIAL_H

#include <Klein/Core/Export.h>
#include <Klein/Render/BasePBRMaterial.h>

namespace Klein
{

class KLEIN_API PBRMaterial : public BasePBRMaterial
{
    Q_OBJECT

public:
    explicit PBRMaterial(Qt3DCore::QNode* parent = nullptr);

    PBRMaterial(ColorMode mode, Qt3DCore::QNode* parent = nullptr);

    virtual ~PBRMaterial() = default;
};

} // namespace Klein

#endif
