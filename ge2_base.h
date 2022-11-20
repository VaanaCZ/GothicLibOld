#pragma once

#include <string>
#include <vector>
#include "gothiclib_base.h"

namespace GothicLib
{
	class FileStream;

	namespace Genome2
	{
		class bCObjectBase;

		typedef std::string bCString;
		template <typename T> using bTRefPtrArray = std::vector<T>;

		/*
			File
		*/

		class bCArchive
		{
		public:

			bCArchive()		{ }
			~bCArchive()	{ }

			bool Read(FileStream*);
			bool Write(FileStream*);

			bCObjectBase* ReadObject(bCObjectBase* o = nullptr);

		private:

			FileStream* file;

		};

		/*
			Archivation
		*/

		enum GAME
		{
			/* Versions */
			GAME_RISEN2,

			GAME_R2_100 = GAME_RISEN2,

			GAME_ALL,

			GAME_NONE = -1
		};

		class bCRuntimeClass
		{
		public:

			bCRuntimeClass()							{ }
			bCRuntimeClass(bCObjectBase* _nativeObject)	{ nativeObject = _nativeObject; }
			~bCRuntimeClass()							{ }

			bool Read(FileStream*);
			bool Write(FileStream*);

			bCObjectBase* GetNativeObject() { return nativeObject; }

			template <class C> C GetNativeObjectAs()
			{
				bCObjectBase* object = GetNativeObject();

				if (object == nullptr)
					return nullptr;

				return dynamic_cast<C>(object);
			}

			GAME game = GAME_NONE;

#pragma pack (push, 1)
			struct ClassWritten
			{
				uint32_t classHash;
				uint16_t version;
				uint32_t propertySize;
				uint32_t dataSize;
				uint32_t size;
				uint16_t propertyCount;
			};

			struct PropertyWritten
			{
				uint32_t typeHash;
				uint32_t nameHash;
				uint32_t size;
			};

			struct ClassDataWritten
			{
				uint32_t classHash;
				uint16_t version;
				uint32_t size;
			};
#pragma pack (pop)

		private:

			bCObjectBase* nativeObject = nullptr;

		};
		
		/*
			Object persistence
		*/

		uint32_t djb2(char const* name);

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

			ClassDefinition(std::string, std::string, bCObjectBase*(*)(), void*, CLASS_REVISION*, size_t);
			~ClassDefinition();

			inline std::string		GetName()			{ return name; };
			inline uint32_t			GetNameHash()		{ return djb2(("class " + name).c_str()); };
			inline std::string		GetBase()			{ return base; };
			inline ClassDefinition*	GetBaseDef()		{ if (!baseDef) { baseDef = GetClassDef(base); } return baseDef; };
			inline bCObjectBase*	CreateInstance()	{ return createFunc(); };
			bool					Read(bCObjectBase*, FileStream*);
			uint16_t				GetVersion(GAME);
			bool					IsVersionSupported(GAME, uint16_t);
			
			inline std::vector<PropertyDefinition*>& GetProperties() { return properties; };

			static ClassDefinition* GetClassDef(std::string);
			static ClassDefinition* GetClassDef(uint32_t);

		private:

			typedef bool (*ReadFunc)(bCObjectBase*, FileStream*);

			std::string			name;
			std::string			base;
			ClassDefinition*	baseDef = nullptr;
			bCObjectBase*		(*createFunc)();
			ReadFunc			readFunc;
			CLASS_REVISION*		revisions;
			size_t				revisionCount;

			std::vector<PropertyDefinition*> properties;

			inline static std::unordered_map<uint32_t, ClassDefinition*>* classList = nullptr;
		};

		class PropertyDefinition
		{
		public:

			PropertyDefinition(ClassDefinition*, GAME, std::string, std::string, size_t, size_t);
			~PropertyDefinition();
			
			inline GAME				GetGame()			{ return game; };
			inline std::string		GetName()			{ return name; };
			inline uint32_t			GetNameHash()		{ return djb2(name.c_str()); };
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

#define GE2_DECLARE_CLASS(C, B)													\
		static bCObjectBase* CreateInstance() { return new C(); }				\
																				\
		static bool OnReadStatic(C* c, FileStream* f)							\
			{ return c->C::OnRead(f); }											\
																				\
		inline static ClassDefinition* classDef =								\
			new ClassDefinition(#C, #B, &C::CreateInstance, &C::OnReadStatic,	\
								revisions,										\
								sizeof(revisions) / sizeof(revisions[0]));		\
																				\
		inline static ClassDefinition* GetStaticClassDef() { return classDef; }	\
		virtual ClassDefinition* GetClassDef() { return classDef; }				\
																				\
		virtual uint16_t GetVersion() { return classDef->GetVersion(game); }	\
																				\
		DECLARE_MEMORY_POOL(C)


#define GE2_DECLARE_PROPERTY(C, G, T, N)										\
		T N;																	\
																				\
		inline static PropertyDefinition* propDef_ ##N =						\
			new PropertyDefinition(classDef, G, #N, #T, sizeof(T),				\
			MemberOffset(&C::N));

		/*
			Base object
		*/

		class bIUnknown
		{
		public:

			bIUnknown()				{}
			virtual ~bIUnknown()	{}

		private:

		};

		class bCObject : public bIUnknown
		{
		public:

			bCObject()			{}
			virtual ~bCObject()	{}

		private:

		};

		class bCObjectBase : public bCObject
		{
		public:

			GAME game = GAME_NONE;

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_RISEN2,	1	},
			};

			GE2_DECLARE_CLASS(bCObjectBase, );

			bCObjectBase()			{}
			virtual ~bCObjectBase()	{}

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

		private:

		};

		class bCObjectRefBase : public bCObjectBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_RISEN2,	1	},
			};

			GE2_DECLARE_CLASS(bCObjectRefBase, bCObjectBase);

			bCObjectRefBase()			{}
			virtual ~bCObjectRefBase()	{}

		private:

		};
	}
};