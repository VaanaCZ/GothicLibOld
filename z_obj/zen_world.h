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

		virtual void Archive(zCArchiver&);
		virtual void Unarchive(zCArchiver&);

	private:

	};

	class oCWorld : public zCWorld
	{
		DEFINE_CLASS(oCWorld, zCWorld);

	public:

		oCWorld() { }
		virtual ~oCWorld() { }

		virtual void Archive(zCArchiver&);
		virtual void Unarchive(zCArchiver&);

	private:

	};
};