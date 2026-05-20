### workflow states badge

[![DebugBuild](https://github.com/einzig1000/CG2/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/einzig1000/CG2/actions/workflows/DebugBuild.yml)

### 機能
## リソース系 Game::Resource
- uint32_t LoadModel(const std::string& filePath);
   - 引数
      - ファイルパス
   - 返り値
      - モデルID
   - 備考
      - Game::Resource::LoadModel("resources/Prototypes/model/test.obj");のように使う
      
- uint32_t LoadTexture(const std::string& filePath);
   - 引数
      - ファイルパス
   - 返り値
      - テクスチャID
   - 備考
      - Game::Resource::LoadTexture("resources/Prototypes/texture/test.png");のように使う
 
- uint32_t LoadAudio(const std::string& filePath);
   - 引数
      - ファイルパス
   - 返り値
      - オーディオID
   - 備考
      - Game::Resource::LoadAudio("resources/Prototypes/audio/BGM/Morning.mp3");のように使う

- TextureData* GetTextureData(uint32_t textureID);
   - 引数
      - テクスチャID
   - 返り値
      - テクスチャIDに応じたmetadataを含むテクスチャの情報
   - 備考
      - ImGuiで画像描画したいからmetadata欲しい！って時とかに使う

- ModelData* GetModelData(uint32_t modelID);
   - 引数
      - モデルID
   - 返り値
      - モデルIDに応じたモデルデータの情報

- AudioData* GetAudioData(uint32_t audioID);
   - 引数
      - モデルID
   - 返り値
      - audioIDに応じたオーディオデータの情報

- size_t GetTextureCount();
   - 返り値
      - 読み込んだテクスチャの数

- size_t GetModelCount();
   - 返り値
      - 読み込んだモデルの数

- size_t GetAudioCount();
   - 返り値
      - 読み込んだオーディオの数

## オーディオ系 Game::Audio
- void PlayAudio(const uint32_t& audioId, bool loop);
   - 引数
     - オーディオID
     - true の時はループ再生
   - 返り値
      - なし
   - 備考
      - オーディオの再生
      
- void StopAudio(const uint32_t& audioId);
   - 引数
     - オーディオID
   - 返り値
      - なし
   - 備考
      - オーディオの停止
  
- void SetAudioVolume(const uint32_t& audioId, float volume);
   - 引数
     - オーディオID
     - 音量
   - 返り値
      - なし
   - 備考
      - オーディオの音量の設定
        
- void SetMasterVolume(float volume);
   - 引数
     - 音量
   - 返り値
      - なし
   - 備考
      - マスター音量の設定

- float GetVolume(const uint32_t& audioId);
   - 引数
     - オーディオID
   - 返り値
      - オーディオの音量
  
- float GetMasterVolume();
   - 引数
      - なし
   - 返り値
      - マスターの音量

- bool IsAudioPlaying(const uint32_t& audioId);
   - 引数
      - オーディオID
   - 返り値
      - オーディオが再生されているか
  

## 入力系 Game::IO
- Vector2 Mouse::GetPosition();
   - 返り値
      - マウスのスクリーン座標
  
- Vector2 Mouse::GetPositionDelta();
   - 返り値
      - マウスのスクリーン座標変化量取得
  
- Vector3 Mouse::GetWorldPosition();
   - 返り値
      - マウスのワールド座標
  
- Ray Mouse::GetRay();
   - 返り値
      - マウスのレイ
  
- bool Mouse::GetWheel();
   - 返り値
      - マウスホイールの回転量
  
- bool Mouse::IsHeld(int i)
   - 引数
      - マウスボタンのID
   - 返り値
      - IDに対応するマウスボタンの入力(現在押されているか)
   - 備考
      - 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
  
- bool Mouse::IsJustPressed(int i)
   - 引数
      - マウスボタンのID
   - 返り値
      - IDに対応するマウスボタンの入力(押した瞬間)
   - 備考
      - 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
  
- bool Mouse::IsJustReleased(int i)
   - 引数
      - マウスボタンのID
   - 返り値
      - IDに対応するマウスボタンの入力(離した瞬間)
   - 備考
      - 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
  
- uint32_t Mouse::IsJustReleased(int i)
   - 引数
      - マウスボタンのID
   - 返り値
      - IDに対応するマウスボタンの入力(押されてからの経過フレーム数)
   - 備考
      - 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
  
- void Mouse::ToggleMouseCursorVisible()
   - 備考
      - カーソルの表示・非表示切り替え
  
- void Mouse::ShowCursor(bool visible)
   - 備考
      - カーソルの表示・非表示切り替え
  
- void Mouse::SetMouseSensitivity(float sensitivity);
   - 引数
      - マウス感度



## カメラ系
- void Game::MoveCenterTarget(Vector3 target, int spendFrame, EaseType easetype);
   - 引数
      - 変更後のカメラ回転中心座標
      - 何フレームで変更完了するか
      - 変更までのイージング
   - 返り値
      - なし
   - 備考
      - MoveCenterTarget(Vector3{0.0f,0.0f,0.0f}, 120, EaseType::IN_BACK);のように使えば120フレームかけて回転中心が{0.0f,0.0f,0.0f}になる
  
- Game::MoveRotateTarget(Vector3 target, int spendFrame, EaseType easetype);
   - 引数
      - 変更後のカメラ回転量
      - 何フレームで変更完了するか
      - 変更までのイージング
   - 返り値
      - なし
   - 備考
      - MoveRotateTarget(Vector3{0.0f,0.0f,0.0f}, 120, EaseType::IN_BACK);のように使えば120フレームかけて回転量が{0.0f,0.0f,0.0f}になる
  
- Game::MoveDistanceTarget(float target, int spendFrame, EaseType easetype);
   - 引数
      - 変更後のカメラ回転中心からの距離
      - 何フレームで変更完了するか
      - 変更までのイージング
   - 返り値
      - なし
   - 備考
      - MoveDistanceTarget(10.0f, 120, EaseType::IN_BACK);のように使えば120フレームかけて回転中心からの距離が10.0fになる
  
- Game::GetCamera();
   - つかわないでほしい

- Game::GetDebugCamera();
   - つかわないでほしい

## 
- Game::toggleWireframeMode();