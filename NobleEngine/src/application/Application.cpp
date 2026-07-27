#include "Application.h"

Application& Application::Instance()
{
	static Application instance;
	return instance;
}

Application::Application()
{
	dataManager_ = std::make_unique<DataManager>();

	editor_ = std::make_unique<Editor>(dataManager_.get());
}


Application::~Application()
{
	dataManager_.reset();
	editor_.reset();
}

void Application::Update()
{
	editor_->Update();
}

void Application::Draw()
{
	editor_->Draw();
}

void Application::DrawImGui()
{
	editor_->DrawImGui();
}
