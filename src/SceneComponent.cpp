#include "SceneComponent.hpp"
#include "UI.hpp"

void ASceneComponent::call(std::string const &funcName)
{
	_uiController->call(funcName);
}
