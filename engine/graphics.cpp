#include "graphics.h"
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glad/glad.h>

#define SOKOL_IMPL
#define SOKOL_GLES3

#include <sokol_gfx.h>
#include <triangle-sapp.glsl.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace {
    static struct {
        sg_pipeline pip;
        sg_bindings bind;
        sg_pass_action pass_action;
        sg_swapchain swapchain;
        sg_buffer guiBuffer;
        glm::mat4 ortho;
    } state;
}


namespace Engine {

    void Graphics::drawGui(const uint8_t* data, uint32_t sizeofdata, uint32_t vertexCount) {
        sg_update_buffer(state.guiBuffer, { data, sizeofdata });
        sg_draw(0, vertexCount, 1);
    }

    void Graphics::initialize() {

        sg_desc ff{ 0 };
        //ff.allocator.
        sg_setup(ff);

        sg_buffer_desc buf{ 0 };
        buf.data = { 0, sizeof(float) * 1000 };
        buf.label = "triangle-vertices";
        buf.usage = SG_USAGE_STREAM;
        state.guiBuffer = sg_make_buffer(buf);
        state.bind.vertex_buffers[0] = state.guiBuffer;
        sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));

        // create a pipeline object (default render states are fine for triangle)
        sg_pipeline_desc desc{ 0 };
        desc.shader = shd;
        desc.layout.attrs[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3;
        desc.layout.attrs[ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4;
        desc.layout.attrs[ATTR_vs_texCoords0].format = SG_VERTEXFORMAT_FLOAT2;
        desc.label = "triangle-pipeline";

        desc.cull_mode = SG_CULLMODE_BACK;
        desc.depth.write_enabled = false;
        state.pip = sg_make_pipeline(desc);

        sg_sampler_desc sampler{0};
        sampler.min_filter = SG_FILTER_LINEAR;
        sampler.mag_filter = SG_FILTER_LINEAR;
        sampler.mipmap_filter = SG_FILTER_LINEAR;

        state.bind.fs.samplers[SLOT_smp] = sg_make_sampler(sampler);
        state.bind.fs.images[SLOT_tex] = sg_alloc_image();
        

        
   


        // a pass action to clear framebuffer to black
        sg_pass_action pass;
        pass.colors[0].clear_value = { 0.0f, 0.0f, 0.0f, 1.0f };
        pass.colors[0].load_action = SG_LOADACTION_CLEAR;
        pass.colors[0].load_action = {};
        state.pass_action = pass;
	}

    void Graphics::beginDraw(uint32_t width, uint32_t height) {
        state.ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
 
        //swapchain.gl.framebuffer = 0;
        state.swapchain.height = height;
        state.swapchain.width = width;

        sg_pass pass{};
        pass.action = state.pass_action;
        pass.swapchain = state.swapchain;

        sg_begin_pass(pass);

        sg_apply_pipeline(state.pip);
        sg_apply_bindings(state.bind);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(state.ortho));
    }

    void Graphics::endDraw() {
        sg_end_pass();
        sg_commit();
    }

    void Graphics::setImage(const std::vector<uint8_t>& pixels, uint32_t width, uint32_t height, uint8_t depth) {
        sg_image_desc imageTexture{0};
        imageTexture.width = width;
        imageTexture.height = height;
        imageTexture.pixel_format = SG_PIXELFORMAT_RGBA8;

        imageTexture.data.subimage[0][0] = { pixels.data(), (size_t)(width * height * depth) };

        sg_init_image(state.bind.fs.images[SLOT_tex], imageTexture);
        /*.width = png_width,
            .height = png_height,
            .pixel_format = SG_PIXELFORMAT_RGBA8,
            .data.subimage[0][0] = {
                .ptr = pixels,
                .size = (size_t)(png_width * png_height * 4),
        }
    });*/
    }

}