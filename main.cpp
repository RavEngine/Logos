#include <RavEngine/App.hpp>
#include <RavEngine/StaticMesh.hpp>
#include <RavEngine/World.hpp>
#include <RavEngine/CameraComponent.hpp>
#include <RavEngine/GameObject.hpp>
#include <RavEngine/Dialogs.hpp>

using namespace RavEngine;
using namespace std;

// Start by defining an application class that derives from RavEngine::App.
struct HelloCubeApp : public RavEngine::App {

	// Implement a constructor that calls this superclass constructor. The parameter
	// passed here is the name of the resources zip file that your application is loading.
	// when using pack_resources in CMake, supply here the name of your target.
	HelloCubeApp() : App("HelloCube") {}

	// Next, implement OnStartup. This is called when your application launches.
	// You are passed argc and argv from the command line. 
	void OnStartup(int argc, char** argv) final;
    
    // Finally, if there are any unrecoverable errors that occur, you can optionally override
    // this method to perform a custom action before closing. In this case, we will display
    // an error dialog to the user.
    void OnFatal(const char* msg) final{
        RavEngine::Dialog::ShowBasic("Fatal Error", msg, Dialog::MessageBoxType::Error);
    }
};

// After defining an App, define a World by creating a class that derives from RavEngine::World.
// A world is akin to a Scene in Unity or a Map in Unreal. Worlds contain all the action in a game.
struct HelloCubeWorld : public RavEngine::World {

	// Implement the World constructor.
	HelloCubeWorld() {

		// Create an Entity. RavEngine uses an ECS, so entities are not like Unity GameObjects.
        // Instead, think of an Entity as a handle to a collection of components stored elsewhere.
        // Unlike other ECS implementations, Entity handles are global, and also have convenience
        // member functions.
        
		// RavEngine Entities do not come with a Transform by default. For convenience, a "GameObject" is also provided. However, this
		// is just an entity with a transform component applied automatically, do not confuse it with the higher-level concept.
        // When you call this, it immediately creates the entity in the world.
		auto cubeEntity = CreatePrototype<GameObject>();

		// We want to display a cube in this world. RavEngine uses a component-based composition model 
		// for describing scene objects. However, you can also subclass RavEngine::Entity and perform
		// setup inside Entity::Create for Unreal-style inheritance. 

		// We will start by loading the cube mesh. The cube is one of the default primitives that comes with RavEngine.
		auto cubeMesh = MeshAsset::Manager::Get("cube.obj");

		// Next we need to define a material for the cube. We can use the default material.
		// RavEngine can optimize your rendering for you by batching if you minimize the number of Material Instances you create,
		// and share them wherever possible. 
		auto cubeMat = RavEngine::New<PBRMaterialInstance>(Material::Manager::Get<PBRMaterial>());

		// A StaticMesh defines a fixed (non-deforming) rendered polygon, which is perfect for our cube.
		// Note that all StaticMeshes must have a material bound to them. A StaticMesh without a material will cause
		// the engine to crash, as that is an invalid state.
		// The EmplaceComponent method constructs the component inline and attaches it to the entity.
        cubeEntity.EmplaceComponent<StaticMesh>(cubeMesh, cubeMat);

		// We want to be able to see our cube, so we need a camera. In RavEngine, cameras are also components, so 
		// we need another entity to hold that. 
		auto cameraEntity = CreatePrototype<GameObject>();

		// Create the CameraComponent. The EmplaceComponent call returns an owning pointer to the constructed component.
		// Be wary of storing these to avoid reference cycles that lead to memory leaks. 
		auto& cameraComponent = cameraEntity.EmplaceComponent<CameraComponent>();

		// By default, cameras are disabled, and do not render. We must enable our camera for it to display anything in the world.
		cameraComponent.SetActive(true);

		// As of this line, the camera is inside the cube. To rectify this, let's place the cube in front of the camera.
        cubeEntity.GetTransform().LocalTranslateDelta(vector3(0,0,-5));

		// We want some lighting on our cube. In RavEngine, lights are also components, so we'll need an entity for those.
		auto lightsEntity = CreatePrototype<GameObject>();
		lightsEntity.EmplaceComponent<DirectionalLight>();					// a light that mimics the sun
		lightsEntity.EmplaceComponent<AmbientLight>().Intensity = 0.2;	// a weak fill light that affects all surfaces equally

		// Lights use the transformation of their parent entity to determine their rotation and position.
		lightsEntity.GetTransform().LocalRotateDelta(vector3{ deg_to_rad(45), deg_to_rad(45),0 });
	}

	// The engine calls this method synchronously every tick after the rest of the pipeline has finished processing.
	void PostTick(float tickrateScale) final {

		// We can hold onto an entity as a class variable, but I'll instead use this opportunity to demonstrate the query system.
		// RavEngine worlds can be queried extremely efficiently by component type. Since we know that this world contains
		// one static mesh, we can use that to get our entity.
		auto& meshComp = GetComponent<StaticMesh>();

        // some components provide a convenience method to get the owning Entity.
        auto entity = meshComp.GetOwner();

        // Let's spin our cube.
        // RavEngine expects rotations in radians. Use deg_to_rad to convert to degrees.
        auto rotVec = vector3(deg_to_rad(1), deg_to_rad(2), deg_to_rad(-0.5));

        // If the engine were to fall behind, we need to ensure our game does not run in slow motion.
		// To accomplish this, we simply multiply our movements by a scale factor passed into this method.
		// RavEngine has already calcuated the scale factor for us. This is *not* a deltaTime,
        // like in Unity.
        rotVec *= tickrateScale;

        // Entities can also be queried efficiently for components, just like worlds. Since Transforms are
        // a very frequent access, GameObjects provide the convenience function GetTransform, to use instead.
        entity.GetComponent<Transform>().LocalRotateDelta(rotVec);
	}
};

// We've defined a world, but now we need to load it. OnStartup is a good place to load your initial world.
void HelloCubeApp::OnStartup(int argc, char** argv) {

	// make an instance of the world
	auto level = RavEngine::New<HelloCubeWorld>();

	// Tell the engine to switch to this world.
	// If the engine has no worlds active, it will automatically set the first one as the active (rendered) world.
	AddWorld(level);
}

// Last thing - we need to launch our application. RavEngine supplies a convenience macro for this,
// which simply inlines a main function that launches your app and invokes its OnStartup method.
// You do not need to use this macro if you don't want to.
START_APP(HelloCubeApp)