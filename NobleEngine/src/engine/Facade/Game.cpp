#include <Facade/Game.h>

#include <Camera/CameraManager.h>
#include <Camera/Camera.h>
#include <AssetManager/AssetManager.h>
#include <DrawSystem/DrawSystem.h>
#include <IO/IOManager.h>
#include <IO/Keyboard/KeyboardController.h>
#include <IO/Pad/PadController.h>
#include <IO/Mouse/MouseController.h>
#include <TimeManager/TimeManager.h>
#include <DirectX/DirectXManager.h>
#include <Window/WindowManager.h>
#include <Utilities/Converter/ColorConverter/ColorConverter.h>
#include <Utilities/Converter/CoordinateConverter/CoordinateConverter.h>
#include <Utilities/Converter/AngleConverter/AngleConverter.h>
#include <Utilities/Random/Random.h>

namespace Game
{
	namespace Asset
	{
		namespace Model
		{
			int32_t Load(const std::string& filePath)
			{
				return Engine::Instance().GetAssetManager()->GetModelManager()->GetModelLoader()->LoadModel(filePath);
			}

			int32_t Create(const std::vector<VertexData>& vertices, const std::string& name, const bool optimize)
			{
				return Engine::Instance().GetAssetManager()->GetModelManager()->GetModelCreater()->CreateModel(vertices, name, optimize);
			}

			const ModelData* GetData(int32_t modelID)
			{
				return Engine::Instance().GetAssetManager()->GetModelManager()->GetModelBank()->GetModelData(modelID);
			}
		}

		namespace Animation
		{
			int32_t Load(const std::string& filePath, const std::string& animationName)
			{
				return Engine::Instance().GetAssetManager()->GetAnimationManager()->GetAnimationLoader()->LoadAnimation(filePath, animationName);
			}

			AnimationData* GetData(int32_t animationID)
			{
				return Engine::Instance().GetAssetManager()->GetAnimationManager()->GetAnimationBank()->GetAnimationData(animationID);
			}

			SkinInstance CreateSkinInstance(int32_t modelID)
			{
				const ModelData* modelData = Game::Asset::Model::GetData(modelID);

				SkinInstance inst;
				inst.skeleton = modelData->skeleton;
				inst.palette.resize(modelData->skeleton.joints.size());
				inst.paletteHandle = Game::Resource::CreateDynamic();
				return inst;
			}

			void ComputeAnimationData(int32_t animationID, SkinInstance& skin, const SkinBindData& bind, float& time)
			{
				Engine::Instance().GetAssetManager()->GetAnimationManager()->GetAnimationComputer()->ComputeAnimationData(animationID, skin, bind, time);
			}

			Matrix4x4 SampleNodeHierarchy(int32_t animationID, const std::string& nodeName, float& time)
			{
				return Engine::Instance().GetAssetManager()->GetAnimationManager()->GetAnimationComputer()->SampleNodeHierarchy(animationID, nodeName, time);
			}
		}

		namespace Texture
		{
			int32_t Load(const std::string& filePath)
			{
				return Engine::Instance().GetAssetManager()->GetTextureManager()->GetTextureLoader()->LoadTexture(filePath);
			}

			const TextureData* GetData(int32_t textureID)
			{
				return Engine::Instance().GetAssetManager()->GetTextureManager()->GetTextureBank()->GetTextureData(textureID);
			}
		}

		namespace Audio
		{
			int32_t Load(const std::string& filePath)
			{
				return Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioLoader()->LoadAudio(filePath);
			}

			const AudioData* GetData(int32_t audioID)
			{
				return Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioBank()->GetAudioData(audioID);
			}


			int32_t PlayAudio(const int32_t& audioId, bool loop, float volume)
			{
				return Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioPlayer()->PlayAudio(audioId, loop, volume);
			}
			void StopAudio(const int32_t& playId)
			{
				Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioPlayer()->StopAudio(playId);
			}
			void SetAudioVolume(const int32_t& playId, float volume)
			{
				Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioPlayer()->SetVolume(playId, volume);
			}
			void SetMasterVolume(float volume)
			{
				Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioPlayer()->SetMasterVolume(volume);
			}
			float GetVolume(const int32_t& playId)
			{
				return Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioPlayer()->GetVolume(playId);
			}
			float GetMasterVolume()
			{
				return Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioPlayer()->GetMasterVolume();
			}
			bool IsAudioPlaying(const int32_t& playId)
			{
				return Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioPlayer()->IsAudioPlaying(playId);
			}
		}

		namespace Font
		{
			int32_t Load(const std::string& filePath)
			{
				return Engine::Instance().GetAssetManager()->GetFontManager()->Load(filePath);
			}
			void DrawString(int32_t renderTextureID, const std::string& text, int32_t charSize, const Vector2& startPos, const Vector4& color, float extraSpacing)
			{
				Engine::Instance().GetAssetManager()->GetFontManager()->DrawString(renderTextureID, text, charSize, startPos, color, extraSpacing);
			}

			Vector2 MeasureJustTextureSize(const std::string& text, int32_t charSize, const Vector2& startPos, float extraSpacing)
			{
				return Engine::Instance().GetAssetManager()->GetFontManager()->MeasureJustTextureSize(text, charSize, startPos, extraSpacing);
			}

		}

		namespace RenderTexture
		{
			int32_t CreateRenderTexture(uint32_t width, uint32_t height, const std::string& textureName, Vector4 clearColor)
			{
				return Engine::Instance().GetDirectXManager()->GetRenderTextureManager()->CreateRenderTarget(width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, textureName, clearColor);
			}
			bool SaveRenderTextureToFile(const std::string& filePath, const std::string& textureName, bool color)
			{
				return Engine::Instance().GetDirectXManager()->GetRenderTextureManager()->SaveTexture(filePath, textureName, color);
			}
			bool SaveAllRenderTextureToFile(const std::string& filePath, bool color)
			{
				return Engine::Instance().GetDirectXManager()->GetRenderTextureManager()->SaveAllRenderTextures(filePath, color);
			}
			int32_t GetRenderTextureID(const std::string& textureName)
			{
				return Engine::Instance().GetDirectXManager()->GetRenderTextureManager()->Get(textureName)->colorsrvAlloc.index;
			}
			int32_t GetRenderTextureDepthID(const std::string& textureName)
			{
				return Engine::Instance().GetDirectXManager()->GetRenderTextureManager()->Get(textureName)->depthsrvAlloc.index;
			}
			UINT64 GetRenderTextureGPUPtr(const std::string& textureName)
			{
				return Engine::Instance().GetDirectXManager()->GetRenderTextureManager()->Get(textureName)->colorsrvAlloc.gpu.ptr;
			}
			UINT64 GetRenderTextureGPUPtr(int32_t renderTextureID)
			{
				return Engine::Instance().GetDirectXManager()->GetRenderTextureManager()->Get(renderTextureID)->colorsrvAlloc.gpu.ptr;
			}
		}
	}

	namespace DebugDraw
	{
		//void AddSphere(const Sphere& sphere, uint32_t color)
		//{
		//	Engine::Instance().GetDrawSystem()->AddSphere(sphere, color);
		//}
		//void AddSphereXYZ(const SphereXYZ& sphere, uint32_t color)
		//{
		//	Engine::Instance().GetDrawSystem()->AddSphereXYZ(sphere, color);
		//}
		//void AddCylinder(const Cylinder& cylinder, uint32_t color)
		//{
		//	Engine::Instance().GetDrawSystem()->AddCylinder(cylinder, color);
		//}
		//void AddAABB(const AABB& aabb, uint32_t color)
		//{
		//	Engine::Instance().GetDrawSystem()->AddAABB(aabb, color);
		//}
		//void AddLine(Vector3 start, Vector3 end, uint32_t color)
		//{
		//	Engine::Instance().GetDrawSystem()->AddDebugLineList(start, end, color);
		//}
	}

	namespace IO
	{
		namespace Mouse
		{
			Vector2 Get2DPosition()
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->Get2DPosition();
			}
			Vector2 Get2DPositionDelta()
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->Get2DPositionDelta();
			}
			Vector3 Get3DPosition(Matrix4x4& viewProjection)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->Get3DPosition(viewProjection);
			}
			Ray GetRay(Matrix4x4& viewProjection)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->GetRay(viewProjection);
			}

			bool IsHeld(int32_t i)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->IsHeld(i);
			}
			bool IsJustPressed(int32_t i)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->IsJustPressed(i);
			}
			bool IsJustReleased(int32_t i)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->IsJustReleased(i);
			}
			float HoldSeconds(int32_t i)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->HoldSeconds(i);
			}
			int32_t GetWheel()
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->GetWheelDelta();
			}
			void ToggleMouseCursorVisible()
			{
				Engine::Instance().GetIOManager()->GetMouseController()->ToggleMouseCursorVisible();
			}
			void ShowCursor(bool visible)
			{
				Engine::Instance().GetIOManager()->GetMouseController()->ShowCursor(visible);
			}
			void SetMouseSensitivity(float sensitivity)
			{
				Engine::Instance().GetIOManager()->GetMouseController()->SetSensitivity(sensitivity);
			}
		}

		namespace Key
		{
			// キー関連
			bool IsHeld(BYTE key)
			{
				return Engine::Instance().GetIOManager()->GetKeyboardController()->IsHeld(key);
			}
			bool IsJustPressed(BYTE key)
			{
				return Engine::Instance().GetIOManager()->GetKeyboardController()->IsJustPressed(key);
			}
			bool IsJustReleased(BYTE key)
			{
				return Engine::Instance().GetIOManager()->GetKeyboardController()->IsJustReleased(key);
			}
			float HoldSeconds(BYTE key)
			{
				return Engine::Instance().GetIOManager()->GetKeyboardController()->HoldSeconds(key);
			}
			int32_t TestTapLong(float thresholdSeconds, BYTE key)
			{
				return Engine::Instance().GetIOManager()->GetKeyboardController()->TestTapLong(thresholdSeconds, key);
			}
		}

		namespace Pad
		{
			bool IsHeld(int32_t padIndex, BYTE button)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->IsHeld(padIndex, button);
			}
			bool IsJustPressed(int32_t padIndex, BYTE button)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->IsJustPressed(padIndex, button);
			}
			bool IsJustReleased(int32_t padIndex, BYTE button)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->IsJustReleased(padIndex, button);
			}
			float HoldSeconds(int32_t padIndex, BYTE button)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->HoldSeconds(padIndex, button);
			}
			Vector2 GetLeftStick(int32_t padIndex)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->GetLeftStick(padIndex);
			}
			Vector2 GetRightStick(int32_t padIndex)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->GetRightStick(padIndex);
			}
			float GetLeftTrigger(int32_t padIndex)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->GetLeftTrigger(padIndex);
			}
			float GetRightTrigger(int32_t padIndex)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->GetRightTrigger(padIndex);
			}
			void SetVibration(int32_t padIndex, float leftMotor, float rightMotor)
			{
				Engine::Instance().GetIOManager()->GetPadController()->SetVibration(padIndex, leftMotor, rightMotor);
			}
			int32_t GetConnectedPadNum()
			{
				return Engine::Instance().GetIOManager()->GetPadController()->GetConnectedPadNum();
			}

		}
	}

	namespace Camera
	{
		int32_t AddCamera(const std::string& name)
		{
			return Engine::Instance().GetCameraManager()->AddCamera(name);
		}

		void Update(int32_t cameraID)
		{
			Engine::Instance().GetCameraManager()->Update(cameraID);
		}

		bool InCamera(const AABB& aabb, int32_t cameraID)
		{
			return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->InCamera(aabb);
		}


		namespace Getter
		{
			Vector3 GetCenter(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetCenter();
			}
			Vector3 GetCameraDirection(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetCameraDirection();
			}
			Vector3 GetWorldPosition(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetTranslate();
			}
			float GetDistance(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetDistance();
			}
			float GetPhi(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetPhi();
			}
			float GetTheta(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetTheta();
			}
			Matrix4x4 GetViewProjectionMatrix(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetViewProjectionMatrix();
			}
			Matrix4x4 GetOrthoProjectionMatrix(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetOrthoProjectionMatrix();
			}
			Matrix4x4 GetViewMatrix(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetViewMatrix();
			}
			Matrix4x4 GetProjectionMatrix(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetProjectionMatrix();
			}
			Matrix4x4 GetBillboardMatrix(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetBillboardMatrix();
			}
		}

		namespace Setter
		{
			void CenterTarget(Vector3 target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetCenterTarget(target, durationSec, easetype);
			}

			void PhiTarget(float target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetPhiTarget(target, durationSec, easetype);
			}

			void ThetaTarget(float target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetThetaTarget(target, durationSec, easetype);
			}

			void DistanceTarget(float target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetDistanceTarget(target, durationSec, easetype);
			}

			void ScreenSizeTarget(Vector2 target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetScreenSizeTarget(target, durationSec, easetype);
			}

			void FovTarget(float target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetFovTarget(target, durationSec, easetype);
			}

			void EnableControl(bool enable, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetEnableControl(enable);
			}

			void CameraMode(CameraMode_ORBIT_FPS mode, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetCameraMode(mode);
			}
		}

		namespace Shake
		{
			void Start(float intensity, float duration, float frequency, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->StartShake(intensity, duration, frequency);
			}
			bool IsShaking(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->IsShaking();
			}
			void Stop(int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->StopShake();
			}
		}
	}

	namespace Utilities
	{
		// Counterとか追加する？
	}

	namespace Math
	{
		namespace Rand
		{
			float RandFloat(float min, float max, int32_t decimalPlaces)
			{
				return Random::RandomFloat(min, max, decimalPlaces);
			}
			int32_t RandInt(int32_t min, int32_t max)
			{
				return Random::RandomInt(min, max);
			}
		}

		namespace Converter
		{
			Vector4 UintToVector4(uint32_t color)
			{
				return ColorConverter::ConvertUintToVector4(color);
			}
			uint32_t Vector4ToUint(Vector4 color)
			{
				return ColorConverter::ConvertVector4ToUint(color);
			}

			float DegreeToRadian(float degree)
			{
				return AngleConverter::ToRadian(degree);
			}
			float RadianToDegree(float radian)
			{
				return AngleConverter::ToDegree(radian);
			}

			Vector3 ToCartesian(const Coordinate_cylindrical& cylindrical)
			{
				return CoordinateConverter::ToCartesian(cylindrical);
			}
			Vector3 ToCartesian(const Coordinate_spherical& spherical)
			{
				return CoordinateConverter::ToCartesian(spherical);
			}

			Coordinate_cylindrical ToCylindrical(const Vector3& cartesian)
			{
				return CoordinateConverter::ToCylindrical(cartesian);
			}
			Coordinate_cylindrical ToCylindrical(const Coordinate_spherical& spherical)
			{
				return CoordinateConverter::ToCylindrical(spherical);
			}

			Coordinate_spherical ToSpherical(const Vector3& cartesian)
			{
				return CoordinateConverter::ToSpherical(cartesian);
			}
			Coordinate_spherical ToSpherical(const Coordinate_cylindrical& cylindrical)
			{
				return CoordinateConverter::ToSpherical(cylindrical);
			}
		}

		Vector3 DirectionFromYawPitch(float yaw, float pitch)
		{
			float sp = std::sinf(pitch);
			float cp = std::cosf(pitch);
			float sy = std::sinf(yaw);
			float cy = std::cosf(yaw);

			Vector3 dir;
			dir.x = sy * cp;
			dir.y = -sp;
			dir.z = cy * cp;
			dir.Normalize();
			return dir;
		}
		Vector3 YawPitchFromDirection(const Vector3& dir)
		{
			Vector3 normDir = dir.Normalized();
			float pitch = std::asinf(-normDir.y); // -sin(pitch) = y 成分
			float yaw = std::atan2f(normDir.x, normDir.z); // sin(yaw) = x 成分, cos(yaw) = z 成分
			return Vector3(pitch, yaw, 0.0f); // rollは0
		}
	}

	namespace Time
	{
		float GetFPS()
		{
			return Engine::Instance().GetTimeManager()->GetFixFPS()->GetClampedFPS();
		}

		void SetTimeScale(float timeScale)
		{
			Engine::Instance().GetTimeManager()->GetTimeScaler()->SetTimeScale(timeScale);
		}

		float GetScaledDeltaTimeMs()
		{
			return Engine::Instance().GetTimeManager()->GetScaledDeltaTimeMs();
		}

		uint32_t GetElapsedFrameTime()
		{
			return Engine::Instance().GetTimeManager()->GetFixFPS()->GetElapsedFrameTime();
		}

		float GetElapsedSecTime()
		{
			return Engine::Instance().GetTimeManager()->GetFixFPS()->GetElapsedSecTime();
		}
	}

	namespace Window
	{
		uint32_t GetWidth()
		{
			return WindowManager::winWidth_;
		}

		uint32_t GetHeight()
		{
			return WindowManager::winHeight_;
		}

		void ToggleFullscreen()
		{
			// ウィンドウサイズ変更
			Engine::Instance().GetWindowManager()->ToggleFullscreen();

			// DirectXのリサイズ処理
			Engine::Instance().GetDirectXManager()->Resize();

			// カメラのアスペクト比更新
			//Engine::Instance().GetCameraManager()->Resize();
		}
	}

	namespace Resource
	{
		int32_t CreateDynamic()
		{
			return Engine::Instance().GetRootBindingManager()->GetStructuredBufferManager()->CreateDynamic();
		}
		int32_t CreateCompute(size_t elementSize, size_t elementCount)
		{
			return Engine::Instance().GetRootBindingManager()->GetStructuredBufferManager()->CreateCompute(elementSize, elementCount);
		}

		void ZeroFillCompute(int32_t resourceID, size_t bytes)
		{
			Engine::Instance().GetRootBindingManager()->GetStructuredBufferManager()->ZeroFillCompute(resourceID, bytes);
		}

		void Destroy(int32_t resourceID)
		{
			Engine::Instance().GetRootBindingManager()->GetStructuredBufferManager()->Destroy(resourceID);
		}

		uint32_t GetSRV(int32_t resourceID)
		{
			return Engine::Instance().GetRootBindingManager()->GetStructuredBufferManager()->GetSRV(resourceID);
		}

		uint32_t GetUAV(int32_t resourceID)
		{
			return Engine::Instance().GetRootBindingManager()->GetStructuredBufferManager()->GetUAV(resourceID);
		}

		int32_t RequestReadback(int32_t resourceID, size_t bytes)
		{
			return Engine::Instance().GetRootBindingManager()->GetStructuredBufferManager()->RequestReadback(resourceID, bytes);
		}

		bool TryGetReadbackResult(int32_t token, void* outData, size_t bytes)
		{
			return Engine::Instance().GetRootBindingManager()->GetStructuredBufferManager()->TryGetReadbackResult(token, outData, bytes);
		}
	}

	namespace System
	{
		void quit()
		{
			Engine::Instance().Quit();
		}

		void ToggleDrawImGui()
		{
			Engine::Instance().GetImGuiManager()->ToggleDraw();
		}
	}
}
