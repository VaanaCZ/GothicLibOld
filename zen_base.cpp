#include "zen_base.h"
#include "zen_vdfs.h"
#include "zen_world.h"

using namespace GothicLib::ZenGin;


bool zCArchiver::Write(FileStream* _file, bool briefHeader)
{
	version = 1;
	//version = 0;

	file = _file;

	// Write header

	/*
		ZenGin Archive
	*/
	file->WriteLine("ZenGin Archive", "\n");

	/*
		ver 0 / ver 1
	*/
	if (version == 0)		file->WriteLine("ver 0", "\n");
	else if(version == 1)	file->WriteLine("ver 1", "\n");

	/*
		zCArchiverGeneric / zCArchiverBinSafe
	*/
	if (version == 1)
	{
		if (mode == ARCHIVER_MODE_BIN_SAFE)
			file->WriteLine("zCArchiverBinSafe", "\n");
		else
			file->WriteLine("zCArchiverGeneric", "\n");
	}

	/*
		ASCII / BINARY / BIN_SAFE
	*/
	if (mode == ARCHIVER_MODE_ASCII)
	{
		if (IsProperties())
			file->WriteLine("ASCII_PROPS", "\n");
		else
			file->WriteLine("ASCII", "\n");
	}
	else if (mode == ARCHIVER_MODE_BINARY)
		file->WriteLine("BINARY", "\n");
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
		file->WriteLine("BIN_SAFE", "\n");

	/*
		saveGame 0 / saveGame 1
	*/
	if (IsSavegame())	file->WriteLine("saveGame 1", "\n");
	else				file->WriteLine("saveGame 0", "\n");

	/*
		date d.m.Y H:M:S
	*/
	if (version == 0 || !briefHeader)
	{
		time_t timer;
		tm tmInfo;

		timer = time(NULL);
		if (localtime_s(&tmInfo, &timer) == 0)
		{
			file->WriteLine("date " + std::to_string(tmInfo.tm_mday) + "."
									+ std::to_string(tmInfo.tm_mon + 1) + "."
									+ std::to_string(tmInfo.tm_year + 1900) + " "
									+ std::to_string(tmInfo.tm_hour) + ":"
									+ std::to_string(tmInfo.tm_min) + ":"
									+ std::to_string(tmInfo.tm_sec), "\n");
		}
	}
	
	if (version == 1 && !briefHeader)
	{
		/*
			user x
		*/
		file->WriteLine("user GothicLib", "\n");
	}
	else if (version == 0)
	{
		objectCountPos = file->Tell() + 8; // "objects "

		/*
			objects n
		*/
		file->WriteLine("objects          ", "\n");
	}

	/*
			END
	*/
	file->WriteLine("END", "\n");

	if (mode != ARCHIVER_MODE_BIN_SAFE)
	{
		if (version == 1)
		{
			objectCountPos = file->Tell() + 8; // "objects "

			/*
				objects n
			*/
			file->WriteLine("objects          ", "\n");

			/*
					END
			*/
			file->WriteLine("END", "\n");
		}

		file->WriteLine("", "\n");
	}



	return true;
}

//
// bool Read(FileStream* _file)
// 
// Opens a file stream for reading as a
// ZenGin Archive.
// 
//   - _file
//         file stream to use.
//
bool zCArchiver::Read(FileStream* _file)
{
	file = _file;

	// Read header
	std::string line;

	/*
		ZenGin Archive
	*/
	file->ReadLine(line);

	if (line != "ZenGin Archive")
	{
		LOG_ERROR("The specified stream is not a valid ZenGin archive!");
		return false;
	}

	// Read lines
	bool first = true;

	while (true)
	{
		if (!file->ReadLine(line))
		{
			LOG_ERROR("Unexpected end of file while reading a ZenGin archive!");
			return false;
		}

		/*
			ver 0 / ver 1
		*/
		if (line == "ver 0")			version = 0;
		else if (line == "ver 1")		version = 1;

		/*
			ASCII / BINARY / BIN_SAFE
		*/
		if (line == "BINARY")			mode = ARCHIVER_MODE_BINARY;
		else if (line == "ASCII")		mode = ARCHIVER_MODE_ASCII;
		else if (line == "BIN_SAFE")	mode = ARCHIVER_MODE_BIN_SAFE;
		else if (line == "ASCII_PROPS")
		{
			mode = ARCHIVER_MODE_ASCII;
			properties = true;
		}

		/*
			saveGame 0 / saveGame 1
		*/
		if (line == "saveGame 0")		savegame = false;
		else if (line == "saveGame 1")	savegame = true;

		/*
			objects n
		*/
		if (line.find("objects ") == 0)
		{
			size_t objectCount = std::stoul(line.substr(8));

			if (objectCount == 0)
			{
				LOG_WARN("The specified ZenGin Archive doesn't contain any objects.");
				return true;
			}

			objectList.resize(objectCount);
		}

		/*
			END
		*/
		if (line == "END")
		{
			if (first && version == 1 &&
				mode != ARCHIVER_MODE_BIN_SAFE)
			{
				first = false;
			}
			else
			{
				break;
			}
		}
	}

	/*
		*empty line*
	*/
	if (mode != ARCHIVER_MODE_BIN_SAFE)
	{
		file->ReadLine(line);

		if (!line.empty())
		{
			LOG_WARN("Expected an empy line after ZenGin archive header, got \"" + line + "\" instead.");
		}
	}
	else
	{
		BinSafeArchiveHeader header;
		if (!file->Read(&header, sizeof(header)))
		{
			LOG_ERROR("Unexpected end of file while reading a BinSafe header!");
			return false;
		}

		if (header.version != 2)
		{
			LOG_ERROR("Unknown BinSafe archive version, expected 2, got " + std::to_string(header.version));
			return false;
		}

		objectList.resize(header.objectCount);

		uint64_t startPos = file->Tell();

		// Read chunk list
		file->Seek(header.chunkPos);

		uint32_t chunkCount;
		if (!file->Read(&chunkCount, sizeof(chunkCount)))
		{
			LOG_ERROR("Unexpected end of file while reading a BinSafe header!");
			return false;
		}

		BinSafeHashTable hash;
		char text[8192];

		for (size_t i = 0; i < chunkCount; i++)
		{
			if (!file->Read(&hash, sizeof(hash)))
			{
				LOG_ERROR("Unexpected end of file while reading a BinSafe hash table!");
				return false;
			}

			uint32_t textLength = hash.stringLength;
			if (textLength > 8192)
				textLength = 8192;

			if (!file->Read(&text, textLength))
			{
				LOG_ERROR("Unexpected end of file while reading a BinSafe hash table!");
				return false;
			}
			
			
			std::string t = std::string(text, textLength);

			if (t.find("childs") != 0)
			{
				LOG_DEBUG(t);
			}

			// Todo: save values
		}

		file->Seek(startPos);
	}

	if (objectList.size() == 0)
	{
		LOG_WARN("Loaded archive contains no objects!");
	}

	// Mark the start of the reading area
	CHUNK chunk;
	chunk.objectOffset = file->Tell();

	chunkStack.push_back(chunk);


	return true;
}

void zCArchiver::EndWrite()
{
	// Write object count
	uint64_t currPos = file->Tell();

	file->Seek(objectCountPos);

	if (mode == ARCHIVER_MODE_BIN_SAFE)
	{

	}
	else
	{
		std::string strCount = std::to_string(objectList.size());
		file->Write((char*)strCount.c_str(), strCount.size()); // Yes, this is correct.
	}

	file->Seek(currPos);
}

void zCArchiver::Close()
{
	EndWrite(); // temp

	// Close file
	file->Close();
}

bool zCArchiver::WriteInt(std::string name, int intVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		return WriteASCIIProperty(name, "int", std::to_string(intVal));
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Write(&intVal, sizeof(intVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_INTEGER, &intVal);
	}

	if (mode != ARCHIVER_MODE_ASCII)
		error = true;

	return false;
}

bool zCArchiver::WriteByte(std::string name, unsigned char byteVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		return WriteASCIIProperty(name, "int", std::to_string(byteVal));
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Write(&byteVal, sizeof(byteVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_BYTE, &byteVal);
	}

	if (mode != ARCHIVER_MODE_ASCII)
		error = true;

	return false;
}

bool zCArchiver::WriteWord(std::string name, unsigned short wordVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		return WriteASCIIProperty(name, "int", std::to_string(wordVal));
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Write(&wordVal, sizeof(wordVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_WORD, &wordVal);
	}

	if (mode != ARCHIVER_MODE_ASCII)
		error = true;

	return false;
}

bool zCArchiver::WriteFloat(std::string name, float floatVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		return WriteASCIIProperty(name, "float", FloatToString(floatVal));
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Write(&floatVal, sizeof(floatVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_FLOAT, &floatVal);
	}

	if (mode != ARCHIVER_MODE_ASCII)
		error = true;

	return false;
}

bool zCArchiver::WriteBool(std::string name, zBOOL boolVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		return WriteASCIIProperty(name, "bool", std::to_string(boolVal));
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		char byteVal = boolVal;

		return file->Write(&byteVal, sizeof(byteVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_BOOL, &boolVal);
	}

	if (mode != ARCHIVER_MODE_ASCII)
		error = true;

	return false;
}

bool zCArchiver::WriteString(std::string name, std::string strVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		return WriteASCIIProperty(name, "string", strVal);
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->WriteNullString(strVal);
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_STRING, &strVal);
	}

	if (mode != ARCHIVER_MODE_ASCII)
		error = true;

	return false;
}

bool zCArchiver::WriteVec3(std::string name, zVEC3 vecVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value = FloatToString(vecVal.x) + " " +
							FloatToString(vecVal.y) + " " +
							FloatToString(vecVal.z);

		return WriteASCIIProperty(name, "vec3", value);
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Write(&vecVal, sizeof(vecVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_VEC3, &vecVal);
	}

	if (mode != ARCHIVER_MODE_ASCII)
		error = true;

	return false;
}

bool zCArchiver::WriteColor(std::string name, zCOLOR colorVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		int vals[4];
		vals[0] = colorVal.b;
		vals[1] = colorVal.g;
		vals[2] = colorVal.r;
		vals[3] = colorVal.a;

		char buffer[16];
		sprintf_s(buffer, 16, "%d %d %d %d", vals[0], vals[1], vals[2], vals[3]);

		return WriteASCIIProperty(name, "color", std::string(buffer));
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Write(&colorVal, sizeof(colorVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_COLOR, &colorVal);
	}

	return false;
}

bool zCArchiver::WriteRaw(std::string name, char* buffer, size_t bufferSize)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;

		char charVal[3];
		unsigned int rawValue = 0;

		for (size_t i = 0; i < bufferSize; i++)
		{
			rawValue = (unsigned char)buffer[i];
			sprintf_s(charVal, "%02x", rawValue);
			value += charVal;
		}

		return WriteASCIIProperty(name, "raw", value);
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Write(buffer, bufferSize);
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_RAW, buffer, bufferSize);
	}

	return false;
}

bool zCArchiver::WriteRawFloat(std::string name, float* floatVals, size_t floatCount)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value = "";

		for (size_t i = 0; i < floatCount; i++)
		{
			value += FloatToString(floatVals[i]) + " ";
		}

		return WriteASCIIProperty(name, "rawFloat", value);
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Write(floatVals, floatCount * sizeof(float));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_RAWFLOAT, floatVals, floatCount * sizeof(float));
	}

	return false;
}

bool zCArchiver::WriteEnum(std::string name, std::string choices, int enumVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string type = "enum";

		if (version == 0 || IsProperties())
		{
			type += ";" + choices;
		}

		return WriteASCIIProperty(name, type, std::to_string(enumVal));
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		char byteVal = enumVal;

		return file->Write(&byteVal, sizeof(byteVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return WriteBinSafeProperty(BINSAFE_TYPE_ENUM, &enumVal);
	}

	return false;
}

bool zCArchiver::WriteObject(std::string name, zCObject* object)
{
	// Stop early if its a nullptr
	if (object == nullptr)
	{
		if (!WriteChunkStart(name, "%", 0, 0) ||
			!WriteChunkEnd())
		{
			error = true;
			return false;
		}

		return true;
	}

	// Find out if the object is in the list
	for (size_t i = 0; i < objectList.size(); i++)
	{
		if (objectList[i] == object)
		{
			if (!WriteChunkStart(name, "§", 0, i) ||
				!WriteChunkEnd())
			{
				error = true;
				return false;
			}

			return true;
		}
	}

	uint32_t objectIndex = objectList.size();
	objectList.push_back(object);

	// Figure out the full class name
	ClassDefinition* classDef = object->GetClassDef();

	std::string	className = classDef->GetName();
	uint16_t	versionSum = classDef->GetVersionSum(game);

	while (true)
	{
		classDef = classDef->GetBaseDef();

		if (!classDef)
			break;

		if (!classDef->IsAbstract())
			className += ":" + classDef->GetName();
	}

	// Write chunk
	if (!WriteChunkStart(name, className, versionSum, objectIndex) ||
		!object->Archive(this) ||
		!WriteChunkEnd())
	{
		error = true;
		return false;
	}

	return true;
}

bool zCArchiver::WriteChunkStart(std::string objectName, std::string className, uint16_t classVersion, uint32_t objectIndex)
{
	CHUNK chunk;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string output = "";

		// Tabs
		if (chunkStack.size() != 0)
		{
			output.resize(chunkStack.size());

			for (size_t i = 0; i < chunkStack.size(); i++)
			{
				output[i] = '\t';
			}
		}

		// Chunk start
		output += "[" + (objectName.empty() ? "%" : objectName) + " " +
						(className.empty() ? "%" : className) + " " +
						std::to_string(classVersion) + " " +
						std::to_string(objectIndex) + "]";

		file->WriteLine(output, "\n");

		chunk.objectOffset = file->Tell();
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		chunk.objectOffset = file->Tell();

		BinaryObjectHeader binHeader;
		binHeader.objectSize	= 0;
		binHeader.classVersion	= classVersion;
		binHeader.objectIndex	= objectIndex;

		if (!file->Write(FILE_ARGS(binHeader)))
		{
			return false;
		}

		if (!file->WriteNullString((objectName.empty() ? "%" : objectName)))
			return false;

		if (!file->WriteNullString((className.empty() ? "%" : className)))
			return false;
	}

	// Mark that we entered a new chunk
	chunk.objectName	= (objectName.empty() ? "%" : objectName);
	chunk.className		= (className.empty() ? "%" : className);
	chunk.objectIndex	= objectIndex;
	
	chunkStack.push_back(chunk);

	return true;
}

bool zCArchiver::WriteChunkEnd()
{
	// Mark that we left a chunk
	CHUNK& chunk = chunkStack[chunkStack.size() - 1];

	// Read end
	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string output = "";

		// Tabs
		size_t tabs = chunkStack.size() - 1;

		if (tabs != 0)
		{
			output.resize(tabs);

			for (size_t i = 0; i < tabs; i++)
			{
				output[i] = '\t';
			}
		}

		// Chunk start
		output += "[]";

		file->WriteLine(output, "\n");
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		uint64_t currPos = file->Tell();

		file->Seek(chunk.objectOffset);

		uint32_t chunkSize = currPos - chunk.objectOffset;
		file->Write(FILE_ARGS(chunkSize));

		file->Seek(currPos);
	}

	chunkStack.pop_back();

	return true;
}

bool zCArchiver::WriteGroupBegin(std::string name)
{
	if (mode == ARCHIVER_MODE_ASCII &&
		IsProperties())
	{
		return WriteASCIIProperty(name, "groupBegin", "");
	}

	return true;
}

bool zCArchiver::WriteGroupEnd(std::string name)
{
	if (mode == ARCHIVER_MODE_ASCII &&
		IsProperties())
	{
		return WriteASCIIProperty(name, "groupEnd", "");
	}

	return true;
}

bool zCArchiver::ReadInt(std::string name, int& intVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "int", value))
		{
			intVal = std::stoi(value);
			return true;
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Read(&intVal, sizeof(intVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_INTEGER, &intVal);
	}

	return false;
}

bool zCArchiver::ReadByte(std::string name, unsigned char& byteVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "int", value))
		{
			byteVal = std::stoi(value);
			return true;
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Read(&byteVal, sizeof(byteVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_BYTE, &byteVal);
	}

	return false;
}

bool zCArchiver::ReadWord(std::string name, unsigned short& wordVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "int", value))
		{
			wordVal = std::stoi(value);
			return true;
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Read(&wordVal, sizeof(wordVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_WORD, &wordVal);
	}

	return false;
}

bool zCArchiver::ReadFloat(std::string name, float& floatVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "float", value))
		{
			floatVal = std::stof(value);
			return true;
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Read(&floatVal, sizeof(floatVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_FLOAT, &floatVal);
	}

	return false;
}

bool zCArchiver::ReadBool(std::string name, zBOOL& boolVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "bool", value))
		{
			boolVal = std::stoi(value);
			return true;
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		char byteVal;
		
		if (!file->Read(&byteVal, sizeof(byteVal)))
			return false;

		boolVal = byteVal;

		return true;
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_BOOL, &boolVal);
	}

	return false;
}

bool zCArchiver::ReadString(std::string name, std::string& strVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "string", value))
		{
			strVal = value;
			return true;
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->ReadNullString(strVal);
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_STRING, &strVal);
	}

	return false;
}

bool zCArchiver::ReadVec3(std::string name, zVEC3& vecVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "vec3", value) &&
			sscanf_s(value.c_str(), "%f %f %f",
				&vecVal.x, &vecVal.y, &vecVal.z) == 3)
		{
			return true;
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Read(&vecVal, sizeof(vecVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_VEC3, &vecVal);
	}

	return false;
}

bool zCArchiver::ReadColor(std::string name, zCOLOR& colorVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		int vals[4];
		std::string value;
		if (ReadASCIIProperty(name, "color", value) &&
			sscanf_s(value.c_str(), "%d %d %d %d",
				&vals[0], &vals[1], &vals[2], &vals[3]) == 4)
		{
			colorVal.b = vals[0];
			colorVal.g = vals[1];
			colorVal.r = vals[2];
			colorVal.a = vals[3];

			return true;
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Read(&colorVal, sizeof(colorVal));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_COLOR, &colorVal);
	}

	return false;
}

bool zCArchiver::ReadRaw(std::string name, char* buffer, size_t bufferSize)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "raw", value) && value.size() / 2 == bufferSize)
		{
			// Read the data
			int	byteValue = 0;
			int byteCount = 0;
			for (int i = 0; i < value.length(); i += 2)
			{
				if (sscanf_s(&value[i], "%2x", &byteValue) != 1)
				{
					LOG_WARN("Malformed byte in \"" + name + "\" property!");
					break;
				}

				buffer[byteCount] = (unsigned char)byteValue;
				byteCount++;
			}

			return true;
		}		
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Read(buffer, bufferSize);
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_RAW, buffer, bufferSize);
	}

	return false;
}

bool zCArchiver::ReadRawFloat(std::string name, float* floatVals, size_t floatCount)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "rawFloat", value))
		{
			// Count floats
			size_t archivedFoatCount = 0;

			size_t p = 0;
			while (true)
			{
				p = value.find(" ", p + 1);
				if (p != std::string::npos)
					archivedFoatCount++;
				else
					break;
			}

			if (floatCount == archivedFoatCount)
			{
				// Read the data
				size_t offset = 0;

				for (size_t i = 0; i < floatCount; i++)
				{
					floatVals[i] = atof(&value[offset]);
					offset = value.find(" ", offset) + 1;
				}

				return true;
			}
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		return file->Read(floatVals, floatCount * sizeof(float));
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_RAWFLOAT, floatVals, floatCount * sizeof(float));
	}

	return false;
}

bool zCArchiver::ReadEnum(std::string name, int& enumVal)
{
	if (error)
		return false;

	if (mode == ARCHIVER_MODE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "enum", value))
		{
			enumVal = std::stoi(value);
			return true;
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		char byteVal;
		
		if (!file->Read(&byteVal, sizeof(byteVal)))
			return false;

		enumVal = byteVal;

		return true;
	}
	else if (mode == ARCHIVER_MODE_BIN_SAFE)
	{
		return ReadBinSafeProperty(BINSAFE_TYPE_ENUM, &enumVal);
	}

	return false;
}

zCObject* zCArchiver::ReadObject(std::string name, zCObject* existingObject)
{
	if (error)
		return nullptr;

	zCObject* object = nullptr;

	// Read object header
	std::string className;
	uint16_t classVersion;
	uint32_t objectIndex;

	if (!ReadChunkStart(&name, &className, &classVersion, &objectIndex))
	{
		error = true;
		return nullptr;
	}

	// Error checking
	if (objectIndex < 0 || objectIndex >= objectList.size())
	{
		LOG_ERROR("The specified object index is not within an accepted range! Archive must be invalid!");

		error = true;
		return nullptr;
	}

	if (className == "%")
	{
		// % = nullptr
		object = nullptr;
	}
	else if (className == "§")
	{
		// § = existing object in list
		object = objectList[objectIndex];

		if (existingObject != nullptr &&
			object != existingObject)
		{
			object = existingObject;
		}
	}
	else
	{
		std::string truncClassName = className;

		if (truncClassName.find(":") != std::string::npos)
			truncClassName = truncClassName.substr(0, truncClassName.find(":"));

		ClassDefinition* classDef = ClassDefinition::GetClassDef(truncClassName);

		if (!classDef)
		{
			LOG_ERROR("Class \"" + className + "\" is not supported!");

			error = true;
			ReadChunkEnd();
			return nullptr;
		}

		// Figure out if the class is supported			
		if (!classDef->IsVersionSumSupported(game, classVersion))
		{
			uint16_t versionSum = classDef->GetVersionSum(game);

			if (versionSum == VERSION_NONE)
			{
				LOG_ERROR("Class " + classDef->GetName() + " is not supported in the specified engine revision!");
			}
			else
			{
				LOG_ERROR("Error while reading class " + classDef->GetName() + ". The version is "
					+ std::to_string(classVersion) + ", expected " + std::to_string(versionSum) + "!");
			}

			error = true;
			ReadChunkEnd();
			return nullptr;
		}

		if (existingObject)
		{
			// Use the specified object if it is the 
			// same class.
			std::string name = className;
			
			if (name.find(":") != std::string::npos)
				name = name.substr(0, name.find(":"));

			if (existingObject->GetClassDef()->GetName() == name)
			{
				object = existingObject;
			}
			else
			{
				LOG_ERROR("Read class does not match existing object. Expected \"" +
					existingObject->GetClassDef()->GetName() + "\", found \"" + name + "\" instead!");

				error = true;
				ReadChunkEnd();
				return nullptr;
			}
		}
		else
		{
			// Create object based on read type
			object = classDef->CreateInstance();

			object->game = game;
		}

		objectList[objectIndex] = object;

		if (!object->Unarchive(this))
		{
			error = true;
			return nullptr;
		}

		if (error)
		{
			LOG_ERROR("Error while reading class \"" + className +"\"");
			return nullptr;
		}
	}

	if (!ReadChunkEnd())
	{
		error = true;
		return nullptr;
	}

	return object;
}

bool zCArchiver::ReadChunkStart(std::string* _objectName, std::string* _className, uint16_t* _classVersion, uint32_t* _objectIndex)
{
	if (error)
		return false;

	BinaryObjectHeader binHeader;

	std::string objectName;
	std::string className; 
	uint16_t classVersion;
	uint32_t objectIndex;

	uint64_t startPos = 0;

	if (mode == ARCHIVER_MODE_ASCII ||
		mode == ARCHIVER_MODE_BIN_SAFE)
	{
		std::string line;

		if (mode == ARCHIVER_MODE_BIN_SAFE)
		{
			if (!ReadBinSafeProperty(BINSAFE_TYPE_STRING, &line))
				return false;
		}
		else
		{
			// While in ASCII mode, it is not always guaranteed
			// that the chunks/properties will be in the correct
			// order. Therefore we need to go through the entirity
			// of the current chuck and find the one that matches
			// the specified criteria.

			bool first = true;

			bool looping = false;
			uint64_t startPos = file->Tell();

			int nesting = 0;

			bool valid = false;

			while (file->ReadLine(line))
			{
				bool isHeader = false;

				if (looping && file->Tell() >= startPos)
				{
					break;
				}

				// Figure out nesting
				size_t tabCount = 0;

				for (size_t i = 0; i < line.size(); i++)
				{
					if (line[i] != '\t')
					{
						tabCount = i;
						break;
					}
				}

				if (line.size() >= tabCount + 2 &&
					line[tabCount] == '[')
				{
					if (line[tabCount + 1] == ']')
						nesting--;
					else
					{
						isHeader = true;
						nesting++;
					}
				}

				// If we have closed the current chunk but
				// haven't found any other, the search failed.
				if (nesting < 0)
				{
					nesting = 0;
					looping = true;

					// Go to start
					file->Seek(chunkStack[chunkStack.size() - 1].objectOffset);

					continue;
				}
				else if (nesting == 1)
				{
					size_t sPos = line.find(" ");

					if (isHeader && sPos != std::string::npos)
					{
						if (objectName.empty())
						{
							valid = true;
							break;
						}
						else
						{
							std::string chunkName = line.substr(tabCount + 1, sPos - tabCount - 1);

							if (objectName == chunkName)
							{
								valid = true;
								break;
							}
						}
					}

					if (first)
					{
						first = false;
						LOG_WARN("Incorrect chunk order, expected \"" + objectName + "\"!");
					}
				}				
			}

			if (!valid)
			{
				LOG_ERROR("No chunk matching the desired criteria has been found!");
				return false;
			}
		}

		size_t p1 = line.find("[");
		size_t p2 = line.find(" ", p1 + 1);
		size_t p3 = line.find(" ", p2 + 1);
		size_t p4 = line.find(" ", p3 + 1);

		objectName		= line.substr(p1 + 1, p2 - p1 - 1);
		className		= line.substr(p2 + 1, p3 - p2 - 1);
		classVersion	= atoi(&line[p3]);
		objectIndex		= atoi(&line[p4]);

		startPos = file->Tell();
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		startPos = file->Tell();

		if (!file->Read(&binHeader, sizeof(binHeader)))
			return false;

		std::string readObjectName;
		if (!file->ReadNullString(readObjectName))
			return false;

		std::string readClassName;
		if (!file->ReadNullString(readClassName))
			return false;

		objectName		= readObjectName;
		className		= readClassName;
		classVersion	= binHeader.classVersion;
		objectIndex		= binHeader.objectIndex;
	}

	if (objectIndex >= objectList.size())
	{
		return false;
	}

	// Mark that we entered a new chunk
	CHUNK chunk;
	chunk.objectName	= objectName;
	chunk.className		= className;
	chunk.objectIndex	= objectIndex;
	chunk.objectOffset	= startPos;
	
	if (mode == ARCHIVER_MODE_BINARY)
	{
		chunk.binObjectSize = binHeader.objectSize;
	}

	chunkStack.push_back(chunk);

	if (_objectName != nullptr)
		*_objectName = objectName;

	if (_className != nullptr)
		*_className = className;

	if (_classVersion != nullptr)
		*_classVersion = classVersion;

	if (_objectIndex != nullptr)
		*_objectIndex = objectIndex;

	return true;
}

bool zCArchiver::ReadChunkEnd()
{
	if (mode == ARCHIVER_MODE_ASCII ||
		mode == ARCHIVER_MODE_BIN_SAFE)
	{
		std::string line;

		if (mode == ARCHIVER_MODE_BIN_SAFE)
		{
			if (!ReadBinSafeProperty(BINSAFE_TYPE_STRING, &line))
				return false;
		}
		else
		{
			int nesting = 0;

			bool valid = false;
			bool first = true;

			while (file->ReadLine(line))
			{
				// Figure out nesting
				size_t tabCount = 0;

				for (size_t i = 0; i < line.size(); i++)
				{
					if (line[i] != '\t')
					{
						tabCount = i;
						break;
					}
				}

				if (line.size() >= tabCount + 2 &&
					line[tabCount] == '[')
				{
					if (line[tabCount + 1] == ']')
						nesting--;
					else
						nesting++;
				}

				// End reached
				if (nesting < 0)
				{
					valid = true;
					break;
				}

				if (!error)
				{
					if (first)
					{
						first = false;
						LOG_WARN("Chunk end expected, but data found. The following properties will be skipped:");
					}

					LOG_WARN(line.substr(tabCount));
				}
			}

			if (!valid)
			{
				LOG_ERROR("End of the file reached, but the chunk remains open. This archive is invalid!");
				return false;
			}
		}
	}
	else if (mode == ARCHIVER_MODE_BINARY)
	{
		CHUNK& chunk = chunkStack[chunkStack.size() - 1];

		if ((chunk.objectOffset + chunk.binObjectSize) != file->Tell())
		{
			uint64_t wrongSize = file->Tell() - chunk.objectOffset;

			LOG_ERROR("Incorrect chunk size! Expected " + std::to_string(chunk.binObjectSize) +
				", got " + std::to_string(wrongSize) + " instead!");
			return false;
		}
	}

	if (error)
	{
		LOG_ERROR("An error occured while reading chunk.");
		error = false;
	}

	// Mark that we left a chunk
	chunkStack.pop_back();

	return true;
}

bool zCArchiver::WriteASCIIProperty(std::string name, std::string type, std::string value)
{
	// Tabs
	std::string output = "";

	if (chunkStack.size() != 0)
	{
		output.resize(chunkStack.size());

		for (size_t i = 0; i < chunkStack.size(); i++)
		{
			output[i] = '\t';
		}
	}

	// Chunk start
	output += name + "=" + type + ":" + value;

	return file->WriteLine(output, "\n");
}

bool zCArchiver::WriteBinSafeProperty(BINSAFE_TYPE type, void* data, size_t size)
{
	// First read the type
	char writeType = type;
	if (!file->Write(&writeType, sizeof(writeType)))
		return false;

	// Proceed based on type
	switch (type)
	{
	case BINSAFE_TYPE_STRING:
	{
		std::string* strVal = (std::string*)data;

		if (!file->WriteString(*strVal))
			return false;

		break;
	}
		
	case BINSAFE_TYPE_INTEGER:
	{
		if (!file->Write(data, sizeof(int)))
			return false;

		break;
	}

	case BINSAFE_TYPE_FLOAT:
	{
		if (!file->Write(data, sizeof(float)))
			return false;

		break;
	}

	case BINSAFE_TYPE_BOOL:
	{
		if (!file->Write(data, sizeof(bool)))
			return false;

		break;
	}

	case BINSAFE_TYPE_VEC3:
	{
		if (!file->Write(data, sizeof(zVEC3)))
			return false;

		break;
	}

	case BINSAFE_TYPE_COLOR:
	{
		if (!file->Write(data, sizeof(zCOLOR)))
			return false;

		break;
	}

	case BINSAFE_TYPE_RAW:
	{
		uint16_t length;
		if (!file->Write(&length, sizeof(length)))
			return false;

		if (length != size)
			return false;

		if (!file->Write(data, length))
			return false;

		break;
	}

	case BINSAFE_TYPE_RAWFLOAT:
	{
		uint16_t length;
		if (!file->Write(&length, sizeof(length)))
			return false;

		if (length != size)
			return false;

		if (!file->Write(data, length))
			return false;

		break;
	}

	case BINSAFE_TYPE_ENUM:
	{
		if (!file->Write(data, sizeof(int)))
			return false;

		break;
	}

	default:
		break;
	}

	return true;
}

bool zCArchiver::ReadASCIIProperty(std::string name, std::string type, std::string& value)
{
	// While in ASCII mode, it is not always guaranteed
	// that the chunks/properties will be in the correct
	// order. Therefore we need to go through the entirity
	// of the current chuck and find the one that matches
	// the specified criteria.

	// Go to the start of the current chunk before we
	// perform the search.

	bool first = true;

	bool looping = false;
	uint64_t startPos = file->Tell();

	int nesting = 0;
	
	std::string line;
	while (file->ReadLine(line))
	{
		if (looping && file->Tell() >= startPos)
		{
			break;
		}

		// Figure out nesting
		size_t tabCount = 0;

		for (size_t i = 0; i < line.size(); i++)
		{
			if (line[i] != '\t')
			{
				tabCount = i;
				break;
			}
		}

		if (line.size() >= tabCount + 2 &&
			line[tabCount] == '[')
		{
			if (line[tabCount + 1] == ']')
				nesting--;
			else
				nesting++;
		}

		// If we have closed the current chunk but
		// haven't found any other, the search failed.
		if (nesting < 0)
		{
			nesting = 0;
			looping = true;

			// Go to start
			file->Seek(chunkStack[chunkStack.size() - 1].objectOffset);

			continue;
		}
		else if (nesting > 0)
		{
			continue;
		}

		// Unpack line
		size_t ePos = line.find("=", tabCount);
		size_t cPos = line.find(":", ePos + 1);

		if (ePos == std::string::npos || cPos == std::string::npos)
		{
			continue;
		}

		std::string readName	= line.substr(tabCount, ePos - tabCount);
		std::string readType	= line.substr(ePos + 1, cPos - ePos - 1);
		std::string readValue	= line.substr(cPos + 1);

		// Props and version 0
		size_t semiColon = readType.find(";");
		if (semiColon != std::string::npos)
			readType = readType.substr(0, semiColon);

		if (readType != "groupBegin" && // ASCII_PROPS
			readType != "groupEnd")
		{
			if (readName == name &&
				readType == type)
			{
				value = readValue;
				return true;
			}
			else if (first)
			{
				first = false;

				if (looping)
				{
					LOG_WARN("Incorrect property order. Expected \"" + name + "\" of type \"" +
						type + "\", but the end of chunk was reached!");
				}
				else
				{
					LOG_WARN("Incorrect property order. Expected \"" + name + "\" of type \"" +
						type + "\", found \"" + readName + "\" of type \"" + readType + "\" instead!");
				}
			}
		}
	}

	LOG_WARN("Specified property not found in chunk!");
	return false;
}

bool zCArchiver::ReadBinSafeProperty(BINSAFE_TYPE type, void* data, size_t size)
{
	while (true)
	{
		// First read the type
		char readType;
		if (!file->Read(&readType, sizeof(readType)))
			return false;

		if (readType != type)
		{
			if (readType == BINSAFE_TYPE_HASH)
			{
				uint32_t key;
				file->Read(&key, sizeof(key));
				continue;
			}
			else
			{
				return false;
			}
		}

		// Proceed based on type
		switch (readType)
		{
		case BINSAFE_TYPE_STRING:
		{
			std::string* strVal = (std::string*)data;

			return file->ReadString(*strVal);
		}
		
		case BINSAFE_TYPE_INTEGER:
		{
			return file->Read(data, sizeof(int));
		}

		case BINSAFE_TYPE_FLOAT:
		{
			return file->Read(data, sizeof(float));
		}

		case BINSAFE_TYPE_BOOL:
		{
			return file->Read(data, sizeof(bool));
		}

		case BINSAFE_TYPE_VEC3:
		{
			return file->Read(data, sizeof(zVEC3));
		}

		case BINSAFE_TYPE_COLOR:
		{
			return file->Read(data, sizeof(zCOLOR));
		}

		case BINSAFE_TYPE_RAW:
		{
			uint16_t length;
			if (!file->Read(&length, sizeof(length)))
				return false;

			if (length != size)
				return false;

			return file->Read(data, length);
		}

		case BINSAFE_TYPE_RAWFLOAT:
		{
			uint16_t length;
			if (!file->Read(&length, sizeof(length)))
				return false;

			if (length != size)
				return false;

			return file->Read(data, length);
		}

		case BINSAFE_TYPE_ENUM:
		{
			return file->Read(data, sizeof(int));
		}

		default:
			break;
		}
	}

	return true;
}

ClassDefinition::ClassDefinition(std::string _name, std::string _base, zCObject* (*_createFunc)(),
	CLASS_REVISION* _revisions, size_t _revisionCount)
{
	name			= _name;
	base			= _base;
	createFunc		= _createFunc;
	revisions		= _revisions;
	revisionCount	= _revisionCount;

	if (!classList)
	{
		classList = new std::unordered_map<std::string, ClassDefinition*>();
	}

	(*classList)[name] = this;
}

ClassDefinition::~ClassDefinition()
{
}

ClassDefinition* ClassDefinition::GetClassDef(std::string name)
{
	if (classList->find(name) != classList->end())
	{
		ClassDefinition* classDef = (*classList)[name];

		if (!classDef->baseDef)
		{
			ClassDefinition* currClassDef = classDef;

			while (true)
			{
				if (currClassDef->GetBase().empty() ||
					currClassDef->GetBaseDef())
				{
					break;
				}

				currClassDef->baseDef = (*classList)[currClassDef->base];
				currClassDef = currClassDef->baseDef;
			}
		}

		return classDef;
	}

	return nullptr;
}

uint16_t ClassDefinition::GetVersionSum(GAME game)
{
	for (size_t i = 0; i < revisionCount; i++)
	{
		if (revisions[i].game == game ||
			revisions[i].game == GAME_ALL)
		{
			return revisions[i].versionSum;
		}
	}

	return VERSION_NONE;
}

bool ClassDefinition::IsVersionSumSupported(GAME game, uint16_t versionSum)
{
	for (size_t i = 0; i < revisionCount; i++)
	{
		if (revisions[i].game == game &&
			revisions[i].versionSum == versionSum)
		{
			return true;
		}
	}

	return false;
}

//
// bool ParseFileLine(std::string _line, std::string& key, std::string& value)
// 
// Parses the line of a legacy file (primarily PWF)
// 
//   - _file
//         file stream to use.
// 
//   - key
//         read key.
// 
//   - value
//         read value.
//
bool zCObject::ParseFileLine(std::string _line, std::string& key, std::string& value)
{
	std::string line = _line;

	for (size_t i = 0; i < line.size(); i++)
	{
		if (line[i] != ' ')
		{
			if (i != 0)
				line = line.substr(i);
			break;
		}
	}

	size_t sPos = line.find(" (");
	size_t ePos = line.find(")", sPos + 2);

	if (sPos == std::string::npos ||
		ePos == std::string::npos)
	{
		return false;
	}

	key = line.substr(0, sPos);
	value = line.substr(sPos + 2, ePos - sPos - 2);

	return true;
}
