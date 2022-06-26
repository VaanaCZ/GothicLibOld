#include "zen_world.h"

/*
	World classes
*/

bool ZEN::zCWorld::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZEN::zCWorld::Unarchive(zCArchiver* archiver)
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

		break;
	}

	return true;
}

bool ZEN::zCWorld::UnarcVobTree(zCVob* vob, zCArchiver* archiver, size_t& vobCount)
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

bool ZEN::oCWorld::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZEN::oCWorld::Unarchive(zCArchiver* archiver)
{
	if (!zCWorld::Unarchive(archiver))
		return false;

	//todo: savegame

	return true;
}

/*
	Vob classes
*/

bool ZEN::zCVob::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZEN::zCVob::Unarchive(zCArchiver* archiver)
{
	if (!archiver->ReadInt("pack", pack))
		return false;

	if (!archiver->ReadString("presetName", presetName))
		return false;

	if (!archiver->ReadRawFloat("bbox3DWS", (float*)&bbox3DWS, 6))
		return false;

	if (!archiver->ReadRaw("trafoOSToWSRot", (char*)&trafoOSToWSRot, sizeof(zMAT3)))
		return false;

	if (!archiver->ReadVec3("trafoOSToWSPos", trafoOSToWSPos))
		return false;

	if (!archiver->ReadString("vobName", vobName))
		return false;

	if (!archiver->ReadString("visual", visual))
		return false;

	if (!archiver->ReadBool("showVisual", showVisual))
		return false;

	if (!archiver->ReadEnum("visualCamAlign", *(int*)&visualCamAlign))
		return false;

	if (version == 52224)
	{
		if (!archiver->ReadEnum("visualAniMode", *(int*)&visualAniMode))
			return false;

		if (!archiver->ReadFloat("visualAniModeStrength", visualAniModeStrength))
			return false;

		if (!archiver->ReadFloat("vobFarClipZScale", vobFarClipZScale))
			return false;
	}

	if (!archiver->ReadBool("cdStatic", cdStatic))
		return false;

	if (!archiver->ReadBool("cdDyn", cdDyn))
		return false;

	if (!archiver->ReadBool("staticVob", staticVob))
		return false;

	if (!archiver->ReadEnum("dynShadow", *(int*)&dynShadow))
		return false;

	if (version == 52224)
	{
		if (!archiver->ReadInt("zbias", zbias))
			return false;

		if (!archiver->ReadBool("isAmbient", isAmbient))
			return false;
	}

	if (!archiver->ReadObject<zCVisual*>(visualPtr))
		return false;

	if (!archiver->ReadObject<zCAIBase*>(aiPtr))
		return false;

	return true;
}

bool ZEN::zCVobLevelCompo::Archive(zCArchiver* archiver)
{
	return zCVob::Archive(archiver);
}

bool ZEN::zCVobLevelCompo::Unarchive(zCArchiver* archiver)
{
	return zCVob::Unarchive(archiver);
}

bool ZEN::zCVobSpot::Archive(zCArchiver* archiver)
{
	return zCVob::Archive(archiver);
}

bool ZEN::zCVobSpot::Unarchive(zCArchiver* archiver)
{
	return zCVob::Unarchive(archiver);
}

bool ZEN::zCVobSound::Archive(zCArchiver* archiver)
{
	if (!zCVob::Archive(archiver))
		return false;

	return false;
}

bool ZEN::zCVobSound::Unarchive(zCArchiver* archiver)
{
	if (!zCVob::Unarchive(archiver))
		return false;

	if (!archiver->ReadFloat("sndVolume", sndVolume))
		return false;

	if (!archiver->ReadEnum("sndMode", *(int*)&sndMode))
		return false;

	if (!archiver->ReadFloat("sndRandDelay", sndRandDelay))
		return false;

	if (!archiver->ReadFloat("sndRandDelayVar", sndRandDelayVar))
		return false;

	if (!archiver->ReadBool("sndStartOn", sndStartOn))
		return false;

	if (!archiver->ReadBool("sndAmbient3D", sndAmbient3D))
		return false;

	if (!archiver->ReadBool("sndObstruction", sndObstruction))
		return false;

	if (!archiver->ReadFloat("sndConeAngle", sndConeAngle))
		return false;

	if (!archiver->ReadEnum("sndVolType", *(int*)&sndVolType))
		return false;

	if (!archiver->ReadFloat("sndRadius", sndRadius))
		return false;

	if (!archiver->ReadString("sndName", sndName))
		return false;

	return true;
}

bool ZEN::zCVobLight::Archive(zCArchiver* archiver)
{
	if (!zCVob::Archive(archiver))
		return false;

	return false;
}

bool ZEN::zCVobLight::Unarchive(zCArchiver* archiver)
{
	if (!zCVob::Unarchive(archiver))
		return false;

	if (!archiver->ReadString("lightPresetInUse", lightPresetInUse))
		return false;

	if (!archiver->ReadEnum("lightType", *(int*)&lightType))
		return false;

	if (!archiver->ReadFloat("range", range))
		return false;

	if (!archiver->ReadColor("color", color))
		return false;

	if (!archiver->ReadFloat("spotConeAngle", spotConeAngle))
		return false;

	if (!archiver->ReadBool("lightStatic", lightStatic))
		return false;

	if (!archiver->ReadEnum("lightQuality", *(int*)&lightQuality))
		return false;

	if (!archiver->ReadString("lensflareFX", lensflareFX))
		return false;

	if (!archiver->ReadBool("turnedOn", turnedOn))
		return false;

	if (!archiver->ReadString("rangeAniScale", rangeAniScale))
		return false;

	if (!archiver->ReadFloat("rangeAniFPS", rangeAniFPS))
		return false;

	if (!archiver->ReadBool("rangeAniSmooth", rangeAniSmooth))
		return false;

	if (!archiver->ReadString("colorAniList", colorAniList))
		return false;

	if (!archiver->ReadFloat("colorAniFPS", colorAniFPS))
		return false;

	if (!archiver->ReadBool("colorAniSmooth", colorAniSmooth))
		return false;

	return true;
}

/*
	Visual classes
*/

bool ZEN::zCVisual::Archive(zCArchiver* archiver)
{
	return true;
}

bool ZEN::zCVisual::Unarchive(zCArchiver* archiver)
{
	return true;
}

bool ZEN::zCMesh::Archive(zCArchiver* archiver)
{
	return true;
}

bool ZEN::zCMesh::Unarchive(zCArchiver* archiver)
{
	return true;
}

bool ZEN::zCParticleFX::Archive(zCArchiver* archiver)
{
	return true;
}

bool ZEN::zCParticleFX::Unarchive(zCArchiver* archiver)
{
	return true;
}

/*
	AI classes
*/

bool ZEN::zCAIBase::Archive(zCArchiver* archiver)
{
	return true;
}

bool ZEN::zCAIBase::Unarchive(zCArchiver* archiver)
{
	return true;
}
