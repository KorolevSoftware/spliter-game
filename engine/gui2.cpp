#include "gui2.h"

namespace Engine {
	GUINodeID GUINodeInvalidID = std::numeric_limits<GUINodeID>::max();

	GUIComposer::GUIComposer(uint32_t vertexPoolSize, uint32_t nodePoolSize) {
		vertexArrayOffset = 0;
		vertexBuffer = new GUIVertex[vertexPoolSize];
		nodePoolBuffer.reserve(nodePoolSize);
	}

	vec2 calculateAdjust(const vec2& aspectRation, GUIAdjustMode adjust) {
		float temp;
		switch (adjust) {
		case GUIAdjustMode::Fit:
			temp = fmin(aspectRation.x, aspectRation.y);
			return vec2(temp);

		case GUIAdjustMode::Zoom:
			temp = fmax(aspectRation.x, aspectRation.y);
			return vec2(temp);

		case GUIAdjustMode::Stretch:
			return aspectRation;
		}
	}

	vec3 calculatePivot(GUIPivot pivot, const vec2& nodeSize) {
		vec3 nodeOffset(0.0f);
		vec2 drawDim = nodeSize * 0.5f;

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

	void GUIComposer::compose(GUINodeID node, const vec2& localResolution, const vec2& actualResolution, const vec2& parentOffset) {
		vertexArrayOffset = 0;
		vec2 aspectRation = actualResolution / localResolution;
		return composeScreen(node, aspectRation);
	}

	void GUIComposer::composeScreen(GUINodeID node, const vec2& parentAspectRation) {
		GUINode& nodeRef = nodePoolBuffer[node];
		const vec2 adjustScale = calculateAdjust(parentAspectRation, nodeRef.adjustMode);
		vec2 offsetAdjustScale = adjustScale;

		if (nodeRef.anchorX) {
			offsetAdjustScale.x = parentAspectRation.x;
		}

		if (nodeRef.anchorY) {
			offsetAdjustScale.y = parentAspectRation.y;
		}

		const vec3 pivotOffset = calculatePivot(nodeRef.pivot, nodeRef.base.size);
		const vec3 resultPosition = (pivotOffset + nodeRef.position) * vec3(offsetAdjustScale, 1.0f);
		const vec3 resultScale = nodeRef.scale * vec3(adjustScale, 1.0);
		nodeRef.localTransform =
			translation(resultPosition)
			* rotationZ(nodeRef.angleZ)
			* scaling(resultScale);

		nodeRef.invAdjustScale = 1.0f / adjustScale;

		if (nodeRef.parent != GUINodeInvalidID) {
			const GUINode& nodeParent = nodePoolBuffer[nodeRef.parent];
			nodeRef.localTransform =
				nodeParent.localTransform
				* scaling(nodeRef.invAdjustScale.x, nodeRef.invAdjustScale.y, 1.0f)
				* nodeRef.localTransform;
		}

		if (nodeRef.visable) {
			[[likely]]
			int addVertexCount = 0;
			if (nodeRef.primitive.generator) {
				addVertexCount = nodeRef.primitive.generator(&vertexBuffer[vertexArrayOffset], &nodeRef.base, nodeRef.primitive.userData);
			}

			for (auto i = vertexArrayOffset; i < vertexArrayOffset + addVertexCount; i++) {
				GUIVertex& vertex = vertexBuffer[i];
				vertex.position = resize_vec<3>(nodeRef.localTransform * vec4(vertex.position, 1.0f));
			}
			vertexArrayOffset += addVertexCount;
		}

		for (const GUINodeID& id : nodeRef.children) {
			composeScreen(id, adjustScale);
		}
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

	bool GUIComposer::pickNode(GUINodeID node, const vec2& pos) const {
		const GUINode& nodeRef = nodePoolBuffer[node];
		mat4 inv = inverse(nodeRef.localTransform);
		mat4 check = nodeRef.localTransform * inv;
		vec2 localPosition = resize_vec<2>(inv * vec4(pos.x, pos.y, 0.0f, 1.0f));
		return nodeRef.primitive.input(localPosition, &nodeRef.base, nodeRef.primitive.userData);
	}

	GUINodeID GUIComposer::makeNode() {
		nodePoolBuffer.emplace_back();
		return nodePoolBuffer.size() - 1;
	}

	GUINodeID GUIComposer::findNode(uint32_t hash) const {
		auto result = std::find_if(std::begin(nodePoolBuffer), std::end(nodePoolBuffer), [hash](const GUINode& node) {
			return node.hash == hash;
			});

		auto index = std::distance(nodePoolBuffer.begin(), result);
		if (index == nodePoolBuffer.size()) {
			return GUINodeInvalidID;
		}
		return index;
	}

	void GUIComposer::setChildren(GUINodeID parent, GUINodeID node) {
		if (nodePoolBuffer[node].parent != GUINodeInvalidID) {
			// TODO remove from old parent
		}
		nodePoolBuffer[node].parent = parent;
		nodePoolBuffer[parent].children.push_back(node);
	}

	bool GUIComposer::isValide(GUINodeID node) const {
		return node != GUINodeInvalidID;
	}

	void GUIComposer::setAdjustMode(GUINodeID node, GUIAdjustMode mode) {
		nodePoolBuffer[node].adjustMode = mode;
	}

	void GUIComposer::setPrimitive(GUIPrimitive primitive, GUINodeID node) {
		nodePoolBuffer[node].primitive = primitive;
	}

	void GUIComposer::setScale(GUINodeID node, vec3 scale) {
		nodePoolBuffer[node].scale = scale;
	}

	void GUIComposer::setPosition(GUINodeID node, vec3 position) {
		nodePoolBuffer[node].position = position;
	}

	void GUIComposer::setRotationZ(GUINodeID node, float angle) {
		nodePoolBuffer[node].angleZ = angle;
	}

	void GUIComposer::setSize(GUINodeID node, vec2 size) {
		nodePoolBuffer[node].base.size = size;
	}

	void GUIComposer::setColor(GUINodeID node, vec4 color) {
		nodePoolBuffer[node].base.color = color;
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

	GUINodeID GUIComposer::getHesh(GUINodeID node) const {
		return nodePoolBuffer[node].hash;
	}

	GUIPivot GUIComposer::getPivot(GUINodeID node) const {
		return nodePoolBuffer[node].pivot;
	}

	bool GUIComposer::getAnchorY(GUINodeID node) const {
		return nodePoolBuffer[node].anchorY;
	}

	bool GUIComposer::getAnchorX(GUINodeID node) const {
		return nodePoolBuffer[node].anchorX;
	}

	vec4 GUIComposer::setColor(GUINodeID node) const {
		return nodePoolBuffer[node].base.color;
	}

	vec2 GUIComposer::getSize(GUINodeID node) const {
		return nodePoolBuffer[node].base.size;
	}

	float GUIComposer::getRotationZ(GUINodeID node) const {
		return nodePoolBuffer[node].angleZ;
	}

	vec3 GUIComposer::getPosition(GUINodeID node) const {
		return nodePoolBuffer[node].position;
	}

	vec3 GUIComposer::getScale(GUINodeID node) const {
		return nodePoolBuffer[node].scale;
	}

	GUIAdjustMode GUIComposer::getAdjustMode(GUINodeID node) const {
		return nodePoolBuffer[node].adjustMode;
	}

	uint32_t GUIComposer::getChildrenCount(GUINodeID node) const {
		const GUINode& rNode = nodePoolBuffer[node];
		return rNode.children.size();
	}

	GUINodeID* GUIComposer::getChildrens(GUINodeID node, uint32_t* size) const {
		const GUINode& rNode = nodePoolBuffer[node];

		*size = rNode.children.size();
		GUINodeID* childrens = new GUINodeID[*size];

		uint32_t index = 0;
		for(auto childrenNode: rNode.children) {
			childrens[index] = childrenNode;
			index++;
		}
		return childrens;
	}

	GUIVertex::GUIVertex() {}

	GUIVertex::GUIVertex(vec3 position, vec4 color, vec2 texCoords) : position(position), color(color), texCoords(texCoords) {}

	GUINode::GUINode() : parent(GUINodeInvalidID) {
		this->base.size = vec2(100.0f);
		this->base.color = vec4(1.0f);
		this->scale = vec3(1.0f);
		this->localTransform = mat4();
		this->adjustMode = GUIAdjustMode::Fit;
		this->anchorX = false;
		this->anchorY = false;
		this->hash = 0;
		this->pivot = GUIPivot::Centre;
		this->angleZ = 0;
		this->invAdjustScale = vec2();
		this->position = vec3(0.0f);
		this->primitive = {};
	}

	float Engine::radians(float angle) {
		return angle * (3.14 / 180.0f);
	}
};