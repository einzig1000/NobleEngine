#pragma once
#include <EngineDefinition/EngineConstexprs.h>
#include <EngineDefinition/EngineDefinition.h>
#include <Utilities/Easing/Easing.h>
#include <RootBinding/StructuredBufferManager/StructuredBufferManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <Engine.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <ComputeSystem/ComputeObject/ComputeObject.h>

/// <summary>
/// ファサードクラス
/// </summary>
namespace Game
{
	/// <summary>
	/// モデル･テクスチャ･アニメーション･オーディオの読み込み
	/// </summary>
	namespace Asset
	{
		namespace Model
		{
			/// <summary>
			/// モデル読み込み
			/// </summary>
			/// <param name="filePath">例:"assets/engine/model/cube.obj"</param>
			/// <returns>モデルID</returns>
			int32_t Load(const std::string& filePath);

			/// <summary>
			/// モデルを作成する
			/// </summary>
			/// <param name="vertices">トライアングルリストの頂点配列</param>
			/// <param name="name">モデルのユニークな名前</param>
			/// <param name="optimize">頂点の最適化を行うかどうか</param>
			/// <returns>モデルID</returns>
			int32_t Create(const std::vector<VertexData>& vertices, const std::string& name, const bool optimize = false);

			/// <summary>
			/// モデルデータ取得
			/// </summary>
			/// <param name="modelID">モデルID</param>
			/// <returns>モデルデータ</returns>
			const ModelData* GetData(int32_t modelID);
		}

		namespace Animation
		{
			/// <summary>
			/// アニメーション読み込み
			/// </summary>
			/// <param name="filePath">例:"assets/engine/animation/animation.fbx"</param>
			/// <param name="animationName">アニメーション名</param>
			/// <returns>アニメーションID</returns>
			int32_t Load(const std::string& filePath, const std::string& animationName);
		
			/// <summary>
			/// アニメーションデータ取得
			/// </summary>
			/// <param name="animationID">アニメーションID</param>
			/// <returns>アニメーションデータ</returns>
			AnimationData* GetData(int32_t animationID);

			/// <summary>
			/// アニメーションの再生
			/// </summary>
			/// <param name="animationID">アニメーションID</param>
			/// <param name="skeleton">スケルトン</param>
			/// <param name="skinCluster">スキンクラスター</param>
			/// <param name="time">時間</param>
			void ComputeAnimationData(int32_t animationID, Skeleton& skeleton, SkinCluster& skinCluster, float& time);
		}

		namespace Texture
		{
			/// <summary>
			/// テクスチャ読み込み
			/// </summary>
			/// <param name="filePath">例:"assets/engine/texture/uvChecker.png"</param>
			/// <returns>テクスチャID</returns>
			int32_t Load(const std::string& filePath);

			/// <summary>
			/// テクスチャデータ取得
			/// </summary>
			/// <param name="textureID">テクスチャID</param>
			/// <returns>メタデータを含むテクスチャデータ</returns>
			const TextureData* GetData(int32_t textureID);
		}

		namespace Audio
		{
			/// <summary>
			/// オーディオ読み込み
			/// </summary>
			/// <param name="filePath">例:"assets/engine/audio/BGM/InGame.mp3"</param>
			/// <returns>オーディオID</returns>
			int32_t Load(const std::string& filePath);

			/// <summary>
			/// オーディオデータ取得
			/// </summary>
			/// <param name="audioID">オーディオID</param>
			/// <returns>オーディオデータ</returns>
			const AudioData* GetData(int32_t audioID);
		}

		namespace Font
		{
			/// <summary>
			/// フォント読み込み
			/// </summary>
			/// <param name="filePath">例:"assets/font/test.ttf"</param>
			/// <returns>フォントID</returns>
			int32_t Load(const std::string& filePath);

			/// <summary>
			/// 指定したテクスチャに文字を書きこむ
			/// </summary>
			/// <param name="renderTextureID">レンダーテクスチャID</param>
			/// <param name="text">書き込む文字列</param>
			/// <param name="charSize">文字サイズ</param>
			/// <param name="startPos">開始位置</param>
			/// <param name="color">文字色</param>
			/// <param name="extraSpacing">文字間隔</param>
			void DrawString(int32_t renderTextureID, const std::string& text, int32_t charSize,
				const Vector2& startPos, const Vector4& color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f }, float extraSpacing = 0.0f);

			/// <summary>
			/// ジャストサイズのテクスチャサイズ計算
			/// </summary>
			/// <param name="text">書き込む文字列</param>
			/// <param name="charSize">文字サイズ</param>
			/// <param name="startPos">開始位置</param>
			/// <param name="extraSpacing">文字間隔</param>
			/// <returns>テクスチャサイズ</returns>
			Vector2 MeasureJustTextureSize(const std::string& text, int32_t charSize,
				const Vector2& startPos, float extraSpacing);
		}

		namespace RenderTexture
		{
			/// <summary>
			/// レンダーテクスチャ作成
			/// </summary>
			/// <param name="width">横幅</param>
			/// <param name="height">縦幅</param>
			/// <param name="label">識別用のラベル</param>
			/// <returns>テクスチャID</returns>
			int32_t CreateRenderTexture(uint32_t width, uint32_t height, const std::string& label, Vector4 clearColor = Vector4{ 0.0f, 0.0f, 0.0f, 0.0f });

			/// <summary>
			/// レンダーテクスチャをファイルに保存する
			/// </summary>
			/// <param name="filePath">保存先のパス</param>
			/// <param name="textureName">保存するレンダーテクスチャの名前</param>
			/// <param name="color">カラーかどうか</param>
			/// <returns>保存に成功したかどうか</returns>
			bool SaveRenderTextureToFile(const std::string& filePath, const std::string& textureName, bool color = true);

			/// <summary>
			/// 全てのレンダーテクスチャをファイルに保存する
			/// </summary>
			/// <param name="filePath">保存先のパス</param>
			/// <param name="color">カラーかどうか</param>
			/// <returns>保存に成功したかどうか</returns>
			bool SaveAllRenderTextureToFile(const std::string& filePath, bool color = true);

			/// <summary>
			/// レンダーテクスチャID取得
			/// </summary>
			/// <param name="textureName">テクスチャ名</param>
			/// <returns>テクスチャID</returns>
			int32_t GetRenderTextureID(const std::string& textureName);

			/// <summary>
			/// 深度レンダーテクスチャID取得
			/// </summary>
			/// <param name="textureName">テクスチャ名</param>
			/// <returns>深度テクスチャID</returns>
			int32_t GetRenderTextureDepthID(const std::string& textureName);

			/// <summary>
			/// レンダーテクスチャgpuポインタ取得
			/// </summary>
			/// <param name="textureName">テクスチャ名</param>
			/// <returns>GPUポインタ</returns>
			UINT64 GetRenderTextureGPUPtr(const std::string& textureName);

			/// <summary>
			/// レンダーテクスチャgpuポインタ取得
			/// </summary>
			/// <param name="renderTextureID">レンダーテクスチャID</param>
			/// <returns>GPUポインタ</returns>
			UINT64 GetRenderTextureGPUPtr(int32_t renderTextureID);
		}
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

	/// <summary>
	/// オーディオの再生
	/// </summary>
	namespace Audio
	{
		/// <summary>
		/// オーディオ再生
		/// </summary>
		/// <param name="audioId">オーディオID(LoadAudioで取得)</param>
		/// <param name="loop">ループ再生するか否か</param>
		/// <returns>再生インスタンスID、失敗時は-1</returns>
		int32_t PlayAudio(const int32_t& audioId, bool loop);

		/// <summary>
		/// 再生停止
		/// </summary>
		/// <param name="playId">再生インスタンスID</param>
		void StopAudio(const int32_t& playId);

		/// <summary>
		/// 音量設定
		/// </summary>
		/// <param name="playId">再生インスタンスID</param>
		/// <param name="volume">音量 (0.0f~1.0f)</param>
		void SetAudioVolume(const int32_t& playId, float volume);

		/// <summary>
		/// マスターボリューム設定
		/// </summary>
		/// <param name="volume">音量 (0.0f~1.0f)</param>
		void SetMasterVolume(float volume);

		/// <summary>
		/// 音量取得
		/// </summary>
		/// <param name="playId">再生インスタンスID</param>
		/// <returns>音量 (0.0f~1.0f)</returns>
		float GetVolume(const int32_t& playId);

		/// <summary>
		/// マスターボリューム取得
		/// </summary>
		/// <returns>音量 (0.0f~1.0f)</returns>
		float GetMasterVolume();

		/// <summary>
		/// 再生中か？
		/// </summary>
		/// <param name="playId">再生インスタンスID</param>
		/// <returns>再生中か？</returns>
		bool IsAudioPlaying(const int32_t& playId);
	};

	/// <summary>
	/// マウス･キーボード･パッドからの入力を取得する
	/// </summary>
	namespace IO
	{
		namespace Mouse
		{
			/// <summary>
			/// マウスのスクリーン座標位置取得
			/// </summary>
			/// <returns>マウスのスクリーン座標</returns>
			Vector2 Get2DPosition();

			/// <summary>
			/// マウスのスクリーン座標位置変化量取得
			/// </summary>
			/// <returns>マウスのスクリーン座標位置変化量</returns>
			Vector2 Get2DPositionDelta();

			/// <summary>
			/// マウスのワールド座標位置取得
			/// </summary>
			/// <pram name="cameraID">カメラID</param>
			/// <returns>マウスのワールド座標</returns>
			Vector3 Get3DPosition(Matrix4x4& viewProjection);

			/// <summary>
			/// マウスのワールド座標レイ取得
			/// </summary>
			/// <pram name="cameraID">カメラID</param>
			/// <returns>マウスのワールド座標レイ</returns>
			Ray GetRay(Matrix4x4& viewProjection);

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
			bool IsHeld(int32_t i);

			/// <summary>
			/// マウスボタンの入力取得(押した瞬間)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>押した瞬間か</returns>
			bool IsJustPressed(int32_t i);

			/// <summary>
			/// マウスボタンの入力取得(離した瞬間)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>離した瞬間か</returns>
			bool IsJustReleased(int32_t i);

			/// <summary>
			/// マウスボタンの入力取得(押されてからの経過フレーム数)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>押されてからの経過フレーム数</returns>
			uint32_t HoldFrames(int32_t i);

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
			int32_t TestTapLong(int32_t n, BYTE key);
		};

		namespace Pad
		{
			/// <summary>
			/// ゲームパッドの入力取得(現在押されているか)
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="button"> ボタン番号(PAD_A～) </param>
			bool IsHeld(int32_t padIndex, BYTE button);

			/// <summary>
			/// ゲームパッドの入力取得(押した瞬間)
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="button"> ボタン番号(PAD_A～) </param>
			bool IsJustPressed(int32_t padIndex, BYTE button);

			/// <summary>
			/// ゲームパッドの入力取得(離した瞬間)
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="button"> ボタン番号(PAD_A～) </param>
			bool IsJustReleased(int32_t padIndex, BYTE button);

			/// <summary>
			/// ゲームパッドの入力取得(押されてからの経過フレーム数)
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="button"> ボタン番号(PAD_A～) </param>
			uint32_t HoldFrames(int32_t padIndex, BYTE button);

			/// <summary>
			/// 左スティックの入力取得
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <returns> -1.0f ～ 1.0f </returns>
			Vector2 GetLeftStick(int32_t padIndex);

			/// <summary>
			/// 右スティックの入力取得
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <returns> -1.0f ～ 1.0f </returns>
			Vector2 GetRightStick(int32_t padIndex);

			/// <summary>
			/// 左トリガーの入力取得
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <returns> 0.0f ～ 1.0f </returns>
			float GetLeftTrigger(int32_t padIndex);

			/// <summary>
			/// 右トリガーの入力取得
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <returns> 0.0f ～ 1.0f </returns>
			float GetRightTrigger(int32_t padIndex);

			/// <summary>
			/// ゲームパッド振動設定
			/// </summary>
			/// <param name="padIndex"> パッド番号(0～3) </param>
			/// <param name="leftMotor"> 左モーター強度(0.0f ～ 1.0f) </param>
			/// <param name="rightMotor"> 右モーター強度(0.0f ～ 1.0f) </param>
			void SetVibration(int32_t padIndex, float leftMotor, float rightMotor);

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
			Vector3 GetCenter(int32_t cameraID);
			/// <returns>カメラの向き</returns>
			Vector3 GetCameraDirection(int32_t cameraID);
			/// <returns>カメラのワールド座標</returns>
			Vector3 GetTranslate(int32_t cameraID);
			/// <returns>カメラのCenterまでの距離</returns>
			float GetDistance(int32_t cameraID);
			/// <returns>カメラのビュープロジェクション行列</returns>
			Matrix4x4 GetViewProjectionMatrix(int32_t cameraID);
			/// <returns>カメラの正射影プロジェクション行列</returns>
			Matrix4x4 GetOrthoProjectionMatrix(int32_t cameraID);
			/// <returns>カメラのビュー行列</returns>
			Matrix4x4 GetViewMatrix(int32_t cameraID);
			/// <returns>カメラのプロジェクション行列</returns>
			Matrix4x4 GetProjectionMatrix(int32_t cameraID);
			/// <returns>カメラのビルボード行列</returns>
			Matrix4x4 GetBillboardMatrix(int32_t cameraID);
		};

		namespace Setter
		{
			/// <summary>
			/// カメラの回転中心座標の変更
			/// </summary>
			/// <param name="target">目標座標</param>
			/// <param name="duration">変更にかけるフレーム数(0で即時変更)</param>
			/// <param name="easeType">変更補完イージングタイプ</param>
			void SetCenter(Vector3 target, float durationSec, EaseType easetype, int32_t cameraID);
			
			/// <summary>
			///	カメラの回転量変更
			/// </summary>
			/// <param name="target">目標回転量</param>
			/// <param name="duration">変更にかけるフレーム数(0で即時変更)</param>
			/// <param name="easeType">変更補完イージングタイプ</param>
			void SetRotate(Vector3 target, float durationSec, EaseType easetype, int32_t cameraID);

			/// <summary>
			///  カメラの回転中心からの距離(ズーム量)変更
			/// </summary>
			/// <param name="target">目標ズーム量</param>
			/// <param name="duration">変更にかけるフレーム数(0で即時変更)</param>
			/// <param name="easeType">変更補完イージングタイプ</param>
			void SetDistance(float target, float durationSec, EaseType easetype, int32_t cameraID);

			/// <summary>
			/// カメラのスクリーンサイズ変更
			/// </summary>
			/// <param name="target">目標スクリーンサイズ</param>
			/// <param name="spendFrame">変更にかけるフレーム数(0で即時変更)</param>
			/// <param name="easetype">変更補完イージングタイプ</param>
			/// <param name="cameraID">カメラID</param>
			void SetScreenSize(Vector2 target, float durationSec, EaseType easetype, int32_t cameraID);
			
			/// <summary>
			/// カメラのfovY変更
			/// </summary>
			/// <param name="target">目標fovY</param>
			/// <param name="duration">変更にかけるフレーム数(0で即時変更)</param>
			/// <param name="easetype">変更補完イージングタイプ</param>
			/// <param name="cameraID">カメラID</param>
			void SetFovTarget(float target, float durationSec, EaseType easetype, int32_t cameraID);

			/// <summary>
			/// カメラコントロールの有効無効設定
			/// </summary>
			void SetEnableControl(bool enable, int32_t cameraID);

			/// <summary>
			/// カメラモード(オービット or FPS)切り替え
			/// </summary>
			/// <param name="mode">カメラモード</param>
			void SetCameraMode(CameraMode_ORBIT_FPS mode, int32_t cameraID);
		}

		namespace Shake
		{
			/// <summary>
			/// カメラシェイク開始関数
			/// </summary>
			/// <param name="intensity">シェイクの最大振幅</param>
			/// <param name="duration">シェイクの継続時間</param>
			/// <param name="frequency">振動の速度 既定値は25.0f</param>
			void Start(float intensity = 1.0f, float duration = 1.0f, float frequency = 25.0f, int32_t cameraID = 0);

			/// <summary>
			/// 今シェイクしているか
			/// </summary>
			/// <returns>今シェイクしているか</returns>
			bool IsShaking(int32_t cameraID);

			/// <summary>
			/// シェイク停止
			/// </summary>
			void Stop(int32_t cameraID);
		}

		int32_t AddCamera(const std::string& name = "");
		void Update(int32_t cameraID);

		/// <summary>
		/// 描画範囲内にAABBがあるか
		/// </summary>
		/// <param name="aabb">検索対象のAABB</param>
		/// <returns>結果</returns>
		bool InCamera(const AABB& aabb, int32_t cameraID);
	};

	namespace Math
	{
		namespace Ease
		{
			/// <summary>
			/// イージング
			/// </summary>
			/// <param name="start"> 初期値 </param>
			/// <param name="end"> 終了値 </param>
			/// <param name="easeType"> イージングタイプ </param>
			/// <param name="t"> 0.0f～1.0f の補完値 </param>
			/// <returns> イージング後の値 </returns>
			template<typename T>
			T Easing(T start, T end, EaseType easeType, float t)
			{
				return Easing::EasingValue(start, end, easeType, t);
			}
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
			float RandFloat(float min, float max, int32_t decimalPlaces);

			/// <summary>
			/// 指定範囲の整数乱数取得
			/// </summary>
			/// <param name="min"> 最小値 </param>
			/// <param name="max"> 最大値 </param>
			/// <returns> 乱数 </returns>
			int32_t RandInt(int32_t min, int32_t max);
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
		float GetDeltaTimeMs();

		/// <returns>起動からの経過時間取得</returns>
		uint32_t GetElapsedTime();

		/// <returns>フレームレート取得</returns>
		float GetFrameRate();
	};

	namespace Window
	{
		/// <summary>
		/// ウィンドウの幅取得
		/// </summary>
		uint32_t GetWidth();

		/// <summary>
		/// ウィンドウの高さ取得
		/// </summary>
		uint32_t GetHeight();

		/// <summary>
		/// フルスクリーン切り替え
		/// </summary>
		void ToggleFullscreen();
	}

	namespace Resource
	{
		/// <summary>
		/// 静的リソースの作成(モデルやテクスチャ等)
		/// </summary>
		/// <typeparam name="T">データ型</typeparam>
		/// <param name="data">データ</param>
		/// <returns>リソースID</returns>
		template<typename T>
		int32_t CreateStatic(const std::vector<T>& data)
		{
			return Engine::Instance().GetStructuredBufferManager()->CreateStatic(data);
		}

		/// <summary>
		/// 動的リソースの作成(毎フレーム変わるパーティクル配列等)
		/// </summary>
		/// <returns>リソースID</returns>
		int32_t CreateDynamic();

		/// <summary>
		/// コンピュートリソースの作成(UAVも作成される)
		/// </summary>
		/// <param name="elementSize">要素のサイズ</param>
		/// <param name="elementCount">要素の数</param>
		/// <returns>リソースID</returns>
		int32_t CreateCompute(size_t elementSize, size_t elementCount);


		/// <summary>
		/// コンピュートリソースの0初期化
		/// </summary>
		/// <param name="resourceID">リソースID</param>
		/// <param name="bytes">初期化するバイト数</param>
		void ZeroFillCompute(int32_t resourceID, size_t bytes);

		/// <summary>
		/// 動的リソースの更新
		/// </summary>
		/// <param name="resourceID">リソースID</param>
		/// <param name="data">更新するデータ</param>
		/// <param name="elementSize">要素のサイズ</param>
		/// <param name="elementCount">要素の数</param>
		void UpdateData(int32_t resourceID, const void* data, size_t elementSize, size_t elementCount);

		/// <summary>
		/// リソースのSRVを取得
		/// </summary>
		/// <param name="resourceID">リソースID</param>
		/// <returns>SRVのヒープスロット</returns>
		uint32_t GetSRV(int32_t resourceID);

		/// <summary>
		/// リソースのUAVを取得(コンピュートリソースのみ)
		/// </summary>
		/// <param name="resourceID">リソースID</param>
		/// <returns>UAVのヒープスロット</returns>
		uint32_t GetUAV(int32_t resourceID);

		/// <summary>
		/// Readbackの要求
		/// </summary>
		/// <param name="resourceID">リソースID</param>
		/// <param name="bytes">読み出すバイト数</param>
		/// <returns>Readbackトークン</returns>
		int32_t RequestReadback(int32_t resourceID, size_t bytes);

		/// <summary>
		/// Readbackの結果を取得
		/// </summary>
		/// <param name="token">Readbackトークン</param>
		/// <param name="outData">結果を格納するバッファ</param>
		/// <param name="bytes">読み出すバイト数</param>
		/// <returns>結果が利用可能であればtrue</returns>
		bool TryGetReadbackResult(int32_t token, void* outData, size_t bytes);
	}

	void quit();
};