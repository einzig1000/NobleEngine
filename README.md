### workflow states badge

[![DebugBuild](https://github.com/einzig1000/CG2/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/einzig1000/CG2/actions/workflows/DebugBuild.yml)

### 機能
## メインループ
- - [x] Game::Initialize(int width, int height, const std::wstring& title);
   (int ウィンドウ横幅, int ウィンドウ縦幅, const std::wstring& タイトル)
   ウィンドウのサイズとウィンドウの名前を設定できる。
- - [x] Game::ProcessMessage()
- - [x] Game::BeginFrame()
- - [x] Game::UpdateTransforms();
- - [x] Game::EndFrame();
- - [x] Game::Finalize();

## ロード系
- uint32_t Game::LoadOBJ(const std::string& directoryPath, const std::string& filename)
   - 引数
      - ディレクトリパス
      - ファイルネーム
   - 返り値
      - モデルID
   - 備考
      - Game::LoadOBJ("resources/Prototypes/model/", "test.obj");のように使う
      
- uint32_t Game::LoadTexture(const std::string& filePath);
   - 引数
      - ディレクトリパス + ファイルネーム
   - 返り値
      - テクスチャID
   - 備考
      - Game::LoadTexture("resources/Prototypes/texture/test.png");のように使う
 
- uint32_t Game::LoadAudio(const std::string& filePath);
   - 引数
      - ディレクトリパス + ファイルネーム
   - 返り値
      - オーディオID
   - 備考
      - Game::LoadAudio("resources/Prototypes/audio/BGM/Morning.mp3");のように使う

- TextureData* Game::GetTexture(uint32_t textureNumber);
   - 引数
      - テクスチャID
   - 返り値
      - テクスチャIDに応じたmetadataを含むテクスチャの情報
   - 備考
      - ImGuiで画像描画したいからmetadata欲しい！って時とかに使う

## オーディオ系
- void Game::PlayAudio(const uint32_t& audioId, bool loop);
   - 引数
     - オーディオID
     - true の時はループ再生
   - 返り値
      - なし
   - 備考
      - オーディオの再生
      
- void Game::StopAudio(const uint32_t& audioId);
   - 引数
     - オーディオID
   - 返り値
      - なし
   - 備考
      - オーディオの停止
  
- void Game::SetAudioVolume(const uint32_t& audioId, float volume);
   - 引数
     - オーディオID
     - 音量
   - 返り値
      - なし
   - 備考
      - オーディオの音量の設定
        
- void Game::SetMasterVolume(float volume);
   - 引数
     - 音量
   - 返り値
      - なし
   - 備考
      - マスター音量の設定

- uint32_t Game::GetVolume(const uint32_t& audioId);
   - 引数
     - オーディオID
   - 返り値
      - オーディオの音量
   - 備考
      - オーディオの音量の取得
  
- void Game::GetMasterVolume();
   - 引数
      - なし
   - 返り値
      - マスターの音量
   - 備考
      - マスター音量の取得

- bool Game::IsAudioPlaying(const uint32_t& audioId);
   - 引数
      - オーディオID
   - 返り値
      - オーディオが再生されているか
   - 備考
      - 
  

## ライト系
- void Game::SetLightColor(const Vector4 color);
   - 引数
      - 変更後のライト色
   - 返り値
      - オーディオが再生されているか
   - 備考
      - ライトの色を変えたい気分の時に
 
- void Game::SetLightDirection(const Vector3 direction);
   - 引数
      - 変更後のライトの向き
   - 返り値
      - なし
   - 備考
      - ライトの向きを変えたい気分の時に
  
- void Game::SetLightIntensity(float intensity);
   - 引数
      - 変更後のライトの輝度
   - 返り値
      - なし
   - 備考
      - ライトの輝度を変えたい気分の時に(0.0f ~ 1.0f)
  
- void Game::ToggleLightMode(const uint32_t mode);
   - 引数
      - ライトモード
   - 返り値
      - なし
   - 備考
      - 0:ハーフランバート  1:ランバート  2:ライトなし

## 入力系
- Vector2 Game::GetMousePosition();
   - 引数
      - なし
   - 返り値
      - マウスの2D平面座標
   - 備考
      - なし
  
- Ray Game::GetMouseRay();
   - 引数
      - なし
   - 返り値
      - マウスのレイ
   - 備考
      - なし
  
- bool Game::IsCollisionMouseRayObject(uint32_t objectNumber, const Transforms& data);
   - 引数
      - オブジェクトID
      - 当たり判定とりたいオブジェクトのTransforms
   - 返り値
      - マウスの2D平面座標
   - 備考
      - マウスのレイとオブジェクトの衝突判定
  
- bool Game::GetMousePress(int i);
   - 引数
      - マウスボタンのID
   - 返り値
      - IDに対応するボタンがクリックされているか
   - 備考
      - 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
  
- bool Game::GetMousePrePress(int i)
   - 引数
      - マウスボタンのID
   - 返り値
      - IDに対応するボタンが１フレーム前にクリックされていたか
   - 備考
      - 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
  
- uint32_t Game::GetMouseWheel();
   - 引数
      - なし
   - 返り値
      - マウスホイール量
   - 備考
      - 0 = 左クリック  1 = 右クリック  2 = ミドルボタン

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