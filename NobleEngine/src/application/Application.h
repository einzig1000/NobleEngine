#pragma once
#include <memory>

class GameManager;
class DataManager;
class Editor;

class Application
{
public:
	// 唯一のインスタンス取得
	static Application& Instance();

	// コピー・ムーブ禁止
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(Application&&) = delete;

	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();
	void Finalize();

public:

	DataManager* GetDataManager() { return dataManager_.get(); }

private:
	Application() = default;
	~Application() = default;

	// メインのゲームループ
	std::unique_ptr<GameManager> gameManager_;

	// データ管理
	std::unique_ptr<DataManager> dataManager_;

	// エディタ
	std::unique_ptr<Editor> editor_;
};

