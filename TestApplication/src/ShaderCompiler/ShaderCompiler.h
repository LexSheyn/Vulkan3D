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

		/// <summary>
		/// Preprocess GLSL shader.
		/// </summary>
		/// <param name="sourceName"></param>
		/// <param name="shaderKind"></param>
		/// <param name="source"></param>
		/// <returns>
		/// GLSL shader source text after proprocessing.
		/// </returns>
		std::string PreprocessShader( const char* sourceName, shaderc_shader_kind shaderKind, const std::string& source );

		/// <summary>
		/// Compiles shader to the SPIR-V assembly.
		/// </summary>
		/// <param name="sourceName"></param>
		/// <param name="shaderKind"></param>
		/// <param name="source"></param>
		/// <param name="optimize"></param>
		/// <returns>
		/// Assembly text as a string.
		/// </returns>
		std::string CompileFileToAssembly( const char* sourceName, shaderc_shader_kind shaderKind, const std::string& source, bool8 optimize = false );

		/// <summary>
		/// Compiles shader to the SPIR-V bytecode.
		/// </summary>
		/// <param name="sourceName"></param>
		/// <param name="shaderKind"></param>
		/// <param name="source"></param>
		/// <param name="optimize"></param>
		/// <returns>
		/// Bytecode as a vector of 32-bit unsigned integers.
		/// </returns>
		std::vector<uint32> CompileFile( const char* sourceName, shaderc_shader_kind shaderKind, const std::string& source, bool8 optimize = false );

	private:

	// Variables:

		shaderc::Compiler m_Compiler;

		shaderc::CompileOptions m_Options;
	};
}

#endif // SHADERCOMPILER_H