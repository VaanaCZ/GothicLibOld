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
	if (openMode == 'r')
	{
		iPath = filename;

		mode	= STREAM_MODE_READ;
		iSource	= STREAM_SOURCE_RAWFILE;

		iFile.open(iPath, std::ios::binary);

		if (!iFile.is_open())
		{
			error = true;
			return false;
		}

		iFile.seekg(0, std::ios::end);
		iTotalSize = iFile.tellg();
		iFile.seekg(0, std::ios::beg);

		iFileCache = new char[FILE_CACHE_SIZE];
		iFile.read(iFileCache, iTotalSize < FILE_CACHE_SIZE ? iTotalSize : FILE_CACHE_SIZE );
		iFileCacheBlock = 0;
	}
	else if (openMode == 'w' or openMode == 'a')
	{
		oPath = filename;

		mode	= STREAM_MODE_WRITE;
		oSource	= STREAM_SOURCE_RAWFILE;

		if (openMode == 'a')
		{
			oFile.open(oPath, std::ios::binary | std::ios::app);
		}
		else
		{
			oFile.open(oPath, std::ios::binary);
		}

		if (!oFile.is_open())
		{
			error = true;
			return false;
		}
	}
	else
	{
		error = true;
		return false;
	}

	return OnOpen();
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
	}

	return OnOpen();
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

	return OnOpen();
}

//
// void Close()
// 
// Closes the file stream and disposes of
// all used resources.
//
void FileStream::Close()
{
	OnClose();

	if (mode == STREAM_MODE_READ)
	{
		if (iSource == STREAM_SOURCE_RAWFILE)
		{
			iFile.close();

			delete[] iFileCache;
		}
		else if (iSource == STREAM_SOURCE_BUFFER && iDisposeBuffer)
		{
			delete[] iBuffer;
		}

		iBuffer			= nullptr;
		iTotalSize		= 0;
		iPosition		= 0;
	}
	else if(mode == STREAM_MODE_WRITE)
	{
		if (iSource == STREAM_SOURCE_RAWFILE)
		{
			oFile.close();
		}

		oTotalSize		= 0;
		oPosition		= 0;
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
	if (error)
	{
		return false;
	}

	if (mode == STREAM_MODE_READ)
	{
		if (iPosition + readSize > iTotalSize)
		{
			error = true;
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
			uint64_t bytesLeft = readSize;

			while (true)
			{
				uint64_t cacheBlock = iPosition / FILE_CACHE_SIZE;

				if (cacheBlock != iFileCacheBlock)
				{
					// Cache next file block
					iFileCacheBlock = cacheBlock;

					uint64_t offset	= iFileCacheBlock * FILE_CACHE_SIZE;
					uint64_t size	= iTotalSize - offset;

					if (FILE_CACHE_SIZE < size)
						size = FILE_CACHE_SIZE;

					iFile.seekg(iSubOffset + offset);
					iFile.read(iFileCache, size);
				}

				// Retrieve from cache
				uint64_t cacheReadOffset	= iPosition % FILE_CACHE_SIZE;
				uint64_t cacheReadSize		= FILE_CACHE_SIZE - cacheReadOffset;

				if (bytesLeft < cacheReadSize)
					cacheReadSize = bytesLeft;

				char* cBuff = (char*)readBuffer;

				memcpy(&cBuff[readSize - bytesLeft], &iFileCache[cacheReadOffset], cacheReadSize);

				bytesLeft -= cacheReadSize;
				iPosition += cacheReadSize;

				if (bytesLeft == 0)
					break;
			}

			//if (iFile.tellg() != iSubOffset + iPosition)
			//	iFile.seekg(iSubOffset + iPosition);
			//
			//iFile.read((char*)readBuffer, readSize);
		}
		else if (iSource == STREAM_SOURCE_BUFFER)
		{
			memcpy(readBuffer, &iBuffer[iSubOffset + iPosition], readSize);

			iPosition += readSize;
		}
		else if (iSource == STREAM_SOURCE_SUBSTREAM)
		{
			iSubStream->Seek(iSubOffset + iPosition);

			if (!iSubStream->Read(readBuffer, readSize))
			{
				error = true;
				return false;
			}

			iPosition += readSize;
		}

	}
	
	return true;
}

bool GothicLib::FileStream::ReadString(std::string& str)
{
	if (error)
	{
		return false;
	}

	uint16_t length;
	if (!Read(&length, sizeof(length)))
		return false;

	if (length > 0)
	{
		char* buff = new char[length];
		if (!Read(buff, length))
			return false;

		str = std::string(buff, length);
	}

	return true;
}

bool FileStream::ReadNullString(std::string& str)
{
	if (error)
	{
		return false;
	}

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
	if (error)
	{
		return false;
	}

	// Slow, but good enough.
	// Doesn't read the last line if it's empty
	// (because I cannot be bothered ¯\_(ツ)_/¯)

	char lineCache[LINE_CACHE_SIZE];

	uint64_t readSize = LINE_CACHE_SIZE;

	char c = 0;
	bool result = false;

	line = "";

	bool end = false;

	bool hasCR = false;
	bool hasLF = false;

	uint64_t startPos = Tell();

	while (true)
	{
		readSize = iTotalSize - iPosition;

		if (LINE_CACHE_SIZE < readSize)
			readSize = LINE_CACHE_SIZE;

		if (readSize == 0)
			break;

		if (!Read(&lineCache, readSize))
			break;

		result = true;

		size_t length = 0;

		for (size_t i = 0; i < readSize; i++)
		{
			c = lineCache[i];

			if (c == '\r')
			{
				length = i;

				hasCR = true;
			}
			else if (c == '\n')
			{
				end = true;

				if (!hasCR)
					length = i;

				hasLF = true;

				break;
			}
			else if (hasCR)
			{
				end = true;

				break;
			}
			else
			{
				length++;
			}
		}

		line += std::string(lineCache, length);

		if (end)
		{
			// Make sure the file position is correct
			uint64_t offsetBack = startPos + line.size();

			if (hasCR)
				offsetBack++;

			if (hasLF)
				offsetBack++;

			Seek(offsetBack);

			break;
		}
	}

	return result;
}

bool FileStream::Write(void* writeBuffer, uint64_t writeSize)
{
	if (error)
	{
		return false;
	}

	if (mode == STREAM_MODE_WRITE)
	{	
		// Read data depending on the source
		if (oSource == STREAM_SOURCE_RAWFILE)
		{
			oFile.write((char*)writeBuffer, writeSize);
		}
		else if (oSource == STREAM_SOURCE_BUFFER)
		{
			//memcpy(readBuffer, &iBuffer[iSubOffset + iPosition], readSize);
		}

		oPosition += writeSize;
	}
	
	return true;
}

bool FileStream::WriteString(std::string str)
{
	if (error)
	{
		return false;
	}

	uint16_t length = str.size();
	Write(&length, sizeof(length));

	const char* cStr = str.c_str();
	Write((void*)cStr, str.size());

	return true;
}

bool FileStream::WriteNullString(std::string str)
{
	if (error)
	{
		return false;
	}

	const char* cStr = str.c_str();
	Write((void*)cStr, str.size() + 1);

	return true;
}

bool FileStream::WriteLine(std::string line, std::string newLine)
{
	if (error)
	{
		return false;
	}

	std::string combStr = (line + newLine);

	const char* cStr = combStr.c_str();
	Write((void*)cStr, combStr.size());

	return true;
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
	if (mode == STREAM_MODE_WRITE)
	{
		oPosition = pos;

		if (oSource == STREAM_SOURCE_RAWFILE)
			oFile.seekp(oPosition);
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
	else if (mode == STREAM_MODE_WRITE)
	{
		return oPosition;
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
	else if (mode == STREAM_MODE_WRITE)
	{
		return oTotalSize;
	}
}

uint64_t FileStream::StartBinChunk(uint16_t type)
{
	uint64_t startPos = Tell();

	uint32_t length = 0;
	Write(FILE_ARGS(type));
	Write(FILE_ARGS(length));

	return startPos;
}

void FileStream::EndBinChunk(uint64_t startPos)
{
	uint64_t currPos = Tell();

	uint32_t length = currPos - startPos - 6;
	Seek(startPos + 2);
	Write(FILE_ARGS(length));

	Seek(currPos);
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
