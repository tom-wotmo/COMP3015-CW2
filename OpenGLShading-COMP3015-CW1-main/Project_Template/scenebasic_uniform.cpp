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
using glm::vec4;
using glm::mat4;
using glm::mat3;
GLuint sofaTex, catTex, tableTex, treeTex;
GLuint skyBoxNightTex, skyBoxDayTex;

//ability to control shaders from an interface
//spotlight = 1
//blinnphong = 2
//treemodel = 1
//sofamodel = 2
int shaderInt = 3;
int modelInt = 2;
int SkyBoxInt = 0;

SceneBasic_Uniform::SceneBasic_Uniform() : rotation(0.0f)
{
    //loading in our models
    sofaMesh = ObjMesh::load("sofa.obj", true);
    treeMesh = ObjMesh::load("treeModel.obj", true);
    sofaTex = Texture::loadTexture("sofa_D.png");
    treeTex = Texture::loadTexture("treeModel.png");

}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);

    //view and projection
    view = glm::lookAt(vec3(0.0f, 0.0f, 9.5f), vec3(0.0f, 0.0f, 0.0f),vec3(0.0f, 12.0f, 0.0f));
    projection = mat4(1.0f);

  
    GLuint skyBoxTex = Texture::loadHdrCubeMap("media/pisa/pisa");

    //activate and bindtexture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTex);

  
}

void SceneBasic_Uniform::compile()
{
	try {

		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
        
        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.link();

        edgeProg.compileShader("shader/edgedetection.vert");
        edgeProg.compileShader("shader/edgedetection.frag");
        edgeProg.link();
       

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
    view = glm::lookAt(vec3(0.0f, 0.0f, 9.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 12.0f, 0.0f));
    view = glm::rotate(view, glm::radians(30.0f * rotation), vec3(0.0f, 1.0f, 0.0f));

 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //SkyBoxSetup();

    //load the fog
 
    //spotlight shader
    if (shaderInt == 1)
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prog.setUniform("Fog.MaxDist", 150.0f);
        prog.setUniform("Fog.MinDist", 100.0f);
        prog.setUniform("Fog.Colour", vec3(1.0f, 1.0f, 1.0f));

        prog.use();
        prog.setUniform("shaderInt", 1);

        prog.setUniform("Spot.L", vec3(0.5f));
        prog.setUniform("Spot.La", vec3(0.5f));
        prog.setUniform("Spot.Exponent", 1.0f);
        prog.setUniform("Spot.Cutoff", glm::radians(20.0f));


        mat4 spotLightPos = glm::lookAt(vec3(1.0f, -0.1f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        vec4 lightPos = vec4(0.0f, 2.0f, 1.0f, 0.0f);
        prog.setUniform("Spot.Position", vec3(spotLightPos * lightPos));
        mat3 normalMatrix = mat3(vec3(spotLightPos[0]), vec3(spotLightPos[0]), vec3(spotLightPos[0]));
        prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

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
        }
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

    }
    if (shaderInt == 2) 
    {
        vec3 lightpos = vec3(0.0f, 1.0f, 1.0f);

        prog.setUniform("Fog.MaxDist", 150.0f);
        prog.setUniform("Fog.MinDist", 100.0f);
        prog.setUniform("Fog.Colour", vec3(1.0f, 1.0f, 1.0f));

        prog.use();
        prog.setUniform("shaderInt", 2);
        prog.setUniform("Lights.La", vec3(0.5f));
        prog.setUniform("Lights.L", vec3(0.5f));
        prog.setUniform("Lights.Position", lightpos);

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
        }
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
    }
    if (shaderInt == 3)
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        edgeProg.use();

        edgeDetectSetup();

        pass1();
        glFlush();
        pass2();
    }
 

}
//edge detection
void SceneBasic_Uniform::edgeDetectSetup() 
{

    setupFBO();

    // Array for full-screen quad
    GLfloat verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // Set up the buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2); // Texture coordinates

    glBindVertexArray(0);

    edgeProg.setUniform("EdgeThreshold", 0.05f);
    edgeProg.setUniform("Light.L", vec3(1.0f));
    edgeProg.setUniform("Light.La", vec3(0.2f));
}

void SceneBasic_Uniform::setupFBO()
{
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    // Create the texture object
    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        renderTex, 0);
    // Create the depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    // Bind the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, depthBuf);
    // Set the targets for the fragment output variables
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
void SceneBasic_Uniform::pass1() 
{
    edgeProg.setUniform("Pass", 1);

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


   view = glm::lookAt(vec3(10.0f, 10.0f, 9.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));
   view = glm::rotate(view, glm::radians(30.0f * rotation), vec3(0.0f, 1.0f, 0.0f));

    edgeProg.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));

    if (modelInt == 1) 
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, treeTex);

        edgeProg.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
        edgeProg.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
        edgeProg.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
        edgeProg.setUniform("Material.Shininess", 1.0f);
        model = mat4(1.0f);
        model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, vec3(0.045f, 0.045f, 0.045f));
        model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));

        setMatrices(edgeProg);
        treeMesh->render();
    }
    if (modelInt == 2)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sofaTex);

        edgeProg.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
        edgeProg.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
        edgeProg.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
        edgeProg.setUniform("Material.Shininess", 1.0f);
        model = mat4(1.0f);
        model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
       // model = glm::scale(model, vec3(0.05f, 0.05f, 0.05f));
        model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));

        setMatrices(edgeProg);
        sofaMesh->render();

    }
}
void SceneBasic_Uniform::pass2()
{
    edgeProg.setUniform("Pass", 2);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices(edgeProg);

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

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
void SceneBasic_Uniform::setMatrices(GLSLProgram& progPass)
{
    mat4 mv = view * model;
    progPass.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix
    progPass.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //we set the uniform for normal matrix
    progPass.setUniform("MVP", projection * mv); //we set the model view matrix by multiplying the mv with the projection matrix

}

