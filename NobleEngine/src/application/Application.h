#pragma once
#include <memory>
#include <ResourceLoader/Data/DataManager.h>
#include <Editor/editor.h>

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

	void Update();
	void Draw();
	void DrawImGui();

public:

	DataManager* GetDataManager() { return dataManager_.get(); }

private:
	Application();
	~Application();

	// データ管理
	std::unique_ptr<DataManager> dataManager_;


	// エディタ
	std::unique_ptr<Editor> editor_;
};

