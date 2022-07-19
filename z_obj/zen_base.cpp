#include "zen_base.h"
#include "zen_vdfs.h"
#include "zen_world.h"

using namespace GothicLib::ZenGin;

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
		if (line == "BINARY")			type = ARCHIVER_TYPE_BINARY;
		else if (line == "ASCII")		type = ARCHIVER_TYPE_ASCII;
		else if (line == "BIN_SAFE")	type = ARCHIVER_TYPE_BIN_SAFE;
		else if (line == "ASCII_PROPS")
		{
			type = ARCHIVER_TYPE_ASCII;
			props = true;
		}

		/*
			saveGame 0 / saveGame 1
		*/
		if (line == "saveGame 0")		savegame = false;
		else if (line == "saveGame 1")	savegame = true;

		/*
			objects n
		*/
		if (line.substr(0, 8) == "objects ")
		{
			objectCount = std::stoul(line.substr(8));

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
			if (first &&
				type != ARCHIVER_TYPE_BIN_SAFE &&
				objectCount == -1)
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
	if (type != ARCHIVER_TYPE_BIN_SAFE)
	{
		file->ReadLine(line);

		if (!line.empty())
		{
			LOG_WARN("Expected an empy line after ZenGin archive header, got \"" + line + "\" instead.");
		}

		if (type == ARCHIVER_TYPE_ASCII)
		{
			asciiChunksPositions.push_back(file->Tell());
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

		objectCount = header.objectCount;

		objectList.resize(objectCount);

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

			// Todo: save values
		}

		file->Seek(startPos);
	}
	
	return true;
}

bool zCArchiver::ReadInt(std::string name, int& intVal)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "int", value))
		{
			intVal = std::stoi(value);
			return true;
		}
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&intVal, sizeof(intVal));
	}

	error = true;
	return false;
}

bool zCArchiver::ReadByte(std::string name, char& byteVal)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "int", value))
		{
			byteVal = std::stoi(value);
			return true;
		}
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&byteVal, sizeof(byteVal));
	}

	error = true;
	return false;
}

bool zCArchiver::ReadWord(std::string name, short& wordVal)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "int", value))
		{
			wordVal = std::stoi(value);
			return true;
		}
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&wordVal, sizeof(wordVal));
	}

	error = true;
	return false;
}

bool zCArchiver::ReadFloat(std::string name, float& floatVal)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "float", value))
		{
			floatVal = std::stof(value);
			return true;
		}
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&floatVal, sizeof(floatVal));
	}

	error = true;
	return false;
}

bool zCArchiver::ReadBool(std::string name, bool& boolVal)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "bool", value))
		{
			boolVal = std::stoi(value);
			return true;
		}
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&boolVal, sizeof(boolVal));
	}

	error = true;
	return false;
}

bool zCArchiver::ReadString(std::string name, std::string& strVal)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "string", value))
		{
			strVal = value;
			return true;
		}
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->ReadNullString(strVal);
	}

	error = true;
	return false;
}

bool zCArchiver::ReadVec3(std::string name, zVEC3& vecVal)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "vec3", value) &&
			sscanf_s(value.c_str(), "%f %f %f",
				&vecVal.x, &vecVal.y, &vecVal.z) == 3)
		{
			return true;
		}
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&vecVal, sizeof(vecVal));
	}

	error = true;
	return false;
}

bool zCArchiver::ReadColor(std::string name, zCOLOR& colorVal)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
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
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&colorVal, sizeof(colorVal));
	}

	error = true;
	return false;
}

bool zCArchiver::ReadRaw(std::string name, char* buffer, size_t bufferSize)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
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
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(buffer, bufferSize);
	}

	error = true;
	return false;
}

bool zCArchiver::ReadRawFloat(std::string name, float* floatVals, size_t floatCount)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
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
				std::string strFloats = value;

				for (size_t i = 0; i < floatCount; i++)
				{
					floatVals[i] = std::stof(strFloats);
					strFloats = strFloats.substr(strFloats.find(" ") + 1);
				}

				return true;
			}
		}
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(floatVals, floatCount * sizeof(float));
	}

	error = true;
	return false;
}

bool zCArchiver::ReadEnum(std::string name, int& enumVal)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (ReadASCIIProperty(name, "enum", value))
		{
			enumVal = std::stoi(value);
			return true;
		}
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&enumVal, sizeof(enumVal));
	}

	error = true;
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
		return nullptr;
	}

	// Error checking
	if (objectIndex < 0 || objectIndex >= objectList.size())
	{
		LOG_ERROR("The specified object index is not within an accepted range! Archive must be invalid!");
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
				return nullptr;
			}
		}
		else
		{
			// Create object based on read type

			std::string truncClassName = className;

			if (truncClassName.find(":") != std::string::npos)
				truncClassName = truncClassName.substr(0, truncClassName.find(":"));

			ClassDefinition* classDef = ClassDefinition::GetClassDef(truncClassName);

			if (classDef == nullptr)
			{
				return nullptr;
			}

			object = classDef->CreateInstance();

			uint16_t versionSum = classDef->GetVersionSum(game);

			if (versionSum == -1)
			{
				LOG_ERROR("Class " + classDef->GetName() + " is not supported in the specified game version!");
				return nullptr;
			}

			if (versionSum != classVersion)
			{
				LOG_ERROR("Error while reading class " + classDef->GetName() + ". The version is " 
					+ std::to_string(classVersion) + ", expected " + std::to_string(versionSum) + "!");
				return nullptr;
			}

			object->game = game;
		}

		objectList[objectIndex] = object;

		if (!object->Unarchive(this))
		{
			return nullptr;
		}
	}

	if (!ReadChunkEnd())
	{
		return nullptr;
	}

	return object;
}

bool zCArchiver::ReadChunkStart(std::string* objectName, std::string* className, uint16_t* classVersion, uint32_t* objectIndex)
{
	if (error)
		return false;

	if (type == ARCHIVER_TYPE_ASCII ||
		type == ARCHIVER_TYPE_BIN_SAFE)
	{
		std::string line;

		if (type == ARCHIVER_TYPE_BIN_SAFE)
		{
			char* str;
			size_t length;
			if (!ReadBinSafeProperty(BINSAFE_TYPE_STRING, str, length))
				return false;

			line = std::string(str, length);

			delete[] str;
		}
		else
		{
			// While in ASCII mode, it is not always guaranteed
			// that the chunks/properties will be in the correct
			// order. Therefore we need to go through the entirity
			// of the current chuck and find the one that matches
			// the specified criteria.

			// Go to the start of the current chunk before we
			// perform the search.
			//file->Seek(asciiChunksPositions[asciiChunksPositions.size() - 1]);

			bool first = true;

			bool looping = false;
			uint64_t startPos = file->Tell();

			int nesting = 0;

			bool valid = false;

			while (file->ReadLine(line))
			{
				//LOG_DEBUG("ReadChunkStart    : " + line);

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
					file->Seek(asciiChunksPositions[asciiChunksPositions.size() - 1]);

					continue;
				}
				else if (nesting == 1)
				{
					size_t sPos = line.find(" ");

					if (isHeader && sPos != std::string::npos)
					{
						if (objectName == nullptr ||
							(*objectName).empty())
						{
							valid = true;
							break;
						}
						else
						{
							std::string chunkName = line.substr(tabCount + 1, sPos - tabCount - 1);

							if ((*objectName) == chunkName)
							{
								valid = true;
								break;
							}
						}
					}

					if (first)
					{
						first = false;
						LOG_WARN("Incorrect chunk order, expected \"" + (*objectName) + "\"!");
					}
				}				
			}

			if (!valid)
			{
				LOG_ERROR("No chunk matching the desired criteria has been found!");
				return false;
			}

			// Mark that we entered a new chunk
			asciiChunksPositions.push_back(file->Tell());
		}

		size_t p1 = line.find("[");
		size_t p2 = line.find(" ", p1 + 1);
		size_t p3 = line.find(" ", p2 + 1);
		size_t p4 = line.find(" ", p3 + 1);

		if (objectName != nullptr)
			*objectName	= line.substr(p1 + 1, p2 - p1 - 1);

		if (className != nullptr)
			*className	= line.substr(p2 + 1, p3 - p2 - 1);

		if (classVersion != nullptr)
			*classVersion = std::stoi(line.substr(p3));

		if (objectIndex != nullptr)
			*objectIndex = std::stoi(line.substr(p4));
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		BinaryObjectHeader header;
		if (!file->Read(&header, sizeof(header)))
			return false;

		std::string readObjectName;
		if (!file->ReadNullString(readObjectName))
			return false;

		std::string readClassName;
		if (!file->ReadNullString(readClassName))
			return false;

		if (objectName != nullptr)
			*objectName = readObjectName;

		if (className != nullptr)
			*className = readClassName;

		if (classVersion != nullptr)
			*classVersion = header.classVersion;

		if (objectIndex != nullptr)
			*objectIndex = header.objectIndex;
	}

	if (objectIndex != nullptr &&
		*objectIndex >= objectList.size())
	{
		return false;
	}

	return true;
}

bool zCArchiver::ReadChunkEnd()
{
	if (error)
	{
		LOG_ERROR("An error occured while reading chunk.");
		return false;
	}

	if (type == ARCHIVER_TYPE_ASCII)
	{
		bool valid = false;

		int nesting = 0;

		std::string line;

		while (file->ReadLine(line))
		{
			//LOG_DEBUG("ReadChunkEnd      : " + line);

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
		}

		if (!valid)
		{
			LOG_ERROR("End of the file reached, but the chunk remains open. This archive is invalid!");
			return false;
		}

		asciiChunksPositions.pop_back();
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
		//LOG_DEBUG("ReadASCIIProperty : " + line);

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
			file->Seek(asciiChunksPositions[asciiChunksPositions.size() - 1]);

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

				LOG_WARN("Incorrect property order, expected \"" + name + "\", found \"" + readName + "\"!");
			}
		}
	}

	LOG_ERROR("Specified property not found in chunk!");
	return false;
}

bool zCArchiver::ReadBinSafeProperty(BINSAFE_TYPE type, char*& data, size_t& size)
{
	// First read the type
	char readType;
	if (!file->Read(&readType, sizeof(readType)))
		return false;

	if (readType != type)
		return false;

	// Proceed based on type
	switch (readType)
	{
	case BINSAFE_TYPE_STRING:
		
		uint16_t length;
		if (!file->Read(&length, sizeof(length)))
			return false;

		data = new char[length];
		if (!file->Read(data, length))
			return false;

		size = length;

		break;

	case BINSAFE_TYPE_INT:
		break;

	case BINSAFE_TYPE_FLOAT:
		break;

	case BINSAFE_TYPE_BOOL:
		break;

	case BINSAFE_TYPE_VEC3:
		break;

	case BINSAFE_TYPE_COLOR:
		break;

	case BINSAFE_TYPE_RAW:
		break;

	case BINSAFE_TYPE_RAWFLOAT:
		break;

	case BINSAFE_TYPE_ENUM:
		break;

	default:
		break;
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

		if (classDef->revisionCount != 0 &&
			!classDef->isRevisionsListInit)
		{
			ClassDefinition* currentClassDef = classDef;

			while (true)
			{
				for (size_t i = 0; i < classDef->revisionCount; i++)
				{
					CLASS_REVISION& rev = classDef->revisions[i];

					for (size_t j = 0; j < currentClassDef->revisionCount; j++)
					{
						CLASS_REVISION& currRev = currentClassDef->revisions[j];

						if (currRev.game == rev.game ||
							currRev.game == GAME_ALL)
						{
							rev.versionSum = CRC16((char*)&currRev.version, sizeof(currRev.version), rev.versionSum);
							break;
						}
					}

				}

				//
				// Due to a bug in the engine, the pointer to the base class
				// is not always initialized when the class is alphabetically
				// later than the base class. Here we account for that bug
				// by bailing the hiarchy crawl early if the current class
				// is alphabetically eariler than the base class.
				//

				if (currentClassDef->GetName() != "zCVob" && 
					// zCVob is always intialized first due to the compilation order
					currentClassDef->GetName() > classDef->GetName())
				{
					break;
				}

				currentClassDef = ClassDefinition::GetClassDef(currentClassDef->GetBase());

				if (currentClassDef == nullptr)
					break;
			}

			classDef->isRevisionsListInit = true;
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

	return -1;
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
