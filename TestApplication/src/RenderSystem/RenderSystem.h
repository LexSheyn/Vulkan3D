#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

namespace t3d
{
	class RenderSystem
	{
	public:

	// Constructors and Destructor:

		RenderSystem();

		~RenderSystem();

	// Functions:

		VkResult InitVulkan( GLFWwindow* window );

	// Private Functions:

		bool8 CheckValidationLayerSupport();

		std::vector<const char*> GetRequiredExtensions();

		void PopulateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo );

		VkResult SetupDebugMessenger( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger );

		VkResult CreateInstance( VkInstance instance );

		VkResult CreateSurface( VkInstance instance, GLFWwindow* pWindow, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface );

	// Callback Functions:

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messgeType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData );

	// Proxy Functions:

		static VkResult CreateDebugUtilsMessengerEXT( VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger );

		static void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator );

	private:

	// Variables:

		const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

		bool8 b_ValidationLayersEnabled;

		VkInstance m_Instance;
		
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		VkSurfaceKHR m_Surface;
	};
}

#endif // RENDERSYSTEM_H