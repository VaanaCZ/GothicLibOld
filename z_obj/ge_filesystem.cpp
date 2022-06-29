﻿
#include "ge_filesystem.h"

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
bool GE::FileStream::Open(std::string filename, char openMode)
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
bool GE::FileStream::Open(std::wstring filename, char openMode)
{
	iPath = filename;

	if (openMode == 'r')
	{
		mode	= STREAM_MODE_READ;
		iSource	= STREAM_SOURCE_RAWFILE;

		iFile.open(iPath, std::ios::binary);

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
			oFile.open(iPath, std::ios::binary | std::ios::app);
		}
		else
		{
			oFile.open(iPath, std::ios::binary);
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
// bool Open(char* buffer, size_t size, char openMode, bool realloc = true)
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
bool GE::FileStream::Open(char* buffer, size_t size, char openMode, bool realloc)
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
// Open(FileStream* stream, uint64_t offset, uint64_t size)
//
// Opens a new file stream with from a prexisting
// file stream.
// 
//   - stream
//         The stream which will serve as a data
//         source for this stream.
// 
//   - offset
//         Offset in the specified stream at which
//         this stream should start.
// 
//   - size
//         Size of this stream.
//
bool GE::FileStream::Open(FileStream* stream, uint64_t offset, uint64_t size)
{
	if (offset + size > stream->iTotalSize)
		return false;

	mode = stream->mode;
	iSource = STREAM_SOURCE_SUBSTREAM;

	iSubStream = stream;
	iSubOffset = offset;
	iTotalSize = size;

	return true;
}

//
// void Close()
// 
// Closes the file stream and disposes of
// all used resources.
//
void GE::FileStream::Close()
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
bool GE::FileStream::Read(void* readBuffer, uint64_t readSize)
{
	if (mode == STREAM_MODE_READ)
	{
		if (iPosition + readSize > iTotalSize)
		{
			return false;
		}

		// If we start reading from a pre-existing
		// stream on a different thread, we must
		// fork it and create a new handle to prevent
		// a race condition.
		if (iSource == STREAM_SOURCE_SUBSTREAM &&
			std::this_thread::get_id() != iSubThreadId)
		{
			ForkSubStream(iSubStream, iSubOffset);
		}
		
		// Read data depending on the source
		if (iSource == STREAM_SOURCE_RAWFILE)
		{
			if (iFile.tellg() != iSubOffset + iPosition)
				iFile.seekg(iSubOffset + iPosition);

			iFile.read((char*)readBuffer, readSize);
		}
		else if (iSource == STREAM_SOURCE_BUFFER)
		{
			memcpy(readBuffer, &iBuffer[iSubOffset + iPosition], readSize);
		}
		else if (iSource == STREAM_SOURCE_SUBSTREAM)
		{
			iSubStream->Seek(iSubOffset + iPosition);

			if (!iSubStream->Read(readBuffer, readSize))
				return false;
		}

		iPosition += readSize;
	}
	
	return true;
}

bool GE::FileStream::ReadString(std::string& stringVal)
{
	return false;
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
bool GE::FileStream::ReadLine(std::string& line)
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
void GE::FileStream::Seek(uint64_t pos)
{
	if (mode == STREAM_MODE_READ)
	{
		iPosition = pos;

		if (iSource == STREAM_SOURCE_RAWFILE)
			iFile.seekg(iSubOffset + iPosition);
	}
}

//
// uint64_t Tell()
//
// Tells the current position in the stream.
//
uint64_t GE::FileStream::Tell()
{
	if (mode == STREAM_MODE_READ)
	{
		return iPosition;
	}
}

//
// uint64_t TotalSize()
//
// Returns the total size of the stream.
//
uint64_t GE::FileStream::TotalSize()
{
	if (mode == STREAM_MODE_READ)
	{
		return iTotalSize;
	}
}

//
// void ForkSubStream(FileStream* sourceStream, uint64_t sourceOffset)
//
// Reinitializes the current stream based on
// the data in the data source stream.
// 
//   - sourceStream
//         Stream which serves as a source
//         to this stream.
// 
//   - offset
//         offset in the source stream.
//
void GE::FileStream::ForkSubStream(FileStream* sourceStream, uint64_t sourceOffset)
{
	if (sourceStream->iSource == STREAM_SOURCE_SUBSTREAM)
	{
		ForkSubStream(sourceStream->iSubStream, sourceStream->iSubOffset + sourceOffset);
	}
	else
	{
		if (sourceStream->iSource == STREAM_SOURCE_RAWFILE)
		{
			iSource = STREAM_SOURCE_RAWFILE;

			iFile.open(sourceStream->iPath, std::ios::binary);
			iPath = sourceStream->iPath;
		}
		else if (sourceStream->iSource == STREAM_SOURCE_BUFFER)
		{
			iSource = STREAM_SOURCE_BUFFER;

			iBuffer = &sourceStream->iBuffer[sourceOffset];
			iDisposeBuffer = false;
		}

		iSubOffset = sourceOffset;
	}
}

void GE::FileStream::OpenGenomeFile()
{
}

//
// bool InitializeDirectory(std::string _rootPath)
//
// Initializes the virtual filesystem in the 
// specified directory. The specified directory
// becomes the root, to which mounted volumes
// will be added.
// 
// Returns false if the directory doesnt exist.
//
bool GE::FileSystem::InitializeDirectory(std::string _rootPath)
{
	return InitializeDirectory(std::wstring(_rootPath.begin(), _rootPath.end()));
}

//
// bool InitializeDirectory(std::wstring _rootPath)
//
// Initializes the virtual filesystem in the 
// specified directory. The specified directory
// becomes the root, to which mounted volumes
// will be added.
// 
// Returns false if the directory doesnt exist.
//
bool GE::FileSystem::InitializeDirectory(std::wstring _rootPath)
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
		TraverseVolume(i, 0, false, "", entryCount);
	}

	return true;
}

//
// bool Mount(std::string path)
//
// Mounts a volume given a valid path. The
// specified volume will always be given
// highest priority and thus overwrite any
// previously mounted volumes.
// Useful for mounting .mod files.
// 
// Returns false if the volume doesnt exist
// or is invalid.
//
bool GE::FileSystem::Mount(std::string path)
{
	return Mount(std::wstring(path.begin(), path.end()));
}

//
// bool Mount(std::wstring path)
//
// Mounts a volume given a valid path. The
// specified volume will always be given
// highest priority and thus overwrite any
// previously mounted volumes.
// Useful for mounting .mod files.
// 
// Returns false if the volume doesnt exist
// or is invalid.
//
bool GE::FileSystem::Mount(std::wstring path)
{
	// If it is, mount it.
	Volume volume;
	volume.path	= path;

	volume.stream = new FileStream();

	//
	// Get timestamp of archive
	//
	if (!volume.stream->Open(volume.path, 'r'))
	{
		// Could not be opened, ignore
		delete volume.stream;
		return false;
	}
	
	VDF_Header header;

	if (!volume.stream->Read(&header, sizeof(header)) ||
		*(uint64_t*)&header.signature != 0x565F435344565350 ||
		header.version != 0x00000050)
	{
		// Invalid archive, ignore
		volume.stream->Close();
		delete volume.stream;
		return false;
	}

	volume.stream->Seek(0);

	volume.timestamp	= 0xffffffff;
	volume.entryCount	= header.entryCount;
	volume.rootOffset	= header.rootOffset;

	// Add to modules and read
	size_t volumeIndex = volumes.size();
	volumes.push_back(volume);

	volumes[volumeIndex].stream->Seek(volumes[volumeIndex].rootOffset);

	size_t entryCount = volumes[volumeIndex].entryCount;
	TraverseVolume(volumeIndex, 0, false, "", entryCount);

	return true;
}

//
// FileStream* OpenFile(std::string path, bool globalSearch)
//
// Looks up a file using the specified path and
// creates a filestream if the file exists.
// 
//   - path
//         The path of the file to search for
//         e.g. _Work/Data/Worlds/World.zen
//         Note: The path is automatically canonized.
// 
//   - globalSearch
//         Specifies whether the search should
//         consider all directories. One may
//         include only the filename without
//         the path if this option is enabled
//         (e.g. "SURFACE.3DS").
//         Note: There is no speed penalty for
//         enabling this option.
// 
// If the look-up fails a nullptr is returned.
//
GE::FileStream* GE::FileSystem::OpenFile(std::string path, bool globalSearch)
{
	CanonizePath(path);

	if (globalSearch)
	{
		size_t s = path.rfind('/');
		if (s != std::string::npos)
			path = path.substr(s + 1);

		if (fileNameCache.find(path) != fileNameCache.end())
		{
			return OpenFile(&files[fileNameCache[path]]);
		}
	}
	else if (!globalSearch && filePathCache.find(path) != filePathCache.end())
	{
		return OpenFile(&files[filePathCache[path]]);
	}

	return nullptr;
}

//
// FileStream* OpenFile(File* file)
//
// Looks up a file given a valid file handle.
// 
// If the look-up fails a nullptr is returned.
//
GE::FileStream* GE::FileSystem::OpenFile(File* file)
{
	FileStream* stream = new FileStream();

	if (file->type == FILE_TYPE_PHYSICAL &&
		stream->Open(file->pPath, 'r'))
	{
		return stream;
	}
	else if (file->type == FILE_TYPE_VIRTUAL)
	{
		Volume& volume = volumes[file->vVolumeIndex];

		if (stream->Open(volume.stream, file->vOffset, file->vSize))
		{
			return stream;
		}
	}

	delete[] stream;
	return nullptr;
}

//
// bool ListDirectory(std::string path, File** filesPtr, size_t* fileCount, Directory** dirsPtr, size_t* dirCount)
//
// Lists all files and directories in the specified directory.
// 
//   - path
//         Path specifying the directory, which
//         should be listed.
// 
//   - filesPtr
//         Pointer to a pointer which will point
//         to the files listed.
//         If left null, no files will be listed.
//         If no files are found, this will be set
//         to nullptr.
// 
//   - fileCount
//         Pointer to file counter.
//         If left null, no files will be listed.
//         If no files are found, this will be set
//         to 0.
// 
//   - dirsPtr
//         Pointer to a pointer which will point
//         to the directories listed.
//         If left null, no directories will be listed.
//         If no directories are found, this will 
//         be set to nullptr.
// 
//   - dirCount
//         Pointer to directory counter.
//         If left null, no directories will be listed.
//         If no directories are found, this will be set
//         to 0.
// 
// Returns false if the specified directory is not found.
//
bool GE::FileSystem::ListDirectory(std::string path, File** filesPtr, size_t* fileCount, Directory** dirsPtr, size_t* dirCount)
{
	CanonizePath(path);

	if (path.back() != '/')
		path += "/";

	if (dirPathCache.find(path) != dirPathCache.end())
	{
		return ListDirectory(&directories[dirPathCache[path]], filesPtr, fileCount, dirsPtr, dirCount);
	}

	return false;
}

//
// bool ListDirectory(Directory* directory, File** filesPtr, size_t* fileCount, Directory** dirsPtr, size_t* dirCount)
//
// Lists all files and directories in the specified directory.
// 
//   - directory
//         A valid directory handle.
// 
//   - filesPtr
//         Pointer to a pointer which will point
//         to the files listed.
//         If left null, no files will be listed.
//         If no files are found, this will be set
//         to nullptr.
// 
//   - fileCount
//         Pointer to file counter.
//         If left null, no files will be listed.
//         If no files are found, this will be set
//         to 0.
// 
//   - dirsPtr
//         Pointer to a pointer which will point
//         to the directories listed.
//         If left null, no directories will be listed.
//         If no directories are found, this will 
//         be set to nullptr.
// 
//   - dirCount
//         Pointer to directory counter.
//         If left null, no directories will be listed.
//         If no directories are found, this will be set
//         to 0.
// 
// Returns false if the specified directory is not found.
//
bool GE::FileSystem::ListDirectory(Directory* directory, File** filesPtr, size_t* fileCount, Directory** dirsPtr, size_t* dirCount)
{
	if (filesPtr != nullptr && fileCount != nullptr)
	{
		if (directory->fileCount == 0)
		{
			*filesPtr = nullptr;
			*fileCount = 0;
		}
		else
		{
			*filesPtr = &files[directory->fileOffset];
			*fileCount = directory->fileCount;
		}
	}

	if (dirsPtr != nullptr && dirCount != nullptr)
	{
		if (directory->directoryCount == 0)
		{
			*dirsPtr = nullptr;
			*dirCount = 0;
		}
		else
		{
			*dirsPtr = &directories[directory->directoryOffset];
			*dirCount = directory->directoryCount;
		}
	}

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
void GE::FileSystem::TraverseDirectory(std::filesystem::path path, std::string fsPath, size_t parentIndex)
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
			dirPathCache[fsPath + dir.name + "/"] = dirIndex;
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
				fileNameCache[file.name]					= fileIndex;
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
// void TraverseVolume(size_t volumeIndex, size_t parentIndex, bool parentEmpty, std::string fsPath, size_t& entryCount)
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
//   - fsPath
//         Current filesystem path (e.g. DATA/ANIMS/).
// 
//   - entryCount
//         Counter of the remaining entries in the volume.
//         Counts down by one everytime time an entry is
//         read to prevent endless looping.
//
void GE::FileSystem::TraverseVolume(size_t volumeIndex, size_t parentIndex, bool parentEmpty, std::string fsPath, size_t& entryCount)
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
			name[i] = toupper(name[i]);

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

			exists = dirPathCache.find(nextFsPath) != dirPathCache.end();

			// If it doesnt exist create it
			if (exists)
			{
				dirIndex = dirPathCache[nextFsPath];
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

					for (auto& it : dirPathCache)
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
				dirPathCache[nextFsPath] = dirIndex;
			}

			// Read subdirs
			uint64_t currPos = volume.stream->Tell();
			uint64_t nextPos = volume.rootOffset + (entry.dataOffset * sizeof(VDF_Entry));

			volume.stream->Seek(nextPos);

			bool isDirEmpty = directories[dirIndex].fileCount == 0;
			TraverseVolume(volumeIndex, dirIndex, isDirEmpty, nextFsPath, entryCount);

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
					// This could be sped up if we batch insertions
					// and increment all the offsets at once.

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

					for (auto& it : fileNameCache)
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
				fileNameCache[insertFile.name]	= fileIndex;
			}
		}

		// Last entry
		if (entry.type & 0x40000000)
		{
			break;
		}
	}
}

//
// void CanonizePath(std::string& path)
//
// Canonizes a path so that it can be used
// to search for files and directories.
// 
// for example:
// "DATA\\anims//../tEXuReS\\..//WoRlDS"
// becomes:
// "DATA/WORLDS/"
//
void GE::FileSystem::CanonizePath(std::string& path)
{
	path = std::filesystem::path(path).lexically_normal().string();

	for (size_t i = 0; i < path.size(); i++)
		if (path[i] == '\\')
			path[i] = '/';

	for (size_t i = 0; i < path.size(); i++)
		path[i] = toupper(path[i]);
}
