#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include "gui.h"

namespace Engine {
	GUIComposer::GUIComposer(uint32_t poolSize) {
        vertexBuffer.reserve(poolSize);
    }

	bool GUIComposer::compose(const GUINode& node, const glm::vec2& localResolution, const glm::vec2& actualResolution, const glm::vec2& parentOffset) {
        glm::vec2 aspectRation = actualResolution / localResolution;

        glm::vec2 adjustScale;
        float temp;
        switch (node.adjustMod) {
        case GUIAdjustMod::Fit:
            temp = std::min(aspectRation.x, aspectRation.y);
            adjustScale = glm::vec2(temp, temp);
            break;

        case GUIAdjustMod::Zoom:
            temp = std::max(aspectRation.x, aspectRation.y);
            adjustScale = glm::vec2(temp, temp);
            break;

        case GUIAdjustMod::Stretch:
            adjustScale = aspectRation;
            break;
        } // case GUIAdjustMod::Stretch: skip adjustScale correct

        glm::vec2 offsetAdjustScale = adjustScale;
        if (node.anchorX) {
            offsetAdjustScale.x = aspectRation.x;
        }

        if (node.anchorY) {
            offsetAdjustScale.y = aspectRation.y;
        }

        glm::vec2 offset = (actualResolution - localResolution * offsetAdjustScale) * 0.5f;

        if (node.visable) {
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

            glm::vec2 screenPosition = node.position * offsetAdjustScale + offset + parentOffset;
            // gen primitive

            glm::vec2 drawDim = node.size / 2.0f * adjustScale;
            switch (node.pivot) {
            case GUIPivot::Centre:
                p1 = screenPosition + drawDim;
                p2 = screenPosition - drawDim;
                break;

            case GUIPivot::North:
                p1 = screenPosition + (drawDim - glm::vec2(0, drawDim.y));
                p2 = screenPosition - (drawDim - glm::vec2(0, drawDim.y));
                break;

            case GUIPivot::NorthEast:
                p1 = screenPosition;
                p2 = screenPosition - node.size;// TODO fix 
                break;

            case GUIPivot::East:
                p1 = screenPosition + (drawDim - glm::vec2(drawDim.x, 0));
                p2 = screenPosition - (drawDim - glm::vec2(drawDim.x, 0));
                break;

            case GUIPivot::SouthEast:
                p1 = screenPosition + glm::vec2(0, node.size.y);// TODO fix 
                p2 = screenPosition - glm::vec2(node.size.x, 0);// TODO fix 
                break;

            case GUIPivot::South:
                p1 = screenPosition + glm::vec2(drawDim.x, node.size.y);// TODO fix 
                p2 = screenPosition + glm::vec2(-drawDim.x, 0);
                break;

            case GUIPivot::SouthWest:
                p1 = screenPosition + glm::vec2(node.size.x, node.size.y); // TODO fix 
                p2 = screenPosition;
                break;

            case GUIPivot::West:
                p1 = screenPosition + glm::vec2(drawDim.x*2, drawDim.y);
                p2 = screenPosition + glm::vec2(0, -drawDim.y);
                break;

            case GUIPivot::NorthWest:
                p1 = screenPosition + glm::vec2(node.size.x, 0);// TODO fix 
                p2 = screenPosition + glm::vec2(0, -node.size.y);// TODO fix 
                break;
            }

            GUINodeScreen snode;
            snode.hash = node.hash;
            snode.screen_p1 = p1;
            snode.screen_p2 = p2;
            nodesFromScreen.push_back(snode);

            glm::vec3 position1(p2.x, p1.y, 0.5f);
            glm::vec3 position2(p1.x, p1.y, 0.5f);
            glm::vec3 position3(p1.x, p2.y, 0.5f);

            glm::vec3 position4(p2.x, p2.y, 0.5f);
            glm::vec3 position5(p2.x, p1.y, 0.5f);
            glm::vec3 position6(p1.x, p2.y, 0.5f);

            //vertexBuffer.emplace_back(position1, node.color, glm::vec2(0.0f, 1.0f));
            //vertexBuffer.emplace_back(position2, node.color, glm::vec2(1.0f));
            //vertexBuffer.emplace_back(position3, node.color, glm::vec2(1.0f, 0.0f));

            //vertexBuffer.emplace_back(position4, node.color, glm::vec2(0));
            //vertexBuffer.emplace_back(position5, node.color, glm::vec2(0.0f, 1.0f));
            //vertexBuffer.emplace_back(position6, node.color, glm::vec2(1.0f, 0.0f));

            vertexBuffer.emplace_back(position1, node.color, glm::vec2(node.textureCoord1.x, node.textureCoord2.y));
            vertexBuffer.emplace_back(position2, node.color, node.textureCoord2);
            vertexBuffer.emplace_back(position3, node.color, glm::vec2(node.textureCoord2.x, node.textureCoord1.y));
            
            vertexBuffer.emplace_back(position4, node.color, node.textureCoord1);
            vertexBuffer.emplace_back(position5, node.color, glm::vec2(node.textureCoord1.x, node.textureCoord2.y));
            vertexBuffer.emplace_back(position6, node.color, glm::vec2(node.textureCoord2.x, node.textureCoord1.y));
        }
        for (auto& child : node.childs) {
             compose(child, node.size, node.size * adjustScale, offset + (node.position) * adjustScale);
        }
        return true;
	}

    void GUIComposer::clearVertexBuffer() {
        vertexBuffer.clear();
        nodesFromScreen.clear();
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

    uint32_t GUIComposer::pickNode(glm::vec2 pos) {
        for (auto& node : nodesFromScreen) {
            if (node.screen_p2.x > pos.x || pos.x > node.screen_p1.x)
                continue;

            if (node.screen_p2.y > pos.y || pos.y > node.screen_p1.y)
                continue;

            return node.hash;
        }
    }

    GUIVertex::GUIVertex(glm::vec3 position, glm::vec4 color, glm::vec2 texCoords):position(position), color(color), texCoords(texCoords){}

};