# OpenGLProject

This is an Computer graphic class project using glad, glfw, glm

Make sure your have configured glad and glfw to run these code.

Features:
- Import model from files
- Translation, rotation with model 
- Control the camera position
- Add spotlights, direction lights and point lights
- Increase or decrease the number of point lights
- Dynamic point lights (point light position can move around)
- Add different material to the model

### Import Model
To import a model into this application is very easy. Users just need to drag a file to this window and a model will be displayed in this window.

![image](https://github.com/BingXiong1995/OpenGLProject/blob/master/Images/1.png?raw=true)

### Transformation
•	N – Enable transformation shader (press again will disable and set the location of model to default)
•	Press 1,2,3 to Rotate
•	Press 4,5,6,7 – to translation
![image](https://github.com/BingXiong1995/OpenGLProject/blob/master/Images/2.png?raw=true)

### Camera
To control the camera, need to enable material shader first, because the code of control camera is in material shader, press Z to enable material shade.
- W – move forward
- S – move backward
- A – move left  
- D – move right  
- E – move down 
- T – move up
- Mouse input

![image](https://github.com/BingXiong1995/OpenGLProject/blob/master/Images/3.png?raw=true)

### Material
- Z - add material and one light
- M - Bronze material
- P - White plastic material
- C - Ruby Material
- O - Dynamic Material

![image](https://github.com/BingXiong1995/OpenGLProject/blob/master/Images/4.png?raw=true)

![image](https://github.com/BingXiong1995/OpenGLProject/blob/master/Images/5.png?raw=true)

### Lighting
-	V - Disable all the shaders
-	X – Enable Multiple lights, direction lights and points light
-	Press mouse left button or right button to increase or decrease the number of points lights.
-	Z – Enable Dynamic point light (need to press V to disable lighting shader)
-	R – show rotate light.
![image](https://github.com/BingXiong1995/OpenGLProject/blob/master/Images/7.png?raw=true)
![image](https://github.com/BingXiong1995/OpenGLProject/blob/master/Images/6.png?raw=true)
