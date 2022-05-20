#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/objmesh.h"
#include "helper/skybox.h"
#include "helper/plane.h"

class SceneBasic_Uniform : public Scene
{
private:
  
    GLuint vaoHandle, fsQuad, fboHandle, renderTex;
    GLSLProgram prog;
    GLSLProgram skyboxProg;
    GLSLProgram edgeProg;
    GLSLProgram silhouetteProg;
    GLSLProgram waterShader;
     
    std::unique_ptr<ObjMesh> catMesh;
    std::unique_ptr<ObjMesh> sofaMesh;
    std::unique_ptr<ObjMesh> tableMesh;
    std::unique_ptr<ObjMesh> treeMesh;
    
    SkyBox Sky;
    Plane plane;

    float angle;
    float rotation;
    float time;
 
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
