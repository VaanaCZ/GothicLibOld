#include "zen_world.h"

using namespace GothicLib;

/*
	World classes
*/

bool ZenGin::zCWorld::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZenGin::zCWorld::Unarchive(zCArchiver* archiver)
{
	// Loop through chunks until the end
	std::string objectName;
	
	while (true)
	{
		archiver->ReadChunkStart(&objectName, nullptr, nullptr, nullptr);

		if (objectName == "VobTree")
		{
			vobTree = new zCVob();
			size_t vobCount = 0;

			if (!UnarcVobTree(vobTree, archiver, vobCount))
			{
				return false;
			}

		}
		else if (objectName == "EndMarker")
		{
			break;
		}
	}

	return true;
}

bool ZenGin::zCWorld::UnarcVobTree(zCVob* vob, zCArchiver* archiver, size_t& vobCount)
{
	// Read vob
	zCVob* nextVob;

	if (vobCount != 0)
	{
		if (!archiver->ReadObject<zCVob*>(nextVob))
			return false;

		vob->children.push_back(nextVob);
	}
	else
	{
		nextVob = vob;
	}
	
	// Read childs
	int childCount;
	if (!archiver->ReadInt("childs" + std::to_string(vobCount), childCount))
		return false;

	vobCount++;

	for (size_t i = 0; i < childCount; i++)
	{
		if (!UnarcVobTree(nextVob, archiver, vobCount))
			return false;
	}

	return true;
}

bool ZenGin::oCWorld::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZenGin::oCWorld::Unarchive(zCArchiver* archiver)
{
	if (!zCWorld::Unarchive(archiver))
		return false;

	//todo: savegame

	return true;
}

/*
	Vob classes
*/

bool ZenGin::zCVob::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZenGin::zCVob::Unarchive(zCArchiver* archiver)
{
	ZCR_START(zCVob);

	ZCR_READINT(pack);
	ZCR_READSTRING(presetName);
	ZCR_READRAWFLOAT(bbox3DWS);
	ZCR_READRAW(trafoOSToWSRot);
	ZCR_READVEC3(trafoOSToWSPos);
	ZCR_READSTRING(vobName);
	ZCR_READSTRING(visual);
	ZCR_READBOOL(showVisual);
	ZCR_READENUM(visualCamAlign);

	if (version == 52224)
	{
		ZCR_READENUM(visualAniMode);
		ZCR_READFLOAT(visualAniModeStrength);
		ZCR_READFLOAT(vobFarClipZScale);
	}

	ZCR_READBOOL(cdStatic);
	ZCR_READBOOL(cdDyn);
	ZCR_READBOOL(staticVob);
	ZCR_READENUM(dynShadow);

	if (version == 52224)
	{
		ZCR_READINT(zbias);
		ZCR_READBOOL(isAmbient);
	}

	ZCR_READOBJECT(visualPtr, "visual", zCVisual*);
	ZCR_READOBJECT(aiPtr, "ai", zCAIBase*);

	ZCR_END();

	return true;
}

bool ZenGin::zCVobLevelCompo::Archive(zCArchiver* archiver)
{
	return zCVob::Archive(archiver);
}

bool ZenGin::zCVobLevelCompo::Unarchive(zCArchiver* archiver)
{
	return zCVob::Unarchive(archiver);
}

bool ZenGin::zCVobSpot::Archive(zCArchiver* archiver)
{
	return zCVob::Archive(archiver);
}

bool ZenGin::zCVobSpot::Unarchive(zCArchiver* archiver)
{
	return zCVob::Unarchive(archiver);
}

bool ZenGin::zCVobSound::Archive(zCArchiver* archiver)
{
	if (!zCVob::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCVobSound::Unarchive(zCArchiver* archiver)
{
	if (!zCVob::Unarchive(archiver))
		return false;

	ZCR_START(zCVobSound);

	ZCR_READFLOAT(sndVolume);
	ZCR_READENUM(sndMode);
	ZCR_READFLOAT(sndRandDelay);
	ZCR_READFLOAT(sndRandDelayVar);
	ZCR_READBOOL(sndStartOn);
	ZCR_READBOOL(sndAmbient3D);
	ZCR_READBOOL(sndObstruction);
	ZCR_READFLOAT(sndConeAngle);
	ZCR_READENUM(sndVolType);
	ZCR_READFLOAT(sndRadius);
	ZCR_READSTRING(sndName);

	ZCR_END();

	return true;
}

bool ZenGin::zCVobLight::Archive(zCArchiver* archiver)
{
	if (!zCVob::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCVobLight::Unarchive(zCArchiver* archiver)
{
	if (!zCVob::Unarchive(archiver))
		return false;


	ZCR_START(zCVobLight);

	ZCR_READSTRING(lightPresetInUse);
	ZCR_READENUM(lightType);
	ZCR_READFLOAT(range);
	ZCR_READCOLOR(color);
	ZCR_READFLOAT(spotConeAngle);
	ZCR_READBOOL(lightStatic);
	ZCR_READENUM(lightQuality);
	ZCR_READSTRING(lensflareFX);
	ZCR_READBOOL(turnedOn);
	ZCR_READSTRING(rangeAniScale);
	ZCR_READFLOAT(rangeAniFPS);
	ZCR_READBOOL(rangeAniSmooth);
	ZCR_READSTRING(colorAniList);
	ZCR_READFLOAT(colorAniFPS);
	ZCR_READBOOL(colorAniSmooth);

	ZCR_END();

	return true;
}

/*
	Visual classes
*/

bool ZenGin::zCVisual::Archive(zCArchiver* archiver)
{
	return true;
}

bool ZenGin::zCVisual::Unarchive(zCArchiver* archiver)
{
	return true;
}

bool ZenGin::zCMesh::Archive(zCArchiver* archiver)
{
	return true;
}

bool ZenGin::zCMesh::Unarchive(zCArchiver* archiver)
{
	return true;
}

bool ZenGin::zCParticleFX::Archive(zCArchiver* archiver)
{
	return true;
}

bool ZenGin::zCParticleFX::Unarchive(zCArchiver* archiver)
{
	return true;
}

/*
	AI classes
*/

bool ZenGin::zCAIBase::Archive(zCArchiver* archiver)
{
	return true;
}

bool ZenGin::zCAIBase::Unarchive(zCArchiver* archiver)
{
	return true;
}
