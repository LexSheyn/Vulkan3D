#include "../PrecompiledHeaders/stdafx.h"
#include "ShaderCompiler.h"

// Disable warnings from Vulkan SDK:
#pragma warning( push ) // Vulkan SDK - Begin

namespace t3d
{
// Constructors and Destructor:

	ShaderCompiler::ShaderCompiler()
	{
		m_Options.AddMacroDefinition("MY_DEFINE", "1");
	}

	ShaderCompiler::~ShaderCompiler()
	{
	}


// Functions:

	std::string ShaderCompiler::PreprocessShader( const char* sourceName, shaderc_shader_kind shaderKind, const std::string& source )
	{
		shaderc::PreprocessedSourceCompilationResult compilationResult = m_Compiler.PreprocessGlsl( source, shaderKind, sourceName, m_Options );

		#pragma warning( disable : 26812) // Warning C26812 The enum type 'shaderc_compilation_status' is unscoped.Prefer 'enum class' over 'enum' (Enum.3).

		if ( compilationResult.GetCompilationStatus() != shaderc_compilation_status_success )
		{
			std::cout << compilationResult.GetErrorMessage() << std::endl;

			return std::string();
		}

		return { compilationResult.cbegin(), compilationResult.cend() };
	}

	std::string ShaderCompiler::CompileFileToAssembly( const char* sourceName, shaderc_shader_kind shaderKind, const std::string& source, bool8 optimize )
	{
		if ( optimize )
		{
			m_Options.SetOptimizationLevel( shaderc_optimization_level_size );
		}
		
		shaderc::AssemblyCompilationResult compilationResult = m_Compiler.CompileGlslToSpvAssembly( source, shaderKind, sourceName, m_Options );

		if ( compilationResult.GetCompilationStatus() != shaderc_compilation_status_success )
		{
			std::cout << compilationResult.GetErrorMessage() << std::endl;

			return std::string();
		}
		
		return { compilationResult.cbegin(), compilationResult.cend() };
	}

	std::vector<uint32> ShaderCompiler::CompileFile( const char* sourceName, shaderc_shader_kind shaderKind, const std::string& source, bool8 optimize )
	{
		if ( optimize )
		{
			m_Options.SetOptimizationLevel( shaderc_optimization_level_size );
		}

		shaderc::SpvCompilationResult compilationResult = m_Compiler.CompileGlslToSpv( source, shaderKind, sourceName, m_Options );

		if ( compilationResult.GetCompilationStatus() != shaderc_compilation_status_success )
		{
			std::cout << compilationResult.GetErrorMessage() << std::endl;

			return std::vector<uint32>();
		}

		return { compilationResult.cbegin(), compilationResult.cend() };
	}

}

#pragma warning( pop ) // Vulkan SDK - End