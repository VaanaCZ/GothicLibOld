#include "zen_world.h"

bool ZEN::zCWorld::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZEN::zCWorld::Unarchive(zCArchiver* archiver)
{
	return true;
}

bool ZEN::oCWorld::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZEN::oCWorld::Unarchive(zCArchiver* archiver)
{
	if (!zCWorld::Unarchive(archiver))
		return false;

	return true;
}
