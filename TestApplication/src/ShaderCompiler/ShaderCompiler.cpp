#include "../PrecompiledHeaders/stdafx.h"
#include "ShaderCompiler.h"

// Disable warnings from Vulkan SDK:
#pragma warning( push ) // Vulkan SDK - Begin

namespace t3d
{
    // Constructors and Destructor:

    ShaderCompiler::ShaderCompiler()
    {
        m_DirectoryGLSL.reserve(MAX_PATH);  // 260
        m_DirectoryGLSL = "../Shaders/";

        m_DirectorySPV.reserve(MAX_PATH); // 260
        m_DirectorySPV = "../CompiledShaders/";

        m_OutputExtension.reserve(4ui64);    // 4
        m_OutputExtension = ".spv";
    }

    ShaderCompiler::~ShaderCompiler()
    {
    }


    // Functions:

    std::string ShaderCompiler::LoadGLSL( const char* fileName )
    {
        std::string shaderCode;
        std::string line;

        std::string filePath = m_DirectoryGLSL + fileName;

        shaderCode.reserve( 10000ui64 );

        std::ifstream inFile;

        inFile.open( filePath );

        if ( inFile.is_open() )
        {
            while ( std::getline(inFile, line) )
            {
                shaderCode += line;

                if ( !inFile.eof() )
                {
                    shaderCode += '\n';
                }
            }
        }
        else
        {
            std::cout << "ERROR::ShaderCompiler::LoadGLSL: Failed to open file " << filePath << std::endl;

            return std::string();
        }

        inFile.close();

        shaderCode.shrink_to_fit();

        return shaderCode;
    }

    shaderc::AssemblyCompilationResult ShaderCompiler::CompileVertexShader( const std::string& shaderCode, shaderc_optimization_level optimizationLevel )
    {
        shaderc::CompileOptions options;

        // Like -DMY_DEFINE=1
        options.AddMacroDefinition("MY_DEFINE", "1");

        options.SetOptimizationLevel( optimizationLevel );

        shaderc::AssemblyCompilationResult compilationResult = m_Compiler.CompileGlslToSpvAssembly( shaderCode, shaderc_glsl_vertex_shader, "test", options );

        return compilationResult;
    }

    shaderc::AssemblyCompilationResult ShaderCompiler::CompileFragmentShader( const std::string& shaderCode, shaderc_optimization_level optimizationLevel )
    {
        shaderc::CompileOptions options;

        // Like -DMY_DEFINE=1
        options.AddMacroDefinition("MY_DEFINE", "1");

        options.SetOptimizationLevel( optimizationLevel );

        shaderc::AssemblyCompilationResult compilationResult = m_Compiler.CompileGlslToSpvAssembly( shaderCode, shaderc_glsl_vertex_shader, "test", options );

        return compilationResult;
    }

    void ShaderCompiler::SaveToSPV( const char* outputFileName, const std::string& assemblyCode )
    {
        std::string filePath = m_DirectorySPV + outputFileName + m_OutputExtension;

    //    std::ofstream outFile;
    //
    //    outFile.open( filePath, std::ios::out | std::ios::binary );
    //
    //    if ( outFile.is_open() )
    //    {
    //        for ( size_t i = 0u; i < assemblyCode.size(); i++ )
    //        {
    //            outFile.write( (char8*)&assemblyCode[i], sizeof(char8) );
    //        }
    //    }
    //    else
    //    {
    //        std::cout << "ERROR::ShaderCompiler::SaveToSPV: Failed to create file " << filePath << std::endl;
    //
    //        return;
    //    }
    //
    //    outFile.close();
    //
    //    if ( !outFile.good() )
    //    {
    //        std::cout << "ERROR::ShaderCompiler::SaveToSPV: Error occured at writing time!" << std::endl;
    //
    //        return;
    //    }

        std::FILE* pFile;
    
        #pragma warning( disable: 4996 )

        pFile = fopen( filePath.c_str(), "w" );
    
        fwrite( assemblyCode.c_str(), sizeof(char8), assemblyCode.size(), pFile );

        fclose( pFile );
    }

    std::vector<char8> ShaderCompiler::LoadSPV(const char* fileName)
    {
        std::string filePath = m_DirectorySPV + fileName + m_OutputExtension;

        size_t fileSize = 0u;

        std::vector<char8> buffer;

        std::ifstream inFile;

        inFile.open( filePath, std::ios::ate | std::ios::binary );

        if ( inFile.is_open() )
        {
            fileSize = static_cast<size_t>( inFile.tellg() );

            buffer.resize( fileSize );

            inFile.seekg(0);

            inFile.read( buffer.data(), fileSize );
        }
        else
        {
            std::cout << "ERROR::ShaderCompiler::LoadSPV: Failed to open " << filePath << std::endl;

            return std::vector<char8>();
        }

        inFile.close();

        return buffer;
    }


    // Modifiers:

    void ShaderCompiler::SetDirectoryGLSL( const std::string& directory )
    {
        m_DirectoryGLSL = directory;
    }

    void ShaderCompiler::SetDirectorySPV( const std::string& directory )
    {
        m_DirectorySPV = directory;
    }

    void ShaderCompiler::SetOutputExtension( const std::string& extension )
    {
        m_OutputExtension = extension;
    }

}