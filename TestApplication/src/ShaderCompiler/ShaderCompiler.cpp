#include "../PrecompiledHeaders/stdafx.h"
#include "ShaderCompiler.h"

// Disable warnings from Vulkan SDK:
#pragma warning( push ) // Vulkan SDK - Begin

namespace t3d
{
// Constructors and Destructor:

	ShaderCompiler::ShaderCompiler()
	{
    //    m_Options.AddMacroDefinition("MY_DEFINE", "1"); // What is that???
    //
    //    m_Options.SetOptimizationLevel(shaderc_optimization_level_size);
	}

	ShaderCompiler::~ShaderCompiler()
	{
	}


// Functions:

    std::string ShaderCompiler::LoadShader( const char* filePath )
    {
        std::string shaderCode;
        std::string line;

        shaderCode.reserve( 1000ui64 );

        std::ifstream inFile;

        inFile.open( filePath );

        if ( inFile.is_open() )
        {
            while ( std::getline( inFile, line ) )
            {
                shaderCode += line + '\n';
            }
        }
        else
        {
            std::cout << "ERROR::ShaderCompiler::LoadVertexShader: Failed to open file " << filePath << std::endl;

            return std::string();
        }

        inFile.close();

        shaderCode.shrink_to_fit();

        return shaderCode;
    }

//    std::vector<uint32> ShaderCompiler::CompileVertexShader( const char* fileName, const std::string& shaderCode )
//    {
//        shaderc::SpvCompilationResult compilationResult = m_Compiler.CompileGlslToSpv( shaderCode, shaderc_glsl_vertex_shader, fileName, m_Options );
//
//        return { compilationResult.cbegin(), compilationResult.cend() };
//    }
//
//    std::vector<uint32> ShaderCompiler::CompileFragmentShader( const char* fileName, const std::string& shaderCode )
//    {
//        shaderc::SpvCompilationResult compilationResult = m_Compiler.CompileGlslToSpv( shaderCode, shaderc_glsl_fragment_shader, fileName, m_Options );
//
//        return { compilationResult.cbegin(), compilationResult.cend() };
//    }

}
/*
#pragma warning( pop ) // Vulkan SDK - End

// Copyright 2016 The Shaderc Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The program demonstrates basic shader compilation using the Shaderc C++ API.
// For clarity, each method is deliberately self-contained.
//
// Techniques demonstrated:
//  - Preprocessing GLSL source text
//  - Compiling a shader to SPIR-V assembly text
//  - Compliing a shader to a SPIR-V binary module
//  - Performing optimization with compilation
//  - Setting basic options: setting a preprocessor symbol.
//  - Checking compilation status and extracting an error message.

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <shaderc/shaderc.hpp>

// Returns GLSL shader source text after preprocessing.
std::string preprocess_shader(const std::string& source_name,
                              shaderc_shader_kind kind,
                              const std::string& source)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    options.AddMacroDefinition("MY_DEFINE", "1");

    shaderc::PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl( source, kind, source_name.c_str(), options );

    return { result.cbegin(), result.cend() };
}

// Compiles a shader to SPIR-V assembly. Returns the assembly text
// as a string.
std::string compile_file_to_assembly(const std::string& source_name,
                                     shaderc_shader_kind kind,
                                     const std::string& source,
                                     bool optimize = false)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    options.AddMacroDefinition("MY_DEFINE", "1");
    
    if (optimize)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_size);
    }

    shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly( source, kind, source_name.c_str(), options );

    return { result.cbegin(), result.cend() };
}

// Compiles a shader to a SPIR-V binary. Returns the binary as
// a vector of 32-bit words.
std::vector<uint32_t> compile_file(const std::string& source_name,
                                   shaderc_shader_kind kind,
                                   const std::string& source,
                                   bool optimize = false)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    options.AddMacroDefinition("MY_DEFINE", "1");

    if (optimize)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_size);
    }

    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv( source, kind, source_name.c_str(), options );

    return { module.cbegin(), module.cend() };
}

int main()
{
    const char kShaderSource[] = "#version 310 es\n" "void main() { int x = MY_DEFINE; }\n";

    {
    // Preprocessing:

        std::string preprocessed = preprocess_shader( "shader_src", shaderc_glsl_vertex_shader, kShaderSource );
    }

    {
    // Compiling:

        std::string assembly = compile_file_to_assembly( "shader_src", shaderc_glsl_vertex_shader, kShaderSource );

        std::vector<uint32_t> spirv = compile_file( "shader_src", shaderc_glsl_vertex_shader, kShaderSource );
    }

    {
    // Compiling with optimizing:

        std::string assembly = compile_file_to_assembly( "shader_src", shaderc_glsl_vertex_shader, kShaderSource, true);

        std::vector<uint32_t> spirv = compile_file( "shader_src", shaderc_glsl_vertex_shader, kShaderSource, true);
    }

    {
    // Compile using the C API:

    // The first example has a compilation problem. The second does not.

        const char source[2][80] = { "void main() {}", "#version 450\nvoid main() {}" };

        shaderc_compiler_t compiler = shaderc_compiler_initialize();

        for ( int i = 0; i < 2; i++ )
        {
            shaderc_compilation_result_t result = shaderc_compile_into_spv( compiler, source[i], std::strlen( source[i] ), shaderc_glsl_vertex_shader, "main.vert", "main", nullptr);

            shaderc_compilation_status status = shaderc_result_get_compilation_status( result );

            shaderc_result_release( result );
        }

        shaderc_compiler_release( compiler );
    }

    return 0;
}
*/