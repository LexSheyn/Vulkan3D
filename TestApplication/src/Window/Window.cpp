#include "../PrecompiledHeaders/stdafx.h"
#include "Window.h"

namespace t3d
{
// Constructors and Destructor:

	Window::Window( int32 width, int32 height, const char8* title )
		: m_Width( width ),
		  m_Height( height ),
		  m_Title( title ),
		  m_Window( nullptr )
	{
		if ( glfwInit() )
		{
			LOG_TRACE( "GLFW initialized successfully!" );
		}
		else
		{
			LOG_ERROR( "Failed to initialize GLFW library!" );

			return;
		}

		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
		glfwWindowHint( GLFW_RESIZABLE , GLFW_FALSE  );

		m_Window = glfwCreateWindow( m_Width, m_Height, m_Title, nullptr, nullptr );

		if ( m_Window )
		{
			LOG_TRACE( "GLFW window created successfully!" );
		}
		else
		{
			LOG_ERROR( "Failed to create GLFW window!" );

			return;
		}

		
	}

	Window::~Window()
	{
		glfwTerminate();
	}

// Functions:

	//

}