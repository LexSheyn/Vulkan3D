#ifndef SHADERCOMPILER_H
#define SHADERCOMPILER_H

namespace t3d
{
	class ShaderCompiler
	{
	public:

	// Constructors and Destructor:

		ShaderCompiler();

		~ShaderCompiler();

	// Fumctions:

		std::string LoadShader( const char* filePath );

	//	std::vector<uint32> CompileVertexShader( const char* fileName, const std::string& shaderCode );
	//
	//	std::vector<uint32> CompileFragmentShader( const char* fileName, const std::string& shaderCode );

	private:

	// Variables:

	//	shaderc::Compiler m_Compiler;
	//
	//	shaderc::CompileOptions m_Options;
	};
}

#endif // SHADERCOMPILER_H