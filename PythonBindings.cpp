#include <memory>
#include <pybind11/pybind11.h>

#include "OpenGLUtil.h"
#include "Matrix.h"
#include "Vector4.h"
#include "GameObject.h"
#include "Engine.h"

namespace py = pybind11;

void makeSimpleScene(Engine & engine)
{
    const char * vertex_shader_filename = "shaders/basic.vs";
    const char * fragment_shader_filename = "shaders/basic.fs";

    auto mesh_shader_program = createShaderProgram( vertex_shader_filename, fragment_shader_filename );
    if( !mesh_shader_program ) { exit(EXIT_FAILURE); }

#if 1
    auto hero = std::make_shared<GameObject>( engine.bunnyPath + "/bun_zipper.ply" );
    engine.hero = hero;
    hero->setShaderProgram( mesh_shader_program );
    hero->position = Vector4( 0.0, 0.0, -5.0 );
    hero->animFunc = [](GameObject * self, float gameTime, float deltaTime) {
        Engine & engine = Engine::instance();
        const Vector4 gravity( 0.0, -15.0, 0.0 );
        Vector4 acceleration = gravity;
        self->position += self->velocity * deltaTime;
        self->velocity += acceleration * deltaTime;
        if( self->position.y < 0.0 ) self->position.y = 0.0;
        self->worldTransform = compose( makeTranslation( self->position ),
                                        makeRotation( gameTime * 0.4, Vector4( 0, 1, 0 ) ),
                                        makeScaling( 10.0 ) );
    };
    engine.addGameObject(hero);

#endif
}

PYBIND11_MODULE(le, m) {
    m.doc() = "LittleEngine";

    py::class_<Engine>(m, "Engine")
        .def(py::init<>())
        .def_readwrite("modelPath", &Engine::modelPath)
        .def_readwrite("bunnyPath", &Engine::bunnyPath)
        .def_readwrite("texturePath", &Engine::texturePath)
        .def("createWindow", [](Engine & engine) { engine.createWindow(); })
        .def("start", &Engine::start)
        .def("addGameObject", &Engine::addGameObject)
        .def("loadTexture", &Engine::loadTexture)
        .def("textureIdAtIndex", &Engine::textureIdAtIndex)
        ;

    py::class_<Vector4>(m, "Vector4")
        .def(py::init<>())
        .def(py::init<const Vector4 &>())
        .def(py::init<float, float, float>())
        .def(py::init<float, float, float, float>())
        ;

    py::class_<GameObject, std::shared_ptr<GameObject>>(m, "GameObject")
        .def(py::init<>())
        .def(py::init([](const std::string & path) {
                return std::make_shared<GameObject>(path);
             }))
        .def_readwrite("position", &GameObject::position)
        .def_readwrite("scaleFactor", &GameObject::scaleFactor)
        .def("addRenderable", &GameObject::addRenderable)
        .def("setShaderProgram", &GameObject::setShaderProgram)
        .def("setRoughness", &GameObject::setRoughness)
        .def("setF0", &GameObject::setF0)
        .def("setTexture", &GameObject::setTexture)
        ;

    py::class_<Renderable, std::shared_ptr<Renderable>>(m, "Renderable")
        .def("setShaderProgram", &Renderable::setShaderProgram)
        ;

    py::class_<Mesh, Renderable, std::shared_ptr<Mesh>>(m, "Mesh")
        .def("upload", &Mesh::upload)
        .def("setTexture", &Mesh::setTexture)
        ;

    py::class_<Program, std::shared_ptr<Program>>(m, "Program");

    m.def("createShaderProgram", &createShaderProgram);

    m.def("makeMeshGroundPlatform", &makeMeshGroundPlatform);

    // TEMP
    m.def("makeSimpleScene", &makeSimpleScene);

}

