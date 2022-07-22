#pragma once

#include <string>
#include <vector>
#include "gothiclib_base.h"

namespace GothicLib
{
	class FileStream;

	namespace Genome
	{
		class bCObjectBase;

		/*
			File		
		*/

		class eCArchiveFile : public FileStream
		{
		public:

			eCArchiveFile()				{ }
			virtual ~eCArchiveFile()	{ }

			virtual bool ReadString(std::string&);

		private:

			virtual bool OnOpen();

			bool isLegacyFile = false;

			char** stringPool = nullptr;
			size_t stringCount = 0;

#pragma pack (push, 1)
			struct ArchiveFileHeader
			{
				uint64_t	magic;
				uint16_t	version;
				uint32_t	offset;
			};

			struct ArchiveFileStringPoolHeader
			{
				uint32_t	magic;
				uint8_t		version;
				uint32_t	count;
			};
#pragma pack (pop)

		};

		/*
			Archivation
		*/

		class bCAccessorPropertyObject
		{
		public:

			bCAccessorPropertyObject()								{ }
			bCAccessorPropertyObject(bCObjectBase* _nativeObject)	{ nativeObject = _nativeObject; }
			~bCAccessorPropertyObject()								{ }

			bool Write(FileStream*);
			bool Read(FileStream*);

			bCObjectBase* GetNativeObject() { return nativeObject; }

			template <class C> C GetNativeObjectAs()
			{
				bCObjectBase* object = GetNativeObject();

				if (object == nullptr)
					return nullptr;

				return dynamic_cast<C>(object);
			}

		private:

			bCObjectBase* nativeObject = nullptr;

#pragma pack (push, 1)
			struct AccessorWritten
			{
				uint16_t	version;
				bool		hasPropertyObject;
			};

			struct AccessorPropertyObjectWritten
			{
				uint16_t	singletonVersion;
				bool		isPersistable;
			};

			struct AccessorPropertyObjectWritten2
			{
				uint16_t	factoryVersion;
				bool		isRoot;
				uint16_t	classVersion;
				uint16_t	propertyObjectBaseversion;
				uint32_t	propertySize;
				uint16_t	propertyObjectBaseversion2;
				uint32_t	propertyCount;
			};

			struct AccessorPropertyWritten
			{
				uint16_t	version;
				uint32_t	size;
			};
#pragma pack (pop)

		};

		/*
			Object persistence
		*/

		class PropertyDefinition;
				
		class ClassDefinition
		{
		public:

			ClassDefinition(std::string, std::string, bCObjectBase*(*)());
			~ClassDefinition();

			inline std::string		GetName()			{ return name; };
			inline std::string		GetBase()			{ return base; };
			inline ClassDefinition*	GetBaseDef()		{ return baseDef; };
			inline bCObjectBase*	CreateInstance()	{ return createFunc(); };
			
			inline std::vector<PropertyDefinition*>& GetProperties() { return properties; };

			static ClassDefinition* GetClassDef(std::string);

		private:

			std::string			name;
			std::string			base;
			ClassDefinition*	baseDef = nullptr;
			bCObjectBase*		(*createFunc)();

			std::vector<PropertyDefinition*> properties;

			inline static std::unordered_map<std::string, ClassDefinition*>* classList = nullptr;
		};

		class PropertyDefinition
		{
		public:

			PropertyDefinition(ClassDefinition*, std::string, std::string, size_t, size_t);
			~PropertyDefinition();

			inline std::string		GetName()			{ return name; };
			inline std::string		GetType()			{ return type; };
			inline size_t			GetSize()			{ return size; };
			inline size_t			GetOffset()			{ return offset; };

		private:

			std::string	name;
			std::string	type;
			size_t		size;
			size_t		offset;
		};

		template<typename T, typename U> constexpr size_t MemberOffset(U T::* m)
		{
			return (char*)&((T*)nullptr->*m) - (char*)nullptr;
		}

#define GE_DECLARE_CLASS(C, B)													\
		inline static C* _nullInstance = nullptr;								\
																				\
		static bCObjectBase* CreateInstance() { return new C(); }				\
																				\
		inline static ClassDefinition* classDef =								\
			new ClassDefinition(#C, #B, &C::CreateInstance);					\
																				\
		inline static ClassDefinition* GetStaticClassDef() { return classDef; }	\
		virtual ClassDefinition* GetClassDef() { return classDef; }


#define GE_DECLARE_PROPERTY(C, T, N)											\
		T N;																	\
																				\
		inline static PropertyDefinition* propDef_ ##N =						\
			new PropertyDefinition(classDef, #N, #T, sizeof(T),					\
			MemberOffset(&C::N));

		/*
			Base object
		*/

		class bCObjectBase
		{
		public:

			GE_DECLARE_CLASS(bCObjectBase, );

			bCObjectBase()			{}
			virtual ~bCObjectBase()	{}

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

		private:

		};

		class bCObjectRefBase : public bCObjectBase
		{
		public:

			GE_DECLARE_CLASS(bCObjectRefBase, bCObjectBase);

			bCObjectRefBase()			{}
			virtual ~bCObjectRefBase()	{}

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

		private:

		};

		/*
			Important classes
		*/

		class eCProcessibleElement : public bCObjectRefBase
		{
		public:

			GE_DECLARE_CLASS(eCProcessibleElement, bCObjectRefBase);

			eCProcessibleElement()			{}
			virtual ~eCProcessibleElement()	{}

			virtual bool DoLoadData(FileStream*) { return true; } // Gothic 3
			virtual bool DoSaveData(FileStream*) { return true; } // Gothic 3

			virtual bool Save(FileStream*);
			virtual bool Load(FileStream*);

			GE_DECLARE_PROPERTY(eCProcessibleElement, bool, IsPersistable);

		private:

		};

	};
};
