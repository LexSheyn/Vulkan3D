#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

/// <summary>
/// Short form for Trick3D game engine.
/// </summary>
namespace t3d
{
	/// <summary>
	/// Class that manages all necessarily shader operations: 
	/// ----------------------------------------------------------------------------------------------------------------------
	/// - Load glsl shader from text file;
	/// ----------------------------------------------------------------------------------------------------------------------
	/// - Compile vertex/fragment shader;
	/// ----------------------------------------------------------------------------------------------------------------------
	/// - Save compiled shader to .spv SPIR-V fprmat for Vulkan.
	/// ----------------------------------------------------------------------------------------------------------------------
	/// - Load shader from .spv file.
	/// ----------------------------------------------------------------------------------------------------------------------
	/// </summary>
	class ShaderManager
	{
	public:

	// Constructors and Destructor:

		ShaderManager();

		~ShaderManager();

	// Fumctions:

		std::string LoadGLSL( const char* fileName );

		shaderc::AssemblyCompilationResult CompileVertexShader( const std::string& shaderCode, shaderc_optimization_level optimizationLevel = shaderc_optimization_level_performance );

		shaderc::AssemblyCompilationResult CompileFragmentShader( const std::string& shaderCode, shaderc_optimization_level optimizationLevel = shaderc_optimization_level_performance );

		void SaveToSPV( const char* outputFileName, const std::string& assemblyCode );

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

#endif // SHADERMANAGER_H