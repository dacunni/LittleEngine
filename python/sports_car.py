import os
import sys
from le import *

engine = Engine()
engine.createWindow()

# Shaders
#shader = createShaderProgram("shaders/basic.vs", "shaders/basic.fs")
shader = createShaderProgram("shaders/basic.vs", "shaders/cooktorrance.fs")

# Textures
uv_grid_texture_index = engine.loadTexture(engine.texturePath + "/uvgrid.jpg")
uv_grid_texture_id = engine.textureIdAtIndex(uv_grid_texture_index)

sports_car_path = engine.modelPath + "/casual-effects.com/sportsCar/sportsCar.obj"
obj = GameObject(sports_car_path)
obj.setShaderProgram(shader)
obj.setRoughness(0.2)
obj.setF0(0.7)
obj.scaleFactor = 1.3
obj.position = Vector4(-3.0, 0.0, 0.0)
engine.addGameObject(obj)

bmw_path = engine.modelPath + "/casual-effects.com/bmw/bmw.obj";
obj = GameObject(bmw_path)
obj.setShaderProgram(shader)
obj.setRoughness(0.2)
obj.setF0(0.7)
obj.scaleFactor = 0.008
obj.position = Vector4(3.0, 0.0, 0.0)
engine.addGameObject(obj)

# Ground

obj = GameObject()
ground = makeMeshGroundPlatform(30.0)
ground.upload()
ground.setShaderProgram(shader)
ground.setTexture(uv_grid_texture_id)
obj.addRenderable(ground)
obj.setRoughness(0.2)
obj.position = Vector4(0.0, -0.01, 0.0)
engine.addGameObject(obj)


print("starting engine")
engine.start()

