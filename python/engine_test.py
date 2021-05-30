import os
import sys
from le import *

print(f"Python startup cwd: {os.getcwd()}")

engine = Engine()
engine.createWindow()

mitsuba_path = engine.modelPath + "/test_objects/mitsuba/mitsuba-sphere.obj"
hero_path = engine.bunnyPath + "/bun_zipper.ply"

# TEMP
makeSimpleScene(engine)

default_shader = createShaderProgram("shaders/basic.vs", "shaders/basic.fs")
cook_torrance_shader = createShaderProgram("shaders/basic.vs", "shaders/cooktorrance.fs")

# Load Textures

uv_grid_texture_index = engine.loadTexture(engine.texturePath + "/uvgrid.jpg")
uv_grid_texture_id = engine.textureIdAtIndex(uv_grid_texture_index)
rust_paint_texture_index = engine.loadTexture(engine.texturePath + "/Rust_Paint_03_UV_H_CM_1.jpg")
rust_paint_texture_id = engine.textureIdAtIndex(rust_paint_texture_index)

# Hero

hero = GameObject(hero_path)
hero.setShaderProgram(cook_torrance_shader)
hero.position = Vector4(5.0, 0.0, -5.0)
engine.addGameObject(hero)


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

obj = GameObject(mitsuba_path)
obj.setShaderProgram(cook_torrance_shader)
obj.position = Vector4(-3.0, 0.0, 0.0)
engine.addGameObject(obj)

obj = GameObject(mitsuba_path)
obj.setShaderProgram(default_shader)
obj.position = Vector4(0.0, 0.0, 0.0)
obj.setTexture(uv_grid_texture_id)
engine.addGameObject(obj)

obj = GameObject(mitsuba_path)
obj.setShaderProgram(cook_torrance_shader)
obj.position = Vector4(3.0, 0.0, 0.0)
obj.setRoughness(0.3)
obj.setTexture(rust_paint_texture_id)
engine.addGameObject(obj)

# Cook-Torrance Varying Roughness

num_obj = 6
for i in range(num_obj):
    obj = GameObject(mitsuba_path)
    obj.setShaderProgram(cook_torrance_shader)
    obj.position = Vector4(-6.0, 0.0, (i - num_obj * 0.5) * 3)
    obj.setRoughness(i / (num_obj - 1))
    engine.addGameObject(obj)


print("starting engine")
engine.start()

