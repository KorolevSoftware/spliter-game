#include "graphics.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_CONFIG_CLIP_CONTROL GLM_CLIP_CONTROL_LH_ZO
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include "glm/gtc/matrix_transform.hpp"
#include <glad/glad.h>
#define SOKOL_LOG_IMPL
#include "sokol_log.h"
#include <SDL3/SDL.h>

#define SOKOL_IMPL
#define SOKOL_METAL

#include <sokol_gfx.h>
// #include <sokol_app.h>
#include <triangle-sapp.glsl.h>
#include <spdlog/spdlog.h>
#include <vector>




#ifdef TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#else
#import <AppKit/NSWindow.h>
#endif

namespace {
    struct RenderPipline {
        sg_pipeline pip;
        sg_bindings bind;
    };

    static struct {
        sg_pass_action pass_action;
        sg_swapchain swapchain;
        sg_buffer guiBuffer;
    } state;

    RenderPipline guiPipline;
    RenderPipline gamePipline;
    CAMetalLayer *metal_layer;
    id <MTLDevice> device;
    id mtk_view_controller;

    RenderPipline make_game_pipline() {
        float vertices[] = {
       -1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
        1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
        1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
       -1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,

       -1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
        1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
        1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
       -1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,

       -1.0, -1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
       -1.0,  1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
       -1.0,  1.0,  1.0,   0.0, 0.0, 1.0, 1.0,
       -1.0, -1.0,  1.0,   0.0, 0.0, 1.0, 1.0,

       1.0, -1.0, -1.0,    1.0, 0.5, 0.0, 1.0,
       1.0,  1.0, -1.0,    1.0, 0.5, 0.0, 1.0,
       1.0,  1.0,  1.0,    1.0, 0.5, 0.0, 1.0,
       1.0, -1.0,  1.0,    1.0, 0.5, 0.0, 1.0,

       -1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,
       -1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
        1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
        1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,

       -1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0,
       -1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
        1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
        1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0
        };
        sg_buffer_desc vbufdesc{ 0 };
        vbufdesc.data = SG_RANGE(vertices);
        vbufdesc.label = "cube-vertices";
        sg_buffer vbuf = sg_make_buffer(vbufdesc);

        /* create an index buffer for the cube */
        uint16_t indices[] = {
            0, 1, 2,  0, 2, 3,
            6, 5, 4,  7, 6, 4,
            8, 9, 10,  8, 10, 11,
            14, 13, 12,  15, 14, 12,
            16, 17, 18,  16, 18, 19,
            22, 21, 20,  23, 22, 20
        };
        sg_buffer_desc ibufdesc{ 0 };
        ibufdesc.type = SG_BUFFERTYPE_INDEXBUFFER;
        ibufdesc.data = SG_RANGE(indices);
        ibufdesc.label = "cube-indices";
        sg_buffer ibuf = sg_make_buffer(ibufdesc);

        /* create shader */
        sg_shader shd = sg_make_shader(cube_shader_desc(sg_query_backend()));
        RenderPipline gamePipline;
        /* create pipeline object */
        sg_pipeline_desc gamePip{0};
        gamePip.layout.buffers[0].stride = 28;
        gamePip.layout.attrs[ATTR_cube_position].format = SG_VERTEXFORMAT_FLOAT3;
        gamePip.layout.attrs[ATTR_cube_color0].format = SG_VERTEXFORMAT_FLOAT4;
        gamePip.shader = shd;
        //gamePip.cull_mode = SG_CULLMODE_BACK;
        gamePip.index_type = SG_INDEXTYPE_UINT16;
        gamePip.depth.write_enabled = true;
        gamePip.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        gamePip.label = "cube-pipeline";
        gamePipline.pip = sg_make_pipeline(gamePip);

        /* setup resource bindings */
        sg_bindings gbinds{ 0 };
        gbinds.vertex_buffers[0] = vbuf;    
        gbinds.index_buffer = ibuf;
        gamePipline.bind = gbinds;
        return gamePipline;
    }

    RenderPipline make_gui_pipline(const std::vector<uint8_t>& pixels, uint32_t width, uint32_t height, uint8_t depth) {
        RenderPipline rPipline{0};
        sg_buffer_desc buf{ 0 };
        buf.data = { 0, sizeof(float) * 4000 };
        buf.label = "gui-vertices";
        buf.usage = SG_USAGE_DYNAMIC;
        state.guiBuffer = sg_make_buffer(buf);
        rPipline.bind.vertex_buffers[0] = state.guiBuffer;
        // create a pipeline object (default render states are fine for triangle)

        sg_pipeline_desc desc{ 0 };
        desc.shader = sg_make_shader(gui_shader_desc(sg_query_backend()));
        desc.layout.attrs[ATTR_gui_position].format = SG_VERTEXFORMAT_FLOAT3;
        desc.layout.attrs[ATTR_gui_color0].format = SG_VERTEXFORMAT_FLOAT4;
        desc.layout.attrs[ATTR_gui_texCoords0].format = SG_VERTEXFORMAT_FLOAT2;
        desc.label = "gui-pipeline";
        desc.cull_mode = SG_CULLMODE_BACK;
        desc.depth.write_enabled = false;
        desc.colors[0].blend.enabled = true;
        desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
        desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                
                

        rPipline.pip = sg_make_pipeline(desc);

        sg_sampler_desc sampler{ 0 };
        sampler.min_filter = SG_FILTER_LINEAR;
        sampler.mag_filter = SG_FILTER_LINEAR;
        sampler.mipmap_filter = SG_FILTER_LINEAR;

        rPipline.bind.samplers[SMP_smp] = sg_make_sampler(sampler);

        sg_image_desc imageTexture{ 0 };
        imageTexture.width = width;
        imageTexture.height = height;
        imageTexture.pixel_format = SG_PIXELFORMAT_RGBA8;
        imageTexture.data.subimage[0][0] = { pixels.data(), (size_t)(width * height * depth) };
        rPipline.bind.images[IMG_tex] = sg_make_image(imageTexture);
        return rPipline;
    }

    glm::mat4 camera(glm::vec3 Translate, glm::vec3 const& Rotate) {
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
        glm::mat4 View = glm::translate(glm::mat4(1.0f), Translate);
        View = glm::rotate(View, Rotate.y, glm::vec3(1.0f, 0.0f, 0.0f));
        View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
        View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
        return Projection * View * Model;
    }

} // namespace


namespace Engine {

    void Graphics::drawGui(const uint8_t* data, uint32_t sizeofdata, uint32_t vertexCount) {
        
        // Draw GUi
        glm::mat4 ortho = glm::ortho(0.0f, (float)state.swapchain.width, 0.0f, (float)state.swapchain.height);
        sg_apply_pipeline(guiPipline.pip);
        sg_apply_bindings(guiPipline.bind);
        auto range = SG_RANGE(ortho);
        sg_apply_uniforms(UB_vs_params1, &range);
        sg_update_buffer(state.guiBuffer, { data, sizeofdata });
        sg_draw(0, vertexCount, 1);
    }

    void Graphics::setZoom(float zoom) {
        this->zoom = zoom;
    }

    void Graphics::setCameraOffsetY(float offset) {
        origin.y = offset;
    }

    void Graphics::initialize(void* GPUContext, uint32_t width, uint32_t height) {
        origin = glm::vec3(0);
        sg_desc ff { 0 };
        ff.environment.defaults.depth_format =SG_PIXELFORMAT_DEPTH;
        ff.logger.func = slog_func;
        
        device = MTLCreateSystemDefaultDevice();
        // Setup the Metal layer
        metal_layer = [CAMetalLayer layer];
        metal_layer.device = device;
        metal_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        metal_layer.frame = [UIScreen mainScreen].bounds;
        
        #if TARGET_OS_IPHONE
        UIWindow* uiWindow = reinterpret_cast<UIWindow*>(GPUContext);
//        UIView *fullScreenView = [[UIView alloc] initWithFrame:[UIScreen mainScreen].bounds];
//        UIViewController *newViewController = [[UIViewController alloc] init];
//        newViewController.view = fullScreenView;
       
        [uiWindow.rootViewController.view.layer addSublayer: metal_layer];
//        [uiWindow setRootViewController:newViewController];
//        [uiWindow makeKeyAndVisible];
//        [fullScreenView.layer addSublayer:metal_layer];

//        [uiWindow addSubview:fullScreenView];
        #endif
        
        #if TARGET_OSX
        NSWindow* nsWindow = reinterpret_cast<NSWindow*>(GPUContext);
        nsWindow.contentView.layer = metal_layer;
        nsWindow.contentView.wantsLayer = NO;
        #endif
        
        
       
        
        MTLTextureDescriptor * depthBufferDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float width:width height:height mipmapped:NO];
        [depthBufferDescriptor setUsage: MTLTextureUsageRenderTarget];
        [depthBufferDescriptor setStorageMode: MTLStorageModePrivate];
        ff.environment.metal.device = device;
       
        sg_setup(ff);
        // a pass action to clear framebuffer to black
        sg_pass_action pass;
        pass.colors[0].clear_value = { 0.0f, 0.0f, 1.0f, 1.0f };
        pass.colors[0].load_action = SG_LOADACTION_CLEAR;
        pass.depth.load_action = SG_LOADACTION_CLEAR;
        pass.depth.clear_value = 1.0f;
        state.pass_action = pass;

        gamePipline = make_game_pipline();
        state.swapchain.color_format = SG_PIXELFORMAT_BGRA8;
        state.swapchain.depth_format = SG_PIXELFORMAT_DEPTH;
        
        state.swapchain.metal.depth_stencil_texture = [device newTextureWithDescriptor: depthBufferDescriptor];
//        state.swapchain.metal.depth_stencil_texture

	}

    void Graphics::beginDraw(uint32_t width, uint32_t height) {
        state.swapchain.height = height;
        state.swapchain.width = width;
        state.swapchain.metal.current_drawable = [metal_layer nextDrawable];
        sg_pass pass{};
        pass.action = state.pass_action;
        pass.swapchain = state.swapchain;


        sg_begin_pass(pass);
        aspectRation = (float)width / (float)height;

    }

    void Graphics::endDraw() {
        sg_end_pass();
        sg_commit();
    }

    void Graphics::drawBoxes(const std::vector<Box>& boxes) {
        // draw Game
    
        glm::mat4 projectionOrthoNO = glm::ortho(-zoom * aspectRation, zoom * aspectRation, -zoom, zoom, 0.001f, 100.0f);
        glm::mat4 camera = glm::lookAt(glm::vec3(12.0f, 16.0f, 12.0f)+ origin, glm::vec3(0.0f)+ origin, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 mvp = projectionOrthoNO * camera;

        sg_apply_pipeline(gamePipline.pip);
        sg_apply_bindings(gamePipline.bind);

        for (const auto& box : boxes) {
            vs_params1_t box_params;
            box_params.mvp = mvp;
            box_params.box_position = box.position;
            box_params.box_size = box.size;
            auto range = SG_RANGE(box_params);
            sg_apply_uniforms(UB_vs_params, &range);
            sg_draw(0, 36, 1);
        }
    }

    void Graphics::setImage(const std::vector<uint8_t>& pixels, uint32_t width, uint32_t height, uint8_t depth) {
        guiPipline = make_gui_pipline(pixels, width, height, depth);
    }
}
