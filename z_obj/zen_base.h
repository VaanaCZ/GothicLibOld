#pragma once

#include <string>
#include <vector>
#include "gothiclib_base.h"

#define GET_MACRO(_1,_2,_3,NAME,...) NAME

#define GOTHICLIB_ZENGIN_OLD_SAVE_LOAD

namespace GothicLib
{
	class FileStream;

	namespace ZenGin
	{
		extern ClassManager* classManager;

		class zCObject;

		/*
			Macros
		*/

		// ZenGin Class Read

#define ZCR_START(C)			std::string zcrName = #C;									\
								bool zcrValid = true;

#define ZCR_READINT(V)			if (!archiver->ReadInt(#V, V))								zcrValid = false;
#define ZCR_READBYTE(V)			if (!archiver->ReadByte(#V, V))								zcrValid = false;
#define ZCR_READWORD(V)			if (!archiver->ReadWord(#V, V))								zcrValid = false;
#define ZCR_READFLOAT(V)		if (!archiver->ReadFloat(#V, V))							zcrValid = false;
#define ZCR_READBOOL(V)			if (!archiver->ReadBool(#V, V))								zcrValid = false;
#define ZCR_READSTRING(V)		if (!archiver->ReadString(#V, V))							zcrValid = false;
#define ZCR_READVEC3(V)			if (!archiver->ReadVec3(#V, V))								zcrValid = false;
#define ZCR_READCOLOR(V)		if (!archiver->ReadColor(#V, V))							zcrValid = false;
#define ZCR_READENUM(V)			if (!archiver->ReadEnum(#V, *(int*)&V))						zcrValid = false;
#define ZCR_READRAW(V)			if (!archiver->ReadRaw(#V, (char*)&V, sizeof(V)))			zcrValid = false;
#define ZCR_READRAWFLOAT(V)		if (!archiver->ReadRawFloat(#V, (float*)&V, sizeof(V) / 4))	zcrValid = false;

#define ZCR_READOBJECT(V, N, C)	if (!archiver->ReadObject<C>(N, V))							zcrValid = false;


#define ZCR_END()				if (!zcrValid)												\
								{															\
									LOG_ERROR("Error while unarchiving class " + zcrName);	\
									return false;											\
								}

		// ZenGin Class Write

#define ZCW_START(C)			std::string zcwName = #C;									\
								bool zcwValid = true;

#define ZCW_WRITEINT(V)			if (!archiver->WriteInt(#V, V))								zcwValid = false;
#define ZCW_WRITEBYTE(V)		if (!archiver->WriteByte(#V, V))							zcwValid = false;
#define ZCW_WRITEWORD(V)		if (!archiver->WriteWord(#V, V))							zcwValid = false;
#define ZCW_WRITEFLOAT(V)		if (!archiver->WriteFloat(#V, V))							zcwValid = false;
#define ZCW_WRITEBOOL(V)		if (!archiver->WriteBool(#V, V))							zcwValid = false;
#define ZCW_WRITESTRING(V)		if (!archiver->WriteString(#V, V))							zcwValid = false;
#define ZCW_WRITEVEC3(V)		if (!archiver->WriteVec3(#V, V))							zcwValid = false;
#define ZCW_WRITECOLOR(V)		if (!archiver->WriteColor(#V, V))							zcwValid = false;
#define ZCW_WRITEENUM(V)		if (!archiver->WriteEnum(#V, *(int*)&V))					zcwValid = false;
#define ZCW_WRITERAW(V)			if (!archiver->ReadRaw(#V, (char*)&V, sizeof(V)))			zcrValid = false;
#define ZCW_WRITERAWFLOAT(V)	if (!archiver->ReadRawFloat(#V, (float*)&V, sizeof(V) / 4))	zcrValid = false;

#define ZCW_END()				if (!zcwValid)												\
								{															\
									LOG_ERROR("Error while archiving class " + zcwName);	\
									return false;											\
								}

		/*
			Basic types
		*/

		struct zVEC2
		{
			float x, y;
		};

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

		struct zCQuat
		{
			float qx, qy, qz, qw;
		};

		struct zTBBox3D
		{
			zVEC3 min, max;
		};

		/*
			Archiver (.zen reading/writing)
		*/

		enum GAME
		{
			GAME_AUTODETECT = -1
		};

		class zCArchiver
		{
		public:

			zCArchiver()	{ }
			~zCArchiver()	{ }

			bool Read(FileStream*);

			bool ReadInt		(std::string, int&);
			bool ReadByte		(std::string, char&);
			bool ReadWord		(std::string, short&);
			bool ReadFloat		(std::string, float&);
			bool ReadBool		(std::string, bool&);
			bool ReadString		(std::string, std::string&);
			bool ReadVec3		(std::string, zVEC3&);
			bool ReadColor		(std::string, zCOLOR&);
			bool ReadEnum		(std::string, int&);
			bool ReadRaw		(std::string, char*, size_t);
			bool ReadRawFloat	(std::string, float*, size_t);

			bool ReadObject(zCObject*& o) { return ReadObject(std::string(), o); }
			bool ReadObject(std::string, zCObject*&);

			bool ReadChunkStart(std::string*, std::string*, uint16_t*, uint32_t*);
			bool ReadChunkEnd();
		
			template <class C> bool ReadObject(std::string name, C& object)
			{
				zCObject* readObject;

				if (!ReadObject(name, readObject))
					return false;

				if (readObject == nullptr)
					return true;

				object = dynamic_cast<C>(readObject);

				if (object == nullptr)
					return false;
			
				return true;
			}

			template <class C> bool ReadObject(C& object)
			{
				return ReadObject<C>(std::string(), object);
			}

			zCObject* GetContainedObject();

			inline FileStream*	GetFile() { return file; }

			inline bool	IsSavegame()	{ return savegame; }
			inline bool	IsProps()		{ return props; }

		private:

			enum BINSAFE_TYPE
			{
				BINSAFE_TYPE_STRING		= 0x1,
				BINSAFE_TYPE_INT		= 0x2,
				BINSAFE_TYPE_FLOAT		= 0x3,
				BINSAFE_TYPE_BYTE		= 0x4,
				BINSAFE_TYPE_WORD		= 0x5,
				BINSAFE_TYPE_BOOL		= 0x6,
				BINSAFE_TYPE_VEC3		= 0x7,
				BINSAFE_TYPE_COLOR		= 0x8,
				BINSAFE_TYPE_RAW		= 0x9,
				BINSAFE_TYPE_RAWFLOAT	= 0x10,
				BINSAFE_TYPE_ENUM		= 0x11,
				BINSAFE_TYPE_HASH		= 0x12
			};

			bool ReadASCIIProperty(std::string, std::string, std::string&);
			bool ReadBinSafeProperty(BINSAFE_TYPE, char*&, size_t&);

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
			uint32_t		objectCount = -1;
			bool			props		= false;

			std::vector<uint64_t> asciiChunksPositions;

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
			Versioning
		*/

		class CLASS_REVISION
		{
			GAME		game;
			uint16_t	version;
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

			virtual bool Archive(zCArchiver*)	{ return true; }
			virtual bool Unarchive(zCArchiver*) { return true; }

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
			virtual void Save(FileStream*) { }
			virtual void Load(FileStream*) { }
#endif


			//virtual const char* GetClassName() = 0;

			static zCObject* CreateObject(std::string);

			int version = -1;

		private:

		};
	};
};
