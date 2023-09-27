#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;


class Game : public Application
{
	URHO3D_OBJECT(Game, Application);

	public:
	explicit Game(Context *context);

	void Setup() override;
	void Start() override;
	void Stop() override;

	private:
		SharedPtr<Scene> scene_;

	void SetWindowTitleAndIcon();
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleUpdate(StringHash eventType, VariantMap &eventData);
};
