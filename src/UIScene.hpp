#pragma once

class IUIScene
{
public:
	virtual ~IUIScene() {};
	virtual void update() = 0;
};
