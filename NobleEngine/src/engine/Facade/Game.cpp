#include <Facade/Game.h>
#include <Engine.h>

#include <Camera/CameraManager.h>
#include <ResourceManager/ResourceManager.h>
#include <DrawSystem/DrawSystem.h>
#include <IO/IOManager.h>
#include <IO/KeyboardController.h>
#include <IO/PadController.h>
#include <IO/MouseController.h>
#include <FixFPS/FixFPS.h>
#include <Physics/PhysicsSystem.h>
#include <Utilities/Easing/Easing.h>
#include <Utilities/Converter/ColorConverter/ColorConverter.h>
#include <Utilities/Converter/CoordinateConverter/CoordinateConverter.h>
#include <Utilities/Converter/AngleConverter/AngleConverter.h>
#include <Utilities/Random/Random.h>

namespace Game
{
	namespace Resource
	{
		uint32_t LoadModel(const std::string& filePath)
		{
			return Engine::Instance().GetResourceManager()->GetModelManager()->LoadModel(filePath);
		}
		uint32_t LoadTexture(const std::string& filePath)
		{
			return Engine::Instance().GetResourceManager()->GetTextureManager()->LoadTexture(filePath);
		}
		uint32_t LoadAudio(const std::string& filePath)
		{
			return Engine::Instance().GetResourceManager()->GetAudioManager()->LoadAudio(filePath);
		}

		TextureData* GetTextureData(uint32_t textureNumber)
		{	
			return Engine::Instance().GetResourceManager()->GetTextureManager()->GetTextureData(textureNumber);
		}
		ModelData* GetModelData(uint32_t modelID)
		{
			return Engine::Instance().GetResourceManager()->GetModelManager()->GetModelData(modelID);
		}
		AudioData* GetAudioData(uint32_t audioID)
		{
			return Engine::Instance().GetResourceManager()->GetAudioManager()->GetAudioData(audioID);
		}

		size_t GetTextureCount()
		{
			return Engine::Instance().GetResourceManager()->GetTextureManager()->GetTextureCount();
		}
		size_t GetModelCount()
		{
			return Engine::Instance().GetResourceManager()->GetModelManager()->GetModelCount();
		}
		size_t GetAudioCount()
		{
			return Engine::Instance().GetResourceManager()->GetAudioManager()->GetAudioCount();
		}
	}

	namespace DebugDraw
	{
		void AddSphere(const Sphere& sphere, uint32_t color)
		{
			Engine::Instance().GetDrawSystem()->AddSphere(sphere, color);
		}
		void AddSphereXYZ(const SphereXYZ& sphere, uint32_t color)
		{
			Engine::Instance().GetDrawSystem()->AddSphereXYZ(sphere, color);
		}
		void AddCylinder(const Cylinder& cylinder, uint32_t color)
		{
			Engine::Instance().GetDrawSystem()->AddCylinder(cylinder, color);
		}
		void AddAABB(const AABB& aabb, uint32_t color)
		{
			Engine::Instance().GetDrawSystem()->AddAABB(aabb, color);
		}
		void AddLine(Vector3 start, Vector3 end, uint32_t color)
		{
			Engine::Instance().GetDrawSystem()->AddDebugLineList(start, end, color);
		}
	}

	namespace Audio
	{
		void PlayAudio(const uint32_t& audioId, bool loop)
		{
			Engine::Instance().GetResourceManager()->GetAudioManager()->PlayAudio(audioId, loop);
		}
		void StopAudio(const uint32_t& audioId)
		{
			Engine::Instance().GetResourceManager()->GetAudioManager()->StopAudio(audioId);
		}
		void SetAudioVolume(const uint32_t& audioId, float volume)
		{
			Engine::Instance().GetResourceManager()->GetAudioManager()->SetVolume(audioId, volume);
		}
		void SetMasterVolume(float volume)
		{
			Engine::Instance().GetResourceManager()->GetAudioManager()->SetMasterVolume(volume);
		}
		float GetVolume(const uint32_t& audioId)
		{
			return Engine::Instance().GetResourceManager()->GetAudioManager()->GetVolume(audioId);
		}
		float GetMasterVolume()
		{
			return Engine::Instance().GetResourceManager()->GetAudioManager()->GetMasterVolume();
		}
		bool IsAudioPlaying(const uint32_t& audioId)
		{
			return Engine::Instance().GetResourceManager()->GetAudioManager()->IsAudioPlaying(audioId);
		}
	}

	namespace Light
	{
		void SetLightColor(const Vector4 color)
		{
			//Engine::Instance().SetLightColor(color);
		}
		void SetLightDirection(const Vector3 direction)
		{
			//Engine::Instance().SetLightDirection(direction);
		}
		void SetLightIntensity(float intensity)
		{
			//Engine::Instance().SetLightIntensity(intensity);
		}
		void ToggleLightMode(const LightMode mode)
		{
			//Engine::Instance().ToggleLightMode(mode);
		}
	}

	namespace IO
	{
		namespace Mouse
		{
			// マウス関連
			Vector2 GetPosition()
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->GetPosition();
			}
			Vector2 GetPositionDelta()
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->GetRawDelta();
			}
			Vector3 GetWorldPosition()
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->GetWorldPosition();
			}
			Ray GetRay()
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->GetRay();
			}
			bool IsHeld(int i)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->IsHeld(i);
			}
			bool IsJustPressed(int i)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->IsJustPressed(i);
			}
			bool IsJustReleased(int i)
			{
				return Engine::Instance().GetIOManager()->GetMouseController()->IsJustReleased(i);
			}
			uint32_t HoldFrames(int i)
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
			int TestTapLong(int n, BYTE key)
			{
				return Engine::Instance().GetIOManager()->GetKeyboardController()->TestTapLong(n, key);
			}
		}

		namespace Pad
		{
			bool IsHeld(int padIndex, BYTE button)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->IsHeld(padIndex, button);
			}
			bool IsJustPressed(int padIndex, BYTE button)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->IsJustPressed(padIndex, button);
			}
			bool IsJustReleased(int padIndex, BYTE button)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->IsJustReleased(padIndex, button);
			}
			uint32_t HoldFrames(int padIndex, BYTE button)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->HoldFrames(padIndex, button);
			}
			Vector2 GetLeftStick(int padIndex)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->GetLeftStick(padIndex);
			}
			Vector2 GetRightStick(int padIndex)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->GetRightStick(padIndex);
			}
			float GetLeftTrigger(int padIndex)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->GetLeftTrigger(padIndex);
			}
			float GetRightTrigger(int padIndex)
			{
				return Engine::Instance().GetIOManager()->GetPadController()->GetRightTrigger(padIndex);
			}
			void SetVibration(int padIndex, float leftMotor, float rightMotor)
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
		void MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype)
		{
			Engine::Instance().GetCameraManager()->SetCenterTarget(target, spendFrame, easetype);
		}
		void MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype)
		{
			Engine::Instance().GetCameraManager()->SetRotateTarget(target, spendFrame, easetype);
		}
		void MoveCameraDistance(float target, int spendFrame, EaseType easetype)
		{
			Engine::Instance().GetCameraManager()->SetDistanceTarget(target, spendFrame, easetype);
		}
		
		void StartCameraShake(float intensity, float duration, float frequency)
		{
			Engine::Instance().GetCameraManager()->StartShake(intensity, duration, frequency);
		}
		bool IsShaking()
		{
			return Engine::Instance().GetCameraManager()->IsShaking();
		}
		void StopShake()
		{
			Engine::Instance().GetCameraManager()->StopShake();
		}
		
		bool InCamera(const AABB& aabb)
		{
			return Engine::Instance().GetCameraManager()->InCamera(aabb);
		}

		void SetEnableControl(bool enable)
		{
			Engine::Instance().GetCameraManager()->SetEnableControl(enable);
		}

		void ToggleCamera()
		{
			Engine::Instance().GetCameraManager()->ToggleCamera();
		}
		void ToggleCamera(const std::string name)
		{
			Engine::Instance().GetCameraManager()->ToggleCamera(name);
		}

		void SetCameraMode(CameraMode_ORBIT_FPS mode)
		{
			Engine::Instance().GetCameraManager()->SetCameraMode(mode);
		}

		namespace Getter
		{
			Vector3 GetCurrentCenter()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentCenter();
			}
			Vector3 GetCurrentTranslate()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentTranslate();
			}
			Vector3 GetCurrentRotate()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentRotate();
			}
			Matrix4x4 GetCurrentViewProjectionMatrix()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentViewProjectionMatrix();
			}
			Matrix4x4 GetCurrentViewMatrix()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentViewMatrix();
			}
			Matrix4x4 GetCurrentProjectionMatrix()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentProjectionMatrix();
			}
			float GetCurrentDistance()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentDistance();
			}
		}
	}

	namespace Utilities
	{
		// Counterとか追加する？
	}

	namespace Math
	{
		namespace Ease
		{
			float EasingFloat(float start, float end, EaseType easeType, float t)
			{
				return Easing::EasingFloat(start, end, easeType, t);
			}
			Vector3 EasingVector3(Vector3 start, Vector3 end, EaseType easeType, float t)
			{
				return Easing::EasingVector3(start, end, easeType, t);
			}
		}

		namespace Rand
		{
			float RandFloat(float min, float max, int decimalPlaces)
			{
				return Random::RandomFloat(min, max, decimalPlaces);
			}
			int RandInt(int min, int max)
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
		// 時間関連
		float GetDeltaTime()
		{
			return Engine::Instance().GetFixFPS()->GetDeltaTime();
		}
		uint32_t GetElapsedTime()
		{
			return Engine::Instance().GetFixFPS()->GetFrameCount();
		}
		float GetFrameRate()
		{
			return Engine::Instance().GetFixFPS()->GetAverageFPS();
		}
		void SetTimeScale(float scale)
		{
			Engine::Instance().GetFixFPS()->SetTimeScale(scale);
		}
	}

	namespace Physics
	{
		// マップ設定
		void AddWorldCollider(IWorldCollider* worldCollider)
		{
			//Engine::Instance().GetPhysicsSystem()->AddWorldCollider(worldCollider);
		}

		// マップと衝突する動的オブジェクトの登録
		void RegisterDynamic(IPhysicsBody* b)
		{
			//Engine::Instance().GetPhysicsSystem()->RegisterDynamic(b);
		}

		// マップと衝突する動的オブジェクトの登録解除
		void UnregisterDynamic(IPhysicsBody* b)
		{
			//Engine::Instance().GetPhysicsSystem()->UnregisterDynamic(b);
		}

		// 全ての動的オブジェクトの登録解除
		void ClearDynamicAll()
		{
			//Engine::Instance().GetPhysicsSystem()->ClearDynamics();
		}

		//void SetGravity(Vector3 gravity)
		//{
		//	Engine::Instance().SetGravity(gravity);
		//}
		//
		//Vector3 GetGravity()
		//{
		//	return Engine::Instance().GetGravity();
		//}
	}

	void Game::quit()
	{
		Engine::Instance().Quit();
	}

}