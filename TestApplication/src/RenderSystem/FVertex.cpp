#include "../PrecompiledHeaders/stdafx.h"
#include "FVertex.h"

namespace t3d
{
// Constructors:

	FVertex::FVertex()
		: Position(0.0f, 0.0f, 0.0f), Color(1.0f, 1.0f, 1.0f)
	{
	}

	FVertex::FVertex(const glm::vec3& Position)
		: Position(Position), Color(1.0f, 1.0f, 1.0f)
	{
	}
	
	FVertex::FVertex(const glm::vec3& Position, const glm::vec3& Color)
		: Position(Position), Color(Color)
	{
	}

// Static Accessors:

	VkVertexInputBindingDescription FVertex::GetBindingDescription()
	{
		VkVertexInputBindingDescription BindingDescription{};

		BindingDescription.binding   = 0;
		BindingDescription.stride    = sizeof(FVertex);
		BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return BindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 2> FVertex::GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> AttributeDescriptions{};

		AttributeDescriptions[0].binding  = 0;
		AttributeDescriptions[0].location = 0;
		AttributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[0].offset   = offsetof(FVertex, Position);

		AttributeDescriptions[1].binding  = 0;
		AttributeDescriptions[1].location = 1;
		AttributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[1].offset   = offsetof(FVertex, Color);

		return AttributeDescriptions;
	}
}