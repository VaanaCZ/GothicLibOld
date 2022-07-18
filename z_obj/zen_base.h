#pragma once

#include <string>
#include <vector>
#include "gothiclib_base.h"

namespace GothicLib
{
	class FileStream;

	namespace ZenGin
	{
		class zCObject;

		/*
			Macros
		*/

#define ARC_ARGS(V)		#V, V
#define ARC_ARGSE(V)	#V, (int&)V
#define ARC_ARGSR(V)	#V, (char*)&V, sizeof(V)
#define ARC_ARGSF(V)	#V, (float*)&V, sizeof(V) / 4

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
			Hash
		*/

		static uint16_t crcTable[256] =
		{
			0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
			0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
			0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
			0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
			0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
			0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
			0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
			0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
			0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
			0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
			0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
			0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
			0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
			0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
			0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
			0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
			0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
			0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
			0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
			0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
			0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
			0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
			0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
			0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
			0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
			0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
			0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
			0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
			0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
			0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
			0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
			0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
		};

		inline uint16_t CRC16(char* buffer, size_t bufferSize, uint16_t _crc = 0)
		{
			int crc = _crc;

			if (bufferSize > 0)
			{
				do
				{
					crc = ((crc >> 8) & 0xff) ^ crcTable[(crc ^ *buffer++) & 0xff];
				} while (--bufferSize);
			}

			return (uint16_t)crc;
		}

		/*
			Archiver (.zen reading/writing)
		*/

		enum GAME
		{
			/* Versions */
			GAME_DEMO3,

				GAME_G1_056C = GAME_DEMO3,

			GAME_DEMO5,

				GAME_G1_064B = GAME_DEMO5,

			GAME_SEPTEMBERDEMO,

				GAME_G1_094K = GAME_SEPTEMBERDEMO,

			//GAME_G1_100B,

			GAME_CHRISTMASEDITION,

				GAME_G1_101D = GAME_CHRISTMASEDITION,
				GAME_G1_101E = GAME_CHRISTMASEDITION,

			GAME_GOTHIC1,

				GAME_G1_104D = GAME_GOTHIC1,
				GAME_G1_106L = GAME_GOTHIC1,
				GAME_G1_107C = GAME_GOTHIC1,
				GAME_G1_108H = GAME_GOTHIC1,
				GAME_G1_108I = GAME_GOTHIC1,
				GAME_G1_108J = GAME_GOTHIC1,
				GAME_G1_108K = GAME_GOTHIC1,

			GAME_GOTHICSEQUEL,

				GAME_G1A_112F = GAME_GOTHICSEQUEL,

			GAME_GOTHIC2,

				GAME_G2_128 = GAME_GOTHIC2,
				GAME_G2_129 = GAME_GOTHIC2,
				GAME_G2_130 = GAME_GOTHIC2,
				GAME_G2_131 = GAME_GOTHIC2,
				GAME_G2_132 = GAME_GOTHIC2,

			GAME_GOTHIC2ADDON,

				GAME_G2A_26 = GAME_GOTHIC2ADDON,

			GAME_ALL,

			GAME_NONE = -1
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

			zCObject* ReadObject(zCObject* o = nullptr) { return ReadObject(std::string(), o); }
			zCObject* ReadObject(std::string, zCObject* = nullptr);

			bool ReadChunkStart(std::string*, std::string*, uint16_t*, uint32_t*);
			bool ReadChunkEnd();
		
			template <class C> C ReadObjectAs(std::string name, C object = nullptr)
			{
				zCObject* readObject = ReadObject(name, object);

				if (readObject == nullptr)
					return nullptr;

				return dynamic_cast<C>(readObject);
			}

			template <class C> C ReadObjectAs(C o = nullptr)
			{
				return ReadObjectAs<C>(std::string(), o);
			}

			inline FileStream*	GetFile() { return file; }

			inline bool	IsSavegame()	{ return savegame; }
			inline bool	IsProps()		{ return props; }

			GAME game = GAME_NONE;
			bool error = false;

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
			Object persistence
		*/

		struct CLASS_REVISION
		{
			GAME		game;
			uint16_t	version;
			uint16_t	versionSum = 0;
		};

		class ClassDefinition
		{
		public:

			ClassDefinition(std::string, std::string, zCObject*(*)(), CLASS_REVISION*, size_t);
			~ClassDefinition();

			inline std::string	GetName()			{ return name; };
			inline std::string	GetBase()			{ return base; };
			inline bool			IsAbstract()		{ return createFunc == nullptr; };
			inline zCObject*	CreateInstance()	{ return createFunc(); };
			uint16_t			GetVersionSum(GAME);

			static ClassDefinition* GetClassDef(std::string);

		private:

			std::string		name;
			std::string		base;
			zCObject*		(*createFunc)();
			CLASS_REVISION*	revisions;
			size_t			revisionCount;

			bool isRevisionsListInit = false;

			inline static std::unordered_map<std::string, ClassDefinition*>* classList = nullptr;

		};

#define ZEN_DECLARE_CLASS(C, B)													\
		static zCObject* CreateInstance() { return new C(); }					\
																				\
		inline static ClassDefinition* classDef =								\
			new ClassDefinition(#C, #B, &C::CreateInstance, revisions,			\
								sizeof(revisions) / sizeof(revisions[0]));		\
																				\
		inline static ClassDefinition* GetStaticClassDef() { return classDef; }	\
		virtual ClassDefinition* GetClassDef() { return classDef; }

#define ZEN_DECLARE_CLASS_ABSTRACT(C, B)										\
		inline static ClassDefinition* classDef =								\
			new ClassDefinition(#C, #B, nullptr, revisions,						\
								sizeof(revisions) / sizeof(revisions[0]));		\
																				\
		inline static ClassDefinition* GetStaticClassDef() { return classDef; }	\
		virtual ClassDefinition* GetClassDef() { return classDef; }

		/*
			Base object
		*/

		class zCObject
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_ALL, 0 }
			};

			ZEN_DECLARE_CLASS_ABSTRACT(zCObject, );

			zCObject()			{ }
			virtual ~zCObject()	{ }

			virtual bool Archive(zCArchiver*)	{ return true; }
			virtual bool Unarchive(zCArchiver*) { return true; }

			virtual bool Save(FileStream*)		{ return true; }
			virtual bool Load(FileStream*)		{ return true; }

			GAME game = GAME_NONE;

		protected:

			bool ParseFileLine(std::string, std::string&, std::string&);

		private:

		};
	};
};
