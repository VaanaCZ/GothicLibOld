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

#define ARC_ARGS(V)			#V, V
#define ARC_ARGSE(V)		#V, (int&)V
#define ARC_ARGSEW(V, C)	#V, C, (int&)V
#define ARC_ARGSR(V)		#V, (char*)&V, sizeof(V)
#define ARC_ARGSF(V)		#V, (float*)&V, sizeof(V) / 4

		/*
			Basic types
		*/

		typedef int zBOOL;

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

		struct zDATE
		{
			uint32_t year;
			uint16_t month;
			uint16_t day;
			uint16_t hour;
			uint16_t minute;
			uint16_t second;
		};

		struct zTPlane
		{
			float distance;
			zVEC3 normal;
		};

		inline double roundDecimals(const double d, const int p)
		{
			if (d == 0.0)
				return 0.0;

			double r = 0.5 / pow(10.0, p);
			if (d < 0.) return d - r;
			return d + r;
		}

		inline std::string FloatToString(float f)
		{
			char buffer[64];
			sprintf_s(buffer, 64, "%.9g", roundDecimals(f, 11));
			return std::string(buffer);
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

			bool Write(FileStream*, bool = false);
			bool Read(FileStream*);

			void EndWrite();
			void Close();

			// Write functions
			bool WriteInt		(std::string, int);
			bool WriteByte		(std::string, unsigned char);
			bool WriteWord		(std::string, unsigned short);
			bool WriteFloat		(std::string, float);
			bool WriteBool		(std::string, zBOOL);
			bool WriteString	(std::string, std::string);
			bool WriteVec3		(std::string, zVEC3);
			bool WriteColor		(std::string, zCOLOR);
			bool WriteEnum		(std::string, std::string, int);
			bool WriteRaw		(std::string, char*, size_t);
			bool WriteRawFloat	(std::string, float*, size_t);

			bool WriteObject(GAME g, zCObject* o) { return WriteObject(std::string(), g, o); }
			bool WriteObject(std::string, GAME, zCObject*);

			bool WriteChunkStart(std::string name) { return WriteChunkStart(name, "", 0, 0); }
			bool WriteChunkStart(std::string, std::string, uint16_t, uint32_t);
			bool WriteChunkEnd();

			bool WriteGroupBegin(std::string);
			bool WriteGroupEnd(std::string);

			// Read functions
			bool ReadInt		(std::string, int&);
			bool ReadByte		(std::string, unsigned char&);
			bool ReadWord		(std::string, unsigned short&);
			bool ReadFloat		(std::string, float&);
			bool ReadBool		(std::string, zBOOL&);
			bool ReadString		(std::string, std::string&);
			bool ReadVec3		(std::string, zVEC3&);
			bool ReadColor		(std::string, zCOLOR&);
			bool ReadEnum		(std::string, int&);
			bool ReadRaw		(std::string, char*, size_t);
			bool ReadRawFloat	(std::string, float*, size_t);

			zCObject* ReadObject(GAME g, zCObject* o = nullptr) { return ReadObject(std::string(), g, o); }
			zCObject* ReadObject(std::string, GAME, zCObject* = nullptr);

			bool ReadChunkStart(std::string*, std::string*, uint16_t*, uint32_t*);
			bool ReadChunkEnd();
		
			template <class C> C ReadObjectAs(std::string name, GAME game, C object = nullptr)
			{
				zCObject* readObject = ReadObject(name, game, object);

				if (readObject == nullptr)
					return nullptr;

				return dynamic_cast<C>(readObject);
			}

			template <class C> C ReadObjectAs(GAME g, C o = nullptr)
			{
				return ReadObjectAs<C>(std::string(), g, o);
			}

			inline FileStream*	GetFile() { return file; }

			inline bool	IsSavegame()	{ return savegame; }
			inline bool	IsProperties()	{ return properties; }

			bool error = false;

		private:

			enum BINSAFE_TYPE
			{
				BINSAFE_TYPE_STRING		= 0x1,
				BINSAFE_TYPE_INTEGER	= 0x2,
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

			bool WriteASCIIProperty(std::string, std::string, std::string);
			bool WriteBinSafeProperty(BINSAFE_TYPE, void*, size_t = 0);

			bool ReadASCIIProperty(std::string, std::string, std::string&);
			bool ReadBinSafeProperty(BINSAFE_TYPE, void*, size_t = 0);

			public: //temp
			enum ARCHIVER_MODE
			{
				ARCHIVER_MODE_BINARY,
				ARCHIVER_MODE_ASCII,
				ARCHIVER_MODE_BIN_SAFE,

				ARCHIVER_MODE_NONE = -1
			};
			private: // temp

			FileStream*		file;

			unsigned short	version		= -1;

			public: //temp
			ARCHIVER_MODE	mode		= ARCHIVER_MODE_NONE;
			private: // temp

			bool			savegame	= false;
			bool			properties	= false;

			struct CHUNK
			{
				std::string	objectName;
				std::string	className;
				uint32_t	objectIndex		= 0;
				uint64_t	objectOffset	= 0;
				uint64_t	binObjectSize	= 0;
			};

			std::vector<CHUNK>		chunkStack;

			std::vector<zCObject*>	objectList;

			uint64_t				objectCountPos = 0;

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

#define VERSION_NONE 0xFFFF

		struct CLASS_REVISION
		{
			GAME		game;
			uint16_t	versionSum;
		};

		// By default the version checksum is automatically calculated
		// by hashing the whole class hierarchy. However, Due to a bug
		// in the engine, the pointer to the base class is not always
		// initialized when the versionSum is being processed. This
		// means the versionSum can differ between compilations
		// meaning we cannot realiably replicate it.

		class ClassDefinition
		{
		public:

			ClassDefinition(std::string, std::string, zCObject*(*)(), CLASS_REVISION*, size_t);
			~ClassDefinition();

			inline std::string		GetName()			{ return name; };
			inline std::string		GetBase()			{ return base; };
			inline ClassDefinition*	GetBaseDef()		{ return baseDef; };
			inline bool				IsAbstract()		{ return createFunc == nullptr; };
			inline zCObject*		CreateInstance()	{ return createFunc(); };
			uint16_t				GetVersionSum(GAME);
			bool					IsVersionSumSupported(GAME, uint16_t);

			static ClassDefinition* GetClassDef(std::string);

		private:

			std::string			name;
			std::string			base;
			ClassDefinition*	baseDef = nullptr;
			zCObject*			(*createFunc)();
			CLASS_REVISION*		revisions;
			size_t				revisionCount;

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
		virtual ClassDefinition* GetClassDef() { return classDef; }				\
																				\
		DECLARE_MEMORY_POOL(C)

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

			virtual bool Archive(zCArchiver*, GAME)		{ return true; }
			virtual bool Unarchive(zCArchiver*, GAME)	{ return true; }

			virtual bool Save(FileStream*, GAME)		{ return true; }
			virtual bool Load(FileStream*, GAME)		{ return true; }

		protected:

			bool ParseFileLine(std::string, std::string&, std::string&);

		private:

		};

		/*
			Resource
		*/

		class zCResource : public zCObject
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_CHRISTMASEDITION,	0	},
				{ GAME_GOTHIC1,				0	},
				{ GAME_GOTHICSEQUEL,		0	},
				{ GAME_GOTHIC2,				0	},
				{ GAME_GOTHIC2ADDON,		0	},
			};

			ZEN_DECLARE_CLASS_ABSTRACT(zCResource, zCObject);

			zCResource()			{ }
			virtual ~zCResource()	{ }

		private:

		};
	};
};
