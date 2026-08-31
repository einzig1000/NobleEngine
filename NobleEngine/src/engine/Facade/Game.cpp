#include <Facade/Game.h>

#include <Camera/CameraManager.h>
#include <Camera/Camera.h>
#include <AssetManager/AssetManager.h>
#include <DrawSystem/DrawSystem.h>
#include <IO/IOManager.h>
#include <IO/Keyboard/KeyboardController.h>
#include <IO/Pad/PadController.h>
#include <IO/Mouse/MouseController.h>
#include <FixFPS/FixFPS.h>
#include <DirectX/DirectXManager.h>
#include <Window/WindowManager.h>
#include <RootBinding/StructuredBufferManager/StructuredBufferManager.h>
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

			void ComputeAnimationData(int32_t animationID, Skeleton& skeleton, SkinCluster& skinCluster, float& time)
			{
				Engine::Instance().GetAssetManager()->GetAnimationManager()->GetAnimationComputer()->UpdateAnimation(animationID, skeleton, skinCluster, time);
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

	namespace Audio
	{
		int32_t PlayAudio(const int32_t& audioId, bool loop)
		{
			return Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioPlayer()->PlayAudio(audioId, loop);
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
				return Engine::Instance().GetIOManager()->GetMouseController()->GetWorldPosition(viewProjection);
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
			uint32_t HoldFrames(int32_t i)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->HoldFrames(i);
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
			uint32_t HoldFrames(BYTE key)
			{
				return Engine::Instance().GetIOManager()->GetKeyboardController()->HoldFrames(key);
			}
			int32_t TestTapLong(int32_t n, BYTE key)
			{
				return Engine::Instance().GetIOManager()->GetKeyboardController()->TestTapLong(n, key);
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
			uint32_t HoldFrames(int32_t padIndex, BYTE button)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->HoldFrames(padIndex, button);
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
			Vector3 GetTranslate(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetTranslate();
			}
			float GetDistance(int32_t cameraID)
			{
				return Engine::Instance().GetCameraManager()->GetCamera(cameraID)->GetDistance();
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
			void SetCenter(Vector3 target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetCenterTarget(target, durationSec, easetype);
			}

			void SetRotate(Vector3 target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetRotateTarget(target, durationSec, easetype);
			}

			void SetDistance(float target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetDistanceTarget(target, durationSec, easetype);
			}

			void SetScreenSize(Vector2 target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetScreenSizeTarget(target, durationSec, easetype);
			}

			void SetFovTarget(float target, float durationSec, EaseType easetype, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetFovTarget(target, durationSec, easetype);
			}

			void SetEnableControl(bool enable, int32_t cameraID)
			{
				Engine::Instance().GetCameraManager()->GetCamera(cameraID)->SetEnableControl(enable);
			}

			void SetCameraMode(CameraMode_ORBIT_FPS mode, int32_t cameraID)
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
			return Vector3(pitch, yaw, 0.0f); // roll はここでは未使用
		}
	}

	namespace Time
	{
		float GetDeltaTimeMs()
		{
			return Engine::Instance().GetFixFPS()->GetClampedDeltaTimeMs();
		}

		float GetFrameRate()
		{
			return Engine::Instance().GetFixFPS()->GetClampedFPS();
		}

		uint32_t GetElapsedTime()
		{
			return Engine::Instance().GetFixFPS()->GetFrameCount();
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
			return Engine::Instance().GetStructuredBufferManager()->CreateDynamic();
		}
		int32_t CreateCompute(size_t elementSize, size_t elementCount)
		{
			return Engine::Instance().GetStructuredBufferManager()->CreateCompute(elementSize, elementCount);
		}

		void ZeroFillCompute(int32_t resourceID, size_t bytes)
		{
			Engine::Instance().GetStructuredBufferManager()->ZeroFillCompute(resourceID, bytes);
		}

		void UpdateData(int32_t resourceID, const void* data, size_t elementSize, size_t elementCount)
		{
			Engine::Instance().GetStructuredBufferManager()->UpdateData(resourceID, data, elementSize, elementCount);
		}

		uint32_t GetSRV(int32_t resourceID)
		{
			return Engine::Instance().GetStructuredBufferManager()->GetSRV(resourceID);
		}

		uint32_t GetUAV(int32_t resourceID)
		{
			return Engine::Instance().GetStructuredBufferManager()->GetUAV(resourceID);
		}

		int32_t RequestReadback(int32_t resourceID, size_t bytes)
		{
			return Engine::Instance().GetStructuredBufferManager()->RequestReadback(resourceID, bytes);
		}

		bool TryGetReadbackResult(int32_t token, void* outData, size_t bytes)
		{
			return Engine::Instance().GetStructuredBufferManager()->TryGetReadbackResult(token, outData, bytes);
		}
	}

	void Game::quit()
	{
		Engine::Instance().Quit();
	}

}
