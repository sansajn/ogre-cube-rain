// OGRE 1.12 starter sample
#include <iostream>
#include <Ogre.h>
#include <OgreApplicationContext.h>

using std::cout, std::endl;
using namespace Ogre;
using namespace OgreBites;

class MyTestApp
	: public ApplicationContext, public InputListener
{
public:
	MyTestApp();
	~MyTestApp() {}
	void setup() override;
	bool keyPressed(KeyboardEvent const & evt) override;
};

MyTestApp::MyTestApp()
	: ApplicationContext{"OgreCubeRain"}
{}

bool MyTestApp::keyPressed(KeyboardEvent const & evt)
{
	if (evt.keysym.sym == SDLK_ESCAPE)
	{
		getRoot()->queueEndRendering();
		return true;
	}
	else
		return false;  // key not processed
}

void MyTestApp::setup()
{
	ApplicationContext::setup();
	addInputListener(this);  // register for input events

	SceneManager * scene = getRoot()->createSceneManager();
	scene->setAmbientLight(ColourValue{0.5, 0.5, 0.5});

	// register our scene with the RTSS
	RTShader::ShaderGenerator * shadergen =
		RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(scene);

	SceneNode * root_node = scene->getRootSceneNode();

	// without light we would just get a black screen
	Light * light = scene->createLight("MainLight");
	SceneNode * light_node = root_node->createChildSceneNode();
	light_node->setPosition(20, 80, 50);
	light_node->attachObject(light);

	// create camera so we can observe scene
	Camera * camera = scene->createCamera("MainCamera");
	camera->setNearClipDistance(5);  // specific to this sample
	camera->setAutoAspectRatio(true);
	SceneNode * camera_node = root_node->createChildSceneNode();
	camera_node->setPosition(100, 200, 800);
	camera_node->lookAt(Vector3{0, 0, -1}, Node::TS_PARENT);
	camera_node->attachObject(camera);

	getRenderWindow()->addViewport(camera);  // render into the main window

	// create and render cube
	Entity * cube = scene->createEntity(SceneManager::PT_CUBE);
	SceneNode * cube_node = root_node->createChildSceneNode();
	cube_node->attachObject(cube);

	// cube size ?
	Vector3 cube_size = cube->getBoundingBox().getSize();
	cout << "cube aabb: " << cube_size << endl;

	// cube2
	Entity * cube2 = cube->clone("cube2");
	SceneNode * cube2_node = root_node->createChildSceneNode();
	cube2_node->attachObject(cube2);
	cube2_node->setPosition(110, 0, 0);

	// cube3
	Entity * cube3 = cube->clone("cube3");
	SceneNode * cube3_node = root_node->createChildSceneNode();
	cube3_node->attachObject(cube3);
	cube3_node->setPosition(50, 0, 110);


}

int main(int argc, char * argv[])
{
	MyTestApp app;
	app.initApp();
	app.getRoot()->startRendering();
	app.closeApp();
	return 0;
}
