#pragma once

//
// Main macros:
// 
// GOTHICLIB_DISABLE
//     Disables the whole library
// 
// GOTHICLIB_ALL
//     Loads everything
// 
// GOTHICLIB_ZENGIN
//     Loads ZenGin libs
// 
// GOTHICLIB_GENOME
//     Loads Genome libs
// 
// Special macros:
// 
// GOTHICLIB_ZENGIN_PWF
// 
//

#ifndef GOTHICLIB_DISABLE

#include <fstream>
#include <thread>

// Load everything
#ifdef GOTHICLIB_ALL

#undef GOTHICLIB_ZENGIN
#define GOTHICLIB_ZENGIN

#undef GOTHICLIB_GENOME
#define GOTHICLIB_GENOME

#endif

// Include ZenGin
#ifdef GOTHICLIB_ZENGIN

#include "zen_base.h"
#include "zen_vdfs.h"

#endif // GOTHICLIB_ZENGIN

namespace GothicLib
{
	//
	// Error logging system
	//

#ifdef _DEBUG
#define LOG_DEBUG(M)	GothicLib::Log::Message(GothicLib::Log::MESSAGE_DEBUG, (M))
#else
#define LOG_DEBUG(M)
#endif
#define LOG_INFO(M)		GothicLib::Log::Message(GothicLib::Log::MESSAGE_INFO, (M))
#define LOG_WARN(M)		GothicLib::Log::Message(GothicLib::Log::MESSAGE_WARN, (M))
#define LOG_ERROR(M)	GothicLib::Log::Message(GothicLib::Log::MESSAGE_ERROR, (M))

	class Log
	{
	public:

		Log() = delete;
		~Log() = delete;

		enum MESSAGE_LEVEL
		{
			MESSAGE_DEBUG,
			MESSAGE_INFO,
			MESSAGE_WARN,
			MESSAGE_ERROR
		};

		typedef void (*LogFunc)(MESSAGE_LEVEL, std::string);

		static void SetCallback(LogFunc);
		static void Message(MESSAGE_LEVEL, std::string);

	private:

		static inline LogFunc callback = nullptr;

	};

	//
	// File stream abstraction which allows reading
	// both from a file and a memory buffer.
	//

	class FileStream
	{
	public:

		FileStream()	{ iSubThreadId = std::this_thread::get_id(); }
		~FileStream()	{ Close(); }

		FileStream(const FileStream&) = delete;
		FileStream& operator=(const FileStream&) = delete;

		/*
			Stream creation / destruction
		*/

		bool		Open(std::string, char);				// File
		bool		Open(std::wstring, char);				// File
		bool		Open(char*, size_t, char, bool = true);	// Buffer
		bool		Open(FileStream*, uint64_t, uint64_t);	// Substream

		void		Close();

		/*
			Basic stream operations	
		*/

		bool		Read(void*, uint64_t);
		bool		ReadNullString(std::string&);
		bool		ReadLine(std::string&);

		void		Seek(uint64_t pos);
		uint64_t	Tell();
		uint64_t	TotalSize();

	private:

		void		ForkSubStream(FileStream*, uint64_t);

		enum STREAM_MODE
		{
			STREAM_MODE_READ,
			STREAM_MODE_WRITE,

			STREAM_MODE_NONE = -1
		};

		STREAM_MODE		mode = STREAM_MODE_NONE;

		// In
		enum STREAM_SOURCE
		{
			STREAM_SOURCE_RAWFILE,
			STREAM_SOURCE_BUFFER,
			STREAM_SOURCE_SUBSTREAM,

			STREAM_SOURCE_NONE = -1
		};

		STREAM_SOURCE	iSource = STREAM_SOURCE_NONE;

		std::ifstream	iFile;
		std::wstring	iPath;

		char*			iBuffer;
		bool			iDisposeBuffer;

		uint64_t		iTotalSize;
		uint64_t		iPosition;

		FileStream*		iSubStream;
		uint64_t		iSubOffset = 0;
		std::thread::id	iSubThreadId;

		// Out
		std::ofstream	oFile;

	};
};

#endif // !GOTHICLIB_DISABLE
