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
namespace {
    static struct {
        sg_pipeline pip;
        sg_bindings bind;
        sg_pass_action pass_action;
        sg_swapchain swapchain;
        sg_buffer from_update;
        glm::mat4 ortho;
        
    } state;

    glm::vec2 virtualResolution(600, 600);




}

namespace Engine {
	void Graphics::drawBox(glm::vec3 size, glm::vec4 color) {}
	void Graphics::drawRect2D(glm::vec2 size, glm::vec2 position) {}

    void Graphics::drawGui(const GUINode& node) {
        float minDimRealScreen = 0;
        switch (node.adjustMod) {
        case GUIAdjustMod::Stretch:
        case GUIAdjustMod::Fit:
            minDimRealScreen = std::min(state.swapchain.width, state.swapchain.height);
            break;

        case GUIAdjustMod::Zoom:
            minDimRealScreen = std::max(state.swapchain.width, state.swapchain.height);
            break;
        }
        
        float maxDimVirtualScreen = std::min(virtualResolution.x, virtualResolution.y);
        float screenSize = minDimRealScreen / maxDimVirtualScreen;
        //spdlog::info("screen_size {0}", screenSize);

        glm::vec2 drawSize = node.size;

        glm::vec2 virtualScreen = virtualResolution;
        glm::vec2 realsScreen = glm::vec2((float)state.swapchain.width, (float)state.swapchain.height);
        glm::vec2 offset = (realsScreen - virtualScreen* screenSize) * 0.5f;
        glm::vec2 drawDim = drawSize / 2.0f ;

        glm::vec2 p1;
        glm::vec2 p2;

        if (node.adjustMod == GUIAdjustMod::Stretch) {
            glm::vec2 stretch = realsScreen / virtualScreen;
            p1 = offset + node.position * screenSize + drawDim * stretch;
            p2 = offset + node.position * screenSize - drawDim * stretch;
        } else {
            p1 = offset + (node.position + drawDim) * screenSize;
            p2 = offset + (node.position - drawDim) * screenSize;
        }


        float vertices[] = {
            // positions            colors
             p2.x,  p1.y, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
             p1.x,  p1.y, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
             p1.x,  p2.y, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
             p2.x,  p2.y, 0.5f,     1.0f, 1.0f, 0.0f, 1.0f,
        };

        sg_update_buffer(state.from_update, SG_RANGE(vertices));
    }


	void Graphics::initialize() {
        
		sg_desc ff{0};
		//ff.allocator.
		sg_setup(ff);
        float vertices[] = {
            // positions            colors
            -10.5f,  10.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
             10.5f,  10.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
             10.5f, -10.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
            -10.5f, -10.5f, 0.5f,     1.0f, 1.0f, 0.0f, 1.0f,
        };

        // an index buffer with 2 triangles
        uint16_t indices[] = { 0, 1, 2,  0, 2, 3 };
        sg_buffer_desc index_buf{ 0 };
        index_buf.type = SG_BUFFERTYPE_INDEXBUFFER;
        index_buf.data = SG_RANGE(indices);
        index_buf.label = "quad-indices";


        sg_buffer_desc buf{0};
        buf.data = {0, sizeof(vertices)};
        buf.label = "triangle-vertices";
        buf.usage = SG_USAGE_STREAM;
        state.from_update = sg_make_buffer(buf);
        state.bind.vertex_buffers[0] = state.from_update;
        
        state.bind.index_buffer = sg_make_buffer(index_buf);



        // create shader from code-generated sg_shader_desc
        sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));

        // create a pipeline object (default render states are fine for triangle)
        sg_pipeline_desc desc {0};
        desc.shader = shd;
        desc.layout.attrs[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3;
        desc.layout.attrs[ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4;
        desc.label = "triangle-pipeline";
        desc.index_type = SG_INDEXTYPE_UINT16;
        state.pip = sg_make_pipeline(desc);

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
        sg_draw(0, 6, 1);


    }

    void Graphics::endDraw() {
        sg_end_pass();
        sg_commit();
    }

    void Graphics::setMatrixGUI() {

    }

}