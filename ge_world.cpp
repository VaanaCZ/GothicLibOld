//
// ge_world.cpp
//
// Copyright (c) 2021-2023 Václav Maroušek
//

#include "ge_world.h"
#include <cassert>

using namespace GothicLib::Genome;

/*
	gCProject
*/

bool gCProject::OnWrite(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnWrite(file, game);
	}

	return WriteData(file, game);
}

bool gCProject::OnRead(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnRead(file, game);
	}

	return ReadData(file, game);
}

bool gCProject::DoLoadData(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return ReadData(file, game);
	}

	return true;
}

bool gCProject::DoSaveData(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return WriteData(file, game);
	}

	return true;
}

bool gCProject::WriteData(FileStream* file, GAME game)
{
	uint16_t version	= 1;
	uint32_t count		= Worlds.size();

	if (game >= GAME_RISEN1)
	{
		version = 200;
	}
	
	file->Write(FILE_ARGS(version));
	file->Write(FILE_ARGS(count));
	
	for (size_t i = 0; i < count; i++)
	{
		file->WriteString(Worlds[i]->name);
	}

	return true;
}

bool gCProject::ReadData(FileStream* file, GAME game)
{
	uint16_t version;
	uint32_t count = 0;

	file->Read(FILE_ARGS(version));
	file->Read(FILE_ARGS(count));

	if (count > 0)
	{
		Worlds.resize(count);

		for (size_t i = 0; i < count; i++)
		{
			Worlds[i] = new gCWorld();
			file->ReadString(Worlds[i]->name);
		}
	}

	return true;
}

/*
	gCWorld
*/

bool gCWorld::OnWrite(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnWrite(file, game);
	}

	return WriteData(file, game);
}

bool gCWorld::OnRead(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnRead(file, game);
	}

	return ReadData(file, game);
}

bool gCWorld::DoLoadData(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return ReadData(file, game);
	}

	return true;
}

bool gCWorld::DoSaveData(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return WriteData(file, game);
	}

	return true;
}

bool gCWorld::WriteData(FileStream* file, GAME game)
{
	uint16_t version = 36;

	if (game >= GAME_RISEN1)
	{
		version = 200;
	}

	file->Write(FILE_ARGS(version));
	file->WriteString(sectorFile);

	return true;
}

bool gCWorld::ReadData(FileStream* file, GAME game)
{
	uint16_t version;

	file->Read(FILE_ARGS(version));
	file->ReadString(sectorFile);

	return true;
}

/*
	gCSector
*/

bool gCSector::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 27;

	if (game >= GAME_RISEN1)
	{
		version = 200;
	}

	file->Write(FILE_ARGS(version));

	if (version >= 27)
		file->Write(FILE_ARGS(enabled));

	if (game >= GAME_RISEN1)
	{
		return WriteData(file, game);
	}

	return true;
}

bool gCSector::OnRead(FileStream* file, GAME game)
{
	uint16_t version;

	file->Read(FILE_ARGS(version));

	if (version >= 27)
		file->Read(FILE_ARGS(enabled));

	if (game >= GAME_RISEN1 &&
		version >= 200)
	{
		return ReadData(file, game);
	}

	return true;
}

bool gCSector::DoLoadData(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		uint16_t version;

		file->Read(FILE_ARGS(version));

		return ReadData(file, game);
	}

	return true;
}

bool gCSector::DoSaveData(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		uint16_t version = 27;

		file->Write(FILE_ARGS(version));

		return WriteData(file, game);
	}

	return true;
}

bool gCSector::WriteData(FileStream* file, GAME game)
{
	uint32_t dynamicLayerCount	= dynamicLayers.size();
	uint32_t geometryLayerCount	= geometryLayers.size();

	file->Write(FILE_ARGS(dynamicLayerCount));
	file->Write(FILE_ARGS(geometryLayerCount));

	for (size_t i = 0; i < dynamicLayerCount; i++)
	{
		file->WriteString(dynamicLayers[i]);
	}

	if (geometryLayerCount > 0 &&
		game >= GAME_RISEN1)
	{
		LOG_WARN("Geometry layers are not supported in Risen, yet they are specified!");
		return false;
	}

	for (size_t i = 0; i < geometryLayerCount; i++)
	{
		file->WriteString(geometryLayers[i]);
	}

	return true;
}

bool gCSector::ReadData(FileStream* file, GAME game)
{
	uint32_t dynamicLayerCount = 0;
	uint32_t geometryLayerCount = 0;

	file->Read(FILE_ARGS(dynamicLayerCount));
	file->Read(FILE_ARGS(geometryLayerCount));

	if (dynamicLayerCount > 0)
	{
		dynamicLayers.resize(dynamicLayerCount);

		for (size_t i = 0; i < dynamicLayerCount; i++)
		{
			file->ReadString(dynamicLayers[i]);
		}
	}

	if (geometryLayerCount > 0)
	{
		if (game >= GAME_RISEN1)
		{
			LOG_WARN("Geometry layers are not supported in Risen, yet they are specified!");
			return false;
		}

		geometryLayers.resize(geometryLayerCount);

		for (size_t i = 0; i < geometryLayerCount; i++)
		{
			file->ReadString(geometryLayers[i]);
		}
	}

	return true;
}

/*
	gCLayerBase
		gCGeometryLayer
		gCDynamicLayer
*/

bool gCGeometryLayer::OnWrite(FileStream* file, GAME game)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	return bCObjectBase::OnWrite(file, game);
}

bool gCGeometryLayer::OnRead(FileStream* file, GAME game)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	return bCObjectBase::OnRead(file, game);
}

bool gCGeometryLayer::DoLoadData(FileStream* file, GAME game)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	// Load context
	bCAccessorPropertyObject accessor;
	accessor.Read(file, game);
	context = accessor.GetNativeObjectAs<eCGeometrySpatialContext*>();

	return true;
}

bool gCGeometryLayer::DoSaveData(FileStream* file, GAME game)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	// Save context
	bCAccessorPropertyObject accessor(context);
	accessor.Write(file, game);

	return true;
}

bool gCDynamicLayer::OnWrite(FileStream* file, GAME game)
{
	if (game >= GAME_RISEN1)
	{
		return WriteData(file, game);
	}

	return bCObjectBase::OnWrite(file, game);
}

bool gCDynamicLayer::OnRead(FileStream* file, GAME game)
{
	if (game >= GAME_RISEN1)
	{
		return ReadData(file, game);
	}

	return bCObjectBase::OnRead(file, game);
}

bool gCDynamicLayer::DoLoadData(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return ReadData(file, game);
	}

	return true;
}

bool gCDynamicLayer::DoSaveData(FileStream* file, GAME game)
{
	if (game <= GAME_GOTHIC3)
	{
		return WriteData(file, game);
	}

	return true;
}

bool gCDynamicLayer::WriteData(FileStream* file, GAME game)
{
	uint64_t magic = 0x4C44454D4F4E4547;
	file->Write(FILE_ARGS(magic));

	uint16_t version = 83;

	if (game >= GAME_RISEN1)
	{
		version = 200;
	}

	file->Read(FILE_ARGS(version));

	// Save context
	bCAccessorPropertyObject accessor(context);
	accessor.Write(file, game);

	return true;
}

bool gCDynamicLayer::ReadData(FileStream* file, GAME game)
{
	uint64_t magic;
	file->Read(FILE_ARGS(magic));

	if (magic != 0x4C44454D4F4E4547)
	{
		LOG_ERROR("Failed to read gCDynamicLayer, invalid magic bytes!");
		return false;
	}

	uint16_t version;
	file->Read(FILE_ARGS(version));

	// Load context
	bCAccessorPropertyObject accessor;
	accessor.Read(file, game);
	context = accessor.GetNativeObjectAs<eCEntityDynamicContext*>();

	return true;
}

/*
	eCContextBase
		eCGeometrySpatialContext
		eCEntityDynamicContext
*/

bool eCGeometrySpatialContext::OnWrite(FileStream* file, GAME game)
{
	return true;
}

bool eCGeometrySpatialContext::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	if (version > 1)
	{
		file->Read(FILE_ARGS(enabled));
	}

	if (version >= 67)
	{
		file->Read(FILE_ARGS(hybridContext));
	}

	file->Read(FILE_ARGS(ContextBox));

	return true;
}

bool eCGeometrySpatialContext::WriteNodes(FileStream* file, GAME game)
{
	return true;
}

bool eCGeometrySpatialContext::ReadNodes(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	uint32_t entityCount = 0;
	file->Read(FILE_ARGS(entityCount));

	if (entityCount > 0)
	{
		std::vector<eCSpatialEntity*> entityList;
		entityList.resize(entityCount);

		for (size_t i = 0; i < entityCount; i++)
		{
			bool hasCreator;
			bool disablePatchWithTemplate;

			file->Read(FILE_ARGS(hasCreator));
			file->Read(FILE_ARGS(disablePatchWithTemplate));

			if (hasCreator && !disablePatchWithTemplate)
			{
				bCGuid templateId;
				file->Read(FILE_ARGS(templateId));
			}

			entityList[i] = new eCSpatialEntity();
			if (!entityList[i]->OnRead(file, game))
			{
				return false;
			}
		}
	}	

	return true;
}

bool eCEntityDynamicContext::OnWrite(FileStream* file, GAME game)
{
	return true;
}

bool eCEntityDynamicContext::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	if (version > 1)
	{
		file->Read(FILE_ARGS(enabled));
	}

	if (version >= 39 && version <= 172)
	{
		file->Read(FILE_ARGS(visualLoDFactor));
		file->Read(FILE_ARGS(objectCullFactor));
	}

	if (version >= 40)
	{
		file->Read(FILE_ARGS(ContextBox));
	}

	uint32_t entityCount = 0;
	file->Read(FILE_ARGS(entityCount));

	if (entityCount > 0)
	{
		std::vector<eCDynamicEntity*> entityList;
		entityList.resize(entityCount);

		for (size_t i = 0; i < entityCount; i++)
		{
			entityList[i] = new eCDynamicEntity();
			if (!entityList[i]->OnRead(file, game))
			{
				return false;
			}
		}
	}

	return true;
}

/*
	eCEntityProxy
	eCNode
		eCEntity
			eCGeometryEntity
				eCSpatialEntity
				eCDynamicEntity
*/

bool eCEntityProxy::Write(FileStream* file, GAME game)
{
	return false;
}

bool eCEntityProxy::Read(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	bool valid;
	file->Read(FILE_ARGS(valid));

	if (valid)
	{
		file->Read(FILE_ARGS(id));
	}

	return true;
}

bool eCNode::OnWrite(FileStream* file, GAME game)
{
	return true;
}

bool eCNode::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	if (game <= GAME_GOTHIC3)
	{
		file->Read(FILE_ARGS(id));
	}

	return true;
}

bool eCEntity::OnWrite(FileStream* file, GAME game)
{
	return true;
}

bool eCEntity::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	eCNode::OnRead(file, game);

	bool unknown;

	file->Read(FILE_ARGS(enabled));
	file->Read(FILE_ARGS(renderingEnabled));
	file->Read(FILE_ARGS(processingDisabled)); // Always zero in Risen 1

	if (version > 210)
	{
		// highlighted, frozen, burned?

		file->Read(FILE_ARGS(unknown));
		//assert(unknown == 1);
		file->Read(FILE_ARGS(unknown));
		//assert(unknown == 1);
		file->Read(FILE_ARGS(unknown));
		//assert(unknown == 1);
	}

	if (version < 214)
	{
		file->Read(FILE_ARGS(deactivationEnabled));
	}

	file->Read(FILE_ARGS(flags));

	if (version < 63)
	{
		bool legacyBool;
		file->Read(FILE_ARGS(legacyBool));
	}

	file->Read(FILE_ARGS(pickingEnabled));
	file->Read(FILE_ARGS(collisionEnabled));

	if (game <= GAME_GOTHIC3)
	{
		file->Read(FILE_ARGS(renderAlphaValue));
	}

	file->Read(FILE_ARGS(insertType));

	if (game <= GAME_GOTHIC3)
	{
		file->Read(FILE_ARGS(lastRenderPriority));
	}

	file->Read(FILE_ARGS(locked));

	if (version >= 58 && version < 213)
	{
		file->Read(FILE_ARGS(specialDepthTexPassEnabled));
	}

	if (version >= 40)
	{
		file->Read(FILE_ARGS(ignoreProcessingRange));
	}

	if (game <= GAME_GOTHIC3)
	{
		file->ReadString(name);

		// These values are duplicit, which is
		// why this was simplified in Risen 1

		file->Read(FILE_ARGS(worldMatrix));
		file->Read(FILE_ARGS(worldMatrix));			// localMatrix
		file->Read(FILE_ARGS(worldNodeBoundary));	// worldTreeBoundary
		file->Read(FILE_ARGS(worldNodeBoundary));	// localNodeBoundary
		file->Read(FILE_ARGS(worldNodeBoundary));

		if (version >= 41)
		{
			file->Read(FILE_ARGS(worldNodeSphere));		// worldTreeSphere
			file->Read(FILE_ARGS(worldNodeSphere));
		}
		
		if (version >= 34)
		{
			file->Read(FILE_ARGS(visualLodFactor));
		}
	}

	if (version >= 38 && version < 213)
	{
		bool legacyBool;
		file->Read(FILE_ARGS(legacyBool));
	}

	if (game <= GAME_GOTHIC3 && version >= 39)
	{
		file->Read(FILE_ARGS(objectCullFactor));
	}

	if (game <= GAME_GOTHIC3 && version >= 56)
	{
		uint32_t oldTimestamp;
		file->Read(FILE_ARGS(oldTimestamp));
		// todo: convert?
	}
	else
	{
		file->Read(FILE_ARGS(dataChangedTimestamp));
	}

	if (game <= GAME_GOTHIC3 && version >= 61)
	{
		file->Read(FILE_ARGS(uniformScaling));
	}

	if (version < 213)
	{
		file->Read(FILE_ARGS(rangedObjectCulling));
		file->Read(FILE_ARGS(processingRangeOutFadingEnabled));
	}

	if (version > 211)
	{
		file->Read(FILE_ARGS(forceOutdoor));
	}

	if (version >= 216)
	{
		file->Read(FILE_ARGS(saveGameRelevant));
	}

	// Property set
	uint32_t propertySetCount = 0;
	file->Read(FILE_ARGS(propertySetCount));

	if (propertySetCount > 0)
	{
		propertySets.resize(propertySetCount);

		for (size_t i = 0; i < propertySetCount; i++)
		{
			uint16_t propertSetVersion;
			file->Read(FILE_ARGS(propertSetVersion));
		
			bCAccessorPropertyObject accessor;
			if (!accessor.Read(file, game))
			{
				return false;
			}

			propertySets[i] = accessor.GetNativeObjectAs<eCEntityPropertySet*>();
			if (propertySets[i] == nullptr)
			{
				LOG_ERROR("Invalid property set contained in entity!");
				return false;
			}

			uint32_t magic;
			file->Read(FILE_ARGS(magic));

			if (magic != 0xDEADC0DE)
			{
				LOG_ERROR("Invalid magic bytes after property set");
				return false;
			}
		}
	}

	return true;
}

bool eCGeometryEntity::OnWrite(FileStream* file, GAME game)
{
	return true;
}

bool eCGeometryEntity::OnRead(FileStream* file, GAME game)
{
	if (game >= GAME_RISEN1) // Not implemented in Gothic 3
	{
		uint16_t version;
		file->Read(FILE_ARGS(version));

		if (version < 214)
		{
			float unknown;
			file->Read(FILE_ARGS(unknown));
		}

		file->Read(FILE_ARGS(worldMatrix));
		file->Read(FILE_ARGS(worldNodeBoundary));
		file->Read(FILE_ARGS(worldNodeSphere));

		if (version >= 213)
		{
			file->Read(FILE_ARGS(renderAlphaValue));
			file->Read(FILE_ARGS(viewRange));
		}

		file->Read(FILE_ARGS(cacheInRange));
	}

	return eCEntity::OnRead(file, game);
}

bool eCSpatialEntity::OnWrite(FileStream* file, GAME game)
{
	return true;
}

bool eCSpatialEntity::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	if (version == 30)
	{
		return eCGeometryEntity::OnRead(file, game);
	}

	bool hasId;
	file->Read(FILE_ARGS(hasId));

	if (hasId)
	{
		file->Read(FILE_ARGS(id));
	}

	if (version > 34)
	{
		file->Read(FILE_ARGS(visualWorldNodeBoundary));
		file->Read(FILE_ARGS(visualWorldNodeOOBoundary));
	}

	return eCGeometryEntity::OnRead(file, game);
}

bool eCDynamicEntity::OnWrite(FileStream* file, GAME game)
{
	return true;
}

bool eCDynamicEntity::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	if (version < 211)
	{
		file->Read(FILE_ARGS(version));
	}

	if (version < 83)
	{
		bool legacyBool;
		file->Read(FILE_ARGS(legacyBool));
	}

	if (game >= GAME_RISEN1)
	{
		file->Read(FILE_ARGS(id));
		file->ReadString(name);
		file->Read(FILE_ARGS(localMatrix));
		file->Read(FILE_ARGS(worldNodeBoundary));
		file->Read(FILE_ARGS(worldNodeSphere));
		file->Read(FILE_ARGS(localNodeBoundary));
	}

	if (version >= 213)
	{
		creator.Read(file, game);
	}
	else
	{
		bool creatorValid;
		file->Read(FILE_ARGS(creatorValid));

		if (creatorValid)
		{
			file->Read(FILE_ARGS(creator.id));
		}
	}

	return eCGeometryEntity::OnRead(file, game);
}

/*
	eCEntityPropertySet
		eCCollisionShapeBase_PS
			eCCollisionShape_PS
		eCDynamicLight_PS
			eCDirectionalLight_PS
			eCHemisphere_PS
		eCVisualMeshBase_PS [GOTHIC3]
			eCVisualMeshStatic_PS [GOTHIC3]
*/


void eCScriptProxyScript::Read(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(&version, sizeof(version));

	bool hasScript = false;
	file->Read(&hasScript, sizeof(hasScript));

	if (hasScript)
	{
		file->ReadString(script);
	}
}

void eCScriptProxyScript::Write(FileStream* file, GAME game)
{
	uint16_t version = 1;
	file->Write(&version, sizeof(version));

	bool hasScript = !script.empty();
	file->Write(&hasScript, sizeof(hasScript));

	if (hasScript)
	{
		file->WriteString(script);
	}
}

bool eCEntityPropertySet::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 2;
	file->Read(FILE_ARGS(version));

	file->Write(FILE_ARGS(renderingEnabled));

	return true;
}

bool eCEntityPropertySet::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	if (version > 1)
	{
		file->Read(FILE_ARGS(renderingEnabled));
	}
	else
	{
		renderingEnabled = true;
	}

	return true;
}

bool gCClock_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 74;
	file->Write(FILE_ARGS(version));

	return true;
}

bool gCClock_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	return true;
}

bool eCCollisionShapeBase_PS::OnWrite(FileStream* file, GAME game)
{
	return false;
}

bool eCCollisionShapeBase_PS::OnRead(FileStream* file, GAME game)
{
	return false;
}

bool eCCollisionShape_PS::OnWrite(FileStream* file, GAME game)
{
	return false;
}

bool eCCollisionShape_PS::OnRead(FileStream* file, GAME game)
{
	return false;
}

bool eCDynamicLight_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 1;
	file->Write(FILE_ARGS(version));

	return true;
}

bool eCDynamicLight_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	return true;
}

bool gCDamage_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 78;
	file->Write(FILE_ARGS(version));

	return true;
}

bool gCDamage_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	return true;
}

bool eCDirectionalLight_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 1;
	file->Write(FILE_ARGS(version));

	return true;
}

bool eCDirectionalLight_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	return true;
}

bool eCHemisphere_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 1;
	file->Write(FILE_ARGS(version));

	return true;
}

bool eCHemisphere_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	return true;
}

bool gCScriptRoutine_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 1;
	file->Write(FILE_ARGS(version));

	return true;
}

bool gCScriptRoutine_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	return true;
}

bool eCSkydome_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 1;
	file->Write(FILE_ARGS(version));

	return true;
}

bool eCSkydome_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	return true;
}

bool eCSpeedTreeWind_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 1;
	file->Write(FILE_ARGS(version));

	if (!eCEntityPropertySet::OnWrite(file, game))
		return false;

	return true;
}

bool eCSpeedTreeWind_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	if (!eCEntityPropertySet::OnRead(file, game))
		return false;

	return true;
}
bool eCVisualMeshBase_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 1;
	file->Write(FILE_ARGS(version));

	return eCEntityPropertySet::OnWrite(file, game);
}

bool eCVisualMeshBase_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	return eCEntityPropertySet::OnRead(file, game);
}

bool eCVisualMeshStatic_PS::OnWrite(FileStream* file, GAME game)
{
	uint16_t version = 50;
	file->Write(FILE_ARGS(version));

	return eCVisualMeshBase_PS::OnWrite(file, game);
}

bool eCVisualMeshStatic_PS::OnRead(FileStream* file, GAME game)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	return eCVisualMeshBase_PS::OnRead(file, game);
}
