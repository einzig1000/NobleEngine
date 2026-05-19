//#include "DrawSystem/RenderData/RenderData.h"
//#include "Utilities/JsonManager.h"
//#include "Game.h"
//#include "Engine.h"
//#include "DirectX/DirectXManager.h"
//
//using namespace DirectX;
//
//std::vector<RenderData_Model*> RenderData_Model::renderModels;
//std::vector<CollisionInf*> RenderData_Model::collisionInfos;
//std::vector<RenderData_Sprite*> RenderData_Sprite::renderSprites;
//std::vector<RenderData_Triangle*> RenderData_Triangle::renderTriangles;
//std::vector<RenderData_Rect*> RenderData_Rect::renderRects;
//std::vector<RenderData_Line*> RenderData_Line::renderLines;
//std::vector<RenderData_Particle*> RenderData_Particle::renderParticles;
//std::vector<RenderData_Block*> RenderData_Block::renderBlocks;
//
//#pragma region model
//
//RenderData_Model::RenderData_Model()
//{
//	renderModels.push_back(this);
//	this->ID = int(renderModels.size());
//	this->parentModel = nullptr;
//}
//
//RenderData_Model::~RenderData_Model()
//{
//	auto it = std::find(renderModels.begin(), renderModels.end(), this);
//	if (it != renderModels.end())
//	{
//		renderModels.erase(it);
//	}
//}
//
//// 他のオブジェクトとの衝突判定
//bool RenderData_Model::isCollision(RenderData_Model* target) const
//{
//	for (const auto& aabb1 : target->aabbs)
//	{
//		for (const auto& aabb2 : this->aabbs)
//		{
//			if (IsCollision(aabb1, aabb2))
//			{
//				return true;
//			}
//		}
//	}
//
//	return false;
//}
//
//void RenderData_Model::SetBlock(RenderData_Model* target)
//{
//	// すでに登録済みなら追加しない
//	if (std::find(blockList.begin(), blockList.end(), target) == blockList.end())
//	{
//		blockList.push_back(target);
//	}
//}
//
//// ワールド行列とワールド座標の取得
//Matrix4x4 RenderData_Model::GetWorldMatrix() const
//{
//	return this->worldMatrix;
//}
//Vector3 RenderData_Model::GetWorldPosition() const
//{
//	return worldPos;
//}
//
//// 任意のポイントを向く
//void RenderData_Model::LookAtOnce(const Vector3& targetWorldPos, float roll)
//{
//	Vector3 direction = (targetWorldPos - this->GetWorldPosition()).Normalized();
//	float yaw = std::atan2(direction.x, direction.z); // Y軸回り
//	float pitch = std::asin(-direction.y);            // X軸回り
//
//	rotate.value = { pitch, yaw, roll };
//}
//void RenderData_Model::LookAtOnce(const RenderData_Model* other, float roll)
//{
//	LookAtOnce(other->GetWorldPosition(), roll);
//}
//void RenderData_Model::LookAtCamera(float roll)
//{
//	LookAtOnce(Game::Camera::Getter::GetTranslate("ReleaseCamera"), roll);
//}
//void RenderData_Model::LookAtFront(float roll)
//{
//	Vector3 frontPos = this->GetWorldPosition() + Vector3{ 0.0f, 0.0f, 1.0f };
//	LookAtOnce(frontPos, roll);
//}
//
//void RenderData_Model::Draw()
//{
//	Engine::Instance().AddModelDrawList(this);
//}
//
//void RenderData_Model::DrawAABB()
//{
//	for (size_t i = 0; i < this->aabbs.size(); ++i)
//	{
//		Game::DebugDraw::AddAABB(this->aabbs[i], 0xFF0000FF);
//	}
//}
//
//void RenderData_Model::DrawImGui()
//{
//	std::optional<std::string> str = "model : " + std::to_string(this->ID);
//	if (this->name != std::nullopt) str = (this->name);
//
//	std::string num = std::to_string(this->ID) + ":";
//
//	ImGui::Begin(str->c_str());
//
//	if (ImGui::TreeNode("----------transforms-----------"))
//	{
//		std::string dragId;
//
//		ImGui::Text("Scale");
//		dragId = std::string("##scale.value") + num;
//		ImGui::Text("Val"); ImGui::SameLine();
//		ImGui::DragFloat3(dragId.c_str(), &scale.value.x, 0.01f);
//		dragId = std::string("##scale.velocity") + num;
//		ImGui::Text("Vel"); ImGui::SameLine();
//		ImGui::DragFloat3(dragId.c_str(), &scale.velocity.x, 0.01f);
//		dragId = std::string("##scale.acceleration") + num;
//		ImGui::Text("Acc"); ImGui::SameLine();
//		ImGui::DragFloat3(dragId.c_str(), &scale.acceleration.x, 0.01f);
//
//		ImGui::Text("translate");
//		dragId = std::string("##translate.value") + num;
//		ImGui::Text("Val"); ImGui::SameLine();
//		ImGui::DragFloat3(dragId.c_str(), &translate.value.x, 0.01f);
//		dragId = std::string("##translate.velocity") + num;
//		ImGui::Text("Vel"); ImGui::SameLine();
//		ImGui::DragFloat3(dragId.c_str(), &translate.velocity.x, 0.01f);
//		dragId = std::string("##translate.acceleration") + num;
//		ImGui::Text("Acc"); ImGui::SameLine();
//		ImGui::DragFloat3(dragId.c_str(), &translate.acceleration.x, 0.01f);
//
//		ImGui::Text("rotate");
//		dragId = std::string("##rotate.value") + num;
//		ImGui::Text("Val"); ImGui::SameLine();
//		ImGui::DragFloat3(dragId.c_str(), &rotate.value.x, 0.01f);
//		dragId = std::string("##rotate.velocity") + num;
//		ImGui::Text("Vel"); ImGui::SameLine();
//		ImGui::DragFloat3(dragId.c_str(), &rotate.velocity.x, 0.01f);
//		dragId = std::string("##rotate.acceleration") + num;
//		ImGui::Text("Acc"); ImGui::SameLine();
//		ImGui::DragFloat3(dragId.c_str(), &rotate.acceleration.x, 0.01f);
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------uvTransforms---------"))
//	{
//		ImGui::DragFloat3((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
//		ImGui::DragFloat3((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
//		ImGui::DragFloat3((num + "UVrotate").c_str(), &uvTransform.rotate.x, 0.01f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------texture--------------"))
//	{
//		size_t textureCount = Game::Resource::GetTextureCount();
//	
//		//static const char* items[] = { "default", "additional" };
//		//int current_item = static_cast<int>();
//		//if (ImGui::Combo((num + "texture.mode").c_str(), &current_item, items, IM_ARRAYSIZE(items)))
//		//{
//		//	options.dirLight.mode = static_cast<>(current_item);
//		//}
//
//		for (size_t i = 0; i < textureCount; ++i)
//		{
//			TextureData* texData = Game::Resource::GetTextureData(static_cast<uint32_t>(i));
//			if (texData)
//			{
//				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));
//
//				// 6個並べたら改行
//				if ((i + 1) % 6 != 0 && i < textureCount - 1)
//				{
//					ImGui::SameLine();
//				}
//				if (ImGui::IsItemClicked())
//				{
//					this->texture = static_cast<uint32_t>(i);
//					//else this->additionalTexture = static_cast<uint32_t>(i);
//				}
//			}
//		}
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------model----------------"))
//	{
//		if (ImGui::Button("-"))this->model -= 1;
//
//		ImGui::SameLine();
//
//		// ラベルを非表示にするために "##" プレフィックスで ID を与える
//		std::string dragId = std::string("##model") + num;
//		ImGui::DragInt(dragId.c_str(), reinterpret_cast<int*>(&this->model));
//
//		ImGui::SameLine();
//
//		if (ImGui::Button("+"))this->model += 1;
//
//		// クランプ
//		if (this->model < 0) this->model = 0;
//		if (this->model > int(Game::Resource::GetModelCount() - 1)) this->model = int(Game::Resource::GetModelCount() - 1);
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------color----------------"))
//	{
//		float floatColor[4] = { this->color.x / 255.0f,  this->color.y / 255.0f,  this->color.z / 255.0f,  this->color.w / 255.0f };
//		ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
//		color = { floatColor[0] * 255.0f, floatColor[1] * 255.0f, floatColor[2] * 255.0f, floatColor[3] * 255.0f };
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------blendMode------------"))
//	{
//		// 表示名を実際のモードに合わせる
//		static const char* items[] = { "None", "Normal", "Add", "Sub", "Mul", "Screen", "Wireframe" };
//		int current_item = static_cast<int>(options.blendMode);
//		if (ImGui::Combo((num + "blendMode").c_str(), &current_item, items, IM_ARRAYSIZE(items)))
//		{
//			options.blendMode = static_cast<BlendMode>(current_item);
//		}
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------Light----------------"))
//	{
//		ImGui::Checkbox("useOwnLight", &options.useOwnLight);
//		if (options.useOwnLight)
//		{
//			ImGui::ColorEdit4((num + "dirLight.color").c_str(), &options.dirLight.color.x, 1);
//			ImGui::DragFloat3((num + "dirLight.direction").c_str(), &options.dirLight.direction.x, 0.01f, -1.0f, 1.0f);
//			ImGui::DragFloat((num + "dirLight.intensity").c_str(), &options.dirLight.intensity, 0.01f, 0.0f, 1.0f);
//
//			static const char* items[] = { "None", "Lambert", "HalfLambert" };
//			int current_item = static_cast<int>(options.dirLight.mode);
//			if (ImGui::Combo((num + "dirLight.mode").c_str(), &current_item, items, IM_ARRAYSIZE(items)))
//			{
//				options.dirLight.mode = static_cast<LightMode>(current_item);
//			}
//
//			ImGui::Checkbox((num + "dirLight.phong").c_str(), &options.dirLight.phong);
//		}
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------option---------------"))
//	{
//		ImGui::Checkbox("wireFrame", &options.wireframe);
//		bool lookAt = false;
//		ImGui::Checkbox("lookAt", &lookAt);
//		if (lookAt)
//		{
//			Game::Camera::MoveCameraCenter(GetWorldPosition(), 0, EaseType::IN_BACK);
//		}
//		ImGui::Text("isCollisionMouse : %d", isCollisionMouseRay);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------load & save----------"))
//	{
//		char buf[256];
//		if (this->filePath.size() < sizeof(buf)) memcpy(buf, this->filePath.c_str(), this->filePath.size() + 1);
//		else buf[sizeof(buf) - 1] = '\0';
//		if (ImGui::InputText(".json", buf, sizeof(buf)))
//		{
//			this->filePath = std::string(buf);
//		}
//		if (ImGui::Button("save"))
//		{
//			//JsonManager::SaveToJson(*this, this->filePath);
//
//
//			JsonManager::AddParam<std::string>(this->filePath, "name", this->name ? *this->name : "");
//
//		}
//		ImGui::SameLine();
//		if (ImGui::Button("load"))
//		{
//			//JsonManager::LoadFromJson(*this, this->filePath);
//		}
//		ImGui::TreePop();
//	}
//
//	ImGui::End();
//}
//
//// 全オブジェクトのSRT更新,それに伴うワールド行列更新
//void RenderData_Model::Update1()
//{
//	// デルタタイム取得
//	float deltaTime = Engine::Instance().GetDeltaTime();
//	deltaTime *= 60.0f; // 60FPS基準に変換
//
//	// S/R/Tの更新
//	this->translate.velocity += this->translate.acceleration * deltaTime;
//	this->translate.value += this->translate.velocity * deltaTime;
//	this->rotate.velocity += this->rotate.acceleration * deltaTime;
//	this->rotate.value += this->rotate.velocity * deltaTime;
//	this->scale.velocity += this->scale.acceleration * deltaTime;
//	this->scale.value += this->scale.velocity * deltaTime;
//
//	// 今フレームでS/R/Tに変化があったか
//	this->movedThisFrame =
//		(this->translate.value != this->preTranslate.value) ||
//		(this->rotate.value != this->preRotate.value) ||
//		(this->scale.value != this->preScale.value) ||
//		!this->initialized;
//
//	// ワールド座標取得
//	//if (this->movedThisFrame)
//	{
//		XMVECTOR scaleVec = XMVectorSet(this->scale.value.x, this->scale.value.y, this->scale.value.z, 0.0f);
//		XMVECTOR translateVec = XMVectorSet(this->translate.value.x, this->translate.value.y, this->translate.value.z, 0.0f);
//		XMVECTOR rotEuler = XMVectorSet(this->rotate.value.x, this->rotate.value.y, this->rotate.value.z, 0.0f);
//		// 1) スケール
//		XMMATRIX S = XMMatrixScalingFromVector(scaleVec);
//		// 2) 回転（オイラー→クォータニオン→行列）
//		XMVECTOR quatEuler = XMQuaternionRotationRollPitchYawFromVector(rotEuler);
//		XMMATRIX R = XMMatrixRotationQuaternion(quatEuler);
//		// 3) 平行移動
//		XMMATRIX T = XMMatrixTranslationFromVector(translateVec);
//		// 4) 合成: S → R → T
//		XMMATRIX world = S * R * T;
//		// 5) 結果を transforms.World に格納
//		XMFLOAT4X4 tmp;
//		DirectX::XMStoreFloat4x4(&tmp, world);
//		for (int i = 0; i < 4; ++i)
//			for (int j = 0; j < 4; ++j)
//				this->localWorldMatrix.m[i][j] = tmp.m[i][j];
//	}
//
//	initialized = true;
//}
//
//// 全オブジェクトの親を考慮したワールド行列更新,AABB更新
//void RenderData_Model::Update2()
//{
//	// 階層を含めた最終ワールド行列を取得
//	this->worldMatrix = this->SetWorldMatrix();
//
//	// ワールド座標取得
//	this->worldPos = Vector3(
//		this->worldMatrix.m[3][0],
//		this->worldMatrix.m[3][1],
//		this->worldMatrix.m[3][2]
//	);
//
//	// AABB更新（最終ワールド行列に基づいて）
//	this->aabbs = Engine::Instance().CreateAABB(this);
//}
//
//Matrix4x4 RenderData_Model::SetWorldMatrix()
//{
//	// 自身のローカル行列がベース
//	Matrix4x4 result = this->localWorldMatrix;
//
//	// 親が存在する場合は親の親の親...をウルトラ再帰する
//	if (this->parentModel)
//	{
//		Matrix4x4 parentWorld = this->parentModel->SetWorldMatrix();
//		result = result * parentWorld;
//	}
//
//	// 親がいなかったらそのまま、いたら親の行列を掛けたものを返す
//	return result;
//}
//
//// 衝突判定,衝突ペア・深度の保存
//void RenderData_Model::Update3()
//{
//	for (auto& blockTarget : blockList)
//	{
//		std::optional<CollisionInf> collisionInf = this->isCollisionAABBInf(*blockTarget);
//		if (collisionInf)
//		{
//			collisionInfos.push_back(new CollisionInf(*collisionInf));
//		}
//	}
//}
//
//// 衝突時の更新,それに伴うワールド行列更新
//void RenderData_Model::Update4()
//{
//	// collisionInfosを探査して、自分が軽い方のオブジェクトとして衝突しているものを処理
//	for (const auto& collisionInfPtr : collisionInfos)
//	{
//		const CollisionInf& collisionInf = *collisionInfPtr;
//		if (collisionInf.IDpair.light == this->ID)
//		{
//			/// 衝突したAABB面のペア・深度がわかっているので、それに基づいて移動
//			if (collisionInf.face.light == AABBFace::LEFT)
//			{
//				this->translate.value.x += collisionInf.depth.x;
//			}
//			else if (collisionInf.face.light == AABBFace::RIGHT)
//			{
//				this->translate.value.x -= collisionInf.depth.x;
//			}
//			else if (collisionInf.face.light == AABBFace::BOTTOM)
//			{
//				this->translate.value.y += collisionInf.depth.y;
//			}
//			else if (collisionInf.face.light == AABBFace::TOP)
//			{
//				this->translate.value.y -= collisionInf.depth.y;
//			}
//			else if (collisionInf.face.light == AABBFace::BACK)
//			{
//				this->translate.value.z += collisionInf.depth.z;
//			}
//			else if (collisionInf.face.light == AABBFace::FRONT)
//			{
//				this->translate.value.z -= collisionInf.depth.z;
//			}
//
//			// 衝突しているのにその方向に加速度がかかっている場合はこのフレームで加速した分を打ち消す
//			if (collisionInf.face.light == AABBFace::LEFT && this->translate.acceleration.x < 0.0f)
//			{
//				this->translate.velocity.x -= this->translate.acceleration.x;
//				//this->translate.acceleration.x = 0.0f;
//			}
//			else if (collisionInf.face.light == AABBFace::RIGHT && this->translate.acceleration.x > 0.0f)
//			{
//				this->translate.velocity.x -= this->translate.acceleration.x;
//				//this->translate.acceleration.x = 0.0f;
//			}
//			else if (collisionInf.face.light == AABBFace::BOTTOM && this->translate.acceleration.y < 0.0f)
//			{
//				this->translate.velocity.y -= this->translate.acceleration.y;
//				//this->translate.acceleration.y = 0.0f;
//			}
//			else if (collisionInf.face.light == AABBFace::TOP && this->translate.acceleration.y > 0.0f)
//			{
//				this->translate.velocity.y -= this->translate.acceleration.y;
//				//this->translate.acceleration.y = 0.0f;
//			}
//			else if (collisionInf.face.light == AABBFace::BACK && this->translate.acceleration.z < 0.0f)
//			{
//				this->translate.velocity.z -= this->translate.acceleration.z;
//				//this->translate.acceleration.z = 0.0f;
//			}
//			else if (collisionInf.face.light == AABBFace::FRONT && this->translate.acceleration.z > 0.0f)
//			{
//				this->translate.velocity.z -= this->translate.acceleration.z;
//				//this->translate.acceleration.z = 0.0f;
//			}
//
//			// translate.velocityの分めり込んだ状態で固定されてしまうので、velocity分座標を戻す。velocityは変えない。
//			this->translate.value -= this->translate.velocity;
//
//			XMVECTOR scaleVec = XMVectorSet(this->scale.value.x, this->scale.value.y, this->scale.value.z, 0.0f);
//			XMVECTOR translateVec = XMVectorSet(this->translate.value.x, this->translate.value.y, this->translate.value.z, 0.0f);
//			XMVECTOR rotEuler = XMVectorSet(this->rotate.value.x, this->rotate.value.y, this->rotate.value.z, 0.0f);
//			// 1) スケール
//			XMMATRIX S = XMMatrixScalingFromVector(scaleVec);
//			// 2) 回転（オイラー→クォータニオン→行列）
//			XMVECTOR quatEuler = XMQuaternionRotationRollPitchYawFromVector(rotEuler);
//			XMMATRIX R = XMMatrixRotationQuaternion(quatEuler);
//			// 3) 平行移動
//			XMMATRIX T = XMMatrixTranslationFromVector(translateVec);
//			// 4) 合成: S → R → T
//			XMMATRIX world = S * R * T;
//			// 5) 結果を transforms.World に格納
//			XMFLOAT4X4 tmp;
//			DirectX::XMStoreFloat4x4(&tmp, world);
//			for (int i = 0; i < 4; ++i)
//				for (int j = 0; j < 4; ++j)
//					this->localWorldMatrix.m[i][j] = tmp.m[i][j];
//		}
//	}
//}
//
//// 全オブジェクトの描画範囲内判定,前フレーム情報保存
//void RenderData_Model::Update5()
//{
//	collisionInfos.clear();
//
//#pragma region 描画範囲内判定
//
//	bool inFrustum = false;
//	for (const auto& aabb : this->aabbs)
//	{
//		if (Game::Camera::InCamera(aabb))
//		{
//			inFrustum = true;
//			break;
//		}
//	}
//	this->inPicture = inFrustum;
//
//#pragma endregion
//
//#pragma region 前フレーム情報保存
//
//	this->preScale = this->scale;
//	this->preTranslate = this->translate;
//	this->preRotate = this->rotate;
//	this->preAABB = this->aabbs;
//
//#pragma endregion
//
//}
//
//
//
//// acceleration→velocity の積分のみ
//void RenderData_Model::UpdateVelocitiesPhysics()
//{
//	// デルタタイム取得
//	float deltaTime = Engine::Instance().GetDeltaTime();
//	deltaTime *= 60.0f; // 60FPS基準に変換
//
//	this->translate.velocity += this->translate.acceleration * deltaTime;
//	this->rotate.velocity += this->rotate.acceleration * deltaTime;
//	this->scale.velocity += this->scale.acceleration * deltaTime;
//}
//
//// 位置に移動量(delta) を適用する（Sweep後の補正deltaを入れる想定）
//void RenderData_Model::ApplyTranslationDelta(const Vector3& delta)
//{
//	this->translate.value += delta;
//}
//
//// SRTにVelocity, Accelerationを反映させる
//void RenderData_Model::UpdateTransformsPhysics()
//{
//	// デルタタイム取得
//	float deltaTime = Engine::Instance().GetDeltaTime();
//	deltaTime *= 60.0f; // 60FPS基準に変換
//
//	// 1) 速度を更新
//	UpdateVelocitiesPhysics();
//
//	// 2) 位置に反映（旧仕様では velocity*dt をそのまま適用）
//	ApplyTranslationDelta(this->translate.velocity * deltaTime);
//
//	// 3) 回転・スケールは旧仕様通り
//	this->rotate.value += this->rotate.velocity * deltaTime;
//	this->scale.value += this->scale.velocity * deltaTime;
//}
//
//// 現状のSRTからローカルマトリックスを作成する
//void RenderData_Model::UpdateLocalMatrix()
//{
//	XMVECTOR scaleVec = XMVectorSet(this->scale.value.x, this->scale.value.y, this->scale.value.z, 0.0f);
//	XMVECTOR translateVec = XMVectorSet(this->translate.value.x, this->translate.value.y, this->translate.value.z, 0.0f);
//	XMVECTOR rotEuler = XMVectorSet(this->rotate.value.x, this->rotate.value.y, this->rotate.value.z, 0.0f);
//	// 1) スケール
//	XMMATRIX S = XMMatrixScalingFromVector(scaleVec);
//	// 2) 回転（オイラー→クォータニオン→行列）
//	XMVECTOR quatEuler = XMQuaternionRotationRollPitchYawFromVector(rotEuler);
//	XMMATRIX R = XMMatrixRotationQuaternion(quatEuler);
//	// 3) 平行移動
//	XMMATRIX T = XMMatrixTranslationFromVector(translateVec);
//	// 4) 合成: S → R → T
//	XMMATRIX world = S * R * T;
//	// 5) 結果を transforms.World に格納
//	XMFLOAT4X4 tmp;
//	DirectX::XMStoreFloat4x4(&tmp, world);
//	for (int i = 0; i < 4; ++i)
//		for (int j = 0; j < 4; ++j)
//			this->localWorldMatrix.m[i][j] = tmp.m[i][j];
//}
//
//// 現状のSRTからワールドマトリックスを作成する
//void RenderData_Model::UpdateWorldMatrix()
//{
//	// 階層を含めた最終ワールド行列を取得
//	this->worldMatrix = this->SetWorldMatrix();
//
//	// ワールド座標取得
//	this->worldPos = Vector3(
//		this->worldMatrix.m[3][0],
//		this->worldMatrix.m[3][1],
//		this->worldMatrix.m[3][2]
//	);
//}
//
//void RenderData_Model::UpdateAABB()
//{
//	this->aabbs = Engine::Instance().CreateAABB(this);
//}
//
//// 現状のAABBから描画範囲内判定を行う
//void RenderData_Model::UpdateInPicture()
//{
//	bool inFrustum = false;
//	for (const auto& aabb : this->aabbs)
//	{
//		if (Game::Camera::InCamera(aabb))
//		{
//			inFrustum = true;
//			break;
//		}
//	}
//	this->inPicture = inFrustum;
//}
//
//// 現状のSRTを前フレームSRTとして保存する
//void RenderData_Model::SavePreTransforms()
//{
//	this->preScale = this->scale;
//	this->preTranslate = this->translate;
//	this->preRotate = this->rotate;
//	this->preAABB = this->aabbs;
//}
//
//void RenderData_Model::SetModel(int32_t modelHandle)
//{
//	// モデルハンドルをセット
//	model = modelHandle;
//	// AABBを生成
//	aabbs = Engine::Instance().CreateAABB(this);
//}
//
//void RenderData_Model::SetTexture(int32_t textureHandle)
//{
//	// テクスチャハンドルをセット
//	texture = textureHandle;
//}
//
////std::optional<CollisionInf> RenderData_Model::isCollisionAABBInf(RenderData_Model& target) const
////{
////	CollisionInf result;
////	// 衝突しているオブジェクトIDの保存（軽い方・重い方）
////	result.IDpair.light = target.ID;
////	result.IDpair.heavy = this->ID;
////	if (this->mass < target.mass)
////	{
////		std::swap(result.IDpair.light, result.IDpair.heavy);
////	}
////	result.AABBpair = CollisionPair{ -1, -1 };
////
////	// AABB同士の衝突判定
////	for (size_t i = 0; i < target.aabbs.size(); ++i)
////	{
////		for (size_t j = 0; j < this->aabbs.size(); ++j)
////		{
////			// 緩めの衝突判定
////			if (IsLooseCollision(target.aabbs[i], this->aabbs[j], 0.001f))
////			{
////				// 衝突深度取得
////				Vector3 depth = this->aabbs[j].GetCollisionDepth(target.aabbs[i]);
////
////				// 衝突ペア・深度の保存
////				result.AABBpair = CollisionPair{ static_cast<int>(i), static_cast<int>(j) };
////				result.depth = -depth;
////				if (this->mass < target.mass)
////				{
////					std::swap(result.AABBpair.light, result.AABBpair.heavy);
////					result.depth = depth;
////				}
////				break;
////			}
////		}
////	}
////	if (result.AABBpair == CollisionPair{ -1, -1 }) return std::nullopt;
////	return result;
////}
//
//std::optional<CollisionInf> RenderData_Model::isCollisionAABBInf(RenderData_Model& target) const {
//	CollisionInf best;
//	bool found = false;
//	float bestPen = std::numeric_limits<float>::infinity();
//
//	for (size_t i = 0; i < target.aabbs.size(); ++i)
//	{ 
//		// AABB中心点取得
//		Vector3 centerT = target.aabbs[i].center();
//		for (size_t j = 0; j < this->aabbs.size(); ++j) 
//		{
//			// AABB中心点取得
//			Vector3 centerS = this->aabbs[j].center();
//
//			// 衝突判定　衝突していなければスキップ
//			if (!IsLooseCollision(target.aabbs[i], this->aabbs[j], 0.001f)) continue;
//
//			// 深度取得
//			Vector3 overlap = this->aabbs[j].GetCollisionDepth(target.aabbs[i]);
//
//
//			// 一番浅い軸を探す
//			float pen = overlap.x;
//			int axis = 0; // 0:x, 1:y, 2:z
//			if (overlap.y < pen) { pen = overlap.y; axis = 1; }
//			if (overlap.z < pen) { pen = overlap.z; axis = 2; }
//
//			// 衝突面の特定
//			CollisionAABBFace faces{ AABBFace::NONE, AABBFace::NONE };
//
//			if (axis == 0) { // x
//				if (centerT.x < centerS.x) { faces.light = AABBFace::RIGHT; faces.heavy = AABBFace::LEFT; }
//				else { faces.light = AABBFace::LEFT; faces.heavy = AABBFace::RIGHT; }
//			}
//			else if (axis == 1) { // y
//				if (centerT.y < centerS.y) { faces.light = AABBFace::TOP; faces.heavy = AABBFace::BOTTOM; }
//				else { faces.light = AABBFace::BOTTOM; faces.heavy = AABBFace::TOP; }
//			}
//			else { // z
//				if (centerT.z < centerS.z) { faces.light = AABBFace::FRONT; faces.heavy = AABBFace::BACK; }
//				else { faces.light = AABBFace::BACK; faces.heavy = AABBFace::FRONT; }
//			}
//
//			// 最小貫通深度の更新
//			if (pen < bestPen) {
//				bestPen = pen;
//				found = true;
//				best.AABBpair = CollisionPair{ static_cast<int>(i), static_cast<int>(j) };
//				best.depth = overlap;
//				best.face = faces;
//			}
//		}
//	}
//
//	// 衝突ペアが見つからなかった場合
//	if (!found) return std::nullopt;
//
//	// 衝突しているオブジェクトIDの保存
//	best.IDpair.light = target.ID;
//	best.IDpair.heavy = this->ID;
//
//	// 軽い方・重い方の入れ替え
//	if (this->mass < target.mass) {
//		std::swap(best.IDpair.light, best.IDpair.heavy);
//		std::swap(best.AABBpair.light, best.AABBpair.heavy);
//		std::swap(best.face.light, best.face.heavy);
//	}
//
//	return best;
//}
//
//#pragma endregion
//
//#pragma region sprite
//
//RenderData_Sprite::RenderData_Sprite()
//{
//	renderSprites.push_back(this);
//	this->ID = int(renderSprites.size());
//}
//
//RenderData_Sprite::~RenderData_Sprite()
//{
//	auto it = std::find(renderSprites.begin(), renderSprites.end(), this);
//	if (it != renderSprites.end())
//	{
//		renderSprites.erase(it);
//	}
//}
//
//void RenderData_Sprite::Draw()
//{
//	Engine::Instance().AddSpriteDrawList(this);
//}
//
//void RenderData_Sprite::DrawImGui()
//{
//	std::optional<std::string> str = "NULL";
//	if (this->name != std::nullopt) str = (this->name);
//	else str = "sprite : " + std::to_string(this->ID);
//
//	std::string num = std::to_string(this->ID) + ":";
//
//	ImGui::Begin(str->c_str());
//
//	if (ImGui::TreeNode("----------transforms-----------"))
//	{
//		ImGui::DragFloat2((num + "scale").c_str(), &transforms.scale.x, 0.01f);
//		ImGui::DragFloat2((num + "translate").c_str(), &transforms.translate.x, 1.0f);
//		ImGui::DragFloat((num + "rotate").c_str(), &transforms.rotate.z, 0.01f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------uvTransforms---------"))
//	{
//		ImGui::DragFloat2((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
//		ImGui::DragFloat2((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
//		ImGui::DragFloat((num + "UVrotate").c_str(), &uvTransform.rotate.z, 0.01f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------cutImage-------------"))
//	{
//		ImGui::DragInt2((num + "cutImageLeftTop").c_str(), &cutImageLeftTop.x, 1);
//		ImGui::DragInt2((num + "cutImageSize").c_str(), &cutImageSize.x, 1);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------pivot----------------"))
//	{
//		ImGui::DragFloat2((num + "pivot").c_str(), &pivot.x, 0.1f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------color----------------"))
//	{
//		Vector4 preColor = ConvertUintToVector4(color);
//		float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
//		ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
//		Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
//		color = ConvertVector4ToUint(vector4Color);
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------texture--------------"))
//	{
//		size_t textureCount = Game::Resource::GetTextureCount();
//
//		for (size_t i = 0; i < textureCount; ++i)
//		{
//			TextureData* texData = Game::Resource::GetTextureData(static_cast<uint32_t>(i));
//			if (texData)
//			{
//				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));
//
//				// 6個並べたら改行
//				if ((i + 1) % 6 != 0 && i < textureCount - 1)
//				{
//					ImGui::SameLine();
//				}
//				if (ImGui::IsItemClicked())
//				{
//					this->texture = static_cast<uint32_t>(i);
//				}
//			}
//		}
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------Anchor---------------"))
//	{
//		const char* items[] =
//		{ "Center","CenterLeft","CenterRight","CenterTop","CenterDown","LeftTop","RightTop","LeftDown","RightDown" };
//		int item_current = int(this->anchor);
//		ImGui::Combo((num + "Anchor").c_str(), &item_current, items, IM_ARRAYSIZE(items));
//		{
//			if (item_current == 0)anchor = Anchor::Center;
//			if (item_current == 1)anchor = Anchor::CenterLeft;
//			if (item_current == 2)anchor = Anchor::CenterRight;
//			if (item_current == 3)anchor = Anchor::CenterTop;
//			if (item_current == 4)anchor = Anchor::CenterDown;
//			if (item_current == 5)anchor = Anchor::LeftTop;
//			if (item_current == 6)anchor = Anchor::RightTop;
//			if (item_current == 7)anchor = Anchor::LeftDown;
//			if (item_current == 8)anchor = Anchor::RightDown;
//		}
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------option---------------"))
//	{
//		if (isCollisionMouseRay)ImGui::Text("isCollisionMouse : true");
//		else ImGui::Text("isCollisionMouse : false");
//
//		ImGui::TreePop();
//	}
//	ImGui::End();
//}
//
//#pragma endregion
//
//#pragma region triangle
//
//RenderData_Triangle::RenderData_Triangle()
//{
//	renderTriangles.push_back(this);
//	this->ID = int(renderTriangles.size());
//}
//
//RenderData_Triangle::~RenderData_Triangle()
//{
//	auto it = std::find(renderTriangles.begin(), renderTriangles.end(), this);
//	if (it != renderTriangles.end())
//	{
//		renderTriangles.erase(it);
//	}
//}
//
//void RenderData_Triangle::Draw()
//{
//	Engine::Instance().AddTriangleDrawList(this);
//}
//
//void RenderData_Triangle::DrawImGui()
//{
//	std::optional<std::string> str = "triangle : " + std::to_string(this->ID);
//	if (this->name != std::nullopt) str = (this->name);
//
//	std::string num = std::to_string(this->ID) + ":";
//
//	ImGui::Begin(str->c_str());
//
//	if (ImGui::TreeNode("----------transforms-----------"))
//	{
//		ImGui::DragFloat3((num + "scale").c_str(), &transforms.scale.x, 0.01f);
//		ImGui::DragFloat3((num + "translate").c_str(), &transforms.translate.x, 1.0f);
//		ImGui::DragFloat3((num + "rotate").c_str(), &transforms.rotate.x, 0.01f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------uvTransforms---------"))
//	{
//		ImGui::DragFloat3((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
//		ImGui::DragFloat3((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
//		ImGui::DragFloat((num + "UVrotate").c_str(), &uvTransform.rotate.z, 0.01f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------position-------------"))
//	{
//		ImGui::DragFloat3((num + "pos1").c_str(), &pos1.x, 0.1f);
//		ImGui::DragFloat3((num + "pos2").c_str(), &pos2.x, 0.1f);
//		ImGui::DragFloat3((num + "pos3").c_str(), &pos3.x, 0.1f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------texture--------------"))
//	{
//		for (size_t i = 0; i < Game::Resource::GetTextureCount(); ++i)
//		{
//			TextureData* texData = Game::Resource::GetTextureData(static_cast<uint32_t>(i));
//			if (texData)
//			{
//				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));
//
//				// 6個並べたら改行
//				if ((i + 1) % 6 != 0 && i < Game::Resource::GetTextureCount() - 1)
//				{
//					ImGui::SameLine();
//				}
//				if (ImGui::IsItemClicked())
//				{
//					this->texture = static_cast<uint32_t>(i);
//				}
//			}
//		}
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------pivot----------------"))
//	{
//		//ImGui::DragFloat2((num + "pivot").c_str(), &pivot.x, 0.1f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------color----------------"))
//	{
//		Vector4 preColor = ConvertUintToVector4(color);
//		float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
//		ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
//		Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
//		color = ConvertVector4ToUint(vector4Color);
//		ImGui::TreePop();
//	}
//
//	ImGui::End();
//}
//
//#pragma endregion
//
//#pragma region line
//
//RenderData_Line::RenderData_Line()
//{
//	renderLines.push_back(this);
//	this->ID = int(renderLines.size());
//	this->points.push_back(Vector3{ 0.0f,0.0f,0.0f });
//}
//
//RenderData_Line::~RenderData_Line()
//{
//	auto it = std::find(renderLines.begin(), renderLines.end(), this);
//	if (it != renderLines.end())
//	{
//		renderLines.erase(it);
//	}
//}
//
//void RenderData_Line::Draw()
//{
//	if (this->points.size() < 3)
//	{
//		this->points.push_back(Vector3{ 0.0f,0.0f,0.0f });
//		this->points.push_back(Vector3{ 0.0f,0.0f,0.0f });
//	}
//	Engine::Instance().AddLineDrawList(this);
//}
//
//void RenderData_Line::DrawPoints()
//{
//
//}
//
//void RenderData_Line::DrawImGui()
//{
//	std::optional<std::string> str = "line : " + std::to_string(this->ID);
//	if (this->name != std::nullopt) str = (this->name);
//
//	std::string num = std::to_string(this->ID) + ":";
//
//	ImGui::Begin(str->c_str());
//
//	if (ImGui::TreeNode("----------color----------------"))
//	{
//		Vector4 preColor = ConvertUintToVector4(color);
//		float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
//		ImGui::ColorEdit4((num + " : color").c_str(), floatColor, 1);
//		Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
//		color = ConvertVector4ToUint(vector4Color);
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------LineType-------------"))
//	{
//		const char* items[] =
//		{ "Line","BezierCurve", "SplineCurve" };
//		int item_current = int(this->lineType);
//		ImGui::Combo((num + "LineType").c_str(), &item_current, items, IM_ARRAYSIZE(items));
//		{
//			if (item_current == 0)lineType = LineType::Line;
//			if (item_current == 1)lineType = LineType::BezierCurve;
//			if (item_current == 2)lineType = LineType::SplineCurve;
//		}
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------kSubdivision---------"))
//	{
//		ImGui::DragInt((num + " : kSubdivision").c_str(), (int*)&kSubdivision, 1, 1, 100);
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------points---------------"))
//	{
//		ImGui::Text("points");
//		size_t i = 1;
//		for (i = 1; i < points.size(); ++i)
//		{
//			ImGui::DragFloat3((num + "." + std::to_string(i)).c_str(), &points[i].x, 0.1f);
//			ImGui::SameLine();
//			if (ImGui::Button(("delete" + num + "." + std::to_string(i)).c_str()))
//			{
//				points.erase(points.begin() + i);
//				--i;
//			}
//		}
//
//		ImGui::Text("addPoint");
//		ImGui::DragFloat3((num + " : addPoint").c_str(), &points[0].x, 0.1f);
//		if (ImGui::Button((num + " : AddPoint").c_str()))
//		{
//			Vector3 newPoint = points[0];
//			points.push_back(newPoint);
//		}
//
//		ImGui::TreePop();
//	}
//
//
//	ImGui::End();
//}
//
//#pragma endregion
//
//#pragma region particle
//
//RenderData_Particle::RenderData_Particle()
//{
//	renderParticles.push_back(this);
//	this->ID = int(renderParticles.size());
//
//	instancingResource_ =
//		Engine::Instance().CreateBufferResource(
//			sizeof(Matrix4x4) * capacity
//		);
//	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
//	for (size_t i = 0; i < capacity; ++i)
//	{
//		instancingData_[i] = Matrix4x4::MakeIdentity4x4();
//	}
//
//	srvAllocation_ = Engine::Instance().GetDirectXManager() ->GetDescriptorHeapManager()->GetSrvManager()->CreateSRVforStructuredBuffer(
//		instancingResource_.Get(),
//	    capacity,
//	    sizeof(Matrix4x4));
//
//	targetScale.value = Vector3{ 1.0f,1.0f,1.0f };
//	targetRotate.value = Vector3{ 0.0f,0.0f,0.0f };
//	targetTranslate.value = Vector3{ 0.0f,0.0f,0.0f };
//
//	scale_.resize(capacity);
//	rotate_.resize(capacity);
//	translate_.resize(capacity);
//	lifeCount_.resize(capacity, 0);
//	isActive_.resize(capacity, false);
//}
//RenderData_Particle::~RenderData_Particle()
//{}
//
//void RenderData_Particle::UpdateAllParticles()
//{
//	for (size_t ID = 0; ID < renderParticles.size(); ++ID)
//	{
//		renderParticles[ID]->Update();
//	}
//}
//
//void RenderData_Particle::Update()
//{
//	// 非アクティブなパーティクルの削除
//	RemoveInactiveParticles();
//
//	// パーティクルの生成
//	SpawnParticle();
//
//	// SRTの更新
//	UpdateTransforms();
//
//	// ワールド行列の更新
//	UpdateTransformationMatrix();
//
//	// 寿命の更新
//	UpdateLife();
//
//	// 非アクティブ化
//	CheckLife();
//
//	frame++;
//}
//
//void RenderData_Particle::SpawnParticle()
//{
//	uint32_t ableParticles = 0;
//	if (frame % emissionDelay == 0)
//	{
//		for (uint32_t index = currentSum; index < currentSum + particlesPerEmission; ++index)
//		{
//			if (index >= capacity) break; // キャパシティ超過防止
//			if (!isActive_[index])
//			{
//				// 有効化
//				isActive_[index] = true;
//
//				// 寿命設定
//				lifeCount_[index] = liveMax;
//
//				// スケール・回転・位置の初期化
//				SetSpawnScale(index);
//				SetSpawnRotate(index);
//				SetSpawnTranslate(index);
//
//				// 作成できたパーティクル数をカウント
//				ableParticles++;
//			}
//		}
//		currentSum += ableParticles;
//	}
//}
//void RenderData_Particle::SetSpawnPosition(uint32_t index)
//{
//	switch (emitterShape)
//	{
//	case PrimitiveType::Sphere:
//	{
//		// 内側
//		if (emitFromInside)
//		{
//			Vector3 center = emitterSphere.center;
//			float radius = emitterSphere.radius;
//
//			// ランダムな方向（単位ベクトル）を生成
//			float theta = RandomFloat(0.0f, 2.0f * float(std::numbers::pi), 3);   // 0〜2π
//			float phi = RandomFloat(0.0f, float(std::numbers::pi), 3);            // 0〜π
//			float r = RandomFloat(0.0f, 1.0f, 3);								  // 0〜1（球内）
//
//			// 球内部の距離に合わせてスケーリング（立方根で均等分布）
//			r = pow(r, 1.0f / 3.0f);
//
//			// 球面座標系から直交座標系へ変換
//			float x = r * sin(phi) * cos(theta) * radius;
//			float y = r * sin(phi) * sin(theta) * radius;
//			float z = r * cos(phi) * radius;
//
//			translate_[index].value.x = center.x + x;
//			translate_[index].value.y = center.y + y;
//			translate_[index].value.z = center.z + z;
//		}
//		// 外殻
//		else
//		{
//			// 球表面上のランダムな点を生成
//			Vector3 center = emitterSphere.center;
//			float radius = emitterSphere.radius;
//
//			// ランダムな方向（単位ベクトル）を生成
//			float theta = RandomFloat(0.0f, 2.0f * float(std::numbers::pi), 3);   // 0〜2π
//			float phi = RandomFloat(0.0f, float(std::numbers::pi), 3);            // 0〜π
//
//			// 球面座標系から直交座標系へ変換
//			float x = sin(phi) * cos(theta) * radius;
//			float y = sin(phi) * sin(theta) * radius;
//			float z = cos(phi) * radius;
//
//			translate_[index].value.x = center.x + x;
//			translate_[index].value.y = center.y + y;
//			translate_[index].value.z = center.z + z;
//			break;
//		}
//
//		break;
//	}
//	case PrimitiveType::SphereXYZ:
//	{
//		// 内側
//		if (emitFromInside)
//		{
//			Vector3 center = emitterSphereXYZ.center;
//			Vector3 radius = emitterSphereXYZ.radius;
//
//			// ランダムな方向（単位ベクトル）を生成
//			float theta = RandomFloat(0.0f, 2.0f * float(std::numbers::pi), 3);   // 0〜2π
//			float phi = RandomFloat(0.0f, float(std::numbers::pi), 3);            // 0〜π
//			float r = RandomFloat(0.0f, 1.0f, 3);								  // 0〜1（球内）
//
//			// 球内部の距離に合わせてスケーリング（立方根で均等分布）
//			r = pow(r, 1.0f / 3.0f);
//
//			// 球面座標系から直交座標系へ変換
//			float x = r * sin(phi) * cos(theta) * radius.x;
//			float y = r * sin(phi) * sin(theta) * radius.y;
//			float z = r * cos(phi) * radius.z;
//
//			translate_[index].value.x = center.x + x;
//			translate_[index].value.y = center.y + y;
//			translate_[index].value.z = center.z + z;
//		}
//		// 外殻
//		else
//		{
//			// 球表面上のランダムな点を生成
//			Vector3 center = emitterSphereXYZ.center;
//			Vector3 radius = emitterSphereXYZ.radius;
//
//			// ランダムな方向（単位ベクトル）を生成
//			float theta = RandomFloat(0.0f, 2.0f * float(std::numbers::pi), 3);   // 0〜2π
//			float phi = RandomFloat(0.0f, float(std::numbers::pi), 3);            // 0〜π
//
//			// 球面座標系から直交座標系へ変換
//			float x = sin(phi) * cos(theta) * radius.x;
//			float y = sin(phi) * sin(theta) * radius.y;
//			float z = cos(phi) * radius.z;
//
//			translate_[index].value.x = center.x + x;
//			translate_[index].value.y = center.y + y;
//			translate_[index].value.z = center.z + z;
//		}
//		break;
//	}
//	case PrimitiveType::AABB:
//	{
//		// 内側
//		if (emitFromInside)
//		{
//			translate_[index].value.x = RandomFloat(emitterAABB.min.x, emitterAABB.max.x);
//			translate_[index].value.y = RandomFloat(emitterAABB.min.y, emitterAABB.max.y);
//			translate_[index].value.z = RandomFloat(emitterAABB.min.z, emitterAABB.max.z);
//		}
//		else
//		{
//			int i = RandomInt(1, 6);
//			if (i == 1 || i == 2)
//			{
//				if (i == 1)
//				{
//					translate_[index].value.x = emitterAABB.min.x;
//				}
//				else
//				{
//					translate_[index].value.x = emitterAABB.max.x;
//				}
//				translate_[index].value.y = RandomFloat(emitterAABB.min.y, emitterAABB.max.y, 3);
//				translate_[index].value.z = RandomFloat(emitterAABB.min.z, emitterAABB.max.z, 3);
//			}
//			else if (i == 3 || i == 4)
//			{
//				if (i == 3)
//				{
//					translate_[index].value.y = emitterAABB.min.y;
//				}
//				else
//				{
//					translate_[index].value.y = emitterAABB.max.y;
//				}
//				translate_[index].value.x = RandomFloat(emitterAABB.min.x, emitterAABB.max.x, 3);
//				translate_[index].value.z = RandomFloat(emitterAABB.min.z, emitterAABB.max.z, 3);
//			}
//			else if (i == 5 || i == 6)
//			{
//				if (i == 5)
//				{
//					translate_[index].value.z = emitterAABB.min.z;
//				}
//				else
//				{
//					translate_[index].value.z = emitterAABB.max.z;
//				}
//				translate_[index].value.y = RandomFloat(emitterAABB.min.y, emitterAABB.max.y, 3);
//				translate_[index].value.x = RandomFloat(emitterAABB.min.x, emitterAABB.max.x, 3);
//			}
//		}
//
//		break;
//	}
//	case PrimitiveType::Plane:
//		break;
//	case PrimitiveType::Circle:
//		break;
//	default:
//		break;
//	}
//}
//
//void RenderData_Particle::SetSpawnScale(uint32_t index)
//{
//	if (targetScale.isRandom_value)
//	{
//		targetScale.randomRange_value.Fix();
//		scale_[index].value.x = RandomFloat(
//			targetScale.randomRange_value.min.x,
//			targetScale.randomRange_value.max.x, 2);
//		scale_[index].value.y = RandomFloat(
//			targetScale.randomRange_value.min.y,
//			targetScale.randomRange_value.max.y, 2);
//		scale_[index].value.z = RandomFloat(
//			targetScale.randomRange_value.min.z,
//			targetScale.randomRange_value.max.z, 2);
//	}
//	else 
//	{
//		scale_[index].value = targetScale.value;
//	}
//	if (targetScale.isRandom_velocity)
//	{
//		targetScale.randomRange_velocity.Fix();
//		scale_[index].velocity.x = RandomFloat(
//			targetScale.randomRange_velocity.min.x,
//			targetScale.randomRange_velocity.max.x, 2);
//		scale_[index].velocity.y = RandomFloat(
//			targetScale.randomRange_velocity.min.y,
//			targetScale.randomRange_velocity.max.y, 2);
//		scale_[index].velocity.z = RandomFloat(
//			targetScale.randomRange_velocity.min.z,
//			targetScale.randomRange_velocity.max.z, 2);
//	}
//	else
//	{
//		scale_[index].velocity = targetScale.velocity;
//	}
//	if (targetScale.isRandom_acceleration)
//	{
//		targetScale.randomRange_acceleration.Fix();
//		scale_[index].acceleration.x = RandomFloat(
//			targetScale.randomRange_acceleration.min.x,
//			targetScale.randomRange_acceleration.max.x, 2);
//		scale_[index].acceleration.y = RandomFloat(
//			targetScale.randomRange_acceleration.min.y,
//			targetScale.randomRange_acceleration.max.y, 2);
//		scale_[index].acceleration.z = RandomFloat(
//			targetScale.randomRange_acceleration.min.z,
//			targetScale.randomRange_acceleration.max.z, 2);
//	}
//	else
//	{
//		scale_[index].acceleration = targetScale.acceleration;
//	}
//}
//
//void RenderData_Particle::SetSpawnRotate(uint32_t index)
//{
//	if (isBillboard)
//	{
//		Vector3 direction = (Game::Camera::Getter::GetTranslate("ReleaseCamera") - translate_[index].value).Normalized();
//		float yaw = std::atan2(direction.x, direction.z); // Y軸
//		float pitch = std::asin(-direction.y);            // X軸
//		rotate_[index].value = {pitch, yaw, 0.0f};
//	}
//	else
//	{
//		if (targetRotate.isRandom_value)
//		{
//			targetRotate.randomRange_value.Fix();
//			rotate_[index].value.x = RandomFloat(
//				targetRotate.randomRange_value.min.x,
//				targetRotate.randomRange_value.max.x, 2);
//			rotate_[index].value.y = RandomFloat(
//				targetRotate.randomRange_value.min.y,
//				targetRotate.randomRange_value.max.y, 2);
//			rotate_[index].value.z = RandomFloat(
//				targetRotate.randomRange_value.min.z,
//				targetRotate.randomRange_value.max.z, 2);
//		}
//		else
//		{
//			rotate_[index].value = targetRotate.value;
//		}
//		if (targetRotate.isRandom_velocity)
//		{
//			targetRotate.randomRange_velocity.Fix();
//			rotate_[index].velocity.x = RandomFloat(
//				targetRotate.randomRange_velocity.min.x,
//				targetRotate.randomRange_velocity.max.x, 2);
//			rotate_[index].velocity.y = RandomFloat(
//				targetRotate.randomRange_velocity.min.y,
//				targetRotate.randomRange_velocity.max.y, 2);
//			rotate_[index].velocity.z = RandomFloat(
//				targetRotate.randomRange_velocity.min.z,
//				targetRotate.randomRange_velocity.max.z, 2);
//		}
//		else
//		{
//			rotate_[index].velocity = targetRotate.velocity;
//		}
//		if (targetRotate.isRandom_acceleration)
//		{
//			targetRotate.randomRange_acceleration.Fix();
//			rotate_[index].acceleration.x = RandomFloat(
//				targetRotate.randomRange_acceleration.min.x,
//				targetRotate.randomRange_acceleration.max.x, 2);
//			rotate_[index].acceleration.y = RandomFloat(
//				targetRotate.randomRange_acceleration.min.y,
//				targetRotate.randomRange_acceleration.max.y, 2);
//			rotate_[index].acceleration.z = RandomFloat(
//				targetRotate.randomRange_acceleration.min.z,
//				targetRotate.randomRange_acceleration.max.z, 2);
//		}
//		else
//		{
//			rotate_[index].acceleration = targetRotate.acceleration;
//		}
//	}
//}
//
//void RenderData_Particle::SetSpawnTranslate(uint32_t index)
//{
//	SetSpawnPosition(index);
//
//	if (useTarget)
//	{
//		// エミッター中心から外側方向に移動
//		if (spawnDependent)
//		{
//			Vector3 direction;
//			switch (emitterShape)
//			{
//			case PrimitiveType::Sphere:
//				direction = translate_[index].value - emitterSphere.center;
//				break;
//			case PrimitiveType::SphereXYZ:
//				direction = translate_[index].value - emitterSphereXYZ.center;
//				break;
//			case PrimitiveType::AABB:
//				direction = translate_[index].value - emitterAABB.center();
//				break;
//			case PrimitiveType::Plane:
//				break;
//			case PrimitiveType::Circle:
//				break;
//			default:
//				break;
//			}
//			direction.Normalize();
//
//			//// 拡散角度を考慮	
//			//float randomAngleX = RandomFloat(-spreadAngle / 2.0f, spreadAngle / 2.0f, 3);
//			//float randomAngleY = RandomFloat(-spreadAngle / 2.0f, spreadAngle / 2.0f, 3);
//			//Matrix4x4 rotationMatrix = Matrix4x4::MakeRotateXMatrix(randomAngleX) * Matrix4x4::MakeRotateYMatrix(randomAngleY);
//			//direction = Vector3::TransformNormal(direction, rotationMatrix);
//
//			translate_[index].velocity = direction * speed;
//		}
//		// ターゲット方向に移動
//		else
//		{
//			Vector3 direction = target - translate_[index].value;
//			direction.Normalize();
//
//			translate_[index].velocity = direction * speed;
//		}
//	}
//	else
//	{
//		// ランダム方向に移動
//		if (targetTranslate.isRandom_velocity)
//		{
//			targetTranslate.randomRange_velocity.Fix();
//			translate_[index].velocity.x = RandomFloat(
//				targetTranslate.randomRange_velocity.min.x,
//				targetTranslate.randomRange_velocity.max.x, 2);
//			translate_[index].velocity.y = RandomFloat(
//				targetTranslate.randomRange_velocity.min.y,
//				targetTranslate.randomRange_velocity.max.y, 2);
//			translate_[index].velocity.z = RandomFloat(
//				targetTranslate.randomRange_velocity.min.z,
//				targetTranslate.randomRange_velocity.max.z, 2);
//		}
//		else
//		{
//			translate_[index].velocity = targetTranslate.velocity;
//		}
//	}
//
//	if (targetTranslate.isRandom_acceleration)
//	{
//		targetTranslate.randomRange_acceleration.Fix();
//		translate_[index].acceleration.x = RandomFloat(
//			targetTranslate.randomRange_acceleration.min.x,
//			targetTranslate.randomRange_acceleration.max.x, 2);
//		translate_[index].acceleration.y = RandomFloat(
//			targetTranslate.randomRange_acceleration.min.y,
//			targetTranslate.randomRange_acceleration.max.y, 2);
//		translate_[index].acceleration.z = RandomFloat(
//			targetTranslate.randomRange_acceleration.min.z,
//			targetTranslate.randomRange_acceleration.max.z, 2);
//	}
//	else
//	{
//		translate_[index].acceleration = targetTranslate.acceleration;
//	}
//}
//
//void RenderData_Particle::UpdateTransformationMatrix()
//{
//	for (size_t i = 0; i < capacity; ++i)
//	{
//		if (isActive_[i])
//		{
//			instancingData_[i]
//				= Matrix4x4::MakeAffineMatrix(
//					scale_[i].value,
//					rotate_[i].value,
//					translate_[i].value
//				);
//		}
//	}
//}
//
//void RenderData_Particle::UpdateTransforms()
//{
//	Vector3 CameraTranslate = Game::Camera::Getter::GetTranslate("ReleaseCamera");
//
//	for (size_t i = 0; i < currentSum; ++i)
//	{
//		// スケールの更新
//		scale_[i].velocity += scale_[i].acceleration;
//		scale_[i].value += scale_[i].velocity;
//		// 回転の更新
//		if (isBillboard)
//		{
//			Vector3 direction = (CameraTranslate - translate_[i].value).Normalized();
//			float yaw = std::atan2(direction.x, direction.z); // Y軸
//			float pitch = std::asin(-direction.y);            // X軸
//			rotate_[i].value = { pitch, yaw, 0.0f };
//		}
//		else
//		{
//			rotate_[i].velocity += rotate_[i].acceleration;
//			rotate_[i].value += rotate_[i].velocity;
//		}
//		// 位置の更新
//		translate_[i].velocity += translate_[i].acceleration;
//		translate_[i].value += translate_[i].velocity;
//	}
//}
//
//void RenderData_Particle::UpdateLife()
//{
//	for (size_t i = 0; i < currentSum; ++i)
//	{
//		lifeCount_[i]--;
//	}
//}
//
//void RenderData_Particle::CheckLife()
//{
//	for (size_t i = 0; i < currentSum; ++i)
//	{
//		// 寿命チェック
//		if (lifeCount_[i] <= 0)
//		{
//			// 非アクティブ化
//			isActive_[i] = false;
//		}
//		// scaleチェック
//		if (scale_[i].value.x <= 0.0f ||
//			scale_[i].value.y <= 0.0f ||
//			scale_[i].value.z <= 0.0f)
//		{
//			// 非アクティブ化
//			isActive_[i] = false;
//		}
//		// 透明度チェック
//		if ((color & 0x000000FF) == 0)
//		{
//			// 非アクティブ化
//			isActive_[i] = false;
//		}
//	}
//}
//
//void RenderData_Particle::RemoveInactiveParticles()
//{
//	size_t writeIndex = 0;
//	for (size_t readIndex = 0; readIndex < capacity; ++readIndex)
//	{
//		if (isActive_[readIndex])
//		{
//			if (writeIndex != readIndex)
//			{
//				// アクティブなパーティクルを前方に詰める
//				scale_[writeIndex] = scale_[readIndex];
//				rotate_[writeIndex] = rotate_[readIndex];
//				translate_[writeIndex] = translate_[readIndex];
//				lifeCount_[writeIndex] = lifeCount_[readIndex];
//				isActive_[writeIndex] = isActive_[readIndex];
//				// ワールド行列・WVP行列も詰める
//				instancingData_[writeIndex] = instancingData_[readIndex];
//			}
//			writeIndex++;
//		}
//	}
//	for (size_t i = writeIndex; i < capacity; ++i)
//	{
//		isActive_[i] = false;
//	}
//
//
//	currentSum = uint32_t(writeIndex);
//}
//
//
//void RenderData_Particle::Draw()
//{
//	Engine::Instance().AddParticleDrawList(this);
//}
//
//void RenderData_Particle::DrawEmitter()
//{
//	if (emitterShape == PrimitiveType::Sphere)
//	{
//		Game::DebugDraw::AddSphereXYZ(emitterSphereXYZ, 0xFFFFFF22);
//	}
//	else
//	{
//		Game::DebugDraw::AddAABB(emitterAABB, 0xFFFFFF22);
//	}
//}
//
//void RenderData_Particle::DrawImGui()
//{
//	//for (size_t i = 0; i < currentSum; ++i)
//	//{
//	//	std::string num = std::to_string(this->ID) + "." + std::to_string(i);
//	//	if (ImGui::TreeNode(("----------particle" + num + "-----------").c_str()))
//	//	{
//	//		ImGui::DragFloat3((num + "scale").c_str(), &transforms_[i].scale.x, 0.01f);
//	//		ImGui::DragFloat3((num + "rotate").c_str(), &transforms_[i].rotate.x, 0.01f);
//	//		ImGui::DragFloat3((num + "translate").c_str(), &transforms_[i].translate.x, 1.0f);
//	//		ImGui::TreePop();
//	//	}
//	//}
//
//
//	std::optional<std::string> str = "particle : " + std::to_string(this->ID);
//	if (this->name != std::nullopt) str = (this->name);
//
//	std::string num = ":" + std::to_string(this->ID);
//
//	ImGui::Begin(str->c_str());
//
//	ImGui::Text("capacity : %d", static_cast<int>(capacity));
//	ImGui::Text("currentSum : %d", static_cast<int>(currentSum));
//
//	if (ImGui::TreeNode("----------Emitter--------------"))
//	{
//		ImGui::Checkbox(("emitFromInside" + num).c_str(), &this->emitFromInside);
//
//		switch (emitterShape)
//		{
//		case PrimitiveType::Sphere:
//			ImGui::DragFloat3(("emitterSphere.center" + num).c_str(), &this->emitterSphere.center.x, 0.1f);
//			ImGui::DragFloat(("emitterSphere.radius" + num).c_str(), &this->emitterSphere.radius, 0.1f);
//			break;
//		case PrimitiveType::SphereXYZ:
//			ImGui::DragFloat3(("emitterSphereXYZ.center" + num).c_str(), &this->emitterSphereXYZ.center.x, 0.1f);
//			ImGui::DragFloat3(("emitterSphereXYZ.radius" + num).c_str(), &this->emitterSphereXYZ.radius.x, 0.1f);
//			break;
//		case PrimitiveType::AABB:
//			ImGui::DragFloat3(("emitterAABB.min" + num).c_str(), &this->emitterAABB.min.x, 0.1f);
//			ImGui::DragFloat3(("emitterAABB.max" + num).c_str(), &this->emitterAABB.max.x, 0.1f);
//			emitterAABB.Fix();
//			break;
//		case PrimitiveType::Plane:
//			break;
//		case PrimitiveType::Circle:
//			break;
//		default:
//			break;
//		}
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------scale----------------"))
//	{
//		ImGui::Checkbox(("S.isRandom" + num).c_str(), &this->targetScale.isRandom_value);
//		if (this->targetScale.isRandom_value)
//		{
//			ImGui::DragFloat3(("S.min" + num).c_str(), &this->targetScale.randomRange_value.min.x, 0.01f);
//			ImGui::DragFloat3(("S.max" + num).c_str(), &this->targetScale.randomRange_value.max.x, 0.01f);
//		}
//		else
//		{
//			ImGui::DragFloat3(("S.val" + num).c_str(), &this->targetScale.value.x, 0.01f);
//		}
//		ImGui::Checkbox(("S.vel.isRandom" + num).c_str(), &this->targetScale.isRandom_velocity);
//		if (this->targetScale.isRandom_velocity)
//		{
//			ImGui::DragFloat3(("S.vel.min" + num).c_str(), &this->targetScale.randomRange_velocity.min.x, 0.01f);
//			ImGui::DragFloat3(("S.vel.max" + num).c_str(), &this->targetScale.randomRange_velocity.max.x, 0.01f);
//		}
//		else
//		{
//			ImGui::DragFloat3(("S.vel" + num).c_str(), &this->targetScale.velocity.x, 0.01f);
//		}
//		ImGui::Checkbox(("S.acc.isRandom" + num).c_str(), &this->targetScale.isRandom_acceleration);
//		if (this->targetScale.isRandom_acceleration)
//		{
//			ImGui::DragFloat3(("S.acc.min" + num).c_str(), &this->targetScale.randomRange_acceleration.min.x, 0.01f);
//			ImGui::DragFloat3(("S.acc.max" + num).c_str(), &this->targetScale.randomRange_acceleration.max.x, 0.01f);
//		}
//		else
//		{
//			ImGui::DragFloat3(("S.acc" + num).c_str(), &this->targetScale.acceleration.x, 0.01f);
//		}
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------rotate---------------"))
//	{
//		ImGui::Checkbox(("R.isRandom" + num).c_str(), &this->targetRotate.isRandom_value);
//		if (this->targetRotate.isRandom_value)
//		{
//			ImGui::DragFloat3(("R.min" + num).c_str(), &this->targetRotate.randomRange_value.min.x, 0.01f);
//			ImGui::DragFloat3(("R.max" + num).c_str(), &this->targetRotate.randomRange_value.max.x, 0.01f);
//		}
//		else
//		{
//			ImGui::DragFloat3(("R.val" + num).c_str(), &this->targetRotate.value.x, 0.01f);
//		}
//		ImGui::Checkbox(("R.vel.isRandom" + num).c_str(), &this->targetRotate.isRandom_velocity);
//		if (this->targetRotate.isRandom_velocity)
//		{
//			ImGui::DragFloat3(("R.vel.min" + num).c_str(), &this->targetRotate.randomRange_velocity.min.x, 0.01f);
//			ImGui::DragFloat3(("R.vel.max" + num).c_str(), &this->targetRotate.randomRange_velocity.max.x, 0.01f);
//		}
//		else
//		{
//			ImGui::DragFloat3(("R.vel" + num).c_str(), &this->targetRotate.velocity.x, 0.01f);
//		}
//		ImGui::Checkbox(("R.acc.isRandom" + num).c_str(), &this->targetRotate.isRandom_acceleration);
//		if (this->targetRotate.isRandom_acceleration)
//		{
//			ImGui::DragFloat3(("R.acc.min" + num).c_str(), &this->targetRotate.randomRange_acceleration.min.x, 0.01f);
//			ImGui::DragFloat3(("R.acc.max" + num).c_str(), &this->targetRotate.randomRange_acceleration.max.x, 0.01f);
//		}
//		else
//		{
//			ImGui::DragFloat3(("R.acc" + num).c_str(), &this->targetRotate.acceleration.x, 0.01f);
//		}
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------translate------------"))
//	{
//		//ImGui::DragFloat3(("T.val" + num).c_str(), &this->targetTranslate.value.x, 0.01f);
//		ImGui::Checkbox(("T.vel.isRandom" + num).c_str(), &this->targetTranslate.isRandom_velocity);
//		if (this->targetTranslate.isRandom_velocity)
//		{
//			ImGui::DragFloat3(("T.vel.min" + num).c_str(), &this->targetTranslate.randomRange_velocity.min.x, 0.01f);
//			ImGui::DragFloat3(("T.vel.max" + num).c_str(), &this->targetTranslate.randomRange_velocity.max.x, 0.01f);
//		}
//		else
//		{
//			ImGui::DragFloat3(("T.vel" + num).c_str(), &this->targetTranslate.velocity.x, 0.01f);
//		}
//		ImGui::Checkbox(("T.acc.isRandom" + num).c_str(), &this->targetTranslate.isRandom_acceleration);
//		if (this->targetTranslate.isRandom_acceleration)
//		{
//			ImGui::DragFloat3(("T.acc.min" + num).c_str(), &this->targetTranslate.randomRange_acceleration.min.x, 0.01f);
//			ImGui::DragFloat3(("T.acc.max" + num).c_str(), &this->targetTranslate.randomRange_acceleration.max.x, 0.01f);
//		}
//		else
//		{
//			ImGui::DragFloat3(("T.acc" + num).c_str(), &this->targetTranslate.acceleration.x, 0.01f);
//		}
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------target---------------"))
//	{
//		ImGui::Checkbox(("useTarget" + num).c_str(), &this->useTarget);
//		ImGui::Checkbox(("spawnDependent" + num).c_str(), &this->spawnDependent);
//		ImGui::DragFloat3(("target" + num).c_str(), &this->target.x, 0.01f);
//		ImGui::DragFloat(("speed" + num).c_str(), &this->speed, 0.1f);
//		ImGui::DragFloat(("angle" + num).c_str(), &this->spreadAngle, 0.1f);
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------texture--------------"))
//	{
//		size_t textureCount = Game::Resource::GetTextureCount();
//
//		for (size_t i = 0; i < textureCount; ++i)
//		{
//			TextureData* texData = Game::Resource::GetTextureData(static_cast<uint32_t>(i));
//			if (texData)
//			{
//				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));
//
//				// 6個並べたら改行
//				if ((i + 1) % 6 != 0 && i < textureCount - 1)
//				{
//					ImGui::SameLine();
//				}
//				if (ImGui::IsItemClicked())
//				{
//					this->texture = static_cast<uint32_t>(i);
//				}
//			}
//		}
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------model----------------"))
//	{
//		if (ImGui::Button("-"))this->model -= 1;
//
//		ImGui::SameLine();
//
//		// ラベルを非表示にするために "##" プレフィックスで ID を与える
//		std::string dragId = std::string("##model") + num;
//		ImGui::DragInt(dragId.c_str(), reinterpret_cast<int*>(&this->model));
//
//		ImGui::SameLine();
//
//		if (ImGui::Button("+"))this->model += 1;
//
//		// クランプ
//		if (this->model < 0) this->model = 0;
//		if (this->model > int(Game::Resource::GetModelCount() - 1)) this->model = int(Game::Resource::GetModelCount() - 1);
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------density--------------"))
//	{
//		int particlesPerEmission = int(this->particlesPerEmission);
//		ImGui::DragInt(("particlePerEmission" + num).c_str(), &particlesPerEmission);
//		if (particlesPerEmission < 0)particlesPerEmission = 0;
//		this->particlesPerEmission = uint32_t(particlesPerEmission);
//		int emissionDelay = int(this->emissionDelay);
//		ImGui::DragInt(("emissionDelay" + num).c_str(), &emissionDelay);
//		if (emissionDelay < 1)emissionDelay = 1;
//		this->emissionDelay = uint32_t(emissionDelay);
//		ImGui::Text("lifetime");
//		ImGui::DragInt(("liveMax" + num).c_str(), &this->liveMax);
//
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------color----------------"))
//	{
//		Vector4 preColor = ConvertUintToVector4(this->color);
//		float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
//		ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
//		Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
//		this->color = ConvertVector4ToUint(vector4Color);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------option---------------"))
//	{
//		ImGui::Checkbox("Billboard", &this->isBillboard);
//		ImGui::TreePop();
//	}
//	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
//	if (ImGui::TreeNode("----------load & save----------"))
//	{
//		char buf[256];
//		if (this->filePath.size() < sizeof(buf)) memcpy(buf, this->filePath.c_str(), this->filePath.size() + 1);
//		else buf[sizeof(buf) - 1] = '\0';
//		if (ImGui::InputText(".json", buf, sizeof(buf)))
//		{
//			this->filePath = std::string(buf);
//		}
//		if (ImGui::Button("save"))
//		{
//			//JsonManager::SaveToJson(*this, this->filePath);
//		}
//		ImGui::SameLine();
//		if (ImGui::Button("load"))
//		{
//			//JsonManager::LoadFromJson(*this, this->filePath);
//		}
//		ImGui::TreePop();
//	}
//
//	ImGui::End();
//}
//
//#pragma endregion
//
//#pragma region rect
//
//RenderData_Rect::RenderData_Rect()
//{
//	renderRects.push_back(this);
//	this->ID = int(renderRects.size());
//}
//
//RenderData_Rect::~RenderData_Rect()
//{
//	auto it = std::find(renderRects.begin(), renderRects.end(), this);
//	if (it != renderRects.end())
//	{
//		renderRects.erase(it);
//	}
//}
//
//void RenderData_Rect::Draw()
//{
//	Engine::Instance().AddRectDrawList(this);
//}
//
//void RenderData_Rect::DrawImGui()
//{
//	std::optional<std::string> str = "rect : " + std::to_string(this->ID);
//	if (this->name != std::nullopt) str = (this->name);
//	std::string num = std::to_string(this->ID) + ":";
//	ImGui::Begin(str->c_str());
//	if (ImGui::TreeNode("----------transforms-----------"))
//	{
//		ImGui::DragFloat3((num + "scale").c_str(), &transforms.scale.x, 0.01f);
//		ImGui::DragFloat3((num + "translate").c_str(), &transforms.translate.x, 1.0f);
//		ImGui::DragFloat3((num + "rotate").c_str(), &transforms.rotate.x, 0.01f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------uvTransforms---------"))
//	{
//		ImGui::DragFloat2((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
//		ImGui::DragFloat2((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
//		ImGui::DragFloat((num + "UVrotate").c_str(), &uvTransform.rotate.z, 0.01f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------position-------------"))
//	{
//		ImGui::DragFloat3((num + "pos1").c_str(), &pos1.x, 0.1f);
//		ImGui::DragFloat3((num + "pos2").c_str(), &pos2.x, 0.1f);
//		ImGui::DragFloat3((num + "pos3").c_str(), &pos3.x, 0.1f);
//		ImGui::DragFloat3((num + "pos4").c_str(), &pos4.x, 0.1f);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------color----------------"))
//	{
//		Vector4 preColor = ConvertUintToVector4(color);
//		float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
//		ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
//		Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
//		color = ConvertVector4ToUint(vector4Color);
//		ImGui::TreePop();
//	}
//	if (ImGui::TreeNode("----------texture--------------"))
//	{
//		size_t textureCount = Game::Resource::GetTextureCount();
//		for (size_t i =
//			0; i < textureCount; ++i)
//		{
//			TextureData* texData = Game::Resource::GetTextureData(static_cast<uint32_t>(i));
//			if (texData)
//			{
//				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));
//				// 6個並べたら改行
//				if ((i + 1) % 6 != 0 && i < textureCount - 1)
//				{
//					ImGui::SameLine();
//				}
//				if (ImGui::IsItemClicked())
//				{
//					this->texture = static_cast<uint32_t>(i);
//				}
//			}
//		}
//		ImGui::TreePop();
//	}
//	ImGui::End();
//}
//
//#pragma endregion
//
//#pragma region block
//
//RenderData_Block::RenderData_Block()
//{
//	renderBlocks.push_back(this);
//	this->ID = int(renderBlocks.size());
//
//
//	// ===== WorldMatrix =====
//	this->worldMatrixResource_ = Engine::Instance().CreateBufferResource(sizeof(Matrix4x4) * this->capacity);
//	this->worldMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&this->worldMatrixData_));
//	this->worldMatrixSrvAllocation_ =
//		Engine::Instance().GetDirectXManager()->GetDescriptorHeapManager()->GetSrvManager()
//		->CreateSRVforStructuredBuffer(this->worldMatrixResource_.Get(), this->capacity, sizeof(Matrix4x4));
//
//	// ===== Color =====
//	this->colorResource_ = Engine::Instance().CreateBufferResource(sizeof(Vector4) * this->capacity);
//	this->colorResource_->Map(0, nullptr, reinterpret_cast<void**>(&this->colorData_));
//	this->colorSrvAllocation_ =
//		Engine::Instance().GetDirectXManager()->GetDescriptorHeapManager()->GetSrvManager()
//		->CreateSRVforStructuredBuffer(this->colorResource_.Get(), this->capacity, sizeof(Vector4));
//
//	// ===== BaseTile =====
//	this->baseTileResource_ = Engine::Instance().CreateBufferResource(sizeof(uint32_t) * this->capacity);
//	this->baseTileResource_->Map(0, nullptr, reinterpret_cast<void**>(&this->baseTileData_));
//	this->baseTileSrvAllocation_ =
//		Engine::Instance().GetDirectXManager()->GetDescriptorHeapManager()->GetSrvManager()
//		->CreateSRVforStructuredBuffer(this->baseTileResource_.Get(), this->capacity, sizeof(uint32_t));
//
//	// ===== BreakTile =====
//	this->breakTileResource_ = Engine::Instance().CreateBufferResource(sizeof(uint32_t) * this->capacity);
//	this->breakTileResource_->Map(0, nullptr, reinterpret_cast<void**>(&this->breakTileData_));
//	this->breakTileSrvAllocation_ =
//		Engine::Instance().GetDirectXManager()->GetDescriptorHeapManager()->GetSrvManager()
//		->CreateSRVforStructuredBuffer(this->breakTileResource_.Get(), this->capacity, sizeof(uint32_t));
//
//	// 初期化
//	for (uint32_t i = 0; i < this->capacity; ++i)
//	{
//		worldMatrixData_[i] = Matrix4x4::MakeIdentity4x4();
//		colorData_[i] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
//		baseTileData_[i] = 0;
//		breakTileData_[i] = 0;
//	}
//
//	this->scale_.resize(capacity);
//	this->rotate_.resize(capacity);
//	this->translate_.resize(capacity);
//	this->indexes_.resize(capacity);
//	this->isActive_.resize(capacity, false);
//}
//RenderData_Block::~RenderData_Block()
//{
//	auto it = std::find(renderBlocks.begin(), renderBlocks.end(), this);
//	if (it != renderBlocks.end())
//	{
//		renderBlocks.erase(it);
//	}
//}
//
//// ブロックの追加
//uint32_t RenderData_Block::AddNewBlock(const Vector3& worldPos, const Vector3int& localIndex, uint32_t baseTile)
//{
//	uint32_t slot = 0;
//
//	if (!freeSlots_.empty())
//	{
//		slot = freeSlots_.back();
//		freeSlots_.pop_back();
//	}
//	else
//	{
//		if (currentDrawSum >= capacity)
//		{
//			Log("RenderData_Block::AddNewBlock キャパオーバー");
//			return UINT32_MAX;
//		}
//
//		slot = currentDrawSum;
//		currentDrawSum++;
//	}
//
//	isActive_[slot] = true;
//	indexes_[slot] = localIndex;
//
//	// 拡縮量の初期化
//	scale_[slot].value = Vector3(1.0f, 1.0f, 1.0f);
//	scale_[slot].velocity = Vector3(0.0f, 0.0f, 0.0f);
//	scale_[slot].acceleration = Vector3(0.0f, 0.0f, 0.0f);
//
//	// 回転量の初期化
//	rotate_[slot].value = Vector3(0.0f, 0.0f, 0.0f);
//	rotate_[slot].velocity = Vector3(0.0f, 0.0f, 0.0f);
//	rotate_[slot].acceleration = Vector3(0.0f, 0.0f, 0.0f);
//
//	// 座標の初期化
//	translate_[slot].value = worldPos;
//	translate_[slot].velocity = Vector3(0.0f, 0.0f, 0.0f);
//	translate_[slot].acceleration = Vector3(0.0f, 0.0f, 0.0f);
//
//	// ワールド行列の初期化
//	worldMatrixData_[slot] = Matrix4x4::MakeAffineMatrix(
//		scale_[slot].value,
//		rotate_[slot].value,
//		translate_[slot].value);
//
//	// 色の初期化
//	colorData_[slot] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
//
//	// テクスチャの初期化
//	baseTileData_[slot] = baseTile;
//	breakTileData_[slot] = 0;
//
//	return slot;
//}
//
//void RenderData_Block::RemoveBlock(uint32_t slot)
//{
//	if (slot >= capacity) return;
//	if (!isActive_[slot]) return;
//
//	isActive_[slot] = false;
//
//	// 見えない場所に飛ばす
//	worldMatrixData_[slot] = Matrix4x4::MakeTranslateMatrix(Vector3(0.0f, -1000000.0f, 0.0f));
//
//	// さらに透明化も行う徹底ぶり
//	colorData_[slot] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
//
//	// アトラスタイル初期化
//	baseTileData_[slot] = 0;
//	breakTileData_[slot] = 0;
//
//	freeSlots_.push_back(slot);
//}
//
//void RenderData_Block::SetColor(uint32_t slot, const Vector4 & color)
//{
//	if (slot >= capacity) return;
//	if (!isActive_[slot]) return;
//
//	colorData_[slot] = color;
//}
//
//void RenderData_Block::SetBreakTile(uint32_t slot, uint32_t breakTile)
//{
//	if (slot >= capacity) return;
//	if (!isActive_[slot]) return;
//
//	breakTileData_[slot] = breakTile;
//}
//
//void RenderData_Block::Draw()
//{
//	Engine::Instance().AddBlockDrawList(this);
//}
//
//void RenderData_Block::DrawImGui()
//{
////	std::string str = EnumToString(this->name);
////
////	std::string num = ":" + std::to_string(this->ID);
////
////	ImGui::Begin(str.c_str());
////
////	ImGui::Text("capacity : %d", static_cast<int>(capacity));
////	ImGui::Text("currentSum : %d", static_cast<int>(currentSum));
////
////	if (ImGui::TreeNode("----------texture--------------"))
////	{
////		size_t textureCount = Game::Resource::GetTextureCount();
////
////		for (size_t i = 0; i < textureCount; ++i)
////		{
////			TextureData* texData = Game::Resource::GetTextureData(static_cast<uint32_t>(i));
////			if (texData)
////			{
////				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));
////
////				// 6個並べたら改行
////				if ((i + 1) % 6 != 0 && i < textureCount - 1)
////				{
////					ImGui::SameLine();
////				}
////				if (ImGui::IsItemClicked())
////				{
////					this->texture = static_cast<uint32_t>(i);
////				}
////			}
////		}
////		ImGui::TreePop();
////	}
////	if (ImGui::TreeNode("----------model----------------"))
////	{
////		if (ImGui::Button("-"))this->model -= 1;
////
////		ImGui::SameLine();
////
////		// ラベルを非表示にするために "##" プレフィックスで ID を与える
////		std::string dragId = std::string("##model") + num;
////		ImGui::DragInt(dragId.c_str(), reinterpret_cast<int*>(&this->model));
////
////		ImGui::SameLine();
////
////		if (ImGui::Button("+"))this->model += 1;
////
////		// クランプ
////		if (this->model < 0) this->model = 0;
////		if (this->model > int(Game::Resource::GetModelCount() - 1)) this->model = int(Game::Resource::GetModelCount() - 1);
////
////		ImGui::TreePop();
////	}
////	if (ImGui::TreeNode("----------color----------------"))
////	{
////		//Vector4 preColor = ConvertUintToVector4(this->color);
////		//float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
////		//ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
////		//Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
////		//this->color = ConvertVector4ToUint(vector4Color);
////		//ImGui::TreePop();
////	}
////	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
////
////	ImGui::End();
//}
//
//#pragma endregion