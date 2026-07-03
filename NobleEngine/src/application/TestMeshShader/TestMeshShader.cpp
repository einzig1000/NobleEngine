#include "TestMeshShader.h"
#include <Game.h>

TestMeshShader::TestMeshShader()
{
	render_ = std::make_unique<RenderObject>();
	render_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_->psoConfig_.ms = "resources/shaders/SimpleModel/SimpleModel.MS.hlsl";
	render_->modelID_ = Game::Resource::Model::Load("resources/prototypes/model/bunny/bunny.obj");
	render_->SetupFromShaders();

	ModelData* modelData = Game::Resource::Model::GetData(render_->modelID_);
	modelInfo_.meshletSrvIndex = modelData->meshletSrvIndex;
	modelInfo_.primIndexSrvIndex = modelData->primitiveIndexSrvIndex;
	modelInfo_.uniqueIndexSrvIndex = modelData->uniqueVertexIndexSrvIndex;
	modelInfo_.vertexSrvIndex = modelData->vertexSrvindex;

	modelInfo_int4.x = modelData->vertexSrvindex;
	modelInfo_int4.y = modelData->meshletSrvIndex;
	modelInfo_int4.z = modelData->uniqueVertexIndexSrvIndex;
	modelInfo_int4.w = modelData->primitiveIndexSrvIndex;

	// メッシュレットの数をインスタンス数として設定
	render_->instanceNum_ = modelData->meshlets.size();
}

TestMeshShader::~TestMeshShader()
{
}

void TestMeshShader::Initialize()
{}

void TestMeshShader::Update(int32_t cameraID)
{
	Vector4 color1 = { 1.0f, 0.0f, 0.0f, 1.0f };
	int32_t t_uvChecker = Game::Resource::Texture::Load("resources/Prototypes/texture/uvChecker.png");

	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
	Matrix4x4 wvp = Matrix4x4::MakeIdentity4x4() * viewProjection;

	render_->SetCBufferData(0, ShaderType::PixelShader, &color1);
	render_->SetCBufferData(1, ShaderType::PixelShader, &t_uvChecker);
	render_->SetCBufferData(0, ShaderType::MeshShader, &modelInfo_int4);
	render_->SetCBufferData(1, ShaderType::MeshShader , &wvp);
}

void TestMeshShader::Draw(int32_t rtID)
{
	render_->Draw(rtID);
}