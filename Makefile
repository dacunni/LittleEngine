
HDR = \
	AssetLoader.h \
	Color.h \
	GameObject.h \
	Renderable.h \
	Mesh.h \
	PointCloud.h \
    Image.h \
	Material.h \
	Matrix.h \
	OpenGLUtil.h \
	RandomNumberGenerator.h \
	Ray.h \
	Scene.h \
	Shader.h \
	ShaderProgram.h \
	Sphere.h \
	Timer.h \
	Traceable.h \
	Transform.h \
	Vector.h \
	Vector4.h \
	Vector2.h \
	Vector4.hpp \
	Vector2.hpp


leOBJ = \
	AssetLoader.o \
	GameObject.o \
	Renderable.o \
	Mesh.o \
	PointCloud.o \
	Matrix.o \
	OpenGLUtil.o \
	RandomNumberGenerator.o \
	Ray.o \
	Scene.o \
	ShaderProgram.o \
	Sphere.o \
	Timer.o \
	Traceable.o \
	Transform.o \
	Vector.o \
	ui.o

INC = -I/usr/local/include
INC += -I/usr/include/ImageMagick
INC += -I/usr/local/include/ImageMagick-6
CXXFLAGS = -std=c++11
CXXFLAGS += -mmacosx-version-min=10.10
CXXFLAGS += -Wno-deprecated
CXXFLAGS += -O2
CXXFLAGS += -g
LDXXFLAGS = -e _main -lassimp -lMagick++-6.Q16 -lm -lc++ -lc -macosx_version_min 10.10
leLDXXFLAGS = $(LDXXFLAGS) -framework GLUT -framework OpenGL

all: le

# Stash object files away in a separate directory so we don't have 
# to look at them
OBJDIR = objs
leOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(leOBJ))
$(leOBJ_IN_DIR): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

le: $(leOBJ_IN_DIR)
	ld $(leOBJ_IN_DIR) -o $@ $(leLDXXFLAGS)

$(OBJDIR)/%.o : %.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INC)

clean:
	rm -rf $(leOBJ_IN_DIR) le
	rmdir $(OBJDIR)

