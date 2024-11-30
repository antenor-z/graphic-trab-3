
#ifdef _WIN32
#include <glad/glad.h>
#elif __linux__
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#else
#include <OpenGL/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include "arcball.h"
#include "scene.h"
#include "state.h"
#include "camera3d.h"
#include "material.h"
#include "texture.h"
#include "transform.h"
#include "cube.h"
#include "quad.h"
#include "sphere.h"
#include "error.h"
#include "shader.h"
#include "light.h"
#include "light.h"
#include "polyoffset.h"
#include "clipplane.h"


#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

static ScenePtr scene;
static ScenePtr reflector;
static Camera3DPtr camera;
static ArcballPtr arcball;

static void initialize (void)
{
  // set background color: white 
  glClearColor(0.8f,0.8f,0.8f,1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);  // cull back faces

  // create objects
  camera = Camera3D::Make(0,0,4);
  //camera->SetOrtho(true);
  arcball = camera->CreateArcball();

  //LightPtr light = ObjLight::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
  LightPtr light = Light::Make(0.0f,0.0f,0.0f,1.0f,"camera");
  light->SetAmbient(0.15f, 0.15f, 0.15f);
  light->SetSpecular(0.8f, 0.8f, 0.8f);

  ClipPlanePtr clipplane = ClipPlane::Make("clipplane");
  clipplane->AddPlane(0.0f,-1.0f,0.0f,0.5f);


  MaterialPtr white = Material::Make(1.0f,1.0f,1.0f);
  MaterialPtr transparent = Material::Make(0.0f,0.0f,0.0f,0.0f);
  MaterialPtr red = Material::Make(1.0f,0.3f,0.3f);
  MaterialPtr yellow = Material::Make(1.0f,1.0f,0.3f);
  MaterialPtr green = Material::Make(0.3f,1.0f,0.3f);


  TransformPtr trCube1 = Transform::Make();
  trCube1->Translate(-1.5f,0.0f,-1.5f);
  trCube1->Rotate(90, 1.0f,0.0f,0.0f);
  trCube1->Scale(3.0f,3.0f,3.0f);
  TransformPtr trCube2 = Transform::Make();
  trCube2->Scale(1.0f,0.4f,1.0f);
  trCube2->Translate(0.0f,0.001f,0.0f);
  TransformPtr trCube3 = Transform::Make();
  trCube3->Scale(0.4f,0.4f,0.4f);
  trCube3->Translate(-2.5f,0.4f,-2.5f);
  trCube3->Rotate(45.0f,1.0f,1.0f,1.0f);
  TransformPtr trSphere1 = Transform::Make();
  trSphere1->Scale(0.2f,0.2f,0.2f);
  trSphere1->Translate(0.0f,3.0f,0.0f);
  TransformPtr trSphere2 = Transform::Make();
  trSphere2->Scale(0.4f,0.4f,0.4f);
  trSphere2->Translate(2.5f,1.0f,2.5f);

TransformPtr trUnit = Transform::Make();
  trUnit->Scale(0.3f,0.3f,0.3f);
  trUnit->Translate(3.0f,1.0f,3.0f);

  Error::Check("before shps");
  ShapePtr cube = Cube::Make();
  Error::Check("before quad");
  ShapePtr quad = Quad::Make();
  Error::Check("before sphere");
  ShapePtr sphere = Sphere::Make();
  Error::Check("after shps");

  // create shader
  ShaderPtr shader = Shader::Make(light,"world");
  shader->AttachVertexShader("../shaders/ilum_vert/vertex.glsl");
  shader->AttachFragmentShader("../shaders/ilum_vert/fragment.glsl");
  shader->Link();

  // build scene
  NodePtr root = Node::Make(shader,
    {
     Node::Make(trCube2,{yellow},{cube}),
     Node::Make(trSphere1,{green},{sphere}),
     Node::Make(trSphere2,{red},{sphere}),
     Node::Make(trCube3,{red},{cube}),
    }
  );
  scene = Scene::Make(root);
  reflector = Scene::Make(Node::Make(shader ,trCube1 ,{transparent},{quad}));

  
}

static void display (GLFWwindow* win)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (camera->GetViewMatrix()[1][2] > 0) { // Estamos vendo de cima?
        // refletor no stencil
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NEVER , 1, 0xFFFF);
        glStencilOp(GL_REPLACE , GL_REPLACE , GL_REPLACE);
        reflector->Render(camera);
        // desenha cena refletida
        glStencilFunc(GL_EQUAL , 1, 0xFFFF);
        glStencilOp(GL_KEEP , GL_KEEP , GL_KEEP);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // desenha cena refletida
        NodePtr root = scene->GetRoot();
        TransformPtr trf = Transform::Make();
        trf->Scale(1.0f,-1.0f,1.0f);
        root->SetTransform(trf);
        glFrontFace(GL_CW);
        // invert front face incidence
        scene->Render(camera);
        glFrontFace(GL_CCW);
        // restore front face incidence
        root->SetTransform(nullptr);
        // desenha cena
        scene->Render(camera);
        // desenha refletor
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
        reflector->Render(camera);
        glDisable(GL_BLEND);

        glDisable(GL_STENCIL_TEST);

    }
    // desenha cena
    scene->Render(camera);
    // desenha refletor
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
    reflector->Render(camera);
    glDisable(GL_BLEND);


}

static void error (int code, const char* msg)
{
  printf("GLFW error %d: %s\n", code, msg);
  glfwTerminate();
  exit(0);
}

static void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize (GLFWwindow* win, int width, int height)
{
  glViewport(0,0,width,height);
}

static void cursorpos (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->AccumulateMouseMotion(x,y);
}
static void cursorinit (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->InitMouseMotion(x,y);
  glfwSetCursorPosCallback(win, cursorpos);     // cursor position callback
}
static void mousebutton (GLFWwindow* win, int button, int action, int mods)
{
  if (action == GLFW_PRESS) {
    glfwSetCursorPosCallback(win, cursorinit);     // cursor position callback
  }
  else // GLFW_RELEASE 
    glfwSetCursorPosCallback(win, nullptr);      // callback disabled
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);       // required for mac os
    #ifndef _WIN32
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER,GLFW_TRUE);  // option for mac os
    #endif

    glfwSetErrorCallback(error);

    GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "Tarefa 2.1", NULL, NULL);
    if (!win) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(win, resize);  // resize callback
    glfwSetKeyCallback(win, keyboard);            // keyboard callback
    glfwSetMouseButtonCallback(win, mousebutton); // mouse button callback

    glfwMakeContextCurrent(win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    initialize();

    float t0 = glfwGetTime();
    while(!glfwWindowShouldClose(win)) {
        float t = glfwGetTime();
        //update(t-t0);
        t0 = t;
        display(win);
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
    }
