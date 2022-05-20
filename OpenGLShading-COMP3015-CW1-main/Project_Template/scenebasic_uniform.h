#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/objmesh.h"
#include "helper/skybox.h"

class SceneBasic_Uniform : public Scene
{
private:
  
    GLuint vaoHandle, fsQuad, fboHandle, renderTex;
    GLSLProgram prog;
    GLSLProgram skyboxProg;
    GLSLProgram edgeProg;
  
     
    std::unique_ptr<ObjMesh> catMesh;
    std::unique_ptr<ObjMesh> sofaMesh;
    std::unique_ptr<ObjMesh> tableMesh;
    std::unique_ptr<ObjMesh> treeMesh;
    
    SkyBox Sky;

    float angle;
    float rotation;
 
    void compile();
    void ImGuiSetup();
    void SkyBoxSetup();

    void edgeDetectSetup();
    void setupFBO();
    void pass1();
    void pass2();
 
    

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);

    void setMatrices(GLSLProgram& prog);
};

#endif // SCENEBASIC_UNIFORM_H
