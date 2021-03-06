#include "terraingraph/device/Archipelago.h"
#include "terraingraph/HemanHelper.h"

#include <heman.h>

namespace terraingraph
{
namespace device
{

void Archipelago::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    heman_points* pts = heman_image_create(m_points.size(), 1, 3);
    sm::vec3* coords = (sm::vec3*)heman_image_data(pts);
    for (size_t i = 0, n = m_points.size(); i < n; ++i) {
        coords[i] = m_points[i];
    }

    heman_image* he_height = heman_generate_archipelago_heightmap(
        m_width, m_height, pts, m_noise_amt, m_seed
    );

    heman_image_destroy(pts);

    m_hf = HemanHelper::Decode(he_height);
    heman_image_destroy(he_height);
}

}
}