#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include "gui.h"

namespace Engine {
	GUIComposer::GUIComposer(uint32_t poolSize) {
        vertexBuffer.reserve(poolSize);
    }

	bool GUIComposer::compose(const GUINode& node, const glm::vec2& localResolution, const glm::vec2& actualResolution, const glm::vec2& parentOffset) {
        glm::vec2 adjustScale = actualResolution / localResolution;

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

        glm::vec2 offset = (actualResolution - localResolution * adjustScale) * 0.5f;

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
            p1 = node.position + glm::vec2(node.size.x, 0);
            p2 = node.position + glm::vec2(0, -node.size.y);
            break;
        }

        if (node.anchorX) {
            offset.x = 0;
        }

        if (node.anchorY) {
            offset.y = 0;
        }

        offset += parentOffset;

        if (node.adjustMod == GUIAdjustMod::Stretch) {
            p1 *= adjustScale;
            p2 *= adjustScale;
        } else {
            p1 = offset + p1 * adjustScale;
            p2 = offset + p2 * adjustScale;
        }

        glm::vec3 position1(p2.x, p1.y, 0.5f);
        glm::vec3 position2(p1.x, p1.y, 0.5f);
        glm::vec3 position3(p1.x, p2.y, 0.5f);

        glm::vec3 position4(p2.x, p2.y, 0.5f);
        glm::vec3 position5(p2.x, p1.y, 0.5f);
        glm::vec3 position6(p1.x, p2.y, 0.5f);

        vertexBuffer.emplace_back(position1, node.color, glm::vec2(0));
        vertexBuffer.emplace_back(position2, node.color, glm::vec2(0));
        vertexBuffer.emplace_back(position3, node.color, glm::vec2(0));

        vertexBuffer.emplace_back(position4, node.color, glm::vec2(0));
        vertexBuffer.emplace_back(position5, node.color, glm::vec2(0));
        vertexBuffer.emplace_back(position6, node.color, glm::vec2(0));

        for (auto& child : node.childs) {
            return compose(child, node.size, node.size * adjustScale, offset + (node.position) * adjustScale);
        }
        return true;
	}

    void GUIComposer::clearVertexBuffer() {
        vertexBuffer.clear();
    }

    const uint8_t* GUIComposer::getBufferData() const {
        return reinterpret_cast<uint8_t const*>(vertexBuffer.data());
    }

    uint32_t GUIComposer::getVertexCount() const {
        return vertexBuffer.size();
    }

    uint32_t GUIComposer::getRenderBufSizeof() {
        return vertexBuffer.size()*sizeof(GUIVertex);
    }

    GUIVertex::GUIVertex(glm::vec3 position, glm::vec4 color, glm::vec2 texCoords):position(position), color(color), texCoords(texCoords){}

};