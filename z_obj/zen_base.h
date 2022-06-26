#pragma once

#include <string>
#include <vector>

#define GET_MACRO(_1,_2,_3,NAME,...) NAME

#define DEFINE_CLASS(c)															\
public:																			\
	static inline const char*	GetStaticClassname()	{ return className; }	\
	virtual const char*			GetClassName()			{ return className; }	\
private:																		\
	static inline const char* className = #c

#define DEFINE_CLASS_EXTENDS(c, b)												\
public:																			\
	static inline const char*	GetStaticClassname()	{ return className; }	\
	virtual const char*			GetClassName()			{ return className; }	\
private:																		\
	static inline const char* className = #c ":" #b

namespace ZEN
{
	class FileStream;
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

		bool ReadChunkStart(std::string*, std::string*, int*, int*);
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

		bool ReadPropertyASCII(std::string, std::string, std::string&);

		enum ARCHIVER_TYPE
		{
			ARCHIVER_TYPE_BINARY,
			ARCHIVER_TYPE_ASCII,
			ARCHIVER_TYPE_BIN_SAFE,

			ARCHIVER_TYPE_NONE = -1
		};

		FileStream*		file;

		int				version		= -1;
		ARCHIVER_TYPE	type		= ARCHIVER_TYPE_NONE;
		bool			savegame	= false;

		zCObject*				containedObject;

		std::vector<zCObject*>	objectList;

	};

	/*
		Base object
	*/

	class zCObject
	{
	public:

		zCObject()			{ }
		virtual ~zCObject()	{ }

		virtual bool Archive(zCArchiver*) = 0;
		virtual bool Unarchive(zCArchiver*) = 0;

		virtual const char* GetClassName() = 0;

		static zCObject* CreateObject(std::string);

		int version = -1;

	private:

	};
};
