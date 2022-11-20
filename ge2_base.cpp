#include "ge2_base.h"

using namespace GothicLib::Genome2;

bool bCArchive::Read(FileStream* _file)
{
	file = _file;

	// Read header
	uint32_t magic;
	file->Read(FILE_ARGS(magic));

	if (magic != 0x33524147)
	{
		LOG_ERROR("Expected GAR3 at the start of a bCArchive!");
		return false;
	}

	uint32_t nested;
	file->Read(FILE_ARGS(nested)); // unused

	return true;
}

bool bCArchive::Write(FileStream* _file)
{
	return false;
}

bCObjectBase* bCArchive::ReadObject(bCObjectBase* o)
{
	// Read header
	uint32_t magic;
	file->Read(FILE_ARGS(magic));

	if (magic != 0x30434547)
	{
		LOG_ERROR("Expected GEC0 at the start of a bCArchive object!");
		return nullptr;
	}

	bCRuntimeClass object(o);
	object.Read(file);

	return object.GetNativeObject();
}

bool bCRuntimeClass::Read(FileStream* file)
{
	// Read beginning of class
	ClassWritten written;
	file->Read(FILE_ARGS(written));

	// Find an create the class
	ClassDefinition* classDef = ClassDefinition::GetClassDef(written.classHash);

	if (nativeObject)
	{
		if (nativeObject->GetClassDef()->GetName() != classDef->GetName())
		{
			LOG_ERROR("Read class does not match existing object. Expected \"" +
				nativeObject->GetClassDef()->GetName() + "\", found \"" + classDef->GetName() + "\" instead!");
			return false;
		}
	}
	else
	{
		nativeObject = classDef->CreateInstance();
		nativeObject->game = game;
	}

	//todo: version checking

	// Read properties
	for (size_t i = 0; i < written.propertyCount; i++)
	{
		PropertyWritten propertyWritten;
		file->Read(FILE_ARGS(propertyWritten));

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

				if (property->GetNameHash() == propertyWritten.nameHash)
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
			if (propertyWritten.typeHash == 0xA83E6117) // bCString
			{
				std::string* str = (std::string*)((size_t)nativeObject + property->GetOffset());

				if (!file->ReadString(*str))
				{
					LOG_ERROR("Failed to read string \"" + property->GetName() + "\"");
					return false;
				}
			}
			else
			{
				// Check size
				if (djb2(property->GetType().c_str()) != propertyWritten.typeHash)
				{
					LOG_ERROR("Could not read property of class \"" + classDef->GetName() +
						"\". Expected type \"" + property->GetType());/* +
						"\", found \"" + propertyType + "\" instead!");*/
					return false;
				}

				if (property->GetSize() != propertyWritten.size)
				{
					LOG_ERROR("Could not read property of class \"" + classDef->GetName() +
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
			file->Seek(file->Tell() + propertyWritten.size);

			//LOG_WARN("Property \"" + propertyName + "\" of type \"" + propertyType +
			//	"\" was not found in \"" + className + "\" object!");

			LOG_WARN("Property was not found in \"" + classDef->GetName() + "\" object!");
		}
	}

	// Read class data
	ClassDefinition* currClassDef = classDef;

	while (true)
	{
		ClassDataWritten dataWritten;
		file->Read(FILE_ARGS(dataWritten));

		if (currClassDef->GetNameHash() != dataWritten.classHash)
		{
			LOG_WARN("Expected \"" + currClassDef->GetName() + "\" object, when browsing inheritance tree!");
			return false;
		}

		//todo: version checking

		// Call read function
		if (dataWritten.size != 0 &&
			!currClassDef->Read(nativeObject, file))
		{
			return false;
		}

		if (currClassDef->GetBaseDef())
			currClassDef = currClassDef->GetBaseDef();
		else
			break;
	}

	return true;
}

bool bCRuntimeClass::Write(FileStream* file)
{
	return false;
}

// C++ is a piece of shit and won't compile
// this without the full namespace.
uint32_t GothicLib::Genome2::djb2(char const* name)
{
	uint32_t hash = 5381;
	if (name)
		while (*name)
			hash += (hash << 5) + *name++;
	return hash;
}

ClassDefinition::ClassDefinition(std::string _name, std::string _base, bCObjectBase* (*_createFunc)(),
	void* _readFunc, CLASS_REVISION* _revisions, size_t _revisionCount)
{
	name			= _name;
	base			= _base;
	createFunc		= _createFunc;
	readFunc		= (ReadFunc)_readFunc;
	revisions		= _revisions;
	revisionCount	= _revisionCount;

	if (!classList)
	{
		classList = new std::unordered_map<uint32_t, ClassDefinition*>();
	}

	(*classList)[GetNameHash()] = this;
}

ClassDefinition::~ClassDefinition()
{
}

ClassDefinition* ClassDefinition::GetClassDef(std::string name)
{
	std::string canonicalName = "class " + name;
	return GetClassDef(djb2(canonicalName.c_str()));
}

bool ClassDefinition::Read(bCObjectBase* object, FileStream* file)
{
	return readFunc(object, file);
}

ClassDefinition* ClassDefinition::GetClassDef(uint32_t hash)
{
	if (classList->find(hash) != classList->end())
	{
		ClassDefinition* classDef = (*classList)[hash];
		
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

				std::string canonicalBasename = "class " + currClassDef->base;
				uint32_t baseHash = djb2(canonicalBasename.c_str());
				currClassDef->baseDef = (*classList)[baseHash];
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

bool bCObjectBase::OnWrite(FileStream* file)
{
	return true;
}

bool bCObjectBase::OnRead(FileStream* file)
{
	return true;
}
