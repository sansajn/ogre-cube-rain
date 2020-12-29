// cuberain OGRE implementation
#include <vector>
#include <string>
#include <memory>
#include <random>
#include <chrono>
#include <iostream>
#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreCameraMan.h>
#include <OgreTrays.h>
#include "axis.hpp"

using std::vector;
using std::string, std::to_string;
using std::unique_ptr, std::make_unique;
using std::random_device, std::default_random_engine;
using std::cout, std::endl;
using std::chrono::steady_clock, std::chrono::duration;

using Ogre::SceneManager, // Ogre::vector name collision with std::vector so `using namespace Ogre` cannot be used
	Ogre::SceneNode,
	Ogre::FrameListener,
	Ogre::FrameEvent,
	Ogre::Light,
	Ogre::Camera,
	Ogre::Entity,
	Ogre::ColourValue,
	Ogre::Vector3,
	Ogre::Real,
	Ogre::Node;

using namespace OgreBites;


Vector3 const camera_position = {0, 0, 10};

// flyweight pattern
struct cube_object
{
	Vector3 position;
	Real scale;  // value between 0.7 and 1.4 used to scale cube model
};

cube_object new_cube();


class ogre_app
	: public ApplicationContext, public InputListener
{
public:
	ogre_app();
	void go();  //!< app entry point

	// user input
	bool keyPressed(KeyboardEvent const & evt) override;
	bool keyReleased(KeyboardEvent const & evt) override;
	bool mouseMoved(MouseMotionEvent const & evt) override;
	bool mousePressed(MouseButtonEvent const & evt) override;
	bool mouseReleased(MouseButtonEvent const & evt) override;
	void frameRendered(Ogre::FrameEvent const & evt) override;

private:
	void update(duration<double> dt);
	void setup() override;

	// renderer events
	bool frameStarted(FrameEvent const & evt) override;

	unique_ptr<CameraMan> _cameraman;
	vector<cube_object> _cubes;  // cube pool
	vector<SceneNode *> _cube_nodes;
	steady_clock::time_point _last_frame_tp;
};

namespace std {

string to_string(CameraStyle style);

}  // std

void ogre_app::update(duration<double> dt)
{
//	cout << "update(dt=" << dt.count() << "s)" << endl;

	constexpr Real fall_speed = 3;
	constexpr Real fall_off_threshold = -10.0;

	auto cube_node_it = begin(_cube_nodes);
	for (cube_object & cube : _cubes)
	{
		cube.position.y -= fall_speed * (2.0 - cube.scale) * dt.count();
		if (cube.position.y < fall_off_threshold)
			cube = new_cube();

		(*cube_node_it)->setPosition(cube.position);  // update scene position
		++cube_node_it;
	}
}

void ogre_app::setup()
{
	ApplicationContext::setup();
	addInputListener(this);  // register for input events

	SceneManager * scene = getRoot()->createSceneManager();
	scene->setAmbientLight(ColourValue{0.5, 0.5, 0.5});

	// register our scene with the RTSS
	Ogre::RTShader::ShaderGenerator * shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(scene);

	SceneNode * root_nd = scene->getRootSceneNode();

	// without light we would just get a black screen
	SceneNode * light_nd = root_nd->createChildSceneNode();
	Light * light = scene->createLight("light");
	light_nd->setPosition(20, 80, 50);
	light_nd->attachObject(light);

	// create camera so we can observe scene
	SceneNode * camera_nd = root_nd->createChildSceneNode();
	camera_nd->setPosition(camera_position);
	camera_nd->lookAt(Vector3{0, 0, -1}, Node::TS_PARENT);

	Camera * camera = scene->createCamera("main_camera");
	camera->setNearClipDistance(0.1);  // specific to this sample
	camera->setAutoAspectRatio(true);
	camera_nd->attachObject(camera);

	_cameraman = make_unique<CameraMan>(camera_nd);
	_cameraman->setStyle(CS_FREELOOK);
	cout << "camera style: " << to_string(_cameraman->getStyle()) << endl;

	getRenderWindow()->addViewport(camera);  // render into the main window

	auto cube_nodes_it = begin(_cube_nodes);

	// add cubes to scene
	for (cube_object & cube : _cubes)
	{
		Entity * cube_model = scene->createEntity(SceneManager::PT_CUBE);  // TODO: find out how to reuse entities
		cube_model->setMaterialName("cube_color");
		Real model_scale = 0.2 * (2.0 / cube_model->getBoundingBox().getSize().x);
		Real cube_scale = model_scale * cube.scale;

		SceneNode * node = root_nd->createChildSceneNode(cube.position);
		node->setScale(cube_scale, cube_scale, cube_scale);
		node->attachObject(cube_model);

		// save node for later update
		*cube_nodes_it = node;
		++cube_nodes_it;
	}

	// axis
	AxisObject axis;
	Ogre::ManualObject * axis_model = axis.createAxis(scene, "axis", 1.0);
	SceneNode * axis_nd = root_nd->createChildSceneNode();
	axis_nd->attachObject(axis_model);

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
{
	// initialize cubes
	constexpr int cube_count = 300;
	_cubes.resize(cube_count);
	for (cube_object & cube : _cubes)
		cube = new_cube();

	_cube_nodes.resize(cube_count);
}

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

bool ogre_app::frameStarted(FrameEvent const & evt)
{
	// update scene before render
	steady_clock::time_point now = steady_clock::now();
	duration<double> dt = now - _last_frame_tp;
	update(dt);
	_last_frame_tp = now;
	return ApplicationContext::frameStarted(evt);
}


cube_object new_cube()
{
	static random_device rd;
	static default_random_engine rand{rd()};

	return cube_object{
		Vector3{
			(rand() % 15) - 7.f,
			7.f + (rand() % 30),
			(rand() % 15) - 7.f},
		0.7f + ((rand() % 70)/100.f)  // scale between 0.7 and 0.7+0.7
	};
}


namespace std {

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

}  // std

int main(int argc, char * argv[])
{
	ogre_app app;
	app.go();
	return 0;
}
