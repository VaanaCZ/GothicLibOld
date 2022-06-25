
#include "zen_vdfs.h"

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
bool ZEN::FileStream::Open(std::string filename, char openMode)
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
bool ZEN::FileStream::Open(std::wstring filename, char openMode)
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
bool ZEN::FileStream::Open(char* buffer, uint64_t size, char openMode, bool realloc)
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
void ZEN::FileStream::Close()
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
bool ZEN::FileStream::Read(void* readBuffer, uint64_t readSize)
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
bool ZEN::FileStream::ReadLine(std::string& line)
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
void ZEN::FileStream::Seek(uint64_t pos)
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
uint64_t ZEN::FileStream::Tell()
{
	if (mode == STREAM_MODE_READ)
	{
		return iPosition;
	}
}

//
// bool FileSystem::InitializeDirectory(std::string _rootPath)
//
// Initializes the virtual filesystem in the 
// specified directory. The specified directory
// becomes the root, to which mounted volumes
// will be added.
// 
// Returns false if the directory doesnt exist.
//
bool ZEN::FileSystem::InitializeDirectory(std::string _rootPath)
{
	return InitializeDirectory(std::wstring(_rootPath.begin(), _rootPath.end()));
}

//
// bool FileSystem::InitializeDirectory(std::wstring _rootPath)
//
// Initializes the virtual filesystem in the 
// specified directory. The specified directory
// becomes the root, to which mounted volumes
// will be added.
// 
// Returns false if the directory doesnt exist.
//
bool ZEN::FileSystem::InitializeDirectory(std::wstring _rootPath)
{
	volumes.clear();
	files.clear();
	directories.clear();

	// Make root
	directories.emplace_back();

	std::filesystem::path rootPath = std::filesystem::absolute(_rootPath);

	if (!std::filesystem::is_directory(rootPath))
		return false;

	TraverseDirectory(rootPath, "", 0);

	// Mount archives
	for (size_t i = 0; i < volumes.size(); i++)
	{
		volumes[i].stream->Seek(volumes[i].rootOffset);
		size_t entryCount = volumes[i].entryCount;

		TraverseVolume(i, 0, false, volumes[i].rootOffset, "", entryCount);


		continue;
	}

	return true;
}

bool ZEN::FileSystem::Mount(std::string path)
{
	return Mount(std::wstring(path.begin(), path.end()));
}

bool ZEN::FileSystem::Mount(std::wstring path)
{
	return true;
}

//
// void TraverseDirectory(std::filesystem::path path, std::string fsPath, size_t parentIndex)
//
// Recursively traverses the specified directory
// searching for physical files and  volumes to mount.
// 
//   - path
//         Current directory path (e.g. C:\Gothic\Data\Anims).
// 
//   - fsPath
//         Current filesystem path (e.g. DATA/ANIMS/).
// 
//   - parentIndex
//         Index of parent folder.
//
void ZEN::FileSystem::TraverseDirectory(std::filesystem::path path, std::string fsPath, size_t parentIndex)
{
	size_t fileOffset	= files.size();
	size_t dirOffset	= directories.size();

	directories[parentIndex].fileOffset			= fileOffset;
	directories[parentIndex].directoryOffset	= dirOffset;

	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			directories[parentIndex].directoryCount++;

			// Add new directory record
			size_t dirIndex = directories.size();
			directories.emplace_back();
			Directory& dir = directories[dirIndex];
			
			dir.type = FILE_TYPE_PHYSICAL;
			dir.name = entry.path().filename().string();

			for (size_t i = 0; i < dir.name.size(); i++)
				dir.name[i] = toupper(dir.name[i]);

			dir.pPath = entry.path().wstring();

			// Add to cache
			fileDirectoryCache[fsPath + dir.name + "/"] = dirIndex;
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
					*(uint64_t*)&header.signature != 0x565F435344565350 ||
					header.version != 0x00000050)
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
				directories[parentIndex].fileCount++;

				// Add new file record
				size_t fileIndex = files.size();
				files.emplace_back();
				File& file = files[fileIndex];

				file.type = FILE_TYPE_PHYSICAL;
				file.name = entry.path().filename().string();

				for (size_t i = 0; i < file.name.size(); i++)
					file.name[i] = toupper(file.name[i]);

				file.pPath = entry.path().wstring();

				// Add to cache
				filePathCache[fsPath + file.name]		= fileIndex;
				fileCache[file.name]					= fileIndex;
			}
		}
	}

	size_t currOffset = 0;

	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			size_t offset = dirOffset + currOffset;
			std::string nextFsPath = fsPath + directories[offset].name + "/";

			TraverseDirectory(entry.path(), nextFsPath, offset);
			currOffset++;
		}
	}
}

//
// void TraverseVolume(size_t volumeIndex, size_t parentIndex, bool parentEmpty, uint64_t rootOffset, std::string fsPath, size_t& entryCount)
//
// Recursively traverses the specified directory
// searching for physical files and  volumes to mount.
// 
//   - volumeIndex
//         Index of the volume being traversed
// 
//   - parentIndex
//         Index of the parent folder
// 
//   - parentEmpty
//         Specifies whether the parent directory
//         is empty or not. If so, then there is
//         no need to check for duplicity -> faster.
// 
//   - rootOffset
//         Offset of the file table
// 
//   - fsPath
//         Current filesystem path (e.g. DATA/ANIMS/).
// 
//   - entryCount
//         Counter of the remaining entries in the volume.
//         Counts down by one everytime time an entry is
//         read to prevent endless looping.
//
void ZEN::FileSystem::TraverseVolume(size_t volumeIndex, size_t parentIndex, bool parentEmpty, uint64_t rootOffset, std::string fsPath, size_t& entryCount)
{
	Volume& volume = volumes[volumeIndex];

	while (true)
	{
		entryCount--;

		if (entryCount < 0)
		{
			// Incorrect number of entries specified!
			// Archive is probably broken.
			break;
		}

		VDF_Entry entry;

		if (!volume.stream->Read(&entry, sizeof(entry)))
		{
			break;
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
			std::string nextFsPath = fsPath + name + "/";

			// Check if the directory already exists
			bool exists = false;

			Directory& parentDir = directories[parentIndex];

			if (parentDir.directoryCount == 0)
			{
				parentDir.directoryOffset = directories.size();
			}

			size_t dirIndex = 0;

			exists = fileDirectoryCache.find(nextFsPath) != fileDirectoryCache.end();

			// If it doesnt exist create it
			if (exists)
			{
				dirIndex = fileDirectoryCache[nextFsPath];
			}
			else
			{
				Directory insertDirectory;

				insertDirectory.name			= name;
				insertDirectory.type			= FILE_TYPE_VIRTUAL;
				insertDirectory.vVolumeIndex	= volumeIndex;
				insertDirectory.directoryOffset = directories.size();
				insertDirectory.fileOffset		= files.size();

				dirIndex = parentDir.directoryOffset + parentDir.directoryCount;

				if (dirIndex != directories.size())
				{
					for (size_t i = 0; i < directories.size(); i++)
					{
						if (i != parentIndex &&
							directories[i].directoryOffset >= dirIndex)
						{
							directories[i].directoryOffset++;
						}
					}

					for (auto& it : fileDirectoryCache)
					{
						if (it.second >= dirIndex)
						{
							it.second++;
						}
					}
				}

				directories.insert(directories.begin() + dirIndex, insertDirectory);

				directories[parentIndex].directoryCount++;

				// Add to cache
				fileDirectoryCache[nextFsPath] = dirIndex;
			}

			// Read subdirs
			uint64_t currPos = volume.stream->Tell();
			uint64_t nextPos = rootOffset + (entry.dataOffset * sizeof(VDF_Entry));

			volume.stream->Seek(nextPos);

			bool isDirEmpty = directories[dirIndex].directoryCount == 0;
			TraverseVolume(volumeIndex, dirIndex, isDirEmpty, rootOffset, nextFsPath, entryCount);

			volume.stream->Seek(currPos);
		}
		else
		{
			std::string nextFsPath = fsPath + name;

			// Check if the file already exists
			bool exists = false;

			Directory& parentDir = directories[parentIndex];

			if (parentDir.fileCount == 0)
			{
				parentDir.fileOffset = files.size();
			}

			size_t fileIndex = 0;
			
			if (!parentEmpty)
			{
				exists = filePathCache.find(nextFsPath) != filePathCache.end();
			}

			if (exists)
			{
				fileIndex = filePathCache[nextFsPath];

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

				fileIndex = parentDir.fileOffset + parentDir.fileCount;

				if (fileIndex != files.size())
				{
					for (size_t i = 0; i < directories.size(); i++)
					{
						if (i != parentIndex &&
							directories[i].fileOffset >= fileIndex)
						{
							directories[i].fileOffset++;
						}
					}

					for (auto& it : filePathCache)
					{
						if (it.second >= fileIndex)
						{
							it.second++;
						}
					}
				}

				files.insert(files.begin() + fileIndex, insertFile);

				directories[parentIndex].fileCount++;

				// Add to cache
				filePathCache[nextFsPath]	= fileIndex;
				fileCache[insertFile.name]	= fileIndex;
			}
		}

		// Last entry
		if (entry.type & 0x40000000)
		{
			break;
		}
	}
}