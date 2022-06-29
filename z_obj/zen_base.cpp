#pragma once

#include "zen_base.h"

#include "zen_vdfs.h"
#include "zen_world.h"


ZEN::zCObject* ZEN::zCObject::CreateObject(std::string className)
{
#define CLASS_CREATE_ATTEMPT(c)									\
	else if (className == c::GetStaticClassname())	\
		return new c();

	if (className == "zCObject")
	{
		return nullptr;
	}

	CLASS_CREATE_ATTEMPT(zCAIBase)
	CLASS_CREATE_ATTEMPT(zCMesh)
	CLASS_CREATE_ATTEMPT(zCParticleFX)
	CLASS_CREATE_ATTEMPT(zCVisual)
	CLASS_CREATE_ATTEMPT(zCVob)
	CLASS_CREATE_ATTEMPT(zCVobLevelCompo)
	CLASS_CREATE_ATTEMPT(zCVobLight)
	CLASS_CREATE_ATTEMPT(zCVobSound)
	CLASS_CREATE_ATTEMPT(zCVobSpot)
	CLASS_CREATE_ATTEMPT(zCWorld)
	CLASS_CREATE_ATTEMPT(oCWorld)
	
	return nullptr;
}

bool ZEN::zCArchiver::Read(FileStream* _file)
{
	file = _file;

	// Read header
	std::string line;

	/*
		ZenGin Archive
	*/
	file->ReadLine(line);

	if (line != "ZenGin Archive") 
		return false;

	/*
		ver 0 / ver 1
	*/
	file->ReadLine(line);

	if (line == "ver 0")			version = 0;
	else if (line == "ver 1")		version = 1;
	else							return false;

	/*
		zCArchiverGeneric / zCArchiverBinSafe
	*/
	if (version == 1)
	{
		file->ReadLine(line);
	}

	/*
		ASCII / BINARY / BIN_SAFE
	*/

	file->ReadLine(line);

	if (line == "ASCII")			type = ARCHIVER_TYPE_ASCII;
	else if (line == "BINARY")		type = ARCHIVER_TYPE_BINARY;
	else if (line == "BIN_SAFE")	type = ARCHIVER_TYPE_BIN_SAFE;
	else							return false;

	/*
		saveGame 0 / saveGame 1
	*/

	file->ReadLine(line);

	bool noSavegame = false;

	if (line == "saveGame 0")		savegame = false;
	else if (line == "saveGame 1")	savegame = true;
	else
	{
		savegame = false;
		noSavegame = true;
	}

	/*
		date DD.MM.YYYY HH:MM:SS
	*/

	if (!noSavegame)
	{
		file->ReadLine(line);
	}

	if (version == 0)
	{
		/*
			objects n
		*/

		file->ReadLine(line);

		uint32_t objectCount = std::stoul(line.substr(8));

		if (objectCount == 0)
			return false;

		objectList.resize(objectCount);

		/*
			csum 00000000
		*/

		if (noSavegame)
		{
			file->ReadLine(line);
		}
	}
	else if (version == 1)
	{
		/*
			user x
		*/

		file->ReadLine(line);
	}

	/*
		END
	*/

	file->ReadLine(line);

	if (line != "END")
		return false;

	if (version == 1 &&
		type != ARCHIVER_TYPE_BIN_SAFE)
	{
		/*
			objects n
		*/

		file->ReadLine(line);

		uint32_t objectCount = std::stoul(line.substr(8));

		if (objectCount == 0)
			return false;

		objectList.resize(objectCount);

		/*
			END
		*/

		file->ReadLine(line);

		if (line != "END")
			return false;

		/*
			*empty line*
		*/

		file->ReadLine(line);

		if (!line.empty())
			return false;
	}
	else if (type == ARCHIVER_TYPE_BIN_SAFE)
	{
		BinSafeArchiveHeader header;
		if (!file->Read(&header, sizeof(header)))
			return false;

		if (header.version != 2)
		{
			return false;
		}

		objectList.resize(header.objectCount);

		uint64_t startPos = file->Tell();

		// Read chunk list
		file->Seek(header.chunkPos);

		uint32_t chunkCount;
		if (!file->Read(&chunkCount, sizeof(chunkCount)))
			return false;

		BinSafeHashTable hash;
		char text[8192];

		for (size_t i = 0; i < chunkCount; i++)
		{
			if (!file->Read(&hash, sizeof(hash)))
				return false;

			uint32_t textLength = hash.stringLength;
			if (textLength > 8192)
				textLength = 8192;

			if (!file->Read(&text, textLength))
				return false;

			// Todo: save values
		}

		file->Seek(startPos);
	}
	
	// Read objects
	if (!ReadObject(containedObject))
		return false;

	return true;
}

bool ZEN::zCArchiver::ReadInt(std::string name, int& intVal)
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (!ReadPropertyASCII(name, "int", value))
			return false;
		intVal = std::stoi(value);

		return true;
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&intVal, sizeof(intVal));
	}

	return true;
}

bool ZEN::zCArchiver::ReadFloat(std::string name, float& floatVal)
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (!ReadPropertyASCII(name, "float", value))
			return false;
		floatVal = std::stof(value);

		return true;
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&floatVal, sizeof(floatVal));
	}

	return true;
}

bool ZEN::zCArchiver::ReadBool(std::string name, bool& boolVal)
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (!ReadPropertyASCII(name, "bool", value))
			return false;
		boolVal = std::stoi(value);

		return true;
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&boolVal, sizeof(boolVal));
	}

	return true;
}

bool ZEN::zCArchiver::ReadString(std::string name, std::string& strVal)
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (!ReadPropertyASCII(name, "string", value))
			return false;
		strVal = value;

		return true;
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->ReadNullString(strVal);
	}

	return false;
}

bool ZEN::zCArchiver::ReadVec3(std::string name, zVEC3& vecVal)
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (!ReadPropertyASCII(name, "vec3", value))
			return false;

		if (sscanf_s(value.c_str(), "%f %f %f",
			&vecVal.x, &vecVal.y, &vecVal.z) != 3)
			return false;

		return true;
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&vecVal, sizeof(vecVal));
	}

	return false;
}

bool ZEN::zCArchiver::ReadColor(std::string name, zCOLOR& colorVal)
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (!ReadPropertyASCII(name, "color", value))
			return false;

		int vals[4];
		if (sscanf_s(value.c_str(), "%d %d %d %d", 
			&vals[0], &vals[1], &vals[2], &vals[3]) != 4)
			return false;

		colorVal.b = vals[0];
		colorVal.g = vals[1];
		colorVal.r = vals[2];
		colorVal.a = vals[3];

		return true;
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&colorVal, sizeof(colorVal));
	}

	return false;
}

bool ZEN::zCArchiver::ReadRaw(std::string name, char* buffer, size_t bufferSize)
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (!ReadPropertyASCII(name, "raw", value))
			return false;

		if (value.size() / 2 != bufferSize)
			return false;

		// Read the data
		int	byteValue = 0;
		int byteCount = 0;
		for (int i = 0; i < value.length(); i += 2)
		{
			if (sscanf_s(&value[i], "%2x", &byteValue) != 1)
				return false;

			buffer[byteCount] = (unsigned char)byteValue;
			byteCount++;
		}

		return true;
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(buffer, bufferSize);
	}

	return false;
}

bool ZEN::zCArchiver::ReadRawFloat(std::string name, float* floatVals, size_t floatCount)
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (!ReadPropertyASCII(name, "rawFloat", value))
			return false;

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

		if (floatCount != archivedFoatCount)
		{
			return false;
		}

		// Read the data
		std::string strFloats = value;

		for (size_t i = 0; i < floatCount; i++)
		{
			floatVals[i] = std::stof(strFloats);
			strFloats = strFloats.substr(strFloats.find(" ") + 1);
		}

		return true;
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(floatVals, floatCount * sizeof(float));
	}

	return false;
}

bool ZEN::zCArchiver::ReadEnum(std::string name, int& enumVal)
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string value;
		if (!ReadPropertyASCII(name, "enum", value))
			return false;
		enumVal = std::stoi(value);

		return true;
	}
	else if (type == ARCHIVER_TYPE_BINARY)
	{
		return file->Read(&enumVal, sizeof(enumVal));
	}

	return false;
}

ZEN::zCObject* ZEN::zCArchiver::GetContainedObject()
{
	if (objectList.size() != 0)
	{
		return objectList[0];
	}

	return nullptr;
}

bool ZEN::zCArchiver::ReadPropertyASCII(std::string name, std::string type, std::string& value)
{
	std::string line;
	file->ReadLine(line);

	std::string s = name + "=" + type + ":";
	size_t sp = line.find(s);

	if (sp == std::string::npos)
		return false;

	value = line.substr(sp + s.size());
	return true;
}

bool ZEN::zCArchiver::ReadPropertyBinSafe(BINSAFE_TYPE type, char*& data, size_t& size)
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

bool ZEN::zCArchiver::ReadChunkStart(std::string* objectName, std::string* className, uint16_t* classVersion, uint32_t* objectIndex)
{
	if (type == ARCHIVER_TYPE_ASCII ||
		type == ARCHIVER_TYPE_BIN_SAFE)
	{
		std::string header;

		if (type == ARCHIVER_TYPE_BIN_SAFE)
		{
			char* str;
			size_t length;
			if (!ReadPropertyBinSafe(BINSAFE_TYPE_STRING, str, length))
				return false;

			header = std::string(str, length);

			delete[] str;
		}
		else
		{
			file->ReadLine(header);
		}

		size_t p1 = header.find("[");
		size_t p2 = header.find(" ", p1 + 1);
		size_t p3 = header.find(" ", p2 + 1);
		size_t p4 = header.find(" ", p3 + 1);

		if (objectName != nullptr)
			*objectName	= header.substr(p1 + 1, p2 - p1 - 1);

		if (className != nullptr)
			*className	= header.substr(p2 + 1, p3 - p2 - 1);

		if (classVersion != nullptr)
			*classVersion = std::stoi(header.substr(p3));

		if (objectIndex != nullptr)
			*objectIndex = std::stoi(header.substr(p4));
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

bool ZEN::zCArchiver::ReadChunkEnd()
{
	if (type == ARCHIVER_TYPE_ASCII)
	{
		std::string end;
		file->ReadLine(end);

		if (end[end.size() - 2] != '[' ||
			end[end.size() - 1] != ']')
			return false;
	}

	return true;
}

bool ZEN::zCArchiver::ReadObject(zCObject*& object)
{
	object = nullptr;

	// Read object header
	std::string objectName, className;
	uint16_t classVersion;
	uint32_t objectIndex;

	if (!ReadChunkStart(&objectName, &className, &classVersion, &objectIndex))
		return false;

	if (className == "%")
	{
		// % = nullptr
		object = nullptr;
	}
	else if (className == "§")
	{
		// § = existing object in list
		return false;
	}
	else
	{
		// Create object based on read type
		object = zCObject::CreateObject(className);

		if (object == nullptr)
			return false;

		object->version = classVersion;

		objectList[objectIndex] = object;

		if (!object->Unarchive(this))
			return false;
	}

	if (!ReadChunkEnd())
		return false;

	return true;
}
