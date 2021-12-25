#include "../PrecompiledHeaders/stdafx.h"
#include "RenderSystem.h"

#pragma warning( push )
#pragma warning( disable : 26812) // Warning C26812 The enum type 'VkResult' is unscoped.Prefer 'enum class' over 'enum' (Enum.3).

namespace t3d
{
// Constructors and Destructor:

	RenderSystem::RenderSystem()
		: m_Instance( VK_NULL_HANDLE ),
		  m_DebugMessenger( VK_NULL_HANDLE ),
		  m_Surface( VK_NULL_HANDLE )
	{
		#ifdef NDEBUG

			b_ValidationLayersEnabled = false;

		#else

			b_ValidationLayersEnabled = true;

		#endif
	}

	RenderSystem::~RenderSystem()
	{
	}


// Functions:

	VkResult RenderSystem::InitVulkan( GLFWwindow* window )
	{
		VkResult result = VK_SUCCESS;

	// Create Vulkan instance:

		result = this->CreateInstance( m_Instance );

		if ( result != VK_SUCCESS )
		{
			std::cout << "ERROR::RenderSystem::InitVulkan::CreateInstance: Failed to create instance!" << std::endl;

			return result;
		}

	// Setup debug messenger if the validation layers are enabled:

		if ( b_ValidationLayersEnabled )
		{
			result = this->SetupDebugMessenger( m_Instance, m_DebugMessenger );

			if ( result != VK_SUCCESS )
			{
				std::cout << "ERROR::RenderSystem::InitVulkan::SetupDebugMessenger: Failed to setup debug messenger!" << std::endl;

				return result;
			}
		}

	// Create window surface to draw on:

		result = this->CreateSurface( m_Instance, window, nullptr, &m_Surface );

		if ( result != VK_SUCCESS )
		{
			std::cout << "ERROR::RenderSystem::InitVulkan::CreateSurface: Failed to create surface!" << std::endl;

			return result;
		}

		return result;
	}


// Private Functions:

	bool8 RenderSystem::CheckValidationLayerSupport()
	{
	// Check for all requested layers are available:
	
		uint32_t layerCount;

		vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
	
		std::vector<VkLayerProperties> availableLayers( layerCount );
	
		vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );
	
	// Check if all of the layers are exist in the availableLayers:
	
		for ( const char* layerName : m_ValidationLayers )
		{
			bool layerFound = false;
	
			for ( const auto& layerProperties : availableLayers )
			{
				if ( std::strcmp( layerName, layerProperties.layerName ) == 0 )
				{
					layerFound = true;
	
					break;
				}
			}
	
			if ( !layerFound )
			{
				return false;
			}
		}
	
		return true;
	}

	std::vector<const char*> RenderSystem::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0u;

		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

		std::vector<const char*> extensions( glfwExtensions, glfwExtensions + glfwExtensionCount );

		if ( b_ValidationLayersEnabled )
		{
			extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
		}

		return extensions;
	}

	void RenderSystem::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo)
	{
		debugMessengerCreateInfo = {};

		debugMessengerCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugMessengerCreateInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugMessengerCreateInfo.pfnUserCallback = RenderSystem::DebugCallback;
		debugMessengerCreateInfo.pUserData       = nullptr; // Optional.
	}

	VkResult RenderSystem::SetupDebugMessenger( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger )
	{
		VkResult result = VK_SUCCESS;

		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};

		this->PopulateDebugMessengerCreateInfo( debugMessengerCreateInfo );

		result = RenderSystem::CreateDebugUtilsMessengerEXT( instance, &debugMessengerCreateInfo, nullptr, &debugMessenger );

		return result;
	}

	VkResult RenderSystem::CreateInstance(VkInstance instance)
	{
		VkResult result = VK_SUCCESS;

	// Check validation layers support:

		#pragma warning( disable : 6237) // Warning C6237(<zero> && <expression>) is always zero. <expression> is never evaluatedand might have side effects.
	
		if ( b_ValidationLayersEnabled && !this->CheckValidationLayerSupport() ) // C6237
		{
			throw std::runtime_error( "ERROR::RenderSystem::CreateInstance: Validation layers requested, but not available!" );
		}
	
	// Application info:
	
		VkApplicationInfo applicationInfo{};
	
		applicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName   = "Test Application";
		applicationInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
		applicationInfo.pEngineName        = "Trick 3D Engine";
		applicationInfo.engineVersion      = VK_MAKE_VERSION( 1, 0, 0 );
		applicationInfo.apiVersion         = VK_API_VERSION_1_0;
	
	// Create info:
	
		std::vector<const char*> glfwExtensions = this->GetRequiredExtensions();
	
		VkInstanceCreateInfo instanceCreateInfo{};
	
		instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo        = &applicationInfo;
		instanceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>( glfwExtensions.size() );
		instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions.data();
		
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
	
		if ( b_ValidationLayersEnabled )
		{
			this->PopulateDebugMessengerCreateInfo( debugMessengerCreateInfo );
	
			instanceCreateInfo.enabledLayerCount   = static_cast<uint32_t>( m_ValidationLayers.size() );
			instanceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
			instanceCreateInfo.pNext               = ( VkDebugUtilsMessengerCreateInfoEXT* )&debugMessengerCreateInfo;
		}
		else
		{
			instanceCreateInfo.enabledLayerCount   = 0;
			instanceCreateInfo.pNext               = nullptr;
		}
	
	// Create instance:
	
		result = vkCreateInstance( &instanceCreateInfo, nullptr, &instance );
	
	// Check for extensions support:
	
		uint32_t extensionCount = 0u;
	
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );
	
		std::vector<VkExtensionProperties> extensions( extensionCount );
	
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensions.data() );
	
		std::cout << "\n";
		std::cout << "\tAvailable extensions" << " [" << extensionCount << "]: " << "\n\n";
	
		for ( size_t i = 0u; i < extensions.size(); i++ )
		{
			std::cout << "\t" << ( i + 1u ) << ".\t" << extensions[i].extensionName << " [version " << extensions[i].specVersion << "]" << "\n\n";
		}
	
		std::cout << std::endl;

		return result;
	}

	VkResult RenderSystem::CreateSurface(VkInstance instance, GLFWwindow* pWindow, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface)
	{
		return glfwCreateWindowSurface( instance, pWindow, pAllocator, pSurface );
	}


// Callback Functions:

	VKAPI_ATTR VkBool32 VKAPI_CALL RenderSystem::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messgeType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

		if ( messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
		{
			// Message is important enough to show.
		}

		return VK_FALSE;
	}


// Proxy Functions:

	VkResult RenderSystem::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		PFN_vkCreateDebugUtilsMessengerEXT debugFunction = ( PFN_vkCreateDebugUtilsMessengerEXT )vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );

		if ( debugFunction != nullptr  )
		{
			#pragma warning( disable : 26812) // Warning C26812 The enum type 'VkResult' is unscoped.Prefer 'enum class' over 'enum' (Enum.3).

			return debugFunction( instance, pCreateInfo, pAllocator, pDebugMessenger );
		}

		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void RenderSystem::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT debugFunction = ( PFN_vkDestroyDebugUtilsMessengerEXT )vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );

		if ( debugFunction != nullptr )
		{
			debugFunction( instance, debugMessenger, pAllocator );
		}
	}

}

#pragma warning( pop )