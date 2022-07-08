#pragma once

#include <string>
#include <vector>
#include "gothiclib_base.h"

#define GET_MACRO(_1,_2,_3,NAME,...) NAME

//#define DEFINE_CLASS(c)															\
//public:																			\
//	static inline const char*	GetStaticClassname()	{ return className; }	\
//	virtual const char*			GetClassName()			{ return className; }	\
//private:																		\
//	static inline const char* className = #c
//
//#define DEFINE_CLASS_EXTENDS(c, b)												\
//public:																			\
//	static inline const char*	GetStaticClassname()	{ return className; }	\
//	virtual const char*			GetClassName()			{ return className; }	\
//private:																		\
//	static inline const char* className = #c ":" #b

namespace GothicLib
{
	class FileStream;

	namespace ZenGin
	{
		extern ClassManager* classManager;

		class zCObject;

		/*
			Basic types
		*/

		struct zVEC3
		{
			float x, y, z;
		};

		struct zVEC4
		{
			float x, y, z, w;
		};

		struct zMAT3
		{
			zVEC3 x, y, z;
		};

		struct zMAT4
		{
			zVEC4 x, y, z, w;
		};

		struct zCOLOR
		{
			unsigned char b, g, r, a;
		};

		struct zTBBox3D
		{
			zVEC3 min, max;
		};

		/*
			Archiver (.zen reading/writing)
		*/

		class zCArchiver
		{
		public:

			zCArchiver()	{ }
			~zCArchiver()	{ }

			bool Read(FileStream*);

			bool ReadInt		(std::string, int&);
			bool ReadFloat		(std::string, float&);
			bool ReadBool		(std::string, bool&);
			bool ReadString		(std::string, std::string&);
			bool ReadVec3		(std::string, zVEC3&);
			bool ReadColor		(std::string, zCOLOR&);
			bool ReadRaw		(std::string, char*, size_t);
			bool ReadRawFloat	(std::string, float*, size_t);
			bool ReadEnum		(std::string, int&);

			bool ReadChunkStart(std::string*, std::string*, uint16_t*, uint32_t*);
			bool ReadChunkEnd();
		
			bool ReadObject(zCObject*&);
		
			template <class C> bool ReadObject(C& object)
			{
				zCObject* readObject;

				if (!ReadObject(readObject))
					return false;

				if (readObject == nullptr)
					return true;

				object = dynamic_cast<C>(readObject);

				if (object == nullptr)
					return false;
			
				return true;
			}

			zCObject* GetContainedObject();

		private:

			enum BINSAFE_TYPE
			{
				BINSAFE_TYPE_STRING = 0x1,
				BINSAFE_TYPE_INT = 0x2,
				BINSAFE_TYPE_FLOAT = 0x3,
				BINSAFE_TYPE_BOOL = 0x6,
				BINSAFE_TYPE_VEC3 = 0x7,
				BINSAFE_TYPE_COLOR = 0x8,
				BINSAFE_TYPE_RAW = 0x9,
				BINSAFE_TYPE_RAWFLOAT = 0x10,
				BINSAFE_TYPE_ENUM = 0x11
			};

			bool ReadPropertyASCII(std::string, std::string, std::string&);
			bool ReadPropertyBinSafe(BINSAFE_TYPE, char*&, size_t&);

			enum ARCHIVER_TYPE
			{
				ARCHIVER_TYPE_BINARY,
				ARCHIVER_TYPE_ASCII,
				ARCHIVER_TYPE_BIN_SAFE,

				ARCHIVER_TYPE_NONE = -1
			};

			FileStream*		file;

			unsigned short	version		= -1;
			ARCHIVER_TYPE	type		= ARCHIVER_TYPE_NONE;
			bool			savegame	= false;

			zCObject*				containedObject;

			std::vector<zCObject*>	objectList;

	#pragma pack(push,1)
			struct BinaryObjectHeader
			{
				uint32_t objectSize;
				uint16_t classVersion;
				uint32_t objectIndex;
			};

			struct BinSafeArchiveHeader
			{
				uint32_t version;
				uint32_t objectCount;
				uint32_t chunkPos;
			};

			struct BinSafeHashTable
			{
				uint16_t stringLength;
				uint16_t linearValue;
				uint32_t hashValue;
			};
	#pragma pack(pop)

		};

		/*
			Base object
		*/

		class zCObject
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

			zCObject()			{ }
			virtual ~zCObject()	{ }

			virtual bool Archive(zCArchiver*) = 0;
			virtual bool Unarchive(zCArchiver*) = 0;


			//virtual const char* GetClassName() = 0;

			static zCObject* CreateObject(std::string);

			int version = -1;

		private:

		};
	};
};
