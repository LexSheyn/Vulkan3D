#include "../PrecompiledHeaders/stdafx.h"

#include "../Application/Application.h"

int32_t main()
{
	Application application;

	try
	{
		application.Run();
	}
	catch ( const std::exception& exception )
	{
		std::cerr << exception.what() << std::endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}