#pragma once
#include <Game.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <memory>

class LineDrawer
{
public:
	LineDrawer();
	~LineDrawer();
	void PushSegment(const Vector3& start, const Vector3& end, const Vector4& color);
	void Initialize();
	void Draw();

private:
	std::unique_ptr<RenderObject> lineRender_;

};

