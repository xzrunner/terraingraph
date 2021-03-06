#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace ur { class Device; }

namespace terraingraph
{
namespace device
{

class Curves : public Device
{
public:
    enum class Type
    {
        Linear,
        Spline,
    };

public:
    Curves()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    float CalcHeight(float h) const;

    void CalcHeightRegion(const ur::Device& dev,
        const hf::HeightField& hf, int32_t& min, int32_t& max);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Curves.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Curves

}
}