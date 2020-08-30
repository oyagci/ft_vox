#pragma once

class IRenderer
{
public:
	virtual void render() = 0;
	virtual ~IRenderer() {};
};
