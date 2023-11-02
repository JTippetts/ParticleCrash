
#include <Urho3D/Core/Object.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/Log.h>

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Renderer.h>

#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/JSONValue.h>

#include <Urho3D/RmlUI/RmlUI.h>
#include <Urho3D/RmlUI/RmlUIComponent.h>

#include "game.h"

class TestRmlClick : public RmlUIComponent
{
	URHO3D_OBJECT(TestRmlClick, RmlUIComponent);
	public:
	TestRmlClick(Context *context) : RmlUIComponent(context)
	{
		SetResource("UI/testrmlclick.rml");
	}
	
	protected:
	
	void Update(float dt) override;
	void OnDataModelInitialized() override;
	
	void HandleClick(Rml::DataModelHandle modelHandle, Rml::Event& ev, const Rml::VariantList& arguments);
};

void TestRmlClick::Update(float dt)
{
	DirtyAllVariables();
}

void TestRmlClick::OnDataModelInitialized()
{
	Rml::DataModelConstructor* constructor = GetDataModelConstructor();
	
	constructor->BindEventCallback("click", &TestRmlClick::HandleClick, this);
}

void TestRmlClick::HandleClick(Rml::DataModelHandle modelHandle, Rml::Event& ev, const Rml::VariantList& arguments)
{
	int button = ev.GetParameter<int>("button", -1);
	
	URHO3D_LOGINFO("Button clicked: {}", button);
}


Game::Game(Context *context) : Application(context)
{
}

void Game::Setup()
{
	engineParameters_[EP_FULL_SCREEN] = false; 
    engineParameters_[EP_WINDOW_HEIGHT] = 768;
    engineParameters_[EP_WINDOW_WIDTH] = 1024;
    engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ".;..";
	engineParameters_[EP_WINDOW_MAXIMIZE] = true;
	engineParameters_[EP_WINDOW_RESIZABLE]=true;
	engineParameters_[EP_LOG_LEVEL]=LOG_DEBUG;
	engineParameters_[EP_FRAME_LIMITER] = false;
	engineParameters_[EP_ORGANIZATION_NAME] = "JTippetts";
	engineParameters_[EP_APPLICATION_NAME] = "ParticleCrash";
	engineParameters_[EP_BORDERLESS] = false;
}

void Game::Start()
{
	SubscribeToEvent("KeyDown", URHO3D_HANDLER(Game, HandleKeyDown));
	SubscribeToEvent("Update", URHO3D_HANDLER(Game, HandleUpdate));
	context_->RegisterFactory<TestRmlClick>();
	
	SetWindowTitleAndIcon();

	scene_ = MakeShared<Scene>(context_);
	scene_->CreateComponent<Octree>();

	Node* camnode = scene_->CreateChild();
	Camera* camera = camnode->CreateComponent<Camera>();
	camnode->SetPosition(Vector3(0, 0, -10));

	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camera));

	Renderer* renderer = context_->GetSubsystem<Renderer>();
	renderer->SetViewport(0, viewport);

	auto input = GetSubsystem<Input>();
	input->SetMouseVisible(true);
	
	RmlUI* rmlui = GetSubsystem<RmlUI>();
	rmlui->LoadFont("Fonts/NotoSans-Condensed.ttf", false);
	rmlui->LoadFont("Fonts/NotoSans-CondensedBold.ttf", false);
	rmlui->LoadFont("Fonts/NotoSans-CondensedBoldItalic.ttf", false);
	rmlui->LoadFont("Fonts/NotoSans-CondensedItalic.ttf", false);
	
	scene_->CreateComponent<TestRmlClick>();
	

}

void Game::Stop()
{
	engine_->DumpResources(true);
}

void Game::SetWindowTitleAndIcon()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
    Graphics* graphics = GetSubsystem<Graphics>();
    Image* icon = cache->GetResource<Image>("Textures/UrhoIcon.png");
    graphics->SetWindowIcon(icon);
    graphics->SetWindowTitle("ParticleCrash");
}

void Game::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyUp;
    int key = eventData[P_KEY].GetInt();
	if (key == KEY_ESCAPE)
    {
        engine_->Exit();
    }
}

void Game::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
	float dt=eventData["TimeStep"].GetFloat();

	auto input = GetSubsystem<Input>();
	auto cache = GetSubsystem<ResourceCache>();
	if (input->GetKeyPress(KEY_SPACE))
	{
		URHO3D_LOGINFO("Spawning particle system");
		Node* pc = scene_->InstantiateJSON(cache->GetResource<JSONFile>("Effects/deathflash.json")->GetRoot(), Vector3(0,0,0), Quaternion(0, Vector3(0,1,0)));
	}
}


URHO3D_DEFINE_APPLICATION_MAIN(Game)

