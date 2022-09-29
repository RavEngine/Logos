#include <RavEngine/App.hpp>
#include <RavEngine/StaticMesh.hpp>
#include <RavEngine/World.hpp>
#include <RavEngine/CameraComponent.hpp>
#include <RavEngine/GameObject.hpp>
#include <RavEngine/Dialogs.hpp>

using namespace RavEngine;
using namespace std;

struct LogoApp : public RavEngine::App {
    LogoApp() : App("RavEngineLogos") {}
	void OnStartup(int argc, char** argv) final;
    void OnFatal(const char* msg) final{
        RavEngine::Dialog::ShowBasic("Fatal Error", msg, Dialog::MessageBoxType::Error);
    }
};

struct LogoWorld : public RavEngine::World {

    LogoWorld() {

		auto cubeEntity = CreatePrototype<GameObject>();
		auto cubeMesh = MeshAsset::Manager::Get("cube.obj");
		auto cubeMat = RavEngine::New<PBRMaterialInstance>(Material::Manager::Get<PBRMaterial>());
        auto tex = Texture::Manager::Get("logo.png");
        cubeMat->SetAlbedoTexture(tex);
        cubeEntity.EmplaceComponent<StaticMesh>(cubeMesh, cubeMat);
        
		auto cameraEntity = CreatePrototype<GameObject>();
		auto& cameraComponent = cameraEntity.EmplaceComponent<CameraComponent>();
		cameraComponent.SetActive(true);
        cameraComponent.FOV = 45;
        cubeEntity.GetTransform().LocalTranslateDelta(vector3(0,0,-5));

		auto lightsEntity = CreatePrototype<GameObject>();
        auto& dirlight = lightsEntity.EmplaceComponent<DirectionalLight>();
		lightsEntity.EmplaceComponent<AmbientLight>().Intensity = 0.2;

		lightsEntity.GetTransform().LocalRotateDelta(vector3{ deg_to_rad(45), 0, 0 });
        cubeEntity.GetTransform().LocalRotateDelta(vector3{ deg_to_rad(45), deg_to_rad(45), 0 });

	}
};

void LogoApp::OnStartup(int argc, char** argv) {
    auto& renderer = GetRenderEngine();
    auto& settings = renderer.VideoSettings;
    settings.width = 1024 * 1.0/renderer.GetDPIScale();
    settings.height = settings.width;
    renderer.SyncVideoSettings();
    AddWorld(RavEngine::New<LogoWorld>());
}
START_APP(LogoApp)
