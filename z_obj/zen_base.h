#pragma once

#include <string>
#include <vector>

#define DEFINE_CLASS(c, b)														\
public:																			\
	static inline const char*	GetStaticClassname()	{ return className; }	\
	virtual const char*			GetClassName()			{ return className; }	\
private:																		\
	static inline const char* className = #c ":" #b

namespace ZEN
{
	class FileStream;
	class zCObject;

	class zCArchiver
	{
	public:

		zCArchiver()	{ }
		~zCArchiver()	{ }

		bool Read(FileStream*);

		zCObject* GetContainedObject();

	private:

		//bool ReadBool(FileStream*);
		zCObject*	ReadObject(FileStream*);

		enum ARCHIVER_TYPE
		{
			ARCHIVER_TYPE_BINARY,
			ARCHIVER_TYPE_ASCII,
			ARCHIVER_TYPE_BIN_SAFE,

			ARCHIVER_TYPE_NONE = -1
		};

		int				version		= -1;
		ARCHIVER_TYPE	type		= ARCHIVER_TYPE_NONE;
		bool			savegame	= false;

		zCObject*				containedObject;

		std::vector<zCObject*>	objectList;

	};

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
