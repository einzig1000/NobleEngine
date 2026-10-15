#pragma once
#include "Generator/TerrainGenerator.h"
#include "Loader/TerrainLoader.h"
#include "Saver/TerrainSaver.h"

class TerrainDataManager
{
public:
	TerrainDataManager();
	~TerrainDataManager();

	TerrainGenerator& GetGenerator() { return terrainGenerator_; }
	TerrainLoader& GetLoader() { return terrainLoader_; }
	TerrainSaver& GetSaver() { return terrainSaver_; }

private:
	// 地形生成
	TerrainGenerator terrainGenerator_;
	TerrainLoader terrainLoader_;
	TerrainSaver terrainSaver_;
};

