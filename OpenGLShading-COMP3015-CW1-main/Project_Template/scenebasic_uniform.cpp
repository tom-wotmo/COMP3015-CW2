#include "scenebasic_uniform.h"
#include "helper/texture.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <sstream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>

#include "helper/glutils.h"


using glm::vec3;
using glm::mat4;
GLuint sofaTex, catTex, tableTex, treeTex;
GLuint skyBoxNightTex, skyBoxDayTex;

//ability to control shaders from an interface
//spotlight = 1
//blinnphong = 2
//treemodel = 1
//sofamodel = 2
int shaderInt = 1;
int modelInt = 1;
int SkyBoxInt = 0;

SceneBasic_Uniform::SceneBasic_Uniform() : rotation(0.0f), Sky(500.0f)
{
    //loading in our models
    sofaMesh = ObjMesh::load("sofa.obj", true);
    treeMesh = ObjMesh::load("treeModel.obj", true);
    sofaTex = Texture::loadTexture("sofa_D.png");
    treeTex = Texture::loadTexture("treeModel.png");
    skyBoxNightTex = Texture::loadCubeMap("media/pisa/pisa");
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);

    //view and projection
    view = glm::lookAt(vec3(0.5f, 0.75f, 0.75f), vec3(0.0f, 0.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);

    //initialise the model matrix
    model = mat4(1.0f);
    projection = mat4(1.0f);
    vec3 lightpos = vec3(0.0f, 1.0f, 1.0f);

   
    //blinn phong spotlight
    if (shaderInt == 1) 
    {
        prog.use();
        prog.setUniform("shaderInt", 1);
        prog.setUniform("Spot.L", vec3(2.0f));
        prog.setUniform("Spot.La", vec3(2.0f));
        prog.setUniform("Spot.Exponent", 10.0f);
        prog.setUniform("Spot.Cutoff", glm::radians(20.0f));
        
    }
    //blinnphong
    if (shaderInt == 2) 
    {
        prog.use();
        prog.setUniform("shaderInt", 2);
        prog.setUniform("Lights.La", vec3(0.5f));
        prog.setUniform("Lights.L", vec3(0.5f));
        prog.setUniform("Lights.Position", lightpos);
       
    }


}

void SceneBasic_Uniform::compile()
{
	try {

		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
        
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.link();
       

	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
   
    //controlling the rotating around the model
    rotation = t;

}


void SceneBasic_Uniform::render()
{
    view = glm::lookAt(vec3(10.0f, 5.0f, 4.15f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));
    view = glm::rotate(view, glm::radians(30.0f * rotation), vec3(0.0f, 1.0f, 0.0f));

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //SkyBoxSetup();

    //load the fog
    prog.setUniform("Fog.MaxDist", 150.0f);
    prog.setUniform("Fog.MinDist", 100.0f);
    prog.setUniform("Fog.Colour", vec3(1.0f,1.0f,1.0f));

 
    if (modelInt == 1) 
    {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, treeTex);

        prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
        prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
        prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
        prog.setUniform("Material.Shininess", 1.0f);
        model = mat4(1.0f);
        model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, vec3(0.45f, 0.45f, 0.45f));
        model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));

        prog.setUniform("Tex1", 0);
        setMatrices(prog);
        treeMesh->render();
        if (SkyBoxInt == 1) 
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxNightTex);
            skyboxProg.setUniform("SkyBoxTex", 0);

        }


    }

    //render the sofa
    if (modelInt == 2)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sofaTex);

        prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
        prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
        prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
        prog.setUniform("Material.Shininess", 1.0f);
        model = mat4(1.0f);
        model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, vec3(0.05f, 0.05f, 0.05f));
        model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));

        prog.setUniform("Tex1", 0);
        setMatrices(prog);
        sofaMesh->render();
    }

    //SkyBoxSetup();

 

}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::ImGuiSetup()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

}
void SceneBasic_Uniform::SkyBoxSetup()
{

    skyboxProg.use();
    model = mat4(1.0f);
    setMatrices(skyboxProg);
    Sky.render();
}
void SceneBasic_Uniform::setMatrices(GLSLProgram& prog)
{
    mat4 mv = view * model;
    prog.setUniform("MVP", projection * mv); //we set the model view matrix by multiplying the mv with the projection matrix
    prog.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //we set the uniform for normal matrix

}

