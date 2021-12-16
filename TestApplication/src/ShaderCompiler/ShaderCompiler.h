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

		std::string LoadGLSL( const char* fileName );

		shaderc::AssemblyCompilationResult CompileVertexShader( const std::string& shaderCode, shaderc_optimization_level optimizationLevel = shaderc_optimization_level_performance );

		shaderc::AssemblyCompilationResult CompileFragmentShader( const std::string& shaderCode, shaderc_optimization_level optimizationLevel = shaderc_optimization_level_performance );

		void SaveToSPV( const char* outputFileName, const std::string& shaderCode );

		std::vector<char8> LoadSPV( const char* fileName );

	// Modifiers:

		void SetDirectoryGLSL( const std::string& directory );

		void SetDirectorySPV( const std::string& directory );

		void SetOutputExtension( const std::string& extension );

	private:

	// Variables:

		std::string m_DirectoryGLSL;
		std::string m_DirectorySPV;

		std::string m_OutputExtension;

		shaderc::Compiler m_Compiler;
	};
}

#endif // SHADERCOMPILER_H