#include "terraingraph/device/Lighting.h"
#include "terraingraph/device/AlbedoMap.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HemanHelper.h"
#include "terraingraph/Context.h"

#include <heightfield/HeightField.h>

#include <heman.h>

namespace terraingraph
{
namespace device
{

void Lighting::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;

    auto he_height = HemanHelper::Encode(dev, *prev_hf);
    auto he_albedo = AlbedoMap::Baking(he_height);
    auto he_img = heman_lighting_apply(he_height, he_albedo, 1, 1, 0.5, m_light_pos.xyz);
    auto he_img_data = heman_image_data(he_img);

    size_t w = prev_hf->Width();
    size_t h = prev_hf->Height();

    m_bmp = std::make_shared<Bitmap>(w, h);
    auto img_data = m_bmp->GetPixels();
    for (size_t i = 0, n = w * h * 3; i < n; ++i) {
        img_data[i] = static_cast<unsigned char>(he_img_data[i] * 255);
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_albedo);
    heman_image_destroy(he_img);
}

}
}