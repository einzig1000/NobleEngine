//#pragma once
//#include "definition/definition.h"
//#include <optional>
//
//class Block;
//
//class RenderData_Model
//{
//public:
//    RenderData_Model();
//    ~RenderData_Model();
//    // 全オブジェクトのSRT更新,それに伴うワールド行列更新
//    void Update1();
//    // 全オブジェクトの親を考慮したワールド行列更新,AABB更新
//    void Update2();
//    // 衝突判定,衝突ペア・深度の保存
//    void Update3();
//    // 衝突時の更新,それに伴うワールド行列更新
//    void Update4();
//    // 全オブジェクトの描画範囲内判定,前フレーム情報保存
//    void Update5();
//
//	// Update〇では分かり難いので、以下で新規命名
//
//	// SRTにVelocity, Accelerationを反映させる
//	void UpdateTransformsPhysics();
//    // ↳acceleration→velocity の積分だけ行う
//    void UpdateVelocitiesPhysics();
//    // ↳位置(translate.value) に移動量(delta)を適用する
//    void ApplyTranslationDelta(const Vector3& delta);
//	// 現状のSRTからローカルマトリックスを作成する
//	void UpdateLocalMatrix();
//	// 現状のSRTからワールドマトリックスを作成する
//	void UpdateWorldMatrix();
//	// 現状のSRTからAABBを作成する
//	void UpdateAABB();
//	// 現状のAABBから描画範囲内判定を行う
//	void UpdateInPicture();
//	// 現状のSRTを前フレームSRTとして保存する
//	void SavePreTransforms();
//
//    // モデル設定
//	void SetModel(int32_t modelHandle);
//	int32_t GetModel() const { return model; }
//	// テクスチャ
//	void SetTexture(int32_t textureHandle);
//	int32_t GetTexture() const { return texture; }
//
//
//    std::optional<std::string> name;
//    VectorDynamics scale = { Vector3(1.0f,1.0f,1.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f) };
//    VectorDynamics rotate = { Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f) };
//    VectorDynamics translate = { Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f) };
//    // 親のワールドマトリックス
//    RenderData_Model* parentModel = nullptr;
//    // 今フレームの移動量
//    Vector3 lastMove;
//    // UV座標
//    EulerTransforms uvTransform;
//    // 色
//    Vector4 color = { 0xFF, 0xFF, 0xFF, 0xFF };
//    // 描画オプション
//    DrawOptions options;
//    // 衝突判定用AABB
//    std::vector<AABB> aabbs;
//    // 重さ
//    float mass = 1.0f;
//    // ファイルパス
//    std::string filePath = "resources/Prototypes/model_json/aaa";
//    // 画面内に存在するか
//    bool inPicture = false;
//    // マウスとの衝突判定
//    int isCollisionMouseRay = -1; // -1:非衝突, 0:最初に衝突, 1:2番目
//
//
//    // 他のオブジェクトとの衝突判定
//    bool isCollision(RenderData_Model* target) const;
//    // 衝突時すりぬけないオブジェクトの設定
//    void SetBlock(RenderData_Model* target);
//
//    // 任意のポイントを向く
//    void LookAtOnce(const Vector3& targetWorldPos, float roll = 0);
//    void LookAtOnce(const RenderData_Model* other, float roll = 0);
//    void LookAtCamera(float roll = 0);
//    void LookAtFront(float roll = 0);
//
//    // ワールド位置を返す
//    Matrix4x4 GetWorldMatrix() const;
//    Vector3 GetWorldPosition() const;
//
//    // 描画
//    void Draw();
//    void DrawAABB();
//    void DrawImGui();
//
//    
//    // マウスと衝突判定とるか否か
//	bool isCheckMouseRay = false;
//
//    static std::vector<RenderData_Model*> renderModels;
//
//private:
//    // 3Dモデル
//    int32_t model = -1;
//    // テクスチャ
//    int32_t texture = -1;
//    // 今フレームでS/R/Tに変化があったか
//    bool movedThisFrame = true;
//
//    // 衝突ペアを全部保存
//    static std::vector<CollisionInf*> collisionInfos;
//
//    // 親を考慮しないワールドマトリックス
//    Matrix4x4 localWorldMatrix;
//    // 親を考慮したワールドマトリックス
//    Matrix4x4 worldMatrix;
//    Vector3 worldPos;
//
//    // 親を考慮したワールドマトリックスを設定する
//    Matrix4x4 SetWorldMatrix();
//
//    // ID
//    int ID = 0;
//
//
//    // 初期化済みフラグ
//    bool initialized = false;
//
//    // 衝突フラグ
//    unsigned int CollisionFlags = 0x00000000;
//    // 前フレームの衝突フラグ
//    unsigned int preCollisionFlags = 0x00000000;
//    // 衝突したときの反発係数
//    const float restitution = 0.0f;
//
//    // 前フレーム位置、回転、スケール、ワールド座標
//    VectorDynamics preScale;
//    VectorDynamics preRotate;
//    VectorDynamics preTranslate;
//    Vector3 preWorldPos;
//    // 前フレームAABB
//    std::vector<AABB> preAABB;
//
//    // 他のオブジェクトと衝突したときのAABBのインデックスペア
//    std::optional<CollisionInf> isCollisionAABBInf(RenderData_Model& target) const;
//
//    std::vector<RenderData_Model*> blockList;
//};
//
//class RenderData_Sprite
//{
//public:
//    RenderData_Sprite();
//    ~RenderData_Sprite();
//
//    // 位置、回転、スケール
//    EulerTransforms transforms;
//    // UV座標
//    EulerTransforms uvTransform;
//    // アンカー
//    Anchor anchor = Anchor::Center;
//    // 親のワールドマトリックス
//    TransformationMatrix parentTransformationMatrix;
//    // 回転の中心点
//    Vector2 pivot = { 0,0 };
//    // 色
//    uint32_t color = 0xFFFFFFFF;
//    // テクスチャ
//    uint32_t texture = -1;
//    // 描画オプション
//    DrawOptions options;
//    // 画像切り取り左上
//    Vector2int cutImageLeftTop = { 0,0 };
//    // 切り出しサイズ
//    Vector2int cutImageSize = { 0,0 };
//
//    // マウスと衝突してるか？
//    bool isCollisionMouseRay = false;
//    // ID
//    int ID = 0;
//    std::optional<std::string> name;
//
//    void Draw();
//    void DrawImGui();
//
//private:
//
//    static std::vector<RenderData_Sprite*> renderSprites;
//
//};
//
//class RenderData_Triangle
//{
//public:
//    RenderData_Triangle();
//    ~RenderData_Triangle();
//
//    // 位置、回転、スケール
//    EulerTransforms transforms;
//    // UV座標
//    EulerTransforms uvTransform;
//    // 上
//    Vector3 pos1 = { 0.0f,  0.5f, 0.0f };
//    // 右下
//    Vector3 pos2 = { 0.5f, -0.5f, 0.0f };
//    // 左下
//    Vector3 pos3 = { -0.5f, -0.5f, 0.0f };
//    // テクスチャ
//    uint32_t texture = 0;
//    // 色
//    uint32_t color = 0xFFFFFFFF;
//    // 描画オプション
//    DrawOptions options;
//    // ID
//    int ID = 0;
//    std::optional<std::string> name;
//
//    void Draw();
//    void DrawImGui();
//
//private:
//
//    static std::vector<RenderData_Triangle*> renderTriangles;
//
//};
//
//class RenderData_Rect
//{
//public:
//    RenderData_Rect();
//    ~RenderData_Rect();
//
//    // 位置、回転、スケール
//    EulerTransforms transforms;
//    // UV座標
//    EulerTransforms uvTransform;
//
//    // 右上
//    Vector3 pos1;
//    // 右下
//    Vector3 pos2;
//    // 左上
//    Vector3 pos3;
//    // 左下
//    Vector3 pos4;
//
//    // テクスチャ
//    uint32_t texture = 0;
//    // 色
//    uint32_t color = 0xFFFFFFFF;
//    // 描画オプション
//    DrawOptions options;
//    // ID
//    int ID = 0;
//    std::optional<std::string> name;
//
//    void Draw();
//    void DrawImGui();
//
//private:
//
//    static std::vector<RenderData_Rect*> renderRects;
//};
//
//class RenderData_Line
//{
//public:
//
//    RenderData_Line();
//    ~RenderData_Line();
//
//    // 点([0]は使用負荷)
//    std::vector<Vector3> points;
//    // ID
//    int ID = 0;
//    std::optional<std::string> name;
//    // 色
//    uint32_t color = 0xFFFFFFFF;
//    // ラインタイプ
//    LineType lineType = LineType::Line;
//    // 補完分割数(線形補完時のみ有効)
//    uint32_t kSubdivision = 10;
//
//    void Draw();
//    void DrawPoints();
//    void DrawImGui();
//
//private:
//
//    static std::vector<RenderData_Line*> renderLines;
//};
//
//class RenderData_Particle
//{
//public:
//
//    RenderData_Particle();
//    ~RenderData_Particle();
//    static void UpdateAllParticles();
//    void Update();
//
//    // ID
//    std::optional<std::string> name;
//
//    // ファイルパス
//    std::string filePath = "resources/Prototypes/particle/aaa";
//
//    bool LoadJson();
//
//    void Draw();
//    void DrawImGui();
//    void DrawEmitter();
//
//
//    /// リソース
//    uint32_t model = 0;
//    uint32_t texture = 0;
//
//    /// エミッター
//    PrimitiveType emitterShape = PrimitiveType::AABB;
//    Sphere emitterSphere = { Vector3{0.0f,0.0f,0.0f}, 1.0f };
//    SphereXYZ emitterSphereXYZ = { Vector3{0.0f,0.0f,0.0f}, Vector3{1.0f,1.0f,1.0f} };
//    AABB emitterAABB = { Vector3{ -10.0f, -10.0f, -10.0f }, Vector3{ 10.0f, 10.0f, 10.0f } };
//    bool emitFromInside = true;     // 内側から出るか外殻上から出るか
//
//    /// 密度
//    int32_t particlesPerEmission = 1;   // 1フレで生む数
//    int32_t emissionDelay = 1;          // 生成間隔フレーム
//    int32_t liveMax = 300;              // 寿命フレーム(マイナスの時は不老)
//
//    /// 方向
//    bool useTarget = false;             // ターゲット方向に飛ばすかどうか
//    bool spawnDependent = false;        // 発生位置に依存した方向に飛ばすかどうか
//    Vector3 target;
//    float speed = 1.0f;                 // 速度
//    float spreadAngle = 0.0f;           // 拡散角度
//
//    /// オプション
//    bool isBillboard = true;    // ビルボードかどうか
//	BlendMode blendMode = BlendMode::kBlendModeAdd;
//
//    /// SRT
//    ParticleSRT targetScale;
//    ParticleSRT targetRotate;
//    ParticleSRT targetTranslate;
//
//    /// マテリアル
//    uint32_t color = 0xFFFFFFFF;
//    Matrix4x4 uvTransform;
//
//
//    uint32_t GetCurrentSum() const { return currentSum; }
//
//
//
//	uint32_t capacity = 1024;
//	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
//    Matrix4x4* instancingData_ = nullptr;
//	SRVAllocation srvAllocation_;
//
//	std::vector<VectorDynamics> scale_;
//	std::vector<VectorDynamics> rotate_;
//	std::vector<VectorDynamics> translate_;
//    std::vector<int32_t> lifeCount_;
//	std::vector<bool> isActive_;
//
//private:
//    /// 現在存在するパーティクル数
//    uint32_t currentSum = 0;
//
//    int ID = 0;
//
//    uint32_t frame = 0;                 // 経過フレーム
//
//	//// パーティクル生成
//    void SpawnParticle();
//    // 生まれる場所設定
//	void SetSpawnPosition(uint32_t index);
//	// SRTの設定
//	void SetSpawnScale(uint32_t index);
//	void SetSpawnRotate(uint32_t index);
//	void SetSpawnTranslate(uint32_t index);
//
//    //// ワールド行列・WVP行列の更新
//	void UpdateTransformationMatrix();
//	//// 各パーティクルの変換行列更新
//	void UpdateTransforms();
//
//	//// 寿命管理
//	void UpdateLife();
//
//	//// 死亡判定
//	void CheckLife();
//
//	//// 非アクティブかしたパーティクルの削除
//	void RemoveInactiveParticles();
//
//    // ロードした結果
//    bool loadResult = false;
//
//    static std::vector<RenderData_Particle*> renderParticles;
//};
//
///// 新しいブロックを作るときはAddNewBlock()
///// ブロックを壊すときはRemoveBlockFromList()
//class RenderData_Block
//{
//public:
//    RenderData_Block();
//    ~RenderData_Block();
//    static void UpdateAllBlock();
//    void Update();
//
//
//    /// <summary>
//	/// リストに新たなブロックを追加する
//    /// </summary>
//    /// <param name="worldPos"> 世界基準の座標 </param>
//    /// <param name="localIndex"> チャンク内のローカルインデックス </param>
//    /// <param name="baseTile"> blockID->アトラスのタイル の関数をアプリケーション層で別途用意する </param>
//    /// <returns>slot(RemoveBlockで必要)</returns>
//    uint32_t AddNewBlock(const Vector3& worldPos, const Vector3int& localIndex, uint32_t baseTile);
//
//    /// <summary>
//	/// リストからブロックを削除する
//    /// </summary>
//    /// <param name="slot"> AddNewBlockの返り値 </param>
//    void RemoveBlock(uint32_t slot);
//
//    /// <summary>
//	/// ブロックの色を変える
//    /// </summary>
//	/// <param name="slot"> AddNewBlockの返り値 </param>
//	/// <param name="color"> 変えたい色 </param>
//    void SetColor(uint32_t slot, const Vector4& color);
//
//    /// <summary>
//	/// ブロックの破壊レイヤーを変える
//    /// </summary>
//	/// <param name="slot"> AddNewBlockの返り値 </param>
//	/// <param name="breakTile"> 破壊レベル->アトラスのタイル の関数をアプリケーション層で別途用意する </param>
//    void SetBreakTile(uint32_t slot, uint32_t breakTile);
//
//    void Draw();
//    void DrawImGui();
//
//
//    /// リソース
//    uint32_t model = 0;
//    uint32_t texture = 0;
//	uint32_t breakTexture = 0;
//
//	uint32_t capacity = CHUNK_X * CHUNK_Y * CHUNK_Z;    // 最大ブロック数
//	uint32_t currentSum = 0;        // チャンク内に存在するブロック数
//	uint32_t currentDrawSum = 0;    // チャンク内の描画されているブロック数
//
//    // GPUに渡すブロックごとに管理したいデータ（いずれ一つにまとめる）
//    Microsoft::WRL::ComPtr<ID3D12Resource> worldMatrixResource_;
//    Matrix4x4* worldMatrixData_ = nullptr;
//    SRVAllocation worldMatrixSrvAllocation_;
//
//    Microsoft::WRL::ComPtr<ID3D12Resource> colorResource_;
//    Vector4* colorData_ = nullptr;
//    SRVAllocation colorSrvAllocation_;
//
//    Microsoft::WRL::ComPtr<ID3D12Resource> baseTileResource_;
//    uint32_t* baseTileData_ = nullptr;
//    SRVAllocation baseTileSrvAllocation_;
//
//    Microsoft::WRL::ComPtr<ID3D12Resource> breakTileResource_;
//    uint32_t* breakTileData_ = nullptr;
//    SRVAllocation breakTileSrvAllocation_;
//
//	// CPU側で管理するブロックごとに管理したいデータ
//    std::vector<VectorDynamics> scale_;
//    std::vector<VectorDynamics> rotate_;
//    std::vector<VectorDynamics> translate_;
//	std::vector<Vector3int> indexes_;
//	std::vector<bool> isActive_;
//
//private:
//	// 空きスロット管理
//    std::vector<uint32_t> freeSlots_;
//
//    int ID = 0;
//
//	static std::vector<RenderData_Block*> renderBlocks;
//};