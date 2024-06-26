#include "graphics.h"
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glad/glad.h>

#define SOKOL_IMPL
#define SOKOL_GLES3

#include <sokol_gfx.h>


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
		sg_desc ff{};
		//ff.allocator.
		sg_setup(ff);
        float vertices[] = {
            // positions            // colors
             0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
        };
        state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
            .data = SG_RANGE(vertices),
                .label = "triangle-vertices"
        });

        // create shader from code-generated sg_shader_desc
        sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));

        // create a pipeline object (default render states are fine for triangle)
        state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
            .shader = shd,
                // if the vertex layout doesn't have gaps, don't need to provide strides and offsets
                .layout = {
                    .attrs = {
                        [ATTR_vs_position] .format = SG_VERTEXFORMAT_FLOAT3,
                        [ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4
                    }
            },
                .label = "triangle-pipeline"
        });

        // a pass action to clear framebuffer to black
        state.pass_action = (sg_pass_action) {
            .colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 1.0f } }
        };
	}

}