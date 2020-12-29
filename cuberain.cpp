// cuberain OGRE implementation
#include <string>
#include <memory>
#include <iostream>
#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreCameraMan.h>
#include <OgreTrays.h>

using std::string;
using std::unique_ptr, std::make_unique;
using std::cout, std::endl;
using namespace Ogre;
using namespace OgreBites;

static string to_string(CameraStyle style);


class ogre_app
	: public ApplicationContext, public InputListener
{
public:
	ogre_app();
	void go();
	void setup() override;

	// user input
	bool keyPressed(KeyboardEvent const & evt) override;
	bool keyReleased(KeyboardEvent const & evt) override;
	bool mouseMoved(MouseMotionEvent const & evt) override;
	bool mousePressed(MouseButtonEvent const & evt) override;
	bool mouseReleased(MouseButtonEvent const & evt) override;
	void frameRendered(Ogre::FrameEvent const & evt) override;

private:
	unique_ptr<CameraMan> _cameraman;
};


void ogre_app::setup()
{
	ApplicationContext::setup();
	addInputListener(this);  // register for input events

	SceneManager * scene = getRoot()->createSceneManager();
	scene->setAmbientLight(ColourValue{0.5, 0.5, 0.5});

	// register our scene with the RTSS
	RTShader::ShaderGenerator * shadergen = RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(scene);

	SceneNode * root_nd = scene->getRootSceneNode();

	// without light we would just get a black screen
	SceneNode * light_nd = root_nd->createChildSceneNode();
	Light * light = scene->createLight("MainLight");
	light_nd->setPosition(20, 80, 50);
	light_nd->attachObject(light);

	// create camera so we can observe scene
	SceneNode * camera_nd = root_nd->createChildSceneNode();
	camera_nd->setPosition(100, 200, 800);
	camera_nd->lookAt(Vector3{0, 0, -1}, Node::TS_PARENT);

	Camera * camera = scene->createCamera("MainCamera");
	camera->setNearClipDistance(5);  // specific to this sample
	camera->setAutoAspectRatio(true);
	camera_nd->attachObject(camera);

	_cameraman = make_unique<CameraMan>(camera_nd);
	_cameraman->setStyle(CS_FREELOOK);
	cout << "camera style: " << to_string(_cameraman->getStyle()) << endl;

	getRenderWindow()->addViewport(camera);  // render into the main window

	// create and render cube
	Entity * cube = scene->createEntity(SceneManager::PT_CUBE);
	SceneNode * cube_node = root_nd->createChildSceneNode();
	cube_node->attachObject(cube);

	// cube size ?
	Vector3 cube_size = cube->getBoundingBox().getSize();
	cout << "cube aabb: " << cube_size << endl;

	// cube2
	Entity * cube2 = cube->clone("cube2");
	SceneNode * cube2_node = root_nd->createChildSceneNode();
	cube2_node->attachObject(cube2);
	cube2_node->setPosition(110, 0, 0);

	// cube3
	Entity * cube3 = cube->clone("cube3");
	SceneNode * cube3_node = root_nd->createChildSceneNode();
	cube3_node->attachObject(cube3);
	cube3_node->setPosition(50, 0, 110);

	setWindowGrab();  //grab mouse
}

void ogre_app::go()
{
	initApp();

	if (getRoot()->getRenderSystem())
		getRoot()->startRendering();  // rendering loop

	closeApp();
}

ogre_app::ogre_app()
	: ApplicationContext{"ogre_cuberain"}
{}

bool ogre_app::keyPressed(KeyboardEvent const & evt)
{
	if (evt.keysym.sym == SDLK_ESCAPE)
	{
		getRoot()->queueEndRendering();
		return true;
	}
	else
		_cameraman->keyPressed(evt);

	return true;
}

bool ogre_app::keyReleased(KeyboardEvent const & evt)
{
	_cameraman->keyReleased(evt);
	return true;
}

bool ogre_app::mouseMoved(MouseMotionEvent const & evt)
{
	return _cameraman->mouseMoved(evt);
}

bool ogre_app::mousePressed(MouseButtonEvent const & evt)
{
	return _cameraman->mousePressed(evt);
}

bool ogre_app::mouseReleased(MouseButtonEvent const & evt)
{
	return _cameraman->mouseReleased(evt);
}

void ogre_app::frameRendered(Ogre::FrameEvent const & evt)
{
	_cameraman->frameRendered(evt);
}




string to_string(CameraStyle style)
{
	switch (style)
	{
		case CS_FREELOOK: return "freelook";
		case CS_ORBIT: return "orbit";
		case CS_MANUAL: return "manual";
		default: return "unknown";
	}
}


int main(int argc, char * argv[])
{
	ogre_app app;
	app.initApp();
	app.getRoot()->startRendering();
	app.closeApp();
	return 0;
}
