#ifndef WINDOW_H
#define WINDOW_H

#include "../Logger/Logger.h"

namespace t3d
{
	class Window
	{
	public:

	// Constructors and Destructor:

		Window( int32 width, int32 height, const char8* title );

		~Window();

	// Functions:

		//

	private:

	// Vaeiables:

		int32 m_Width;
		int32 m_Height;

		const char8* m_Title;

		GLFWwindow* m_Window;
	};
}

#endif // WINDOW_H