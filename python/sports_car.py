import os
import sys
from le import *

engine = Engine()
engine.createWindow()

#cornell_path = engine.modelPath + "/cornell-box/CornellBox-Original.obj"
cornell_path = engine.modelPath + "/casual-effects.com/sportsCar/sportsCar.obj"

shader = createShaderProgram("shaders/basic.vs", "shaders/basic.fs")
#shader = createShaderProgram("shaders/basic.vs", "shaders/cooktorrance.fs")

obj = GameObject(cornell_path)
obj.setShaderProgram(shader)
engine.addGameObject(obj)

print("starting engine")
engine.start()

