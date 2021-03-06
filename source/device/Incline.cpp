#include "terraingraph/device/Incline.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/EvalGPU.h"
#include "terraingraph/Context.h"

#include <heightfield/HeightField.h>
#include <painting0/ShaderUniforms.h>

namespace
{

std::shared_ptr<terraingraph::EvalGPU> EVAL = nullptr;

const char* vs = R"(

#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texcoord;

out VS_OUT {
    vec2 texcoord;
} vs_out;

void main()
{
	vs_out.texcoord = texcoord;
	gl_Position = position;
}

)";

const char* fs = R"(

#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texcoord;
} fs_in;

uniform vec2 offsets;
uniform float offset;
uniform float factor;
uniform sampler2D heightmap;
uniform bool invert;
const float pih = 3.14159265358979323846264*0.5;

vec3 get(float s, float t){
    float height = texture2D(heightmap, vec2(s, t)).r;
    return vec3(s, height, t);
}

vec3 get_normal(vec3 p){
    float x = offsets.x;
    float z = offsets.y;
    return normalize(vec3(
        get(p.x-x, p.z).y - get(p.x+x, p.z).y,
        x+z,
        get(p.x, p.z-z).y - get(p.x, p.z+z).y
    ));
}

void main(void){
    vec2 uv = fs_in.texcoord;
    vec3 pos = get(uv.s, uv.t);
    vec3 normal = get_normal(pos);
    float result = dot(normal, vec3(0.0, 1.0, 0.0));
    result = result * factor + offset;
    if(invert){
        result = 1.0-result;
    }
    FragColor = vec4(result, result, result, 1.0);
}

)";

}

namespace terraingraph
{
namespace device
{

void Incline::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;

    //std::vector<uint32_t> textures;
    auto heightmap = prev_hf->GetHeightmap(dev);
    //textures.push_back(heightmap->TexID());

    pt0::ShaderUniforms vals;
    vals.AddVar("offsets", pt0::RenderVariant(m_offsets));
    vals.AddVar("offset",  pt0::RenderVariant(m_offset));
    vals.AddVar("factor",  pt0::RenderVariant(m_factor));
    vals.AddVar("invert",  pt0::RenderVariant(m_invert));

    m_hf = std::make_shared<hf::HeightField>(prev_hf->Width(), prev_hf->Height());

    if (!EVAL)
    {
        //std::vector<std::string> texture_names;
        //texture_names.push_back("heightmap");

        EVAL = std::make_shared<EvalGPU>(dev, vs, fs);
    }
    EVAL->RunPS(dev, vals, *m_hf);
}

}
}