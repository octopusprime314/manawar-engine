ReBoot is a graphics and physics engine framework.

BUILD INSTRUCTIONS:

    create a build folder at root directory
    cd into build
    cmake -G "Visual Studio 14 2015 Win64" ..   OR    cmake -G "Visual Studio 15 2017 Win64" ..
    cmake --build . --config Release

Copy contents of extracted zip folder called demo-party-data.zip at the root directory level located below.
https://drive.google.com/file/d/1DA97KYg-dbORFlPJE6wNrI1h3PXNmACy/view?usp=sharing

Copy the libs/freeimage/lib/FreeImage.dll into the executable directory.

PUNCH LIST:

1) Animation Engine: Takes multiple animation transform influences and generates a single transform.
For example, if a model is transitiong from walking left to right there needs to be an animation engine that interpolates
smoothly between the two animation transforms.  Walking from left to right might also incorporate another transition
animation that is responsible for making the transition smooth so there could be up to 3 different animations needing to 
be interpolated at one single instance.  Another animation influence could be forcing the model to lean forward when walking
at an uphill incline and lean backward when walking at a downhill incline.

2) Physics Engine Scope: Develop physics collision detection capability including sphere-sphere, sphere-triangle detection.  
Sphere-sphere is primarily geared towards colliding with models that are both mobile.  Multiple spheres will define the 
collision geometry for a mobile model.  Sphere-triangle is primarily geared towards models interacting with static models 
including a model colliding with a house or ground terrain.  Triangle-triangle detection will not be incorporated in this 
scope.  Eventually add rigid body mechanics to the physics engine when framework is at an acceptable state.

3) Scripting: Command line terminal that will accept scripts commands to dynamically place objects in a scene for testing.

4) Shadow Mapping: Use a pre render depth buffer to generate a texture that contains depth information about the scene.
In the render phase incorporate this texture into the model's shaders by testing the fragment's depth against the 
depth stored in the shadow texture.  If the fragment's depth is greater than the stored shadow texture's depth then shade
the fragment as barely visible i.e. black.  Only properly shade the fragment will a full color texture when the fragment's
depth is less than or equal to the shadow texture depth. 