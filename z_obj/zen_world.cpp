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
	
	while (true)
	{
		std::string objectName;

		archiver->ReadChunkStart(&objectName, nullptr, nullptr, nullptr);

		if (objectName == "MeshAndBsp")
		{
			if (!bsp.LoadBIN(archiver->GetFile()))
			{
				return false;
			}
		}
		if (objectName == "VobTree")
		{
			vobTree = new zCVob();
			size_t vobCount = 0;

			if (!UnarcVobTree(vobTree, archiver, vobCount))
			{
				return false;
			}
		}
		else if (objectName == "WayNet")
		{
			if (!archiver->ReadObject<zCWayNet*>(wayNet))
			{
				return false;
			}
		}
		else if (objectName == "EndMarker")
		{
			break;
		}
		else
		{
			LOG_ERROR("Unknown chunk \"" + objectName + "\" encountered in oCWorld!");
			return false;
		}

		archiver->ReadChunkEnd();
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
	BSP
*/

bool ZenGin::zCBspTree::SaveBIN(FileStream* file)
{
	return false;
}

bool ZenGin::zCBspTree::LoadBIN(FileStream* file)
{
	return false;
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

bool ZenGin::zCVobSoundDaytime::Archive(zCArchiver* archiver)
{
	if (!zCVobSound::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCVobSoundDaytime::Unarchive(zCArchiver* archiver)
{
	if (!zCVobSound::Unarchive(archiver))
		return false;

	ZCR_START(zCVobSound);

	ZCR_READFLOAT(sndStartTime);
	ZCR_READFLOAT(sndEndTime);
	ZCR_READSTRING(sndName2);

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

bool ZenGin::oCMobLockable::Archive(zCArchiver* archiver)
{
	if (!oCMobInter::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::oCMobLockable::Unarchive(zCArchiver* archiver)
{
	if (!oCMobInter::Unarchive(archiver))
		return false;

	ZCR_START(oCMobLockable);

	ZCR_READBOOL(locked);
	ZCR_READSTRING(keyInstance);
	ZCR_READSTRING(pickLockStr);

	ZCR_END();

	return true;
}

bool ZenGin::oCMobContainer::Archive(zCArchiver* archiver)
{
	if (!oCMobLockable::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::oCMobContainer::Unarchive(zCArchiver* archiver)
{
	if (!oCMobLockable::Unarchive(archiver))
		return false;

	ZCR_START(oCMobContainer);

	ZCR_READSTRING(contains);

	ZCR_END();

	return true;
}

bool ZenGin::oCMobFire::Archive(zCArchiver* archiver)
{
	if (!oCMobInter::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::oCMobFire::Unarchive(zCArchiver* archiver)
{
	if (!oCMobInter::Unarchive(archiver))
		return false;

	ZCR_START(oCMobFire);

	ZCR_READSTRING(fireSlot);
	ZCR_READSTRING(fireVobtreeName);

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

bool ZenGin::zCTriggerList::Archive(zCArchiver* archiver)
{
	if (!zCTrigger::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCTriggerList::Unarchive(zCArchiver* archiver)
{
	if (!zCTrigger::Unarchive(archiver))
		return false;

	ZCR_START(zCTriggerList);

	ZCR_READENUM(listProcess);

	char numTarget = 6;

	if (!archiver->IsProps())
	{
		ZCR_READBYTE(numTarget);
	}

	fireDelayList.resize(numTarget);
	triggerTargetList.resize(numTarget);

	for (size_t i = 0; i < numTarget; i++)
	{
		std::string	currTriggerTarget;
		float		currFireDelay;

		std::string currTriggerTargetKey	= "triggerTarget" + std::to_string(i);
		std::string currFireDelayKey		= "fireDelay" + std::to_string(i);

		if (!archiver->ReadString(currTriggerTargetKey, currTriggerTarget) ||
			!archiver->ReadFloat(currFireDelayKey, currFireDelay))
		{
			zcrValid = false;
		}

		triggerTargetList[i]	= currTriggerTarget;
		fireDelayList[i]		= currFireDelay;
	}

	if (archiver->IsSavegame())
	{
		ZCR_READBYTE(actTarget);
		ZCR_READBOOL(sendOnTrigger);
	}

	ZCR_END();

	return true;
}

bool ZenGin::zCMover::Archive(zCArchiver* archiver)
{
	if (!zCTrigger::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCMover::Unarchive(zCArchiver* archiver)
{
	if (!zCTrigger::Unarchive(archiver))
		return false;

	ZCR_START(zCMover);

	ZCR_READENUM(moverBehavior);
	ZCR_READFLOAT(touchBlockerDamage);
	ZCR_READFLOAT(stayOpenTimeSec);
	ZCR_READBOOL(moverLocked);
	ZCR_READBOOL(autoLinkEnabled);
	// todo: G2 - autoRotate

	short numKeyframes;
	ZCR_READWORD(numKeyframes);

	if (numKeyframes > 0)
	{
		ZCR_READFLOAT(moveSpeed);
		ZCR_READENUM(posLerpType);
		ZCR_READENUM(speedType);

		if (!archiver->IsProps())
		{
			keyframeList.resize(numKeyframes);

			if (!archiver->ReadRaw("keyframes", (char*)&keyframeList[0],
				numKeyframes * sizeof(zTMov_Keyframe)))
			{
				zcrValid = false;
			}
		}
	}

	if (archiver->IsSavegame())
	{
		ZCR_READVEC3(actKeyPosDelta);
		ZCR_READFLOAT(actKeyframeF);
		ZCR_READINT(actKeyframe);
		ZCR_READINT(nextKeyframe);
		ZCR_READFLOAT(moveSpeedUnit);
		ZCR_READFLOAT(advanceDir);
		ZCR_READENUM(moverState);
		ZCR_READINT(numTriggerEvents);
		ZCR_READFLOAT(stayOpenTimeDest);
	}

	ZCR_READSTRING(sfxOpenStart);
	ZCR_READSTRING(sfxOpenEnd);
	ZCR_READSTRING(sfxMoving);
	ZCR_READSTRING(sfxCloseStart);
	ZCR_READSTRING(sfxCloseEnd);
	ZCR_READSTRING(sfxLock);
	ZCR_READSTRING(sfxUnlock);
	ZCR_READSTRING(sfxUseLocked);

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

bool ZenGin::oCZoneMusic::Archive(zCArchiver* archiver)
{
	if (!zCZone::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::oCZoneMusic::Unarchive(zCArchiver* archiver)
{
	if (!zCZone::Unarchive(archiver))
		return false;

	ZCR_START(oCZoneMusic);

	ZCR_READBOOL(enabled);
	ZCR_READINT(priority);
	ZCR_READBOOL(ellipsoid);
	ZCR_READFLOAT(reverbLevel);
	ZCR_READFLOAT(volumeLevel);
	ZCR_READBOOL(loop);

	if (archiver->IsSavegame())
	{
		ZCR_READBOOL(local_enabled);
		ZCR_READBOOL(dayEntranceDone);
		ZCR_READBOOL(nightEntranceDone);
	}
	else
	{
		local_enabled = enabled;
	}

	ZCR_END();

	return true;
}

bool ZenGin::zCZoneZFog::Archive(zCArchiver* archiver)
{
	if (!zCZone::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCZoneZFog::Unarchive(zCArchiver* archiver)
{
	if (!zCZone::Unarchive(archiver))
		return false;

	ZCR_START(zCZoneZFog);

	ZCR_READFLOAT(fogRangeCenter);
	ZCR_READFLOAT(innerRangePerc);
	ZCR_READCOLOR(fogColor);
	// todo: G2 - fadeOutSky
	// todo: G2 - overrideColor

	ZCR_END();

	return true;
}

bool ZenGin::zCZoneVobFarPlane::Archive(zCArchiver* archiver)
{
	if (!zCZone::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCZoneVobFarPlane::Unarchive(zCArchiver* archiver)
{
	if (!zCZone::Unarchive(archiver))
		return false;

	ZCR_START(zCZoneVobFarPlane);

	ZCR_READFLOAT(vobFarPlaneZ);
	ZCR_READFLOAT(innerRangePerc);

	ZCR_END();

	return true;
}

bool ZenGin::zCPFXControler::Archive(zCArchiver* archiver)
{
	if (!zCEffect::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCPFXControler::Unarchive(zCArchiver* archiver)
{
	if (!zCEffect::Unarchive(archiver))
		return false;

	ZCR_START(zCPFXControler);

	ZCR_READSTRING(pfxName);
	ZCR_READBOOL(killVobWhenDone);
	ZCR_READBOOL(pfxStartOn);

	ZCR_END();

	return true;
}

bool ZenGin::zCVobScreenFX::Archive(zCArchiver* archiver)
{
	if (!zCEffect::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCVobScreenFX::Unarchive(zCArchiver* archiver)
{
	if (!zCEffect::Unarchive(archiver))
		return false;

	ZCR_START(zCVobScreenFX);

	if (archiver->IsSavegame())
	{
		// todo
		//ZCR_READRAW(blend);
		//ZCR_READRAW(cinema);
		//ZCR_READRAW(fovMorph);
		//ZCR_READRAW(fovSaved);
		//ZCR_READRAW(fovSaved1st);
	}

	ZCR_END();

	return true;
}

bool ZenGin::zCVobAnimate::Archive(zCArchiver* archiver)
{
	if (!zCEffect::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCVobAnimate::Unarchive(zCArchiver* archiver)
{
	if (!zCEffect::Unarchive(archiver))
		return false;

	ZCR_START(zCVobAnimate);

	ZCR_READBOOL(startOn);

	if (archiver->IsSavegame())
	{
		ZCR_READBOOL(isRunning);
	}
	else
	{
		isRunning = startOn;
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


/*
	Waynet
*/

bool ZenGin::zCWayNet::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZenGin::zCWayNet::Unarchive(zCArchiver* archiver)
{
	ZCR_START(zCWayNet);

	ZCR_READINT(waynetVersion);

	// Waypoints
	int numWaypoints = 0;
	ZCR_READINT(numWaypoints);
	
	if (numWaypoints > 0)
	{
		waypointList.resize(numWaypoints);

		for (size_t i = 0; i < numWaypoints; i++)
		{
			std::string waypointKey = "waypoint" + std::to_string(i);

			zCWaypoint* waypoint;
			ZCR_READOBJECT(waypoint, waypointKey, zCWaypoint*);

			waypointList[i] = waypoint;
		}
	}

	// Ways
	int numWays = 0;
	ZCR_READINT(numWays);

	if (numWays > 0)
	{
		wayList.resize(numWays);

		for (size_t i = 0; i < numWays; i++)
		{
			std::string waylKey = "wayl" + std::to_string(i);

			zCWaypoint* wayl;
			ZCR_READOBJECT(wayl, waylKey, zCWaypoint*);

			std::string wayrKey = "wayr" + std::to_string(i);

			zCWaypoint* wayr;
			ZCR_READOBJECT(wayr, wayrKey, zCWaypoint*);

			zCWay way;
			way.left	= wayl;
			way.right	= wayr;
			wayList[i] = way;
		}
	}

	ZCR_END();

	return true;
}

bool ZenGin::zCWaypoint::Archive(zCArchiver* archiver)
{
	return false;
}

bool ZenGin::zCWaypoint::Unarchive(zCArchiver* archiver)
{
	ZCR_START(zCWaypoint);

	ZCR_READSTRING(wpName);
	ZCR_READINT(waterDepth);
	ZCR_READBOOL(underWater);
	ZCR_READVEC3(position);
	ZCR_READVEC3(direction);

	ZCR_END();

	return true;
}