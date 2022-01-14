#ifndef FVERTEX_H
#define FVERTEX_H

namespace t3d
{
	class FVertex
	{
	public:

	// Constructors:

		FVertex();
		FVertex(const glm::vec3& Position);
		FVertex(const glm::vec3& Position, const glm::vec3& Color);

	// Static Accessors:

		static VkVertexInputBindingDescription GetBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();

	// Components:

		glm::vec3 Position;

		glm::vec3 Color;
	};
}

#endif // FVERTEX_H