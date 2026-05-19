#pragma once
#include "definition/definition.h"

class IWorldCollider;
class IPhysicsBody;

/// <summary>
/// ファサードクラス
/// </summary>
namespace Game
{
	namespace Resource
	{
		/// <summary>
		/// モデル読み込み
		/// </summary>
		/// <param name="directoryPath">例:"Resources/Prototypes/model/"</param>
		/// <param name="filename">"cube.obj"</param>
		/// <returns>モデルID</returns>
		uint32_t LoadModel(const std::string& filePath);

		/// <summary>
		/// テクスチャ読み込み
		/// </summary>
		/// <param name="filePath">例:"Resources/Prototypes/texture/uvChecker.png"</param>
		/// <returns>テクスチャID</returns>
		uint32_t LoadTexture(const std::string& filePath);

		/// <summary>
		/// オーディオ読み込み
		/// </summary>
		/// <param name="filePath">例:"Resources/Prototypes/audio/BGM/InGame.mp3"</param>
		/// <returns></returns>
		uint32_t LoadAudio(const std::string& filePath);

		/// <summary>
		/// テクスチャデータ取得
		/// </summary>
		/// <param name="textureID">テクスチャID</param>
		/// <returns>メタデータを含むテクスチャデータ</returns>
		TextureData* GetTextureData(uint32_t textureID);

		/// <summary>
		/// モデルデータ取得
		/// </summary>
		/// <param name="modelID">モデルID</param>
		/// <returns>モデルデータ</returns>
		ModelData* GetModelData(uint32_t modelID);

		/// <summary>
		/// オーディオデータ取得
		/// </summary>
		/// <param name="audioID">オーディオID</param>
		/// <returns>オーディオデータ</returns>
		AudioData* GetAudioData(uint32_t audioID);

		/// <summary>
		/// 読み込んだテクスチャ数取得
		/// </summary>
		/// <returns>読み込んだテクスチャ数</returns>
		size_t GetTextureCount();

		/// <summary>
		/// 読み込んだモデル数取得
		/// </summary>
		/// <returns>読み込んだモデル数</returns>
		size_t GetModelCount();

		/// <summary>
		/// 読み込んだオーディオ数取得
		/// </summary>
		/// <returns>読み込んだオーディオ数</returns>
		size_t GetAudioCount();
	};

	namespace DebugDraw
	{
		/// <summary>
		/// ワイヤーフレーム球描画
		/// </summary>
		/// <param name="sphere">球</param>
		/// <param name="color">色</param>
		void AddSphere(const Sphere& sphere, uint32_t color);

		/// <summary>
		/// ワイヤーフレーム楕円球描画
		/// </summary>
		/// <param name="sphere">楕円球</param>
		/// <param name="color">色</param>
		void AddSphereXYZ(const SphereXYZ& sphere, uint32_t color);

		/// <summary>
		/// ワイヤーフレーム円柱描画
		/// </summary>
		/// <param name="cylinder">円柱</param>
		/// <param name="color">色</param>
		void AddCylinder(const Cylinder& cylinder, uint32_t color);

		/// <summary>
		/// ワイヤーフレームAABB描画
		/// </summary>
		/// <param name="aabb">AABB</param>
		/// <param name="color">色</param>
		void AddAABB(const AABB& aabb, uint32_t color);

		/// <summary>
		/// ワイヤーフレーム線分描画
		/// </summary>
		/// <param name="start">線分開始座標</param>
		/// <param name="end">線分終了座標</param>
		/// <param name="color">色</param>
		void AddLine(Vector3 start, Vector3 end, uint32_t color);


	};

	namespace Audio
	{
		/// <summary>
		/// オーディオ再生
		/// </summary>
		/// <param name="audioId">Resource::LoadAudioで取得したオーディオID</param>
		/// <param name="loop">trueなら自動ループ</param>
		void PlayAudio(const uint32_t& audioId, bool loop);

		/// <summary>
		/// オーディオ停止
		/// </summary>
		/// <param name="audioId">Resource::LoadAudioで取得したオーディオID</param>
		void StopAudio(const uint32_t& audioId);

		/// <summary>
		/// オーディオボリューム設定
		/// </summary>
		/// <param name="audioId"Resource::LoadAudioで取得したオーディオID></param>
		/// <param name="volume">0.0f～1.0f</param>
		void SetAudioVolume(const uint32_t& audioId, float volume);

		/// <summary>
		/// マスターボリューム設定
		/// </summary>
		/// <param name="volume">0.0f～1.0f</param>
		void SetMasterVolume(float volume);

		/// <summary>
		/// オーディオボリューム取得
		/// </summary>
		/// <param name="audioId">Resource::LoadAudioで取得したオーディオID</param>
		/// <returns>現在の音量</returns>
		float GetVolume(const uint32_t& audioId);

		/// <summary>
		/// マスターボリューム取得
		/// </summary>
		/// <returns>現在のマスター音量</returns>
		float GetMasterVolume();

		/// <summary>
		/// オーディオが再生中か？
		/// </summary>
		/// <param name="audioId">Resource::LoadAudioで取得したオーディオID</param>
		/// <returns>bool </returns>
		bool IsAudioPlaying(const uint32_t& audioId);
	};

	namespace Light
	{
		/// <summary>
		/// 共通ライトカラー設定
		/// </summary>
		/// <param name="color">Vector4のcolor</param>
		void SetLightColor(const Vector4 color);

		/// <summary>
		/// 共通ライト方向設定
		/// </summary>
		/// <param name="direction">Vector3の方向ベクトル</param>
		void SetLightDirection(const Vector3 direction);
		void ToggleLightMode(const LightMode mode);
		void SetLightIntensity(float intensity);
	};

	namespace IO
	{
		namespace Mouse
		{
			/// <summary>
			/// マウスのスクリーン座標位置取得
			/// </summary>
			/// <returns>マウスのスクリーン座標</returns>
			Vector2 GetPosition();

			/// <summary>
			/// マウスのスクリーン座標位置変化量取得
			/// </summary>
			/// <returns>マウスのスクリーン座標位置変化量</returns>
			Vector2 GetPositionDelta();

			/// <summary>
			/// マウスのワールド座標位置取得
			/// </summary>
			/// <returns>マウスのワールド座標</returns>
			Vector3 GetWorldPosition();

			/// <summary>
			/// マウスのワールド座標レイ取得
			/// </summary>
			/// <returns></returns>
			Ray GetRay();

			/// <summary>
			/// マウスホイールの回転量取得
			/// </summary>
			/// <returns>マウスホイールの回転量</returns>
			int32_t GetWheel();

			/// <summary>
			/// マウスボタンの入力取得(現在押されているか)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>現在押されているか</returns>
			bool IsHeld(int i);

			/// <summary>
			/// マウスボタンの入力取得(押した瞬間)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>押した瞬間か</returns>
			bool IsJustPressed(int i);

			/// <summary>
			/// マウスボタンの入力取得(離した瞬間)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>離した瞬間か</returns>
			bool IsJustReleased(int i);

			/// <summary>
			/// マウスボタンの入力取得(押されてからの経過フレーム数)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>押されてからの経過フレーム数</returns>
			uint32_t HoldFrames(int i);

			// カーソルの表示・非表示切り替え
			void ToggleMouseCursorVisible();

			// カーソルの表示・非表示設定
			void ShowCursor(bool visible);

			// マウス感度設定
			void SetMouseSensitivity(float sensitivity);
		};

		namespace Key
		{
			/// <summary>
			/// キーボードの入力取得(現在押されているか)
			/// </summary>
			bool IsHeld(BYTE key);

			/// <summary>
			/// キーボードの入力取得(押した瞬間)
			/// </summary>
			bool IsJustPressed(BYTE key);

			/// <summary>
			/// キーボードの入力取得(離した瞬間)
			/// </summary>
			bool IsJustReleased(BYTE key);

			/// <summary>
			/// キーボードの入力取得(押されてからの経過フレーム数)
			/// </summary>
			uint32_t HoldFrames(BYTE key);

			/// <summary>
			/// キーの単押し・長押し判定
			/// </summary>
			/// <param name="n">nフレーム以上で長押しと判定</param>
			/// <param name="key">キー</param>
			/// <returns>0: なし  1:単押し  2:長押し</returns>
			int TestTapLong(int n, BYTE key);
		};

		namespace Pad
		{
			/// <summary>
			/// ゲームパッドの入力取得(現在押されているか)
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="button"> ボタン番号(PAD_A～) </param>
			bool IsHeld(int padIndex, BYTE button);

			/// <summary>
			/// ゲームパッドの入力取得(押した瞬間)
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="button"> ボタン番号(PAD_A～) </param>
			bool IsJustPressed(int padIndex, BYTE button);

			/// <summary>
			/// ゲームパッドの入力取得(離した瞬間)
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="button"> ボタン番号(PAD_A～) </param>
			bool IsJustReleased(int padIndex, BYTE button);

			/// <summary>
			/// ゲームパッドの入力取得(押されてからの経過フレーム数)
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="button"> ボタン番号(PAD_A～) </param>
			uint32_t HoldFrames(int padIndex, BYTE button);

			/// <summary>
			/// 左スティックの入力取得
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <returns> -1.0f ～ 1.0f </returns>
			Vector2 GetLeftStick(int padIndex);

			/// <summary>
			/// 右スティックの入力取得
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <returns> -1.0f ～ 1.0f </returns>
			Vector2 GetRightStick(int padIndex);

			/// <summary>
			/// 左トリガーの入力取得
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <returns> 0.0f ～ 1.0f </returns>
			float GetLeftTrigger(int padIndex);

			/// <summary>
			/// 右トリガーの入力取得
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <returns> 0.0f ～ 1.0f </returns>
			float GetRightTrigger(int padIndex);

			/// <summary>
			/// ゲームパッド振動設定
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="leftMotor"> 左モーター強度(0.0f ～ 1.0f) </param>
			/// <param name="rightMotor"> 右モーター強度(0.0f ～ 1.0f) </param>
			void SetVibration(int padIndex, float leftMotor, float rightMotor);

			/// <summary>
			/// 接続されているゲームパッドの数取得
			/// </summary>
			int32_t GetConnectedPadNum();
		};
	};

	namespace Camera
	{
		namespace Getter
		{
			/// <returns>カメラが向いてる座標</returns>
			Vector3 GetCurrentCenter();
			/// <returns>カメラの球面座標上位置座標</returns>
			Vector3 GetCurrentTranslate();
			/// <returns>カメラの回転量</returns>
			Vector3 GetCurrentRotate();
			/// <returns>カメラのCenterまでの距離</returns>
			float GetCurrentDistance();
			/// <returns>カメラのビュープロジェクション行列</returns>
			Matrix4x4 GetCurrentViewProjectionMatrix();
			/// <returns>カメラのビュー行列</returns>
			Matrix4x4 GetCurrentViewMatrix();
			/// <returns>カメラのプロジェクション行列</returns>
			Matrix4x4 GetCurrentProjectionMatrix();
		};

		/// <summary>
		/// カメラの回転中心座標の移動
		/// </summary>
		/// <param name="target">目標座標</param>
		/// <param name="spendFrame">移動にかけるフレーム数</param>
		/// <param name="easeType">移動補完イージングタイプ</param>
		void MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype);

		/// <summary>
		///	カメラの回転量変更
		/// </summary>
		/// <param name="target">目標回転量</param>
		/// <param name="spendFrame">変更にかけるフレーム数</param>
		/// <param name="easeType">変更補完イージングタイプ</param>
		void MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype);

		/// <summary>
		///  カメラの回転中心からの距離(ズーム量)変更
		/// </summary>
		/// <param name="target">目標ズーム量</param>
		/// <param name="spendFrame">変更にかけるフレーム数</param>
		/// <param name="easeType">変更補完イージングタイプ</param>
		void MoveCameraDistance(float target, int spendFrame, EaseType easetype);


		/// <summary>
		/// カメラシェイク開始関数
		/// </summary>
		/// <param name="intensity">シェイクの最大振幅</param>
		/// <param name="duration">シェイクの継続時間</param>
		/// <param name="frequency">振動の速度 既定値は25.0f</param>
		void StartCameraShake(float intensity, float duration, float frequency = 25.0f);

		/// <summary>
		/// 今シェイクしているか
		/// </summary>
		/// <returns>今シェイクしているか</returns>
		bool IsShaking();

		/// <summary>
		/// シェイク停止関数
		/// </summary>
		void StopShake();


		/// <summary>
		/// 描画範囲内にAABBがあるか
		/// </summary>
		/// <param name="aabb">検索対象のAABB</param>
		/// <returns>結果</returns>
		bool InCamera(const AABB& aabb);


		/// <summary>
		/// カメラモード(オービット or FPS)切り替え
		/// </summary>
		/// <param name="mode">カメラモード</param>
		void SetCameraMode(CameraMode_ORBIT_FPS mode);

		/// <summary>
		/// カメラコントロールの有効無効設定
		/// </summary>
		void SetEnableControl(bool enable);

		/// <summary>
		/// カメラ切り替え
		/// </summary>
		void ToggleCamera(const std::string name);

		/// <summary>
		/// 順番にカメラ切り替え
		/// </summary>
		void ToggleCamera();
	};

	namespace Utilities
	{

	};

	namespace Math
	{
		namespace Ease
		{
			/// <summary>
			/// イージング float
			/// </summary>
			/// <param name="start"> 初期値 </param>
			/// <param name="end"> 終了値 </param>
			/// <param name="easeType"> イージングタイプ </param>
			/// <param name="t"> 0.0f～1.0f の補完値 </param>
			/// <returns> イージング後の値 </returns>
			float EasingFloat(float start, float end, EaseType easeType, float t);

			/// <summary>
			/// イージング Vector3
			/// </summary>
			/// <param name="start"> 初期値 </param>
			/// <param name="end"> 終了値 </param>
			/// <param name="easeType"> イージングタイプ </param>
			/// <param name="t"> 0.0f～1.0f の補完値 </param>
			/// <returns> イージング後の値 </returns>
			Vector3 EasingVector3(Vector3 start, Vector3 end, EaseType easeType, float t);
		}

		namespace Rand
		{
			/// <summary>
			/// 指定範囲の小数点付き乱数取得
			/// </summary>
			/// <param name="min"> 最小値 </param>
			/// <param name="max"> 最大値 </param>
			/// <param name="decimalPlaces"> 小数点以下の桁数 </param>
			/// <returns> 乱数 </returns>
			float RandFloat(float min, float max, int decimalPlaces);

			/// <summary>
			/// 指定範囲の整数乱数取得
			/// </summary>
			/// <param name="min"> 最小値 </param>
			/// <param name="max"> 最大値 </param>
			/// <returns> 乱数 </returns>
			int RandInt(int min, int max);
		}


		namespace Converter
		{
			/// <summary>
			/// 度数法を弧度法に変換
			/// </summary>
			float DegreeToRadian(float degree);

			/// <summary>
			/// 弧度法を度数法に変換
			/// </summary>
			float RadianToDegree(float radian);

			/// <summary>
			/// uint32_tをVector4(0.0f～1.0f)に変換
			/// </summary>
			/// <param name="color"> RGBA </param>
			/// <returns> Vector4 </returns>
			Vector4 UintToVector4(uint32_t color);

			/// <summary>
			/// Vector4(0.0f～1.0f)をuint32_tに変換
			/// </summary>
			/// <param name="color"> Vector4 </param>
			/// <returns> RGBA </returns>
			uint32_t Vector4ToUint(Vector4 color);

			// 円柱座標系を直交座標系に変換
			Vector3 ToCartesian(const Coordinate_cylindrical& cylindrical);

			// 球座標系を直交座標系に変換
			Vector3 ToCartesian(const Coordinate_spherical& spherical);

			// 直交座標系を円柱座標系に変換
			Coordinate_cylindrical ToCylindrical(const Vector3& cartesian);

			// 球座標系を円柱座標系に変換
			Coordinate_cylindrical ToCylindrical(const Coordinate_spherical& spherical);

			// 直交座標系を球座標系に変換
			Coordinate_spherical ToSpherical(const Vector3& cartesian);

			// 円柱座標系を球座標系に変換
			Coordinate_spherical ToSpherical(const Coordinate_cylindrical& cylindrical);
		}

		Vector3 DirectionFromYawPitch(float yaw, float pitch);

		Vector3 YawPitchFromDirection(const Vector3& dir);
	};

	namespace Time
	{
		/// <returns>デルタタイム</returns>
		float GetDeltaTime();

		/// <returns>起動からの経過時間取得</returns>
		uint32_t GetElapsedTime();

		/// <returns>フレームレート取得</returns>
		float GetFrameRate();

		/// <summary>
		/// 時間の速さを調整する。1.0が通常速度、0.5が半分の速度、2.0が2倍の速度になる。
		/// </summary>
		/// <param name="scale">タイムスケール</param>
		void SetTimeScale(float scale);
	};

	namespace Physics
	{
		/// <summary>
		/// WorldColliderの設定
		/// </summary>
		void AddWorldCollider(IWorldCollider* worldCollider);

		/// <summary>
		/// RenderData_Modelの物理演算有効化
		/// </summary>
		void RegisterDynamic(IPhysicsBody* b);

		/// <summary>
		/// RenderData_Modelの物理演算無効化
		/// </summary>
		void UnregisterDynamic(IPhysicsBody* b);

		/// <summary>
		/// 登録されている全てのRenderData_Modelの物理演算無効化
		/// </summary>
		void ClearDynamicAll();
	};

	void quit();
};