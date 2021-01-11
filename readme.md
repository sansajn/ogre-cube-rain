# cube rain

One of my OGRE based mini project, the goal here is to make rain of cubes with a camera system and diffuse lighting. Sample shows how to set up a scene with camera and moving objects in it all rendered with diffuse based lighting. How to handle user input (mouse and keyboard) to change camera rotation.

![cube rain screenshot](cube_rain.png)



## controls

**space:** freeze time, **left mouse:** rotate camera, **right mouse:** pan camera

## building

Building is easy, all we need to do is

```sh
cd ogre-cube-rain
scons -j8
```

but before we can do it we need to install *glfw3*, *glew*, *glm* and *scons* libraries.

> **Note:** in Ubuntu 19.10 use
>
> ```sh
> sudo apt install libglfw3-dev libglew-dev libglm-dev scons
> ```
>
> command.

After successful build, `./cube_rain` command can be used to run sample.
