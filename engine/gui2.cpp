#include <glm/ext/scalar_common.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include "gui2.h"


namespace Engine {
	GUINodeID GUINodeInvalidID = std::numeric_limits<GUINodeID>::max();

	GUIComposer::GUIComposer(uint32_t vertexPoolSize, uint32_t nodePoolSize) {
		vertexArrayOffset = 0;
		vertexBuffer = new GUIVertex[vertexPoolSize];
		nodePoolBuffer.reserve(nodePoolSize);
	}

	glm::vec2 calculateAdjust(const glm::vec2& aspectRation, GUIAdjustMode adjust) {
		float temp;
		switch (adjust) {
		case GUIAdjustMode::Fit:
			temp = glm::fmin(aspectRation.x, aspectRation.y);
			return glm::vec2(temp);

		case GUIAdjustMode::Zoom:
			temp = glm::fmax(aspectRation.x, aspectRation.y);
			return glm::vec2(temp);

		case GUIAdjustMode::Stretch:
			return aspectRation;
		}
	}

	glm::vec3 calculatePivot(GUIPivot pivot, const glm::vec2& nodeSize) {
		glm::vec3 nodeOffset = glm::vec3(0);
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

	void GUIComposer::compose(GUINodeID node, const glm::vec2& localResolution, const glm::vec2& actualResolution, const glm::vec2& parentOffset) {
		glm::vec2 aspectRation = actualResolution / localResolution;
		return composeScreen(node, aspectRation);
	}

	void GUIComposer::composeScreen(GUINodeID node, const glm::vec2& parentAspectRation) {
		GUINode& nodeRef = nodePoolBuffer[node];
		const glm::vec2 adjustScale = calculateAdjust(parentAspectRation, nodeRef.adjustMode);
		glm::vec2 offsetAdjustScale = adjustScale;

		if (nodeRef.anchorX) {
			offsetAdjustScale.x = parentAspectRation.x;
		}

		if (nodeRef.anchorY) {
			offsetAdjustScale.y = parentAspectRation.y;
		}

		const glm::vec3 pivotOffset = calculatePivot(nodeRef.pivot, nodeRef.base.size);
		const glm::vec3 resultPosition = (pivotOffset + nodeRef.position) * glm::vec3(offsetAdjustScale, 1.0f);
		const glm::vec3 resultScale = nodeRef.scale * glm::vec3(adjustScale, 1.0);
		nodeRef.localTransform =
			glm::translate(resultPosition)
			* glm::rotate(nodeRef.angleZ, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(resultScale);

		nodeRef.invTransform = glm::scale(glm::vec3(1.0f / adjustScale, 1.0));

		if (nodeRef.parent != GUINodeInvalidID) {
			const GUINode& nodeParent = nodePoolBuffer[nodeRef.parent];
			nodeRef.localTransform = nodeParent.localTransform * nodeParent.invTransform * nodeRef.localTransform;
		}


		if (nodeRef.visable) {
			[[likely]]
			int addVertexCount = 0;
			if (nodeRef.primitive.generator) {
				addVertexCount = nodeRef.primitive.generator(&vertexBuffer[vertexArrayOffset], &nodeRef.base, nodeRef.primitive.userData);
			}

			for (auto i = vertexArrayOffset; i < vertexArrayOffset + addVertexCount; i++) {
				GUIVertex& vertex = vertexBuffer[i];
				vertex.position = nodeRef.localTransform * glm::vec4(vertex.position, 1.0);
			}
			vertexArrayOffset += addVertexCount;
		}

		for (const GUINodeID& id : nodeRef.children) {
			composeScreen(id, adjustScale);
		}
	}


	void GUIComposer::clearVertexBuffer() {
		vertexArrayOffset = 0;
	}

	const uint8_t* GUIComposer::getBufferData() const {
		return reinterpret_cast<uint8_t const*>(vertexBuffer);
	}

	uint32_t GUIComposer::getVertexCount() const {
		return vertexArrayOffset;
	}

	uint32_t GUIComposer::getRenderBufSizeof() const {
		return getVertexCount() * sizeof(GUIVertex);
	}
	bool GUIComposer::pickNode(GUINodeID node, const glm::vec2& pos) const {
		const GUINode& nodeRef = nodePoolBuffer[node];
		glm::mat4 inv = glm::inverse(nodeRef.localTransform);
		glm::vec2 localPosition = inv * glm::vec4(pos, 0.0f, 1.0f);
		return nodeRef.primitive.input(localPosition, &nodeRef.base, nodeRef.primitive.userData);
	}

	GUINodeID GUIComposer::makeNode() {
		nodePoolBuffer.emplace_back();
		return nodePoolBuffer.size() - 1;
	}

	void GUIComposer::setAdjustMode(GUINodeID node, GUIAdjustMode mode) {
		nodePoolBuffer[node].adjustMode = mode;
	}

	void GUIComposer::setPrimitive(GUIPrimitive primitive, GUINodeID node) {
		nodePoolBuffer[node].primitive = primitive;
	}

	void GUIComposer::setScale(GUINodeID node, glm::vec3 scale) {
		nodePoolBuffer[node].scale = scale;
	}

	void GUIComposer::setPosition(GUINodeID node, glm::vec3 position) {
		nodePoolBuffer[node].position = position;
	}

	void GUIComposer::setRotationZ(GUINodeID node, float angle) {
		nodePoolBuffer[node].angleZ = angle;
	}

	void GUIComposer::setSize(GUINodeID node, glm::vec2 size) {
		nodePoolBuffer[node].base.size = size;
	}

	void GUIComposer::setColor(GUINodeID node, glm::vec4 color) {
		nodePoolBuffer[node].base.color = color;
	}

	void GUIComposer::setChildren(GUINodeID parent, GUINodeID node) {
		nodePoolBuffer[node].parent = parent;
		nodePoolBuffer[parent].children.push_back(node);
	}

	void GUIComposer::setAnchorY(GUINodeID node, bool isEnable) {
		nodePoolBuffer[node].anchorY = isEnable;
	}

	void GUIComposer::setAnchorX(GUINodeID node, bool isEnable) {
		nodePoolBuffer[node].anchorX = isEnable;
	}

	void GUIComposer::setPivot(GUINodeID node, GUIPivot pivot) {
		nodePoolBuffer[node].pivot = pivot;
	}

	void GUIComposer::setHesh(GUINodeID node, uint32_t hash) {
		nodePoolBuffer[node].hash = hash;
	}

	GUINodeID GUIComposer::findNode(uint32_t hash) const {
		auto result = std::find_if(std::begin(nodePoolBuffer), std::end(nodePoolBuffer), [hash](const GUINode& node) {
			return node.hash == hash;
			});

		size_t index = std::distance(nodePoolBuffer.begin(), result);
		if (index == nodePoolBuffer.size()) {
			return GUINodeInvalidID;
		}
		return index;
	}

	bool GUIComposer::isValide(GUINodeID node) const {
		return node != GUINodeInvalidID;
	}

	GUIVertex::GUIVertex() : position(0.0f, 0.0f, 0.0f), color(0.0f, 0.0f, 0.0f, 0.0f), texCoords(0.0f, 0.0f) {}

	GUIVertex::GUIVertex(glm::vec3 position, glm::vec4 color, glm::vec2 texCoords) : position(position), color(color), texCoords(texCoords) {}

	GUINode::GUINode() : parent(GUINodeInvalidID) {
		this->base.size = glm::vec2(100.0f);
		this->base.color = glm::vec4(1.0f);
		this->scale = glm::vec3(1.0f);
		this->localTransform = glm::mat4(1.0f);
		this->adjustMode = GUIAdjustMode::Fit;
		this->anchorX = false;
		this->anchorY = false;
		this->hash = 0;
		this->pivot = GUIPivot::Centre;
		this->angleZ = 0;
		this->invTransform = glm::mat4(1.0f);
		this->position = glm::vec4(0);
		this->primitive = {};
	}
};