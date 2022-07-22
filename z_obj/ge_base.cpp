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

	if (index >= stringCount)
	{
		return false;
	}

	str = std::string(stringPool[index]);

	return true;
}

bool eCArchiveFile::OnOpen()
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
		stringCount	= stringPoolHeader.count;
		stringPool	= new char*[stringCount];

		for (size_t i = 0; i < stringCount; i++)
		{
			uint16_t length;
			if (!Read(&length, sizeof(length)))
			{
				LOG_ERROR("Unexpected end of file while reading string pool, expected length!");
				return false;
			}

			stringPool[i]			= new char[length + 1];
			stringPool[i][length]	= NULL;

			if (!Read(stringPool[i], length))
			{
				LOG_ERROR("Unexpected end of file while reading string pool, expected a string!");
				return false;
			}
		}
	}

	Seek(startPos);

	return true;
}

bool bCAccessorPropertyObject::Write(FileStream* file)
{
	return false;
}

bool bCAccessorPropertyObject::Read(FileStream* file)
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
	if (!file->Read(&propertyObjectWritten2, sizeof(propertyObjectWritten2)))
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
	for (size_t i = 0; i < propertyObjectWritten2.propertyCount; i++)
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

		if (propertyWritten.size == 0) continue; // temp

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
		}

		if (found)
		{
			// Special cases
			if (propertyType == "bCString")
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

				if (property->GetType() != propertyType)
				{
					LOG_ERROR("Could not read property \"" + propertyName + "\" of class \"" + className +
						"\". Expected size \"" + std::to_string(property->GetSize()) +
						"\", found \"" + std::to_string(propertyWritten.size) + "\" instead!");
					return false;
				}

				// Read data
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
			LOG_WARN("Property \"" + propertyName + "\" of type \"" + propertyType +
				"\" was not found in \"" + className + "\" object!");
		}
	}

	// Read class data
	if (!nativeObject->OnRead(file))
	{
		return false;
	}

	return true;
}

ClassDefinition::ClassDefinition(std::string _name, std::string _base, bCObjectBase* (*_createFunc)())
{
	name			= _name;
	base			= _base;
	createFunc		= _createFunc;

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

PropertyDefinition::PropertyDefinition(ClassDefinition* classDef, std::string _name,
	std::string _type, size_t _size, size_t _offset)
{
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

bool bCObjectBase::OnWrite(FileStream* file)
{
	return false;
}

bool bCObjectBase::OnRead(FileStream* file)
{
	uint16_t version;
	file->Read(&version, sizeof(version));

	return true;
}

bool bCObjectRefBase::OnWrite(FileStream* file)
{
	return false;
}

bool bCObjectRefBase::OnRead(FileStream* file)
{
	uint16_t version;
	file->Read(&version, sizeof(version));

	return true;
}

bool eCProcessibleElement::Save(FileStream* file)
{
	return false;
}

bool eCProcessibleElement::Load(FileStream* file)
{
	uint32_t magic;
	if (!file->Read(&magic, sizeof(magic)))
	{
		LOG_ERROR("Unexpected end of file, expected eCProcessibleElement magic!");
		return false;
	}

	// Read contained object
	bCAccessorPropertyObject accessor(this);

	if (!accessor.Read(file))
	{
		return false;
	}

	return true;
}
