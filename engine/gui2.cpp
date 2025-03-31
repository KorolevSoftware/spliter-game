#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/ext/scalar_common.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include "gui2.h"

namespace Engine2 {
	GUIComposer::GUIComposer(uint32_t poolSize) {
        vertexArrayOffset = 0;
        vertexBuffer = new GUIVertex[poolSize];
    }

    glm::vec2 calculateAdjust(const glm::vec2& aspectRation, GUIAdjustMod adjust) {
        float temp;
        switch (adjust) {
        case GUIAdjustMod::Fit:
            temp = glm::fmin(aspectRation.x, aspectRation.y);
            return glm::vec2(temp);

        case GUIAdjustMod::Zoom:
            temp = glm::fmax(aspectRation.x, aspectRation.y);
            return glm::vec2(temp);

        case GUIAdjustMod::Stretch:
            return aspectRation;
        }
    }

    glm::vec2 calculatePivot(GUIPivot pivot, glm::vec2 nodeSize) {
        glm::vec2 nodeOffset = glm::vec2(0, 0);
        glm::vec2 drawDim = nodeSize * 0.5f;

        switch (pivot) {
        case GUIPivot::Centre:
            break;

        case GUIPivot::North:
            nodeOffset.y = -drawDim.y;
            break;

        case GUIPivot::NorthEast:
            nodeOffset.y = -drawDim.y;
            nodeOffset.x = -drawDim.x;
            break;

        case GUIPivot::NorthWest:
            nodeOffset.y = -drawDim.y;
            nodeOffset.x = drawDim.x;
            break;

        case GUIPivot::East:
            nodeOffset.y = 0;
            nodeOffset.x = -drawDim.x;
            break;

        case GUIPivot::South:
            nodeOffset.y = drawDim.y;
            nodeOffset.x = 0;
            break;

        case GUIPivot::SouthEast:
            nodeOffset.y = drawDim.y;
            nodeOffset.x = -drawDim.x;
            break;

        case GUIPivot::SouthWest:
            nodeOffset.y = drawDim.y;
            nodeOffset.x = drawDim.x;
            break;

        case GUIPivot::West:
            nodeOffset.y = 0;
            nodeOffset.x = drawDim.x;
            break;
        }
        return nodeOffset;
    }

    bool GUIComposer::compose(const GUINode& node, const glm::vec2& localResolution, const glm::vec2& actualResolution, const glm::vec2& parentOffset) {
        glm::vec2 aspectRation = actualResolution / localResolution;
        glm::vec2 adjustScale = calculateAdjust(aspectRation, node.adjustMod);

        if (node.anchorX) {
            adjustScale.x = aspectRation.x;
        }

        if (node.anchorY) {
            adjustScale.y = aspectRation.y;
        }

        glm::vec2 offset = (actualResolution - localResolution * adjustScale) * 0.5f; // left offset (is fit offset == 0)
        return composeScreen(node, aspectRation, parentOffset + offset, 0);
    }

    bool GUIComposer::composeScreen(const GUINode& node, const glm::vec2& aspectRation, const glm::vec2& parentOffset, const float parentRotate) {
        glm::vec2 adjustScale = calculateAdjust(aspectRation, node.adjustMod);
        glm::vec2 offsetAdjustScale = adjustScale;

        if (node.anchorX) {
            offsetAdjustScale.x = aspectRation.x;
        }

        if (node.anchorY) {
            offsetAdjustScale.y = aspectRation.y;
        }

        glm::vec2 screenPosition = node.base.position * offsetAdjustScale + parentOffset;
        glm::vec2 pivotOffset = calculatePivot(node.pivot, node.base.size);
        glm::vec2 rPos = screenPosition + pivotOffset * offsetAdjustScale;

        if (node.visable) {            
            int addVertexCount = 0;
            if (node.generator.generator) {
                addVertexCount = node.generator.generator(&vertexBuffer[vertexArrayOffset], &node.base, node.generator.userData);
            }

            const float radians = glm::radians(node.base.angle);
            for (size_t i = vertexArrayOffset; i < vertexArrayOffset + addVertexCount; i++) {
                GUIVertex& vertex = vertexBuffer[i];
                vertex.position = glm::rotateZ(vertex.position, radians);
                vertex.position.x *= adjustScale.x;
                vertex.position.y *= adjustScale.y;
                vertex.position += glm::vec3(rPos.x, rPos.y, 0.5f);
            }
            vertexArrayOffset += addVertexCount;
        }

        for (int i = 0; i < 10 && node.children[i] != nullptr; i++) {
            composeScreen(*node.children[i], adjustScale, screenPosition, node.base.angle + parentRotate);
        }
    
        return true;
	}


    void GUIComposer::clearVertexBuffer() {
        vertexArrayOffset = 0;
        nodesFromScreen.clear();
    }

    const uint8_t* GUIComposer::getBufferData() const {
        return reinterpret_cast<uint8_t const*>(vertexBuffer);
    }

    uint32_t GUIComposer::getVertexCount() const {
        return vertexArrayOffset;
    }

    uint32_t GUIComposer::getRenderBufSizeof() {
        return getVertexCount() *sizeof(GUIVertex);
    }

    uint32_t GUIComposer::pickNode(glm::vec2 pos) {
        for (auto& node : nodesFromScreen) {
            if (node.screen_p2.x > pos.x || pos.x > node.screen_p1.x)
                continue;

            if (node.screen_p2.y > pos.y || pos.y > node.screen_p1.y)
                continue;

            return node.hash;
        }
    }

    GUIVertex::GUIVertex(): position(0.0f, 0.0f, 0.0f), color(0.0f, 0.0f, 0.0f, 0.0f), texCoords(0.0f, 0.0f) {}

    GUIVertex::GUIVertex(glm::vec3 position, glm::vec4 color, glm::vec2 texCoords): position(position), color(color), texCoords(texCoords){
    }
};