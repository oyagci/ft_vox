#pragma once

namespace ecs {

struct IComponentBase
{
public:
	IComponentBase() = default;
	virtual ~IComponentBase() {}
};

struct IComponent : public IComponentBase
{
public:
	IComponent() = default;
	virtual ~IComponent() {}
};

}
