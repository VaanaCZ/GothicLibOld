//
// ge_base.h
//
// Copyright (c) 2021-2023 V�clav Marou�ek
//

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

		typedef std::string bCString;
		typedef std::string bCMeshResourceString;
		typedef std::string bCImageResourceString;
		template <typename T> using bTRefPtrArray = std::vector<T>;
		template <typename T> using bTPOSmartPtr = std::shared_ptr<T>;
		template <typename T> using bTPropertyContainer = int;

		struct bCGuid
		{
			uint32_t	data1;
			uint16_t	data2;
			uint16_t	data3;
			uint8_t		data4[8];
			uint32_t	isValid;
		};

		struct bCVector2
		{
			float x, y;
		};

		struct bCVector
		{
			float x, y, z;
		};

		struct bCVector4
		{
			float x, y, z, w;
		};

		struct bCMatrix3
		{
			bCVector x, y, z;
		};

		struct bCMatrix
		{
			bCVector4 x, y, z, w;
		};

		struct bCEulerAngles
		{
			float x, y, z;
		};

		struct bCFloatColor
		{
			float a, b, g, r;
		};

		struct bCBox
		{
			bCVector min;
			bCVector max;
		};

		struct bCOrientedBox
		{
			bCVector min;
			bCVector max;
			bCMatrix3 orientation;
		};

		struct bCSphere
		{
			bCVector position;
			float radius;
		};

		struct bCDateTime
		{
			uint64_t timestamp;
		};

		/*
			File		
		*/

		class eCArchiveFile : public FileStream
		{
		public:

			eCArchiveFile()				{ }
			virtual ~eCArchiveFile()	{ }

			virtual bool ReadString(std::string&);
			virtual bool WriteString(std::string);

		private:

			virtual bool OnOpen();
			virtual void OnClose();

			bool isLegacyFile = false;

			std::vector<std::string> stringPool;

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

		enum GAME
		{
			/* Genome versions */

			GAME_GOTHIC3,
				GAME_G3_102 = GAME_GOTHIC3,
				GAME_G3_107 = GAME_GOTHIC3,
				GAME_G3_108 = GAME_GOTHIC3,
				GAME_G3_109 = GAME_GOTHIC3,
				GAME_G3_112 = GAME_GOTHIC3,

			GAME_RISEN1,
				GAME_R1_100 = GAME_RISEN1,
				GAME_R1_101 = GAME_RISEN1,
				GAME_R1_110 = GAME_RISEN1,
				GAME_R1_111 = GAME_RISEN1,

			GAME_ALL,

			GAME_NONE = -1
		};

		class bCAccessorPropertyObject
		{
		public:

			bCAccessorPropertyObject()								{ }
			bCAccessorPropertyObject(bCObjectBase* _nativeObject)	{ nativeObject = _nativeObject; }
			~bCAccessorPropertyObject()								{ }

			bool Write(FileStream*, GAME);
			bool Read(FileStream*, GAME);

			bCObjectBase*	GetNativeObject() { return nativeObject; }

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
				uint16_t	propertyObjectBaseVersion;
				uint32_t	propertySize;
			};

			struct AccessorPropertyObjectWritten3
			{
				uint16_t	propertyObjectBaseVersion;
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

#define VERSION_NONE 0xFFFF

		struct CLASS_REVISION
		{
			GAME		game;
			uint16_t	version;
		};

		class PropertyDefinition;
				
		class ClassDefinition
		{
		public:

			ClassDefinition(std::string, std::string, bCObjectBase*(*)(), CLASS_REVISION*, size_t);
			~ClassDefinition();

			inline std::string		GetName()			{ return name; };
			inline std::string		GetBase()			{ return base; };
			inline ClassDefinition*	GetBaseDef()		{ if (!baseDef) { baseDef = GetClassDef(base); } return baseDef; };
			inline bCObjectBase*	CreateInstance()	{ return createFunc(); };
			uint16_t				GetVersion(GAME);
			bool					IsVersionSupported(GAME, uint16_t);
			
			inline std::vector<PropertyDefinition*>& GetProperties() { return properties; };

			static ClassDefinition* GetClassDef(std::string);

		private:

			std::string			name;
			std::string			base;
			ClassDefinition*	baseDef = nullptr;
			bCObjectBase*		(*createFunc)();
			CLASS_REVISION*		revisions;
			size_t				revisionCount;

			std::vector<PropertyDefinition*> properties;

			inline static std::unordered_map<std::string, ClassDefinition*>* classList = nullptr;
		};

		class PropertyDefinition
		{
		public:

			PropertyDefinition(ClassDefinition*, GAME, std::string, std::string, size_t, size_t);
			~PropertyDefinition();
			
			inline GAME				GetGame()			{ return game; };
			inline std::string		GetName()			{ return name; };
			inline std::string		GetType()			{ return type; };
			inline size_t			GetSize()			{ return size; };
			inline size_t			GetOffset()			{ return offset; };

		private:

			GAME		game;
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
		static bCObjectBase* CreateInstance() { return new C(); }				\
																				\
		inline static ClassDefinition* classDef =								\
			new ClassDefinition(#C, #B, &C::CreateInstance, revisions,			\
								sizeof(revisions) / sizeof(revisions[0]));		\
																				\
		inline static ClassDefinition* GetStaticClassDef() { return classDef; }	\
		virtual ClassDefinition* GetClassDef() { return classDef; }				\
																				\
		virtual uint16_t GetVersion(GAME game)									\
			{ return classDef->GetVersion(game); }								\
																				\
		DECLARE_MEMORY_POOL(C)


#define GE_DECLARE_PROPERTY(C, G, T, N)											\
		T N;																	\
																				\
		inline static PropertyDefinition* propDef_ ##N =						\
			new PropertyDefinition(classDef, G, #N, #T, sizeof(T),				\
			MemberOffset(&C::N));

		/*
			Base object
		*/

		class bCObjectBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	201	},
			};

			GE_DECLARE_CLASS(bCObjectBase, );

			bCObjectBase()			{}
			virtual ~bCObjectBase()	{}

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

		private:

		};

		class bCObjectRefBase : public bCObjectBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	1	},
			};

			GE_DECLARE_CLASS(bCObjectRefBase, bCObjectBase);

			bCObjectRefBase()			{}
			virtual ~bCObjectRefBase()	{}

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

		private:

		};

		/*
			Important classes
		*/

		class eCProcessibleElement : public bCObjectRefBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	1	},
			};

			GE_DECLARE_CLASS(eCProcessibleElement, bCObjectRefBase);

			eCProcessibleElement()			{}
			virtual ~eCProcessibleElement()	{}

			virtual bool DoLoadData(FileStream*, GAME) { return true; } // Gothic 3
			virtual bool DoSaveData(FileStream*, GAME) { return true; } // Gothic 3

			virtual bool Save(FileStream*, FileStream*, GAME);
			virtual bool Load(FileStream*, FileStream*, GAME);

			GE_DECLARE_PROPERTY(eCProcessibleElement, GAME_ALL, bool, IsPersistable);

		private:

		};

		/*
			Resource
		*/

		class eCResource2 : public bCObjectBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_RISEN1,	1	},
			};

			GE_DECLARE_CLASS(eCResource2, bCObjectBase);

			eCResource2()			{}
			virtual ~eCResource2()	{}

			virtual bool LoadData(FileStream*, GAME) { return true; }
			virtual bool SaveData(FileStream*, GAME) { return true; }

			bool Save(FileStream*, GAME);
			bool Load(FileStream*, GAME);

		private:

		};
	};
};
