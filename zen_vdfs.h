﻿//
// zen_vdfs.h
//
// Copyright (c) 2021-2023 Václav Maroušek
//

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace GothicLib
{
	class FileStream;

	namespace ZenGin
	{
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
			bool ListDirectory(Directory*, File**, size_t*, Directory**, size_t*);

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
};
