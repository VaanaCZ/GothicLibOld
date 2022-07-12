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

void GothicLib::ZenGin::zCWorld::LoadWorld(FileStream* file)
{
}

void GothicLib::ZenGin::zCWorld::SaveWorld(FileStream* file)
{
}

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD

void ZenGin::zCWorld::LoadWorldFile(FileStream* file)
{
	std::string line;

	while (file->ReadLine(line))
	{
		// Check if we entered the right chunk
		if (line.find("VobHierarchy") != std::string::npos)
		{
			vobTree = new zCVob();
			LoadPWFVobTree(vobTree, file);
		}
	}

}

void ZenGin::zCWorld::SaveWorldFile(FileStream* file)
{
}

void ZenGin::zCWorld::LoadPWFVobTree(zCVob* parentVob, FileStream* file)
{	
	bool inVob = false;
	zCVob* vob = nullptr;

	std::string line;

	while (file->ReadLine(line))
	{
		std::string key;
		std::string value;

		bool chunkStart = ParsePWFLine(line, key, value);

		if (chunkStart && key == "zCVob")
		{
			int vobId = std::stoi(value);
			zTVobType vobType = (zTVobType)std::stoi(value.substr(value.find(" ") + 1));

			switch (vobType)
			{
			case zVOB_TYPE_NORMAL:			vob = new zCVob();				break;
			case zVOB_TYPE_LIGHT:			vob = new zCVobLight();			break;
			case zVOB_TYPE_SOUND:			vob = new zCVobSound();			break;
			case zVOB_TYPE_LEVEL_COMPONENT:	vob = new zCVobLevelCompo();	break;
			case zVOB_TYPE_SPOT:			vob = new zCVobSpot();			break;
			case zVOB_TYPE_CAMERA:			vob = new zCVob();				break;
			case zVOB_TYPE_STARTPOINT:		vob = new zCVob();				break;
			case zVOB_TYPE_WAYPOINT:		vob = new zCVob();				break;
			case zVOB_TYPE_MARKER:			vob = new zCVob();				break;
			case zVOB_TYPE_MOB:				vob = new oCMOB();				break;
			case zVOB_TYPE_ITEM:			vob = new oCItem();				break;
			case zVOB_TYPE_NSC:				vob = new oCNpc();				break;
			default:
				break;
			}

			inVob = true;

			parentVob->children.push_back(vob);
		}
		else if (inVob)
		{
			if (line == "{")
			{
				vob->Load(file);
			}
			else if (line == "}")
			{
				inVob = false;
				return;
			}
			else if (ParsePWFLine(line, key, value))
			{
				if (key == "zCDecal")
				{
					vob->visualPtr = new zCDecal();
					((zCDecal*)vob->visualPtr)->name = vob->visual;
					vob->visualPtr->Load(file);
				}
				else
				{
					// error
				}
			}
		}
		else if (chunkStart && key == "childs")
		{
			int childCount = std::stoi(value);

			for (size_t i = 0; i < childCount; i++)
			{
				LoadPWFVobTree(parentVob, file);
			}
		}
	}
}

void ZenGin::zCWorld::SavePWFVobTree(zCVob* parentVob, FileStream* file)
{
}

void ZenGin::zCVob::Save(FileStream* file)
{
}

void ZenGin::zCVob::Load(FileStream* file)
{
	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return;
			}
			else if (ParsePWFLine(line, key, value))
			{
				if (key == "vobType")
				{
					vobType = (zTVobType)std::stoi(value);
				}
				else if (key == "visual")
				{
					visual = value;
				}
				else if (key == "vobName")
				{
					vobName = value;
				}
				else if (key == "visualCamAlign")
				{
					if (value == "NONE")		visualCamAlign = zVISUAL_CAMALIGN_NONE;
					else if (value == "YAW")	visualCamAlign = zVISUAL_CAMALIGN_YAW;
					else if (value == "FULL")	visualCamAlign = zVISUAL_CAMALIGN_FULL;
				}
				else if (key == "showVisual")
				{
					showVisual = (value == "1");
				}
				else if (key == "drawBBox3D")
				{
					drawBBox3D = (value == "1");
				}
				else if (key == "cdStatic")
				{
					cdStatic = (value == "1");
				}
				else if (key == "cdDyn")
				{
					cdDyn = (value == "1");
				}
				else if (key == "staticVob")
				{
					staticVob = (value == "1");
				}
				else if (key == "bbox3DWSpace")
				{
					if (sscanf_s(value.c_str(), "%f %f %f %f %f %f",
						&bbox3DWS.min.x, &bbox3DWS.min.y, &bbox3DWS.min.z,
						&bbox3DWS.max.x, &bbox3DWS.max.y, &bbox3DWS.max.z) != 6)
					{
						return;
					}
				}
				else if (key == "trafoRot")
				{
					if (sscanf_s(value.c_str(), "%f %f %f %f %f %f %f %f %f",
						&trafoRot.x.x, &trafoRot.x.y, &trafoRot.x.z, 
						&trafoRot.y.x, &trafoRot.y.y, &trafoRot.y.z,
						&trafoRot.z.x, &trafoRot.z.y, &trafoRot.z.z) != 9)
					{
						return;
					}
				}
				else if (key == "trafoPos")
				{
					if (sscanf_s(value.c_str(), "%f %f %f",
						&trafoPos.x, &trafoPos.y, &trafoPos.z) != 3)
					{
						return;
					}
				}
				else if (key == "trafoOSToWSRot")
				{
					if (sscanf_s(value.c_str(), "%f %f %f %f %f %f %f %f %f",
						&trafoOSToWSRot.x.x, &trafoOSToWSRot.x.y, &trafoOSToWSRot.x.z, 
						&trafoOSToWSRot.y.x, &trafoOSToWSRot.y.y, &trafoOSToWSRot.y.z,
						&trafoOSToWSRot.z.x, &trafoOSToWSRot.z.y, &trafoOSToWSRot.z.z) != 9)
					{
						return;
					}
				}
				else if (key == "trafoOSToWSPos")
				{
					if (sscanf_s(value.c_str(), "%f %f %f",
						&trafoOSToWSPos.x, &trafoOSToWSPos.y, &trafoOSToWSPos.z) != 3)
					{
						return;
					}
				}
			}
		}
	}
}

void ZenGin::zCVobLight::Save(FileStream* file)
{
}

void ZenGin::zCVobLight::Load(FileStream* file)
{
	zCVob::Load(file);

	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return;
			}
			else if (ParsePWFLine(line, key, value))
			{
				if (key == "range")
				{
					range = std::stof(value);
				}
				else if (key == "colorRGBA")
				{
					float r, g, b, a;

					if (sscanf_s(value.c_str(), "%f %f %f %f", &r, &g, &b, &a) != 4)
					{
						return;
					}

					color.r = r;
					color.g = g;
					color.b = b;
					color.a = a;
				}
				else if (key == "lightStatic")
				{
					lightStatic = (value == "1");
				}
				else if (key == "lightQual")
				{
					lightQuality = (zTVobLightQuality)std::stoi(value);
				}
				else if (key == "lensflareFX")
				{
					lensflareFX = value; // todo: fix
				}
				else if (key == "rangeAniScale")
				{
					rangeAniScale = value;
				}
				else if (key == "rangeAniFPS")
				{
					rangeAniFPS = std::stof(value);
				}
				else if (key == "rangeAniSmooth")
				{
					rangeAniSmooth = (value == "1");
				}
				else if (key == "colorAniList")
				{
					colorAniList = value; // todo: parse array
				}
				else if (key == "colorAniFPS")
				{
					colorAniFPS = std::stof(value);
				}
				else if (key == "colorAniSmooth")
				{
					colorAniSmooth = (value == "1");
				}
			}
		}
	}
}

void ZenGin::oCNpc::Save(FileStream* file)
{
}

void ZenGin::oCNpc::Load(FileStream* file)
{
	zCVob::Load(file);

	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return;
			}
			else if (ParsePWFLine(line, key, value))
			{
				if (key == "INSTANCE")
				{
					instance = value;
				}
			}
		}
	}
}

void ZenGin::oCMOB::Save(FileStream* file)
{
}

void ZenGin::oCMOB::Load(FileStream* file)
{
	zCVob::Load(file);

	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return;
			}
			else if (ParsePWFLine(line, key, value))
			{
				if (key == "INSTANCE")
				{
					instance = value;
				}
			}
		}
	}
}

void ZenGin::oCItem::Save(FileStream* file)
{
}

void ZenGin::oCItem::Load(FileStream* file)
{
	zCVob::Load(file);

	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return;
			}
			else if (ParsePWFLine(line, key, value))
			{
				if (key == "INSTANCE")
				{
					itemInstance = value;
				}
			}
		}
	}
}

void ZenGin::zCDecal::Save(FileStream* file)
{
}

void ZenGin::zCDecal::Load(FileStream* file)
{
	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return;
			}
			else if (ParsePWFLine(line, key, value))
			{
				if (key == "decalDim")
				{
					if (sscanf_s(value.c_str(), "%f %f",
						&decalDim.x, &decalDim.y) != 2)
					{
						return;
					}
				}
				else if (key == "decalOffset")
				{
					if (sscanf_s(value.c_str(), "%f %f",
						&decalOffset.x, &decalOffset.y) != 2)
					{
						return;
					}
				}
				else if (key == "decal2Sided")
				{
					decal2Sided = (value == "1");
				}
				else if (key == "decalAlphaFunc")
				{
					if (value == "MAT_DEFAULT")		decalAlphaFunc = zRND_ALPHA_FUNC_MAT_DEFAULT;
					else if (value == "NONE")		decalAlphaFunc = zRND_ALPHA_FUNC_NONE;
					else if (value == "BLEND")		decalAlphaFunc = zRND_ALPHA_FUNC_BLEND;
					else if (value == "ADD")		decalAlphaFunc = zRND_ALPHA_FUNC_ADD;
					else if (value == "SUB")		decalAlphaFunc = zRND_ALPHA_FUNC_SUB;
					else if (value == "MUL")		decalAlphaFunc = zRND_ALPHA_FUNC_MUL;
				}
				else if (key == "decalTexAniFPS")
				{
					decalTexAniFPS = std::stof(value);
				}
			}
		}
	}
}

#endif

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