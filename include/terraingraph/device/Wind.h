#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
{
namespace device
{

class Wind : public Device
{
public:
    Wind()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };

        Init();
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    void Init();

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Wind.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Wind

}
}