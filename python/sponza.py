import os
import sys
from le import *

print(f"Python startup cwd: {os.getcwd()}")

engine = Engine()
engine.createWindow()

mitsuba_path = engine.modelPath + "/test_objects/mitsuba/mitsuba-sphere.obj"

sponza_path = engine.modelPath + "/dabrovic-sponza/sponza.obj";
#sponza_path = engine.modelPath + "/crytek-sponza/sponza.obj";
#sponza_path = engine.modelPath + "/casual-effects.com/fireplace_room/fireplace_room.obj";

default_shader = createShaderProgram("shaders/basic.vs", "shaders/basic.fs")
cook_torrance_shader = createShaderProgram("shaders/basic.vs", "shaders/cooktorrance.fs")

# Load Textures

uv_grid_texture_index = engine.loadTexture(engine.texturePath + "/uvgrid.jpg")
uv_grid_texture_id = engine.textureIdAtIndex(uv_grid_texture_index)

# Ground

obj = GameObject()
ground = makeMeshGroundPlatform(30.0)
ground.upload()
ground.setShaderProgram(default_shader)
ground.setTexture(uv_grid_texture_id)
obj.position = Vector4(0.0, -0.01, 0.0)
obj.addRenderable(ground)
engine.addGameObject(obj)

# Other objects

obj = GameObject(sponza_path)
obj.setShaderProgram(default_shader)
#obj.setShaderProgram(cook_torrance_shader)
obj.position = Vector4(0.0, 0.0, 0.0)
#obj.setRoughness(0.3)
#obj.setTexture(rust_paint_texture_id)
engine.addGameObject(obj)

#obj = GameObject(mitsuba_path)
##obj.setShaderProgram(default_shader)
#obj.setShaderProgram(cook_torrance_shader)
#obj.position = Vector4(0.0, 0.0, 0.0)
#obj.setRoughness(0.3)
#engine.addGameObject(obj)


print("starting engine")
engine.start()

