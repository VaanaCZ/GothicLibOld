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
		template <typename T> using bTRefPtrArray = std::vector<T>;
		template <typename T> using bTPOSmartPtr = std::shared_ptr<T>;
		template <typename T> using bTPropertyContainer = int;

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
			/* Versions */
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

			bool Write(FileStream*);
			bool Read(FileStream*);

			bCObjectBase*	GetNativeObject() { return nativeObject; }

			template <class C> C GetNativeObjectAs()
			{
				bCObjectBase* object = GetNativeObject();

				if (object == nullptr)
					return nullptr;

				return dynamic_cast<C>(object);
			}
			
			GAME game = GAME_NONE;

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

		struct CLASS_REVISION
		{
			GAME		game;
			uint16_t	version;
		};

		class PropertyDefinition;
				
		class ClassDefinition
		{
		public:

			ClassDefinition(std::string, std::string, bCObjectBase*(*)());
			~ClassDefinition();

			inline std::string		GetName()			{ return name; };
			inline std::string		GetBase()			{ return base; };
			inline ClassDefinition*	GetBaseDef()		{ if (!baseDef) { baseDef = GetClassDef(base); } return baseDef; };
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
			new ClassDefinition(#C, #B, &C::CreateInstance);					\
																				\
		inline static ClassDefinition* GetStaticClassDef() { return classDef; }	\
		virtual ClassDefinition* GetClassDef() { return classDef; }				\
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

			GE_DECLARE_CLASS(bCObjectBase, );

			bCObjectBase()			{}
			virtual ~bCObjectBase()	{}

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			GAME game = GAME_NONE;

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

			virtual bool Save(FileStream*, FileStream* = nullptr);
			virtual bool Load(FileStream*, FileStream* = nullptr);

			GE_DECLARE_PROPERTY(eCProcessibleElement, GAME_ALL, bool, IsPersistable);

		private:

		};

	};
};
