#pragma once
#include <GameObjects/Effect/Particle/IParticle.h>
#include <System/EventBus/EventBus.h>
#include <Game.h>
#include <vector>

class GoTargetCurving : public IParticle
{
public:
	struct Params
	{
		Vector3 start;						// 開始位置
		const Vector3* target = nullptr;	// 目標位置(nullptrならtargetOffsetを目標座標として使う)
		Vector3 targetOffset;				// 目標位置のオフセット
		float speed = 8.0f;					// 速度
		float height = 1.0f;				// 弧の高さ
		float spread = 0.5f;				// 弧の横ずれの最大
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float scale = 0.2f;					// 全軸scale(板ポリが-1～1なので半径)
		int32_t textureID = -1;				// -1なら既定のテクスチャ
		Event arriveEventData = {};			// 着いたら投げる(typeがMAXなら投げない)
	};

private:
	struct Particle
	{
		Params params;
		Vector3 controlOffset;
		Vector3 position;
		float progress = 0.0f;	// 0→1で到着
	};

	// GPUに送る1粒分(GoTargetCurving.VS.hlslのInstanceと同じ並び)
	struct Instance
	{
		Vector3 position;
		float scale = 1.0f;
		Vector4 color;
		int32_t textureID = -1;
	};

	struct PerView
	{
		Matrix4x4 viewProjection;
		Matrix4x4 billboard;
	};

public:
	GoTargetCurving();
	~GoTargetCurving();

	void SetEventBus(EventBus* eventBus) { eventBus_ = eventBus; }

	void Emit(const Params& params);
	void Update(int32_t c_ID) override;
	void Draw(int32_t rt_ID) override;

private:
	// 目標の今の位置
	Vector3 GetTargetPosition(const Params& params) const;

	// 目標が逃げ続けても、この秒数で必ず届く
	static constexpr float kMaxFlightSeconds = 3.0f;

	EventBus* eventBus_ = nullptr;

	std::vector<Particle> particles_;
	std::vector<Instance> instances_;
	PerView perView_;

	RenderObject render_;
	int32_t instanceBufferID_ = -1;
	int32_t t_default_ = -1;
};