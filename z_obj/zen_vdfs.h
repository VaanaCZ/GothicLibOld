//
// zen_vdfs.hpp
//
// Copyright (c) 2021-2022 Václav Maroušek
//

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <fstream>

#include <iostream>

namespace ZEN
{
	//
	// File stream abstraction which allows reading
	// both from a file and a memory buffer.
	//

	class FileStream
	{
	public:

		FileStream()	{}
		~FileStream()	{ Close(); }

		FileStream(const FileStream&) = delete;
		FileStream& operator=(const FileStream&) = delete;

		/*
			Stream creation / destruction
		*/

		bool		Open(std::string, char);					// File
		bool		Open(std::wstring, char);					// File
		bool		Open(char*, uint64_t, char, bool = true);	// buffer
		bool		Open(FileStream*, uint64_t, uint64_t);		// Substream

		void		Close();

		/*
			Basic stream operations	
		*/

		bool		Read(void* readBuffer, uint64_t readSize);
		bool		ReadLine(std::string& line);

		void		Seek(uint64_t pos);
		uint64_t	Tell();

	private:

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
		char*			iBuffer;
		bool			iDisposeBuffer;

		uint64_t		iTotalSize;
		uint64_t		iPosition;

		FileStream*		iSubStream;
		uint64_t		iSubOffset = 0;

		// Out
		std::ofstream	oFile;

	};

	//
	// Virtual file system implementation for VDFS
	//

	class FileSystem
	{
	public:

		FileSystem()	{}
		~FileSystem()	{}

		FileSystem(const FileSystem&) = delete;
		FileSystem& operator=(const FileSystem&) = delete;

		/*
			Structs
		*/

		struct Volume
		{
			std::wstring	path;
			FileStream*		stream;
			uint32_t		timestamp;

			uint64_t		rootOffset;
			uint64_t		entryCount;
		};

		enum FILE_TYPE
		{
			FILE_TYPE_PHYSICAL,
			FILE_TYPE_VIRTUAL,

			FILE_TYPE_NONE= -1
		};

		struct File
		{
			std::string name;

			FILE_TYPE type = FILE_TYPE_NONE;

			std::wstring pPath;

			uint64_t vVolumeIndex = -1;
			uint64_t vOffset = -1;
			uint64_t vSize = 0;
		};

		struct Directory
		{
			std::string name;

			FILE_TYPE type = FILE_TYPE_NONE;

			std::wstring pPath;

			uint64_t vVolumeIndex = -1;

			uint64_t fileOffset = -1;
			uint64_t fileCount = 0;

			uint64_t directoryOffset = -1;
			uint64_t directoryCount = 0;
		};

		/*
			Main functions
		*/

		bool InitializeDirectory(std::string);
		bool InitializeDirectory(std::wstring);

		bool Mount(std::string);
		bool Mount(std::wstring);

		FileStream* OpenFile(std::string, bool = false);
		FileStream* OpenFile(File*);

		bool ListDirectory(std::string, File**, size_t*, Directory**, size_t*);
		bool ListDirectory(File*, File**, size_t*, Directory**, size_t*);

		void PrintDebug(Directory* root = nullptr, size_t tab = 0)
		{
			if (root == nullptr)
				root = &directories[0];

			std::string tabs = "";
			for (size_t i = 0; i < tab; i++)
				tabs += "  ";

			if (root->type == FILE_TYPE_VIRTUAL)
			{
				std::cout << tabs << "[V] " << root->name << std::endl;

				//std::cout << tabs << root->name << std::endl;

			}
			else
			{
				std::cout << tabs << root->name << std::endl;

			}

			for (size_t i = 0; i < root->directoryCount; i++)
			{
				PrintDebug(&directories[root->directoryOffset + i], tab + 1);
			}

			for (size_t i = 0; i < root->fileCount; i++)
			{
				std::cout << tabs << "  \033[32m" << files[root->fileOffset + i].name << "\033[0m" << std::endl;
			}
		}

	private:

		void TraverseDirectory(std::filesystem::path, std::string, size_t);
		void TraverseVolume(size_t, size_t, bool, std::string, size_t&);

		void CanonizePath(std::string&);

	public:

		std::vector<Volume>			volumes;

		std::vector<File>			files;
		std::vector<Directory>		directories;

		// Caches for quick lookups
		std::unordered_map<std::string, size_t>	fileNameCache; // e.g "MESH.3DS"
		std::unordered_map<std::string, size_t>	filePathCache; // e.g "_WORK/DATA/MESHES/MESH.3ds"
		std::unordered_map<std::string, size_t>	dirPathCache;  // e.g "_WORK/DATA/MESHES/"

#pragma pack(push,1)
		struct VDF_Header
		{
			char		comment[256];
			char		signature[16];
			uint32_t	entryCount;
			uint32_t	fileCount;
			uint32_t	timestamp;
			uint32_t	volumeSize;
			uint32_t	rootOffset;
			uint32_t	version;
		};

		struct VDF_Entry
		{
			char		name[64];
			uint32_t	dataOffset;
			uint32_t	dataSize;
			uint32_t	type;
			uint32_t	attributes;
		};
#pragma pack(pop)

	};
};
