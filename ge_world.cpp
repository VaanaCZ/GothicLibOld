#include "ge_world.h"
#include <cassert>

using namespace GothicLib::Genome;

/*
	gCProject
*/

bool gCProject::OnWrite(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnWrite(file);
	}

	return WriteData(file);
}

bool gCProject::OnRead(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnRead(file);
	}

	return ReadData(file);
}

bool gCProject::DoLoadData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return ReadData(file);
	}

	return true;
}

bool gCProject::DoSaveData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return WriteData(file);
	}

	return true;
}

bool gCProject::WriteData(FileStream* file)
{
	uint16_t version	= 1;
	uint32_t count		= worlds.size();

	if (game >= GAME_RISEN1)
	{
		version = 200;
	}
	
	file->Write(FILE_ARGS(version));
	file->Write(FILE_ARGS(count));
	
	for (size_t i = 0; i < count; i++)
	{
		file->WriteString(worlds[i]);
	}

	return true;
}

bool gCProject::ReadData(FileStream* file)
{
	uint16_t version;
	uint32_t count = 0;

	file->Read(FILE_ARGS(version));
	file->Read(FILE_ARGS(count));

	if (count > 0)
	{
		worlds.resize(count);

		for (size_t i = 0; i < count; i++)
		{
			file->ReadString(worlds[i]);
		}
	}

	return true;
}

/*
	gCWorld
*/

bool gCWorld::OnWrite(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnWrite(file);
	}

	return WriteData(file);
}

bool gCWorld::OnRead(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnRead(file);
	}

	return ReadData(file);
}

bool gCWorld::DoLoadData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return ReadData(file);
	}

	return true;
}

bool gCWorld::DoSaveData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return WriteData(file);
	}

	return true;
}

bool gCWorld::WriteData(FileStream* file)
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

bool gCWorld::ReadData(FileStream* file)
{
	uint16_t version;

	file->Read(FILE_ARGS(version));
	file->ReadString(sectorFile);

	return true;
}

/*
	gCSector
*/

bool gCSector::OnWrite(FileStream* file)
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
		return WriteData(file);
	}

	return true;
}

bool gCSector::OnRead(FileStream* file)
{
	uint16_t version;

	file->Read(FILE_ARGS(version));

	if (version >= 27)
		file->Read(FILE_ARGS(enabled));

	if (game >= GAME_RISEN1 &&
		version >= 200)
	{
		return ReadData(file);
	}

	return true;
}

bool gCSector::DoLoadData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		uint16_t version;

		file->Read(FILE_ARGS(version));

		return ReadData(file);
	}

	return true;
}

bool gCSector::DoSaveData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		uint16_t version = 27;

		file->Write(FILE_ARGS(version));

		return WriteData(file);
	}

	return true;
}

bool gCSector::WriteData(FileStream* file)
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

bool gCSector::ReadData(FileStream* file)
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

bool gCGeometryLayer::OnWrite(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	return bCObjectBase::OnWrite(file);
}

bool gCGeometryLayer::OnRead(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	return bCObjectBase::OnRead(file);
}

bool gCGeometryLayer::DoLoadData(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	// Load context
	bCAccessorPropertyObject accessor;
	accessor.game = game;
	accessor.Read(file);
	context = accessor.GetNativeObjectAs<eCGeometrySpatialContext*>();

	return true;
}

bool gCGeometryLayer::DoSaveData(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	// Save context
	bCAccessorPropertyObject accessor(context);
	accessor.game = game;
	accessor.Write(file);

	return true;
}

bool gCDynamicLayer::OnWrite(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		return WriteData(file);
	}

	return bCObjectBase::OnWrite(file);
}

bool gCDynamicLayer::OnRead(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		return ReadData(file);
	}

	return bCObjectBase::OnRead(file);
}

bool gCDynamicLayer::DoLoadData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return ReadData(file);
	}

	return true;
}

bool gCDynamicLayer::DoSaveData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return WriteData(file);
	}

	return true;
}

bool gCDynamicLayer::WriteData(FileStream* file)
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
	accessor.game = game;
	accessor.Write(file);

	return true;
}

bool gCDynamicLayer::ReadData(FileStream* file)
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
	accessor.game = game;
	accessor.Read(file);
	context = accessor.GetNativeObjectAs<eCEntityDynamicContext*>();

	return true;
}

/*
	eCContextBase
		eCGeometrySpatialContext
		eCEntityDynamicContext
*/

bool eCGeometrySpatialContext::OnWrite(FileStream* file)
{
	return true;
}

bool eCGeometrySpatialContext::OnRead(FileStream* file)
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

bool eCGeometrySpatialContext::WriteNodes(FileStream* file)
{
	return true;
}

bool eCGeometrySpatialContext::ReadNodes(FileStream* file)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	uint32_t entityCount = 0;
	file->Read(FILE_ARGS(entityCount));

	if (entityCount > 0)
	{
		for (size_t i = 0; i < entityCount; i++)
		{

		}
	}	

	return true;
}

bool eCEntityDynamicContext::OnWrite(FileStream* file)
{
	return true;
}

bool eCEntityDynamicContext::OnRead(FileStream* file)
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
		for (size_t i = 0; i < entityCount; i++)
		{
			eCDynamicEntity entity;
			entity.game = this->game;
			entity.OnRead(file);
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

bool eCEntityProxy::Write(FileStream* file)
{
	return false;
}

bool eCEntityProxy::Read(FileStream* file)
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

bool eCNode::OnWrite(FileStream* file)
{
	return true;
}

bool eCNode::OnRead(FileStream* file)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	if (game <= GAME_GOTHIC3)
	{
		file->Read(FILE_ARGS(id));
	}

	return true;
}

bool eCEntity::OnWrite(FileStream* file)
{
	return true;
}

bool eCEntity::OnRead(FileStream* file)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	eCNode::OnRead(file);

	bool unknown;

	file->Read(FILE_ARGS(enabled));
	file->Read(FILE_ARGS(renderingEnabled));
	file->Read(FILE_ARGS(processingDisabled)); // Always zero in Risen 1

	if (version > 210)
	{
		// highlighted, frozen, burned?

		file->Read(FILE_ARGS(unknown));
		assert(unknown == 0);
		file->Read(FILE_ARGS(unknown));
		assert(unknown == 0);
		file->Read(FILE_ARGS(unknown));
		assert(unknown == 0);
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
		uint16_t propertSetVersion;
		file->Read(FILE_ARGS(propertSetVersion));
		
		bCAccessorPropertyObject accessor;
		accessor.game = game;
		if (!accessor.Read(file))
		{
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

	return true;
}

bool eCGeometryEntity::OnWrite(FileStream* file)
{
	return true;
}

bool eCGeometryEntity::OnRead(FileStream* file)
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

	return eCEntity::OnRead(file);
}

bool eCSpatialEntity::OnWrite(FileStream* file)
{
	return true;
}

bool eCSpatialEntity::OnRead(FileStream* file)
{
	return true;
}

bool eCDynamicEntity::OnWrite(FileStream* file)
{
	return true;
}

bool eCDynamicEntity::OnRead(FileStream* file)
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
		creator.Read(file);
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

	return eCGeometryEntity::OnRead(file);
}
