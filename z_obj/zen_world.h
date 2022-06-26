#pragma once

#include "zen_base.h"

namespace ZEN
{
	class zCWorld : public zCObject
	{
		DEFINE_CLASS(zCWorld, zCObject);

	public:
		
		zCWorld()			{ }
		virtual ~zCWorld()	{ }

		virtual bool Archive(zCArchiver*);
		virtual bool Unarchive(zCArchiver*);

	private:

	};

	class oCWorld : public zCWorld
	{
		DEFINE_CLASS(oCWorld, zCWorld);

	public:

		oCWorld() { }
		virtual ~oCWorld() { }

		virtual bool Archive(zCArchiver*);
		virtual bool Unarchive(zCArchiver*);

	private:

	};
};