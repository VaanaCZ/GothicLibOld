#include "gothiclib_base.h"

using namespace GothicLib;

//
// void SetCallback(LogFunc _callback)
// 
// Sets up a callback which will be used for
// error logging by the library.
// 
//   - _callback
//         Pointer to function.
//
void Log::SetCallback(LogFunc _callback)
{
	callback = _callback;
}

//
// void Message(MESSAGE_LEVEL level, std::string message)
// 
// Sends a message to the callback.
// 
//   - level
//         The severity of the message.
//         DEBUG/INFO/WARN/ERROR
// 
//   - message
//         The message to send.
//
void Log::Message(MESSAGE_LEVEL level, std::string message)
{
	if (callback)
		callback(level, message);
}

/*void GothicLib::ClassManager::AddClassDef(ClassDefinition* definition)
{
	classes[definition->name] = definition;
}

ClassDefinition* ClassManager::GetClassDef(std::string name)
{
	if (classes.find(name) != classes.end())
		return classes[name];

	return nullptr;
}*/

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
bool FileStream::Open(std::string filename, char openMode)
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
bool FileStream::Open(std::wstring filename, char openMode)
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
bool FileStream::Open(char* buffer, size_t size, char openMode, bool realloc)
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
bool FileStream::Open(FileStream* stream, uint64_t offset, uint64_t size)
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
void FileStream::Close()
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
bool FileStream::Read(void* readBuffer, uint64_t readSize)
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

bool FileStream::ReadNullString(std::string& str)
{
	// Slow, but good enough.
	char c;
	bool result = false;

	str = "";

	while (Read(&c, sizeof(c)))
	{
		result = true;

		if (c == NULL)
		{
			break;
		}

		str += c;
	}

	return result;
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
bool FileStream::ReadLine(std::string& line)
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
void FileStream::Seek(uint64_t pos)
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
uint64_t FileStream::Tell()
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
uint64_t FileStream::TotalSize()
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
void FileStream::ForkSubStream(FileStream* sourceStream, uint64_t sourceOffset)
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
