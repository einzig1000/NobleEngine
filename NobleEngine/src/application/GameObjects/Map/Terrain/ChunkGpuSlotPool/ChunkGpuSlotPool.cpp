#include "ChunkGpuSlotPool.h"
#include <GameObjects/Map/Terrain/FaceDataPagePool/FaceDataPagePool.h>
#include <definition/definition.h>

ChunkGpuSlotPool::ChunkGpuSlotPool(FaceDataPagePool* pagePool)
	: pagePool_(pagePool)
{
}

ChunkGpuSlotPool::~ChunkGpuSlotPool()
{
	for (const auto& slot : slots_)
	{
		for (int32_t i = 0; i < 2; ++i)
		{
			Game::Resource::Destroy(slot->faceCountResourceID[i]);
			Game::Resource::Destroy(slot->groupOffsetResourceID[i]);
			Game::Resource::Destroy(slot->myPagesResourceID[i]);
		}
		Game::Resource::Destroy(slot->faceWriteCounterResourceID);
		Game::Resource::Destroy(slot->blockIDsResourceID);
	}
}

void ChunkGpuSlotPool::BeginFrame()
{ 
	createdThisFrame_ = 0; 
}

ChunkGpuSlot* ChunkGpuSlotPool::TryGetFreeSlot()
{
	// 空きがあればそれを貸す
	if (!freeSlots_.empty())
	{
		ChunkGpuSlot* slot = freeSlots_.back();
		freeSlots_.pop_back();
		return slot;
	}

	// 無ければ作る
	if (createdThisFrame_ < kMaxCreatePerFrame)
	{
		slots_.push_back(CreateSlot());
		++createdThisFrame_;
		return slots_.back().get();
	}
	
	return nullptr;
}

void ChunkGpuSlotPool::Release(ChunkGpuSlot* slot)
{
	if (!slot) return;
	freeSlots_.push_back(slot);
}

void ChunkGpuSlotPool::CreateSlots(size_t freeCount)
{
	for (size_t i = 0; i < freeCount; ++i)
	{
		slots_.push_back(CreateSlot());
		freeSlots_.push_back(slots_.back().get());
	}
}

std::unique_ptr<ChunkGpuSlot> ChunkGpuSlotPool::CreateSlot()
{
	auto slot = std::make_unique<ChunkGpuSlot>();

	// リソース作成
	for (int32_t i = 0; i < 2; ++i)
	{
		slot->faceCountResourceID[i] = Game::Resource::CreateCompute(sizeof(uint32_t), Constexprs::kGroupCount);
		slot->groupOffsetResourceID[i] = Game::Resource::CreateCompute(sizeof(uint32_t), Constexprs::kGroupCount);
		slot->myPagesResourceID[i] = Game::Resource::CreateDynamic();
	}
	slot->faceWriteCounterResourceID = Game::Resource::CreateCompute(sizeof(uint32_t), 1);
	slot->blockIDsResourceID = Game::Resource::CreateDynamic();

	// 描画オブジェクト
	slot->render = std::make_unique<RenderObject>();
	slot->render->psoConfig_.ms = "assets/shaders/Block/4.3.0/Block.MS.hlsl";
	slot->render->psoConfig_.ps = "assets/shaders/Block/2.0.0/Block.PS.hlsl";
	slot->render->psoConfig_.as = "assets/shaders/Block/4.3.0/Block.AS.hlsl";
	slot->render->psoConfig_.rasterizerID = RasterizerID::Solid_FrontCull;
	slot->render->SetupFromShaders();
	constexpr uint32_t kASGroupSize = 32;
	slot->render->instanceNum_ = (Constexprs::kGroupCount + kASGroupSize - 1) / kASGroupSize;

	// 露出面カウント用の計算オブジェクト
	slot->countCompute = std::make_unique<ComputeObject>();
	slot->countCompute->psoConfig_.cs = "assets/shaders/Block/4.4.0/Block.CreateMesh.Count.CS.hlsl";
	slot->countCompute->SetupFromShaders();
	slot->countCompute->size = { static_cast<int32_t>(Constexprs::kGroupCount), 1, 1 };
	slot->countCompute->RegisterOutput(slot->faceCountResourceID[0]);
	slot->countCompute->RegisterOutput(slot->faceCountResourceID[1]);
	slot->countCompute->RegisterOutput(slot->faceWriteCounterResourceID);
	slot->countCompute->RegisterOutput(slot->groupOffsetResourceID[0]);
	slot->countCompute->RegisterOutput(slot->groupOffsetResourceID[1]);

	// 露出面書き込み用の計算オブジェクト
	slot->writeCompute = std::make_unique<ComputeObject>();
	slot->writeCompute->psoConfig_.cs = "assets/shaders/Block/4.4.0/Block.CreateMesh.Write.CS.hlsl";
	slot->writeCompute->SetupFromShaders();
	slot->writeCompute->size = { static_cast<int32_t>(Constexprs::kGroupCount), 1, 1 };
	slot->writeCompute->RegisterOutput(pagePool_->GetSharedResourceID());

	return slot;
}
