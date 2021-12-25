#ifndef LOGGER_H
#define LOGGER_H

#if _MSC_VER && !__INTEL_COMPILER

#define LOG_TRACE( message ) Logger::Trace( __FUNCTION__, message )

#define LOG_WARNING( message ) Logger::Warning( __FUNCTION__, message )

#define LOG_ERROR( message ) Logger::Error( __FUNCTION__, message )

#endif

namespace t3d
{
	class Logger
	{
	public:

	// Levels:

		enum class Level : uint32
		{
			Trace = 0,
			Warning,
			Error
		};

	// Constructors and Destructor:

		Logger();

		~Logger();

	// Functions:

		/// <summary>
		/// Show trace message in console.
		/// </summary>
		/// <param name="functionName"> - Name of the function that message came from. </param>
		/// <param name="message"> - Actual message. </param>
		static void Trace( const char8* functionName, const char8* message );

		/// <summary>
		/// Show warning message in console.
		/// </summary>
		/// <param name="functionName"> - Name of the function that message came from. </param>
		/// <param name="message"> - Actual message. </param>
		static void Warning( const char8* functionName, const char8* message );

		/// <summary>
		/// Show error message in console.
		/// </summary>
		/// <param name="functionName"> - Name of the function that message came from. </param>
		/// <param name="message"> - Actual message. </param>
		static void Error( const char8* functionName, const char8* message );

	// Modifiers:

		/// <summary>
		/// Set log messeges level.
		/// <para> TRACES - Show all messeges include trace messages. </para>
		/// <para> WARNINGS - Show only warning and error messages. </para>
		/// <para> ERRORS - Show error messages only. </para>
		/// </summary>
		/// <param name="logLevel">
		/// - Log level enum.
		/// </param>
		static void SetLevel( const Level logLevel );

	private:

	// Variables:

		static Level m_LogLevel;
	};
}

#endif // LOGGER_H