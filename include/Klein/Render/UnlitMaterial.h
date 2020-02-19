#ifndef KLEIN_UNLITMATERIAL_H
#define KLEIN_UNLITMATERIAL_H

#include <Klein/Core/Export.h>
#include <Klein/Render/BaseUnlitMaterial.h>

namespace Klein
{

class KLEIN_API UnlitMaterial : public BaseUnlitMaterial
{
    Q_OBJECT

public:
    explicit UnlitMaterial(Qt3DCore::QNode* parent = nullptr);

    virtual ~UnlitMaterial() = default;

public:
    static const QString effectName;

    static Qt3DRender::QEffect* createEffect();
};

} // namespace Klein

#endif
