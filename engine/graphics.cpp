#include "graphics.h"
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glad/glad.h>

#define SOKOL_IMPL
#define SOKOL_GLES3

#include <sokol_gfx.h>
#include <triangle-sapp.glsl.h>

namespace {
    static struct {
        sg_pipeline pip;
        sg_bindings bind;
        sg_pass_action pass_action;
    } state;
}

namespace Engine {
	void Graphics::drawBox(glm::vec3 size, glm::vec4 color) {}
	void Graphics::drawRect2D(glm::vec2 size, glm::vec2 position) {}


	void Graphics::initialize() {
		sg_desc ff{0};
		//ff.allocator.
		sg_setup(ff);
        float vertices[] = {
            // positions            // colors
             0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
        };
        sg_buffer_desc buf{0};
        buf.data = SG_RANGE(vertices);
        buf.label = "triangle-vertices";
        state.bind.vertex_buffers[0] = sg_make_buffer(buf);

        // create shader from code-generated sg_shader_desc
        sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));

        // create a pipeline object (default render states are fine for triangle)
        sg_pipeline_desc desc {0};
        desc.shader = shd;
        desc.layout.attrs[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3;
        desc.layout.attrs[ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4;
        desc.label = "triangle-pipeline";
        state.pip = sg_make_pipeline(desc);

        // a pass action to clear framebuffer to black
        sg_pass_action pass;
        pass.colors[0].clear_value = { 0.0f, 0.0f, 0.0f, 1.0f };
        pass.colors[0].load_action = SG_LOADACTION_CLEAR;
        pass.colors[0].load_action = {};
        state.pass_action = pass;
	}

    void Graphics::beginDraw() {

        sg_swapchain swapchain{0};
 
        //swapchain.gl.framebuffer = 0;
        swapchain.height = 600;
        swapchain.width = 400;

        sg_pass pass{};
        pass.action = state.pass_action;
        pass.swapchain = swapchain;

        sg_begin_pass(pass);
        sg_apply_pipeline(state.pip);
        sg_apply_bindings(state.bind);
        sg_draw(0, 3, 1);


    }

    void Graphics::endDraw() {
        sg_end_pass();
        sg_commit();
    }

}