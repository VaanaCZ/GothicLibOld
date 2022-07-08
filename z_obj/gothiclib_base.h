#pragma once

#include <fstream>
#include <thread>
#include <unordered_map>

#ifndef GOTHICLIB_DISABLE

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
	// Object reflection
	//

	class ClassDefinition;

	/*
		Manager
	*/

	class ClassManager
	{
	public:

		ClassManager()
		{
			size_t a = 0;
		}
		~ClassManager() {}

		ClassManager(const ClassManager&) = delete;
		ClassManager& operator=(const ClassManager&) = delete;

		void AddClassDef(ClassDefinition*);
		ClassDefinition* GetClassDef(std::string);

	private:

		std::unordered_map<std::string, ClassDefinition*> classes;

	};

	/*
		Macros and templates
	*/

#define DEFINE_CLASS(C, B)													\
	static C* CreateInstance() { return new C(); }							\
	inline static ClassDefinition* classDef =								\
		new ClassDefinition(classManager, #C, #B, &C::CreateInstance);


#define DEFINE_PROPERTY(T, N)												\
	T N;																	\
	inline static PropertyDefinition* classDef ##N = 						\
		new PropertyDefinition(classDef, #N, #T, ((T*)nullptr)->N &C::CreateInstance);


	class ClassDefinition;

	class PropertyDefinition
	{
	public:

		PropertyDefinition(ClassDefinition* classDefinition, std::string _name, std::string _type, size_t _offset, size_t _size)
		{
			name	= _name;
			type	= _type;
			offset	= _offset;
			size	= _size;

			//classDefinition->AddClassDef(this);
		}

		std::string	name;
		std::string	type;
		size_t		offset;
		size_t		size;

	};

	class ClassDefinition
	{
	public:

		ClassDefinition(ClassManager*& classManager, std::string _name, std::string _baseName, void* _createFunc)
		{
			name		= _name;
			baseName	= _baseName;
			createFunc	= _createFunc;

			if (!classManager)
				classManager = new ClassManager();

			classManager->AddClassDef(this);
		}

		std::string	name;
		std::string	baseName;
		void*		createFunc;

		std::vector<PropertyDefinition*> properties;

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
