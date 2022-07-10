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

	if (archiver->IsSavegame())
	{
		ZCR_READBOOL(physicsEnabled);
	}

	if (archiver->IsSavegame())
	{
		char sleepMode;
		ZCR_READBYTE(sleepMode);
		this->sleepMode = (zTVobSleepingMode)sleepMode;
		ZCR_READFLOAT(nextOnTimer);
	}

	ZCR_END();

	return true;
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

	if (archiver->IsSavegame())
	{
		ZCR_READBOOL(soundIsRunning);
		ZCR_READBOOL(soundAllowedToRun);
	}
	else
	{
		soundIsRunning		= sndStartOn;
		soundAllowedToRun	= sndStartOn;
	}

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

	// start of zCVobLightData::Unarchive
	ZCR_READENUM(lightType);
	ZCR_READFLOAT(range);
	ZCR_READCOLOR(color);
	ZCR_READFLOAT(spotConeAngle);
	ZCR_READBOOL(lightStatic);

	if (archiver->IsProps() ||  !lightStatic)
	{
		ZCR_READENUM(lightQuality);
		ZCR_READSTRING(lensflareFX);
		ZCR_READBOOL(turnedOn);
		ZCR_READSTRING(rangeAniScale);
		ZCR_READFLOAT(rangeAniFPS);
		ZCR_READBOOL(rangeAniSmooth);
		ZCR_READSTRING(colorAniList);
		ZCR_READFLOAT(colorAniFPS);
		ZCR_READBOOL(colorAniSmooth);
		// todo: G2 - bool canMove
	}

	ZCR_END();

	return true;
}

bool ZenGin::oCMOB::Archive(zCArchiver* archiver)
{
	if (!oCVob::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::oCMOB::Unarchive(zCArchiver* archiver)
{
	if (!oCVob::Unarchive(archiver))
		return false;

	ZCR_START(oCMOB);

	ZCR_READSTRING(focusName);
	ZCR_READINT(hitpoints);
	ZCR_READINT(damage);
	ZCR_READBOOL(moveable);
	ZCR_READBOOL(takeable);
	ZCR_READBOOL(focusOverride);
	ZCR_READENUM(soundMaterial);
	ZCR_READSTRING(visualDestroyed);
	ZCR_READSTRING(owner);
	ZCR_READSTRING(ownerGuild);

	if (!archiver->IsProps())
	{
		ZCR_READBOOL(isDestroyed);
	}

	ZCR_END();

	return true;
}

bool ZenGin::oCMobInter::Archive(zCArchiver* archiver)
{
	if (!oCMOB::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::oCMobInter::Unarchive(zCArchiver* archiver)
{
	if (!oCMOB::Unarchive(archiver))
		return false;

	ZCR_START(oCMobInter);

	if (!archiver->IsProps())
	{
		ZCR_READINT(stateNum);
	}

	ZCR_READSTRING(triggerTarget);
	ZCR_READSTRING(useWithItem);
	ZCR_READSTRING(conditionFunc);
	ZCR_READSTRING(onStateFunc);
	ZCR_READBOOL(rewind);

	ZCR_END();

	return true;
}

bool ZenGin::zCTriggerBase::Archive(zCArchiver* archiver)
{
	if (!zCVob::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCTriggerBase::Unarchive(zCArchiver* archiver)
{
	if (!zCVob::Unarchive(archiver))
		return false;

	ZCR_START(zCTriggerBase);

	ZCR_READSTRING(triggerTarget);

	ZCR_END();

	return true;
}

bool ZenGin::zCTrigger::Archive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCTrigger::Unarchive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Unarchive(archiver))
		return false;

	ZCR_START(zCTrigger);


	if (!archiver->IsProps())
	{
		ZCR_READRAW(flags);
		ZCR_READRAW(filterFlags);
	}
	else
	{
		bool reactToOnTrigger;
		ZCR_READBOOL(reactToOnTrigger);
		filterFlags.reactToOnTrigger = reactToOnTrigger;

		bool reactToOnTouch;
		ZCR_READBOOL(reactToOnTouch);
		filterFlags.reactToOnTouch = reactToOnTouch;

		bool reactToOnDamage;
		ZCR_READBOOL(reactToOnDamage);
		filterFlags.reactToOnDamage = reactToOnDamage;

		bool respondToObject;
		ZCR_READBOOL(respondToObject);
		filterFlags.respondToObject = respondToObject;

		bool respondToPC;
		ZCR_READBOOL(respondToPC);
		filterFlags.respondToPC = respondToPC;

		bool respondToNPC;
		ZCR_READBOOL(respondToNPC);
		filterFlags.respondToNPC = respondToNPC;

		bool startEnabled;
		ZCR_READBOOL(startEnabled);
		flags.startEnabled = startEnabled;
	}

	ZCR_READSTRING(respondToVobName);
	ZCR_READINT(numCanBeActivated);
	ZCR_READFLOAT(retriggerWaitSec);
	ZCR_READFLOAT(damageThreshold);
	ZCR_READFLOAT(fireDelaySec);

	if (archiver->IsProps())
	{
		ZCR_READBOOL(sendUntrigger);
	}

	if (archiver->IsSavegame())
	{
		ZCR_READFLOAT(nextTimeTriggerable);
		ZCR_READOBJECT(savedOtherVobPtr, "savedOtherVob", zCVob*);
		ZCR_READINT(countCanBeActivated);

		bool isEnabled;
		ZCR_READBOOL(isEnabled);
		flags.isEnabled = isEnabled;
	}
	else
	{
		flags.isEnabled = flags.startEnabled;
	}

	ZCR_END();

	return true;
}

bool ZenGin::oCTriggerChangeLevel::Archive(zCArchiver* archiver)
{
	if (!zCTrigger::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::oCTriggerChangeLevel::Unarchive(zCArchiver* archiver)
{
	if (!zCTrigger::Unarchive(archiver))
		return false;

	ZCR_START(oCTriggerChangeLevel);

	ZCR_READSTRING(levelName);
	ZCR_READSTRING(startVobName);

	ZCR_END();

	return true;
}

bool ZenGin::oCItem::Archive(zCArchiver* archiver)
{
	if (!oCVob::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::oCItem::Unarchive(zCArchiver* archiver)
{
	if (!oCVob::Unarchive(archiver))
		return false;

	ZCR_START(oCItem);

	ZCR_READSTRING(itemInstance);

	if (archiver->IsSavegame())
	{
		ZCR_READINT(amount);
		ZCR_READINT(flags);
	}

	ZCR_END();

	return true;
}

/*
	Visual classes
*/

bool ZenGin::zCDecal::Archive(zCArchiver* archiver)
{
	if (!zCVisual::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCDecal::Unarchive(zCArchiver* archiver)
{
	if (!zCVisual::Unarchive(archiver))
		return false;

	ZCR_START(zCDecal);

	ZCR_READSTRING(name);
	ZCR_READRAWFLOAT(decalDim);
	ZCR_READRAWFLOAT(decalOffset);
	ZCR_READBOOL(decal2Sided);
	ZCR_READENUM(decalAlphaFunc);
	ZCR_READFLOAT(decalTexAniFPS);
	// todo: decalAlphaWeight - G2
	// todo: ignoreDayLight - G2

	ZCR_END();

	return true;
}


/*
	AI classes
*/
