//
// vdfs.hpp
//
// Copyright (c) 2021-2022 Václav Maroušek
//

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <fstream>

#include <iostream>

namespace VDFS
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

		//
		// bool Open(std::string filename, char openMode)
		// 
		// Opens a new file stream with the specified
		// file serving as the data source.
		// 
		//   - filename
		//         Path to the file which should be opened.
		// 
		//   - openMode
		//         Mode with which to open file. Accepted
		//         values are 'r', 'w' and 'a'
		//
		bool Open(std::string filename, char openMode)
		{
			return Open(std::wstring(filename.begin(), filename.end()), openMode);
		}

		//
		// bool Open(std::wstring filename, char openMode)
		// 
		// Opens a new file stream with the specified
		// file serving as the data source.
		// 
		//   - filename
		//         Path to the file which should be opened.
		// 
		//   - openMode
		//         Mode with which to open file. Accepted
		//         values are: 'r', 'w' and 'a'
		//
		bool Open(std::wstring filename, char openMode)
		{
			if (openMode == 'r')
			{
				mode	= STREAM_MODE_READ;
				iSource	= STREAM_SOURCE_RAWFILE;

				iFile.open(filename, std::ios::binary);

				if (!iFile.is_open())
					return false;

				iFile.seekg(0, std::ios::end);
				iTotalSize = iFile.tellg();
				iFile.seekg(0, std::ios::beg);

			}
			else if (openMode == 'w' or openMode == 'a')
			{
				mode = STREAM_MODE_WRITE;

				if (openMode == 'a')
				{
					oFile.open(filename, std::ios::binary | std::ios::app);
				}
				else
				{
					oFile.open(filename, std::ios::binary);
				}

				if (!oFile.is_open())
					return false;

			}
			else
			{
				return false;
			}
		}

		//
		// bool Open(char* buffer, uint64_t size, char openMode, bool realloc = true)
		//
		// Opens a new file stream with the specified
		// buffer serving as the data source.
		// 
		//   - buffer
		//         Pointer to buffer which should serve 
		//         as the data source.
		// 
		//   - size
		//         Size of aforementioned buffer.
		// 
		//   - openMode
		//         Mode with which to open file. Accepted
		//         values are: 'r'.
		// 
		//   - realloc
		//         Determines whether or not the specified 
		//         buffer should be reallocated.
		//         If set to true a new buffer of identical
		//         size will be allocated and the data will 
		//         be copied over.
		//         If set to false, the caller must bear the
		//         responsibility of managing the specified 
		//         buffer over its lifespan.
		//
		bool Open(char* buffer, uint64_t size, char openMode, bool realloc = true)
		{
			if (openMode == 'r')
			{
				mode	= STREAM_MODE_READ;
				iSource	= STREAM_SOURCE_BUFFER;

				if (realloc)
				{
					iBuffer = new char[size];
					memcpy(iBuffer, buffer, size);

					iDisposeBuffer = true;
				}
				else
				{
					iBuffer = buffer;
				}

				iTotalSize = size;

				return true;
			}
		}

		//
		// void Close()
		// 
		// Closes the file stream and disposes of
		// all used resources.
		//
		void Close()
		{
			if (mode == STREAM_MODE_READ)
			{
				if (iSource == STREAM_SOURCE_RAWFILE)
				{
					iFile.close();
				}
				else if (iSource == STREAM_SOURCE_BUFFER && iDisposeBuffer)
				{
					delete[] iBuffer;
				}

				iBuffer			= nullptr;
				iTotalSize		= 0;
				iPosition		= 0;
			}
		}

		/*
			Basic stream operations	
		*/

		//
		// bool Read(char* readBuffer, uint64_t readSize)
		//
		// Reads data from the stream and copies it into
		// the specified buffer.
		// 
		// Returns false if the read operation fails.
		// 
		//   - readBuffer
		//         Pointer to buffer to which the read
		//         data shall be copied.
		// 
		//   - readSize
		//         Size of aforementioned buffer.
		//
		bool Read(void* readBuffer, uint64_t readSize)
		{
			if (mode == STREAM_MODE_READ)
			{
				if (iPosition + readSize > iTotalSize)
				{
					return false;
				}

				if (iSource == STREAM_SOURCE_RAWFILE)
				{
					iFile.read((char*)readBuffer, readSize);
				}
				else if (iSource == STREAM_SOURCE_BUFFER)
				{
					memcpy(readBuffer, &iBuffer[iPosition], readSize);
				}

				iPosition += readSize;
			}
		}

		//
		// bool ReadLine(std::string& line)
		//
		// Reads a line from the stream and copies it into
		// the specified string.
		// 
		// Returns false if the read operation fails.
		// 
		//   - line
		//         String to which the read data 
		//         shall be copied.
		//
		bool ReadLine(std::string& line)
		{
			// Slow, but good enough.
			// Doesn't read the last line if it's empty
			// (because I cannot be bothered ¯\_(ツ)_/¯)

			char c;
			bool result = false;

			line = "";

			while (Read(&c, sizeof(c)))
			{
				result = true;

				if (c == '\r')
				{
					uint64_t currPos = Tell();
					char nextChar;

					if (!Read(&nextChar, sizeof(nextChar)) ||
						nextChar != '\n')
					{
						Seek(currPos);
					}

					break;
				}

				if (c == '\n')
				{
					break;
				}

				line += c;
			}

			return result;
		}

		//
		// void Seek(uint64_t pos)
		//
		// Seeks a specified position in the stream.
		// 
		//   - pos
		//         Position in the stream
		//
		void Seek(uint64_t pos)
		{
			if (mode == STREAM_MODE_READ)
			{
				iPosition = pos;

				if (iSource == STREAM_SOURCE_RAWFILE)
					iFile.seekg(iPosition);
			}
		}

		//
		// uint64_t Tell()
		//
		// Tells the current position in the stream.
		//
		uint64_t Tell()
		{
			if (mode == STREAM_MODE_READ)
			{
				return iPosition;
			}
		}

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

			STREAM_SOURCE_NONE = -1
		};

		STREAM_SOURCE	iSource = STREAM_SOURCE_NONE;

		std::ifstream	iFile;
		char*			iBuffer;
		bool			iDisposeBuffer;

		uint64_t		iTotalSize;
		uint64_t		iPosition;

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

		bool InitializeDirectory(std::string _rootPath)
		{
			return InitializeDirectory(std::wstring(_rootPath.begin(), _rootPath.end()));
		}

		bool InitializeDirectory(std::wstring _rootPath)
		{
			volumes.clear();
			files.clear();
			directories.clear();

			// Make root
			directories.emplace_back();

			TraverseDirectory(std::filesystem::absolute(_rootPath), 0);

			// Mount archives
			for (size_t i = 0; i < volumes.size(); i++)
			{
				volumes[i].stream->Seek(volumes[i].rootOffset);
				size_t entryCount = volumes[i].entryCount;

				TraverseVolume(i, 0, entryCount);
			}

			return true;
		}

		bool Mount(std::string path)
		{
			return Mount(std::wstring(path.begin(), path.end()));
		}

		bool Mount(std::wstring path)
		{

		}


		void PrintDebug(Directory* root = nullptr, size_t tab = 0)
		{
			if (root == nullptr)
				root = &directories[0];

			std::string tabs = "";
			for (size_t i = 0; i < tab; i++)
				tabs += "  ";

			std::cout << tabs << root->name << std::endl;

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

		bool TraverseDirectory(std::filesystem::path path, size_t rootIndex)
		{
			size_t fileOffset	= files.size();
			size_t dirOffset	= directories.size();

			directories[rootIndex].fileOffset		= fileOffset;
			directories[rootIndex].directoryOffset	= dirOffset;

			for (auto& entry : std::filesystem::directory_iterator(path))
			{
				if (entry.is_directory())
				{
					directories[rootIndex].directoryCount++;

					// Add new directory record
					size_t dirIndex = directories.size();
					directories.emplace_back();
					Directory& dir = directories[dirIndex];
					
					dir.type = FILE_TYPE_PHYSICAL;
					dir.name = entry.path().filename().string();

					for (size_t i = 0; i < dir.name.size(); i++)
						dir.name[i] = toupper(dir.name[i]);

					dir.pPath = entry.path().wstring();
				}
				else
				{
					//
					// Check if the current file is a VDF
					//
					std::wstring extension = entry.path().extension().wstring();

					for (size_t i = 0; i < extension.size(); i++)
						extension[i] = toupper(extension[i]);

					if (extension == L".VDF")
					{
						// If it is, mount it.
						Volume volume;
						volume.path	= entry.path().wstring();

						volume.stream = new FileStream();

						//
						// Get timestamp of archive
						//
						if (!volume.stream->Open(volume.path, 'r'))
						{
							// Could not be opened, ignore
							delete volume.stream;
							continue;
						}
						
						VDF_Header header;

						if (!volume.stream->Read(&header, sizeof(header)) ||
							!*(uint64_t*)&header.signature == 0x565F435344565350)
						{
							// Invalid archive, ignore
							volume.stream->Close();
							delete volume.stream;
							continue;
						}

						volume.stream->Seek(0);

						volume.timestamp	= header.timestamp;
						volume.entryCount	= header.entryCount;
						volume.rootOffset	= header.rootOffset;

						size_t insertPos = 0;

						for (size_t i = 0; i < volumes.size(); i++)
						{
							if (volumes[i].timestamp < volume.timestamp)
							{
								insertPos++;
							}
							else
							{
								break;
							}
						}

						volumes.insert(volumes.begin() + insertPos, volume);
					}
					else
					{
						// If it isn't, add it to the list.
						directories[rootIndex].fileCount++;

						// Add new file record
						size_t fileIndex = files.size();
						files.emplace_back();
						File& file = files[fileIndex];

						file.type = FILE_TYPE_PHYSICAL;
						file.name = entry.path().filename().string();

						for (size_t i = 0; i < file.name.size(); i++)
							file.name[i] = toupper(file.name[i]);

						file.pPath = entry.path().wstring();
					}
				}
			}

			size_t currOffset = 0;

			for (auto& entry : std::filesystem::directory_iterator(path))
			{
				if (entry.is_directory())
				{
					TraverseDirectory(entry.path(), dirOffset + currOffset);
					currOffset++;
				}
			}

			return true;
		}

		bool TraverseVolume(size_t volumeIndex, size_t rootIndex, size_t& entryCount)
		{
			Volume& volume = volumes[volumeIndex];

			while (true)
			{
				entryCount++;

				VDF_Entry entry;

				if (!volume.stream->Read(&entry, sizeof(entry)))
				{
					return false;
				}

				// Trim name
				size_t nameEnd = 63;

				for (int i = 64; i--; )
				{
					if (entry.name[i] == 0x20)
						nameEnd = i;
					else
						break;
				}

				std::string name(entry.name, nameEnd);

				for (size_t i = 0; i < name.size(); i++)
				{
					name[i] = toupper(name[i]);
				}

				// Directory
				if (entry.type & 0x80000000)
				{
					// Check if the directory already exists
					bool exists = false;

					Directory& root = directories[rootIndex];
					size_t dirIndex = 0;

					if (root.directoryOffset != -1)
					{
						for (size_t i = 0; i < root.directoryCount; i++)
						{
							if (directories[root.directoryOffset + i].name == name)
							{
								dirIndex = root.directoryOffset + i;

								exists = true;
								break;
							}
						}
					}

					// If it doesnt exist create it
					if (!exists)
					{
						Directory insertDirectory;

						insertDirectory.name			= name;
						insertDirectory.type			= FILE_TYPE_VIRTUAL;
						insertDirectory.vVolumeIndex	= volumeIndex;

						dirIndex = root.directoryOffset + root.directoryCount;

						for (size_t i = 0; i < directories.size(); i++)
						{
							if (directories[i].directoryOffset != -1 &&
								directories[i].directoryOffset >= dirIndex)
							{
								directories[i].directoryOffset++;
							}
						}

						directories.insert(directories.begin() + dirIndex, insertDirectory);

						directories[rootIndex].directoryCount++;
					}

					TraverseVolume(volumeIndex, dirIndex, entryCount);

				}
				else
				{
					// Check if the file already exists
					bool exists = false;

					Directory& root = directories[rootIndex];
					size_t fileIndex = 0;
					
					if (root.fileOffset != -1)
					{
						for (size_t i = 0; i < root.fileCount; i++)
						{
							if (files[root.fileOffset + i].name == name)
							{
								fileIndex = root.fileOffset + i;

								exists = true;
								break;
							}
						}
					}

					if (exists)
					{
						File& existingFile = files[fileIndex];

						existingFile.type			= FILE_TYPE_VIRTUAL;
						existingFile.pPath			= L"";
						existingFile.vVolumeIndex	= volumeIndex;
						existingFile.vOffset		= entry.dataOffset;
						existingFile.vSize			= entry.dataSize;
					}
					else
					{
						File insertFile;

						insertFile.name			= name;
						insertFile.type			= FILE_TYPE_VIRTUAL;
						insertFile.vVolumeIndex = volumeIndex;
						insertFile.vOffset		= entry.dataOffset;
						insertFile.vSize		= entry.dataSize;

						fileIndex = root.fileOffset + root.fileCount;

						for (size_t i = 0; i < directories.size(); i++)
						{
							if (directories[i].fileOffset != -1 &&
								directories[i].fileOffset >= fileIndex)
							{
								directories[i].fileOffset++;
							}
						}

						files.insert(files.begin() + fileIndex, insertFile);

						directories[rootIndex].fileCount++;

					}

				}

				// Last entry
				if (entry.type & 0x40000000)
				{
					break;
				}	
			}

			return true;
		}

	public:

		std::vector<Volume>		volumes;

		std::vector<File>		files;
		std::vector<Directory>	directories;

		std::unordered_map<std::string, size_t>	filePathCache;
		std::unordered_map<std::string, size_t>	fileDirectoryCache;		
		std::unordered_map<std::string, size_t> fileCache;

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
