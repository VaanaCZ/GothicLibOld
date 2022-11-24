#include "ge_base.h"

using namespace GothicLib::Genome;

bool eCArchiveFile::ReadString(std::string& str)
{
	if (error)
	{
		return false;
	}

	if (isLegacyFile)
	{
		return FileStream::ReadString(str);
	}

	uint16_t index;
	if (!Read(&index, sizeof(index)))
	{
		return false;
	}

	if (index >= stringPool.size())
	{
		return false;
	}

	str = std::string(stringPool[index]);

	return true;
}

bool eCArchiveFile::WriteString(std::string str)
{
	if (error)
	{
		return false;
	}

	if (isLegacyFile)
	{
		return FileStream::WriteString(str);
	}

	// Find if the string is already pooled
	bool found = false;
	uint16_t index;

	for (size_t i = 0; i < stringPool.size(); i++)
	{
		if (stringPool[i] == str)
		{
			index = i;
			found = true;

			break;
		}
	}

	if (!found)
	{
		index = stringPool.size();
		stringPool.push_back(str);
	}

	if (!Write(&index, sizeof(index)))
	{
		return false;
	}

	return true;
}

bool eCArchiveFile::OnOpen()
{
	if (mode == STREAM_MODE_READ)
	{
		uint64_t startPos = Tell();

		// Attempt to open read file beginning
		ArchiveFileHeader header;
		if (!Read(&header, sizeof(header)))
		{
			isLegacyFile = true;
			Seek(startPos);

			return true;
		}

		if (header.magic != 0x454C464D4F4E4547 ||
			header.version != 1 ||
			header.offset < sizeof(header))
		{
			isLegacyFile = true;
			Seek(startPos);

			return true;
		}

		// If checks were sucessul, this is a valid eCArchiveFile
		startPos = Tell();

		Seek(header.offset);

		ArchiveFileStringPoolHeader stringPoolHeader;
		if (!Read(&stringPoolHeader, sizeof(stringPoolHeader)))
		{
			LOG_ERROR("Unexpected end of file, expected a eCArchiveFile string pool!");
			return false;
		}

		if (stringPoolHeader.magic != 0xDEADBEEF ||
			stringPoolHeader.version != 1)
		{
			LOG_ERROR("Control bytes of eCArchiveFile string pool do not match!");
			return false;
		}

		// Read strings
		if (stringPoolHeader.count != 0)
		{
			stringPool.resize(stringPoolHeader.count);

			for (size_t i = 0; i < stringPoolHeader.count; i++)
			{
				if (!FileStream::ReadString(stringPool[i]))
				{
					LOG_ERROR("Unexpected end of file while reading string pool, expected a string!");
					return false;
				}
			}
		}

		Seek(startPos);
	}
	else if (mode == STREAM_MODE_WRITE &&
		!isLegacyFile)
	{
		// Attempt to write file beginning
		ArchiveFileHeader header;
		memset(&header, 0, sizeof(header));

		if (!Write(&header, sizeof(header)))
		{
			LOG_ERROR("Failed to write eCArchiveFile header");
			return false;
		}
	}

	return true;
}

void eCArchiveFile::OnClose()
{
	if (mode == STREAM_MODE_WRITE &&
		!isLegacyFile)
	{
		uint64_t offset = Tell();

		// Attempt to write string pool
		ArchiveFileStringPoolHeader stringPoolHeader;
		stringPoolHeader.magic		= 0xDEADBEEF;
		stringPoolHeader.version	= 1;
		stringPoolHeader.count		= stringPool.size();

		Write(&stringPoolHeader, sizeof(stringPoolHeader));

		for (size_t i = 0; i < stringPool.size(); i++)
		{
			FileStream::WriteString(stringPool[i]);
		}

		// Write the actual header now
		Seek(0);

		ArchiveFileHeader header;
		header.magic	= 0x454C464D4F4E4547;
		header.version	= 1;
		header.offset	= offset;

		Write(&header, sizeof(header));
	}
}

bool bCAccessorPropertyObject::Write(FileStream* file, GAME game)
{
	// Write beginning
	AccessorWritten written;
	written.version				= 1;
	written.hasPropertyObject	= !(!nativeObject);

	if (!file->Write(&written, sizeof(written)))
	{
		LOG_ERROR("Failed to write beginning of bCAccessorPropertyObject!");
		return false;
	}

	if (!nativeObject)
	{
		return true;
	}

	// Write singleton
	ClassDefinition* classDef = nativeObject->GetClassDef();

	AccessorPropertyObjectWritten propertyObjectWritten;
	propertyObjectWritten.singletonVersion = 1;
	propertyObjectWritten.isPersistable = true;

	if (!file->Write(&propertyObjectWritten, sizeof(propertyObjectWritten)))
	{
		LOG_ERROR("Failed to write contents of bCAccessorPropertyObject!");
		return false;
	}

	if (!file->WriteString(classDef->GetName()))
	{
		LOG_ERROR("Failed to write bCAccessorPropertyObject, could not write name of class!");
		return false;
	}

	// Write rest of class
	uint64_t propObjPos = file->Tell();

	AccessorPropertyObjectWritten2 propertyObjectWritten2;
	memset(&propertyObjectWritten2, 0, sizeof(propertyObjectWritten2));

	if (!file->Write(&propertyObjectWritten2, sizeof(propertyObjectWritten2)))
	{
		LOG_ERROR("Falied to write contents of bCAccessorPropertyObject!");
		return false;
	}


	AccessorPropertyObjectWritten3 propertyObjectWritten3;
	memset(&propertyObjectWritten2, 0, sizeof(propertyObjectWritten2));

	if (!file->Write(&propertyObjectWritten3, sizeof(propertyObjectWritten3)))
	{
		LOG_ERROR("Falied to write contents of bCAccessorPropertyObject!");
		return false;
	}

	// Write properties
	ClassDefinition* currClassDef = nativeObject->GetClassDef();
	std::vector<PropertyDefinition*>& properties = classDef->GetProperties();

	size_t propCount = 0;

	while (true)
	{
		for (size_t i = 0; i < properties.size(); i++)
		{
			PropertyDefinition* property = properties[i];

			if (property->GetGame() != GAME_ALL &&
				property->GetGame() != game)
			{
				continue;
			}

			std::string type = property->GetType();

			// Gothic 3 exceptions
			if (game <= GAME_GOTHIC3 &&
				type.find("bTRefPtrArray<class") == 0)
			{
				type = "bTRefPtrArray<class bCPropertyObjectBase *>";
			}

			if (!file->WriteString(property->GetName()) ||
				!file->WriteString(type))
			{
				LOG_ERROR("Failed to write property name or type!");
				return false;
			}

			uint64_t propPos = file->Tell();

			AccessorPropertyWritten propertyWritten;
			memset(&propertyWritten, 0, sizeof(propertyWritten));

			if (!file->Write(&propertyWritten, sizeof(propertyWritten)))
			{
				LOG_ERROR("Failed to write a bCAccessorPropertyObject property!");
				return false;
			}

			// Special cases
			if (type == "bCString" ||
				type == "bCMeshResourceString")
			{
				std::string* str = (std::string*)((size_t)nativeObject + property->GetOffset());

				if (!file->WriteString(*str))
				{
					LOG_ERROR("Failed to write string \"" + property->GetName() + "\"");
					return false;
				}
			}
			else if (type.find("bTRefPtrArray") == 0 ||
				type.find("bTPOSmartPtr") == 0)
			{
				// Don't write anything
			}
			else
			{
				// Temporary solution
				if (type.find("bTPropertyContainer") == 0)
				{
					uint16_t ver = 1;
					file->Write(&ver, sizeof(ver));
				}

				void* dest = (void*)((size_t)nativeObject + property->GetOffset());
				if (!file->Write(dest, property->GetSize()))
				{
					LOG_ERROR("Failed to write bCAccessorPropertyObject property value!");
					return false;
				}
			}

			// Write data from before
			uint64_t currPos = file->Tell();

			file->Seek(propPos);

			propertyWritten.version = 30;
			propertyWritten.size	= currPos - propPos - sizeof(propertyWritten);

			if (!file->Write(&propertyWritten, sizeof(propertyWritten)))
			{
				LOG_ERROR("Failed to write a bCAccessorPropertyObject property!");
				return false;
			}

			file->Seek(currPos);

			propCount++;
		}

		currClassDef = currClassDef->GetBaseDef();

		if (currClassDef == nullptr)
			break;

		properties = currClassDef->GetProperties();
	}

	// Write class data
	if (!nativeObject->OnWrite(file, game))
	{
		return false;
	}

	if (file->Error())
	{
		LOG_ERROR("Error encountered while writing class \"" + nativeObject->GetClassDef()->GetName() + "\"!");
		return false;
	}

	// Write data from before
	uint64_t currPos = file->Tell();

	file->Seek(propObjPos);

	propertyObjectWritten2.factoryVersion				= 1;
	propertyObjectWritten2.isRoot						= false;

	propertyObjectWritten2.classVersion					= 83;
	propertyObjectWritten2.propertyObjectBaseVersion	= 83;

	if (game >= GAME_RISEN1)
	{
		propertyObjectWritten2.classVersion					= 200;
		propertyObjectWritten2.propertyObjectBaseVersion	= 201;
	}

	propertyObjectWritten2.propertySize					= currPos - propObjPos - sizeof(propertyObjectWritten2);

	if (!file->Write(&propertyObjectWritten2, sizeof(propertyObjectWritten2)))
	{
		LOG_ERROR("Falied to write contents of bCAccessorPropertyObject!");
		return false;
	}

	propertyObjectWritten3.propertyObjectBaseVersion = 30;

	if (game >= GAME_RISEN1)
	{
		propertyObjectWritten3.propertyObjectBaseVersion = 201;
	}

	propertyObjectWritten3.propertyCount				= propCount;

	if (!file->Write(&propertyObjectWritten3, sizeof(propertyObjectWritten3)))
	{
		LOG_ERROR("Falied to write contents of bCAccessorPropertyObject!");
		return false;
	}

	file->Seek(currPos);

	return true;
}

bool bCAccessorPropertyObject::Read(FileStream* file, GAME game)
{
	// Read beginning
	AccessorWritten written;
	if (!file->Read(&written, sizeof(written)))
	{
		LOG_ERROR("Unexpected end of file, expected beginning of bCAccessorPropertyObject!");
		return false;
	}

	if (written.version != 1)
	{
		LOG_WARN("Unknown bCAccessorPropertyObject version, expected 1, found " + std::to_string(written.version));
	}

	if (!written.hasPropertyObject)
	{
		return true;
	}

	// Read singleton
	AccessorPropertyObjectWritten propertyObjectWritten;
	if (!file->Read(&propertyObjectWritten, sizeof(propertyObjectWritten)))
	{
		LOG_ERROR("Unexpected end of file, expected contents of bCAccessorPropertyObject!");
		return false;
	}

	std::string className;
	
	if (!file->ReadString(className))
	{
		LOG_ERROR("Error while reading bCAccessorPropertyObject, could not read name of class!");
		return false;
	}

	// Read rest of class
	AccessorPropertyObjectWritten2 propertyObjectWritten2;
	AccessorPropertyObjectWritten3 propertyObjectWritten3;
	if (!file->Read(&propertyObjectWritten2, sizeof(propertyObjectWritten2)) ||
		!file->Read(&propertyObjectWritten3, sizeof(propertyObjectWritten3)))
	{
		LOG_ERROR("Unexpected end of file, expected contents of bCAccessorPropertyObject!");
		return false;
	}

	// Instantiate class, if it hasnt been already
	ClassDefinition* classDef = ClassDefinition::GetClassDef(className);

	if (!classDef)
	{
		LOG_ERROR("Class \"" + className + "\" is not supported!");
		return false;
	}

	// Figure out if the class is supported			
	if (classDef->GetVersion(game) == VERSION_NONE)
	{
		LOG_ERROR("Class " + classDef->GetName() + " is not supported in the specified engine revision!");
		return false;
	}

	if (nativeObject)
	{
		if (nativeObject->GetClassDef()->GetName() != className)
		{
			LOG_ERROR("Read class does not match existing object. Expected \"" +
				nativeObject->GetClassDef()->GetName() + "\", found \"" + className + "\" instead!");
			return false;
		}
	}
	else
	{
		nativeObject = classDef->CreateInstance();
	}

	// Read properties
	for (size_t i = 0; i < propertyObjectWritten3.propertyCount; i++)
	{
		std::string propertyName, propertyType;
		if (!file->ReadString(propertyName) ||
			!file->ReadString(propertyType))
		{
			LOG_ERROR("Could not read property name or type!");
			return false;
		}

		AccessorPropertyWritten propertyWritten;
		if (!file->Read(&propertyWritten, sizeof(propertyWritten)))
		{
			LOG_ERROR("Unexpected end of file, expected a bCAccessorPropertyObject property!");
			return false;
		}

		// Check if the native object contains this property
		ClassDefinition* currClassDef = classDef;
		std::vector<PropertyDefinition*> properties;

		bool found = false;
		PropertyDefinition* property = nullptr;

		while (true)
		{
			properties = currClassDef->GetProperties();

			for (size_t i = 0; i < properties.size(); i++)
			{
				property = properties[i];

				if (property->GetName() == propertyName)
				{
					found = true;
					break;
				}
			}

			if (found)
				break;

			if (currClassDef->GetBaseDef())
				currClassDef = currClassDef->GetBaseDef();
			else
				break;
		}

		if (found)
		{
			if (propertyWritten.size == 0)
				continue;

			// Special cases
			if (propertyType == "bCString" ||
				propertyType == "bCMeshResourceString")
			{
				std::string* str = (std::string*)((size_t)nativeObject + property->GetOffset());

				if (!file->ReadString(*str))
				{
					LOG_ERROR("Failed to read string \"" + propertyName + "\"");
					return false;
				}
			}
			else
			{
				// Check size
				if (property->GetType() != propertyType)
				{
					LOG_ERROR("Could not read property \"" + propertyName + "\" of class \"" + className +
						"\". Expected type \"" + property->GetType() +
						"\", found \"" + propertyType + "\" instead!");
					return false;
				}

				//if (property->GetSize() != propertyWritten.size)
				//{
				//	LOG_ERROR("Could not read property \"" + propertyName + "\" of class \"" + className +
				//		"\". Expected size \"" + std::to_string(property->GetSize()) +
				//		"\", found \"" + std::to_string(propertyWritten.size) + "\" instead!");
				//	return false;
				//}

				// Read data

				// Temporary solution
				if (propertyType.find("bTPropertyContainer") == 0)
				{
					uint16_t ver;
					file->Read(&ver, sizeof(ver));
				}

				void* dest = (void*)((size_t)nativeObject + property->GetOffset());
				if (!file->Read(dest, property->GetSize()))
				{
					LOG_ERROR("Unexpected end of file while reading bCAccessorPropertyObject property value!");
					return false;
				}
			}
		}
		else
		{
			file->Seek(file->Tell() + propertyWritten.size);

			LOG_WARN("Property \"" + propertyName + "\" of type \"" + propertyType +
				"\" was not found in \"" + className + "\" object!");
		}
	}

	// Read class data
	if (!nativeObject->OnRead(file, game))
	{
		return false;
	}

	if (file->Error())
	{
		LOG_ERROR("Error encountered while reading class \"" + className + "\"!");
		return false;
	}

	return true;
}

ClassDefinition::ClassDefinition(std::string _name, std::string _base, bCObjectBase* (*_createFunc)(),
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

uint16_t ClassDefinition::GetVersion(GAME game)
{
	for (size_t i = 0; i < revisionCount; i++)
	{
		if (revisions[i].game == game ||
			revisions[i].game == GAME_ALL)
		{
			return revisions[i].version;
		}
	}

	return VERSION_NONE;
}

bool ClassDefinition::IsVersionSupported(GAME game, uint16_t version)
{
	for (size_t i = 0; i < revisionCount; i++)
	{
		if (revisions[i].game == game &&
			revisions[i].version == version)
		{
			return true;
		}
	}

	return false;
}

PropertyDefinition::PropertyDefinition(ClassDefinition* classDef, GAME _game,
	std::string _name, std::string _type, size_t _size, size_t _offset)
{
	game	= _game;
	name	= _name;
	type	= _type;
	size	= _size;
	offset	= _offset;

	if (!classDef)
	{
		// big bad error
		return;
	}

	classDef->GetProperties().push_back(this);
}

PropertyDefinition::~PropertyDefinition()
{
}

bool bCObjectBase::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 1;

	if (game >= GAME_RISEN1)
	{
		version = 201;
	}

	file->Write(&version, sizeof(version));

	return true;
}

bool bCObjectBase::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(&version, sizeof(version));

	return true;
}

bool bCObjectRefBase::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 1;
	file->Write(&version, sizeof(version));

	return true;
}

bool bCObjectRefBase::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(&version, sizeof(version));

	return true;
}

bool eCProcessibleElement::Save(FileStream* file, FileStream* datFile, GAME game)
{
	if (game >= GAME_RISEN1)
	{
		uint32_t magic = 0xD0DEFADE;
		if (!file->Write(&magic, sizeof(magic)))
		{
			LOG_ERROR("Failed to write eCProcessibleElement magic!");
			return false;
		}
	}

	// Read contained object
	bCAccessorPropertyObject accessor(this);

	if (!accessor.Write(file, game))
	{
		return false;
	}

	if (game <= GAME_GOTHIC3 && datFile)
	{
		if (!DoSaveData(datFile, game))
		{
			return false;
		}

		if (datFile->Error())
		{
			LOG_ERROR("Error encountered while saving data of class \"" +
				accessor.GetNativeObject()->GetClassDef()->GetName() + "\"!");
			return false;
		}
	}

	return true;
}

bool eCProcessibleElement::Load(FileStream* file, FileStream* datFile, GAME game)
{
	if (game >= GAME_RISEN1)
	{
		uint32_t magic;
		if (!file->Read(&magic, sizeof(magic)))
		{
			LOG_ERROR("Unexpected end of file, expected eCProcessibleElement magic!");
			return false;
		}

		if (magic != 0xD0DEFADE)
		{
			LOG_ERROR("Magic bytes do not match when reading eCProcessibleElement!");
			return false;
		}
	}

	// Read contained object
	bCAccessorPropertyObject accessor(this);

	if (!accessor.Read(file, game))
	{
		return false;
	}

	if (game <= GAME_GOTHIC3 && datFile)
	{
		if (!DoLoadData(datFile, game))
		{
			return false;
		}

		if (datFile->Error())
		{
			LOG_ERROR("Error encountered while loading data of class \"" +
				accessor.GetNativeObject()->GetClassDef()->GetName() + "\"!");
			return false;
		}
	}

	return true;
}
