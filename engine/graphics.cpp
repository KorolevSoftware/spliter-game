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
        sg_buffer from_update;
        glm::mat4 ortho;
        
    } state;

    glm::vec2 virtualResolution(600, 600);
}


std::vector<float> guiVertexData;
uint32_t current_index = 0;
glm::vec2 parent_pffset;

glm::vec2 now_size;
namespace Engine {
	void Graphics::drawBox(glm::vec3 size, glm::vec4 color) {}
	void Graphics::drawRect2D(glm::vec2 size, glm::vec2 position) {}

    void Graphics::drawGui(const GUINode& node) {
        glm::vec2 realResolution = glm::vec2((float)state.swapchain.width, (float)state.swapchain.height);
        glm::vec2 divResolution = virtualResolution;
        if (node.parent) {
            divResolution = node.parent->size;
            realResolution = now_size;
        } else {
           
        }
      
        glm::vec2 adjustScale = realResolution / divResolution;

        float temp;
        switch (node.adjustMod) {
        case GUIAdjustMod::Fit:
            temp = std::min(adjustScale.x, adjustScale.y);
            adjustScale = glm::vec2(temp, temp);
            break;

        case GUIAdjustMod::Zoom:
            temp = std::max(adjustScale.x, adjustScale.y);
            adjustScale = glm::vec2(temp, temp);
            break;
        } // case GUIAdjustMod::Stretch: skip adjustScale correct

        glm::vec2 offset = (realResolution - divResolution * adjustScale) * 0.5f;
        
        glm::vec2 p1;
        glm::vec2 p2;

        //-----------------------p1
        //|                      |
        //|                      |
        //|                      |
        //|                      |
        //|                      |
        //|                      |
        //|                      |
        //p2----------------------

        // gen primitive
        glm::vec2 drawDim = node.size / 2.0f;

        switch (node.pivot) {
        case GUIPivot::Centre:
            p1 = node.position + drawDim;
            p2 = node.position - drawDim;
            break;

        case GUIPivot::North:
            p1 = node.position + drawDim - glm::vec2(0, drawDim.y);
            p2 = node.position - drawDim - glm::vec2(0, drawDim.y);
            break;

        case GUIPivot::NorthEast:
            p1 = node.position;
            p2 = node.position - node.size;
            break;

        case GUIPivot::East:
            p1 = node.position + drawDim - glm::vec2(drawDim.x, 0);
            p2 = node.position - drawDim - glm::vec2(drawDim.x, 0);
            break;

        case GUIPivot::SouthEast:
            p1 = node.position + glm::vec2(0, node.size.y);
            p2 = node.position - glm::vec2(node.size.x, 0);
            break;

        case GUIPivot::South:
            p1 = node.position + glm::vec2(drawDim.x, node.size.y);
            p2 = node.position + glm::vec2(-drawDim.x, 0);
            break;

        case GUIPivot::SouthWest:
            p1 = node.position + glm::vec2(node.size.x, node.size.y);
            p2 = node.position;
            break;

        case GUIPivot::West:
            p1 = node.position + glm::vec2(node.size.x, drawDim.y);
            p2 = node.position + glm::vec2(0, -drawDim.y);
            break;

        case GUIPivot::NorthWest:
            p1 = node.position + glm::vec2(node.size.x, 0 );
            p2 = node.position + glm::vec2(0, -node.size.y);
            break;
        }

        if (node.anchorX) {
            offset.x = 0;
        }

        if (node.anchorY) {
            offset.y = 0;
        }
      
        if (node.parent) {
            offset += parent_pffset;
        }

        if (node.adjustMod == GUIAdjustMod::Stretch) {
            p1 *= adjustScale;
            p2 *= adjustScale;
        } else {
            p1 = offset + p1 * adjustScale;
            p2 = offset + p2 * adjustScale;
        }
        if (!node.parent) {
            parent_pffset = offset + (node.position)* adjustScale;
            now_size = node.size * adjustScale;
        }
        //float vertices[] = {
        //    // positions            colors
        //     p2.x,  p1.y, 0.5f,     node.color.r, node.color.g, node.color.b, node.color.a,
        //     p1.x,  p1.y, 0.5f,     node.color.r, node.color.g, node.color.b, node.color.a,
        //     p1.x,  p2.y, 0.5f,     node.color.r, node.color.g, node.color.b, node.color.a,
        //     p2.x,  p2.y, 0.5f,     node.color.r, node.color.g, node.color.b, node.color.a,
        //};

        

        guiVertexData[current_index++] = p2.x;
        guiVertexData[current_index++] = p1.y;
        guiVertexData[current_index++] = 0.5f;
        guiVertexData[current_index++] = node.color.r;
        guiVertexData[current_index++] = node.color.g;
        guiVertexData[current_index++] = node.color.b;
        guiVertexData[current_index++] = node.color.a;

        guiVertexData[current_index++] = p1.x;
        guiVertexData[current_index++] = p1.y;
        guiVertexData[current_index++] = 0.5f;
        guiVertexData[current_index++] = node.color.r;
        guiVertexData[current_index++] = node.color.g;
        guiVertexData[current_index++] = node.color.b;
        guiVertexData[current_index++] = node.color.a;

        guiVertexData[current_index++] = p1.x;
        guiVertexData[current_index++] = p2.y;
        guiVertexData[current_index++] = 0.5f;
        guiVertexData[current_index++] = node.color.r;
        guiVertexData[current_index++] = node.color.g;
        guiVertexData[current_index++] = node.color.b;
        guiVertexData[current_index++] = node.color.a;

        guiVertexData[current_index++] = p2.x;
        guiVertexData[current_index++] = p2.y;
        guiVertexData[current_index++] = 0.5f;
        guiVertexData[current_index++] = node.color.r;
        guiVertexData[current_index++] = node.color.g;
        guiVertexData[current_index++] = node.color.b;
        guiVertexData[current_index++] = node.color.a;
        
      
    }


	void Graphics::initialize() {
        guiVertexData.resize(56);
		sg_desc ff{0};
		//ff.allocator.
		sg_setup(ff);

        sg_buffer_desc buf{0};
        buf.data = {0, sizeof(float)* 56 };
        buf.label = "triangle-vertices";
        buf.usage = SG_USAGE_STREAM;
        state.from_update = sg_make_buffer(buf);
        state.bind.vertex_buffers[0] = state.from_update;
        


        // an index buffer with 2 triangles
        uint16_t indices[] = { 0, 1, 2,  0, 2, 3,  4, 5, 6,  4, 6, 7 };
        sg_buffer_desc index_buf{ 0 };
        index_buf.type = SG_BUFFERTYPE_INDEXBUFFER;
        index_buf.data = SG_RANGE(indices);
        index_buf.label = "quad-indices";
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
        desc.cull_mode = SG_CULLMODE_BACK;
        desc.depth.write_enabled = false;
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

    }

    void Graphics::endDraw() {
      
        sg_update_buffer(state.from_update, { guiVertexData.data(), sizeof(float) * guiVertexData.size() });
        sg_draw(0, 12, 1);

        sg_end_pass();
        sg_commit();
        current_index = 0;
    }

    void Graphics::setMatrixGUI() {

    }

}