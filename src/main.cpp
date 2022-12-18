#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>



#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3        (x,y,z)
#include <glm/vec4.hpp> // glm::vec4        (x,y,z,w)
#include <glm/mat4x4.hpp> // glm::mat4
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Shader/cShaderManager.h"
#include "VAOManager/cVAOManager.h"
#include "MeshObj/cMeshObj.h"
#include "Light/cLightManager.h"
#include "GUI/cGUI.h"
#include "Texture/cTextureManager.h"
#include "time.h"
#include "GameObj/cGameObj.h"
#include "Lua/cLuaBrain.h"

#define MODEL_LIST_XML          "asset/model.xml"
#define VERTEX_SHADER_FILE      "src/shader/vertexShader.glsl"
#define FRAGMENT_SHADER_FILE    "src/shader/fragmentShader.glsl"
#define TEXTURE_PATH            "asset/texture"

glm::vec3 g_cameraEye = glm::vec3(0.0, 5.0, 0.0f);
glm::vec3 g_cameraTarget = glm::vec3(-16.0f, 4.0f, 0.0f);
glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 g_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
bool bIsWalkAround = false;
bool firstMouse = true;
float yaw = -90.0f;	
float pitch = 0.0f;
float lastX = 1280.0f / 2.0;
float lastY = 800.0 / 2.0;
float fov = 45.0f;
int toggle = 0;

double g_LastCall;
double g_LastCall5s;
double g_CurrentTime;

const int FRAMES_PER_SECOND = 30;
const double FRAME_RATE = (double)1 / FRAMES_PER_SECOND;

#define SEC_UPDATE 5

cLightManager* g_pTheLightManager = NULL;
static GLFWwindow* window = nullptr;
cTextureManager* g_pTextureManager = NULL;
cLuaBrain* g_Brain = NULL;
std::vector<std::string> cmdA, cmdB, cmdC;
cGameObj* p_Beholder1;
cGameObj* p_Beholder2;
cGameObj* p_Beholder3;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void updateInstanceObj(cShaderManager* pShaderManager, cVAOManager* pVAOManager, glm::mat4x4 matView, glm::mat4x4 matProjection);
void drawObj(cMeshObj* pCurrentMeshObject, glm::mat4x4 mat_PARENT_Model, cShaderManager* pShaderManager, cVAOManager* pVAOManager, glm::mat4x4 matView, glm::mat4x4 matProjection);
void light0Setup();
void light1Setup(cVAOManager* pVAOManager);
void light2Setup(cVAOManager* pVAOManager);
//void light3Setup();
//void light4Setup();

void updateByFrameRate();
void patrol(cGameObj* obj, std::vector<std::string> cmd);

int main(void)
{

    bool result;
    std::cout << "starting..\n";
    //GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    //GLint mvp_location, vpos_location, vcol_location;
    GLuint shaderID = 0;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWmonitor* pMainScreen = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(pMainScreen);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    //window = glfwCreateWindow(mode->width, mode->height, "6028 Final", pMainScreen, NULL); //full screen
    window = glfwCreateWindow(1280, 800, "6028 Final", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    std::cout << "created window" << std::endl;

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //initialize imgui
    cGUI* gui_ = new cGUI(&g_cameraEye,&g_cameraTarget);
    result = gui_->ImGUI_init(window);
    if (!result)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    //create shader program
    cShaderManager* pShaderManager = new cShaderManager();
    cShaderManager::cShader vertexShader;
    cShaderManager::cShader fragmentShader;

    vertexShader.fileName = VERTEX_SHADER_FILE;
    fragmentShader.fileName = FRAGMENT_SHADER_FILE;
    result = pShaderManager->createShaderProgram("Shader01", vertexShader, fragmentShader);
    if (!result)
    {
        std::cout << "error: Shader compilation fail" << std::endl;

        glfwDestroyWindow(window);

        delete pShaderManager;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "shader compilation OK" << std::endl;
    }

    pShaderManager->useShaderPRogram("Shader01");
    shaderID = pShaderManager->getIDfromName("Shader01");
    glUseProgram(shaderID);

    //todo lighting
    ::g_pTheLightManager = new cLightManager();


    
    ::g_pTheLightManager->loadLightUniformLocation(shaderID);
    for (size_t i = 0; i < MAX_LIGHT_SOURCE; i++)
    {
        gui_->pLight[i] = ::g_pTheLightManager->plight[i];
    }
    //load model
    cVAOManager* pVAOManager = new cVAOManager();

    result = pVAOManager->loadModelList(MODEL_LIST_XML, shaderID);
    if (!result)
    {
        std::cout << "cannot load model list" << std::endl;

        glfwDestroyWindow(window);

        delete pVAOManager;
        delete pShaderManager;

        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    ::g_cameraEye = pVAOManager->cameraEyeFromXML;

    //gui_->pMapInstanceNametoMeshObj = &pVAOManager->mapInstanceNametoMeshObj;
    gui_->pVecInstanceMeshObj = &pVAOManager->pVecInstanceMeshObj;

    //load texture
    ::g_pTextureManager = new cTextureManager();
    ::g_pTextureManager->setBasePath(TEXTURE_PATH);
    ::g_pTextureManager->create2DTextureFromBMP("Dungeons_2_Texture_01_A.bmp");
    ::g_pTextureManager->create2DTextureFromBMP("lroc_color_poles_4k.bmp");
    ::g_pTextureManager->create2DTextureFromBMP("glowing-fire-flame.bmp");
    ::g_pTextureManager->create2DTextureFromBMP("glowing-fire-flame_bw.bmp");
    ::g_pTextureManager->create2DTextureFromBMP("photos_2018_7_4_fst_water-blue.bmp"); 
    ::g_pTextureManager->create2DTextureFromBMP("Beholder_Base_color.bmp");

    std::string load_texture_error = "";
    if (g_pTextureManager->createCubeTextureFromBMP("SpaceBox",
        "SpaceBox_right1_posX.bmp", /* positive X */
        "SpaceBox_left2_negX.bmp",  /* negative X */
        "SpaceBox_top3_posY.bmp",    /* positive Y */
        "SpaceBox_bottom4_negY.bmp",  /* negative Y */
        "SpaceBox_front5_posZ.bmp",  /* positive Z */
        "SpaceBox_back6_negZ.bmp", /* negative Z */
        true, load_texture_error))
    {
        std::cout << "Loaded the tropical sunny day cube map OK" << std::endl;
    }
    else
    {
        std::cout << "ERROR: Didn't load the tropical sunny day cube map.->" << load_texture_error << std::endl;
    }
    //setup object

    result = pVAOManager->setDungeonTexture("floorA", "Dungeons_2_Texture_01_A.bmp");
    result = pVAOManager->setTexture("moon", "lroc_color_poles_4k.bmp", 0);
    result = pVAOManager->setInstanceObjPosition("moon", glm::vec4(200.f,200.f,-100.f,0.f));
    result = pVAOManager->setInstanceObjScale("moon", 10);
    result = pVAOManager->setInstanceObjLighting("moon", false);
    result = pVAOManager->setTexture("water", "photos_2018_7_4_fst_water-blue.bmp", 0);
    result = pVAOManager->setInstanceObjRGB("water", glm::vec4(1.f, 1.f, 1.f, 0.5f));
    result = pVAOManager->setTorchTexture("flame", "glowing-fire-flame.bmp", "glowing-fire-flame_bw.bmp");
    
    result = pVAOManager->setIslandModelFlag("water", true);

    result = pVAOManager->setTexture("boss1", "Beholder_Base_color.bmp", 0);
    result = pVAOManager->setTexture("boss2", "Beholder_Base_color.bmp", 0);
    result = pVAOManager->setTexture("boss3", "Beholder_Base_color.bmp", 0);
    result = pVAOManager->bindingChild("boss1_vision", "boss1");
    result = pVAOManager->bindingChild("boss2_vision", "boss2");
    result = pVAOManager->bindingChild("boss3_vision", "boss3");
    result = pVAOManager->setSkyBoxFlag("skybox",true);

    ::g_Brain = new cLuaBrain();
    std::vector< cGameObj* > vec_pGOs;
    p_Beholder1 = new cGameObj();
    p_Beholder2 = new cGameObj();
    p_Beholder3 = new cGameObj();
    //cGameObj* cam = new cGameObj();

    p_Beholder1->pMeshObj = pVAOManager->findMeshObjAddr("boss1");
    p_Beholder2->pMeshObj = pVAOManager->findMeshObjAddr("boss2");
    p_Beholder3->pMeshObj = pVAOManager->findMeshObjAddr("boss3");

    p_Beholder1->name = p_Beholder1->pMeshObj->instanceName;
    p_Beholder2->name = p_Beholder2->pMeshObj->instanceName;
    p_Beholder3->name = p_Beholder3->pMeshObj->instanceName;

    p_Beholder1->position.y = p_Beholder1->pMeshObj->position.y;
    p_Beholder2->position.y = p_Beholder2->pMeshObj->position.y;
    p_Beholder3->position.y = p_Beholder3->pMeshObj->position.y;

    vec_pGOs.push_back(p_Beholder1);
    vec_pGOs.push_back(p_Beholder2);
    vec_pGOs.push_back(p_Beholder3);
    //vec_pGOs.push_back(cam);

    ::g_Brain->SetObjVector(&vec_pGOs);
    //////////////////////////////////////////////////////////
    //  temp script  -->must be improve by load from external file
    //////////////////////////////////////////////////////////
    std::string s = "setgoto(50,12.5,-17.5)";
    cmdA.push_back(s);
    s = "setrotate(50,1.57)";
    cmdA.push_back(s);
    s = "setgoto(50,-17.5,-17.5)";
    cmdA.push_back(s);
    s = "setrotate(50,3.14)";
    cmdA.push_back(s);
    s = "setgoto(50,-17.5,12.5)";
    cmdA.push_back(s);
    s = "setrotate(50,4.71)";
    cmdA.push_back(s);
    s = "setgoto(50,12.5,12.5)";
    cmdA.push_back(s);
    s = "setrotate(50,6.28)";
    cmdA.push_back(s);

    s = "setgoto(51,-52.5,-47.5)";
    cmdB.push_back(s);
    s = "setrotate(51,1.57)";
    cmdB.push_back(s);
    s = "setgoto(51,-62.5,-47.5)";
    cmdB.push_back(s);
    s = "setrotate(51,3.14)";
    cmdB.push_back(s);
    s = "setgoto(51,-62.5,-32.5)";
    cmdB.push_back(s);
    s = "setrotate(51,4.71)";
    cmdB.push_back(s);
    s = "setgoto(51,-52.5,-32.5)";
    cmdB.push_back(s);
    s = "setrotate(51,6.28)";
    cmdB.push_back(s);

    s = "setgoto(52,62.5,-17.5)";
    cmdC.push_back(s);
    s = "setrotate(52,1.57)";
    cmdC.push_back(s);
    s = "setgoto(52,47.5,-17.5)";
    cmdC.push_back(s);
    s = "setrotate(52,3.14)";
    cmdC.push_back(s);
    s = "setgoto(52,47.5,-7.5)";
    cmdC.push_back(s);
    s = "setrotate(52,4.71)";
    cmdC.push_back(s);
    s = "setgoto(52,62.5,-7.5)";
    cmdC.push_back(s);
    s = "setrotate(52,6.28)";
    cmdC.push_back(s);

    /// //////////////////////////////////////////////////////
    
    g_Brain->RunScriptImmediately(cmdA[0]);
    g_Brain->RunScriptImmediately(cmdB[0]);
    g_Brain->RunScriptImmediately(cmdC[0]);

    light0Setup(); // Dir light
    light1Setup(pVAOManager);// torch
    light2Setup(pVAOManager); //beholder eye

    cTime::update();

    while (!glfwWindowShouldClose(window))
    {
        ::g_pTheLightManager->setLightToShader(shaderID);

        float ratio;
        int width, height;
        //glm::mat4x4 matModel;
        glm::mat4x4 matProjection;
        glm::mat4x4 matView;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        updateByFrameRate();

        //glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraDirection = glm::normalize(g_cameraEye - g_cameraTarget);
        glm::vec3 cameraRight = glm::normalize(glm::cross(g_upVector, cameraDirection));
        //if (!bIsWalkAround)
        {

            matView = glm::lookAt(::g_cameraEye, ::g_cameraTarget, ::g_upVector);
        }
        //else
        {
           // matView = glm::lookAt(::g_cameraEye, ::g_cameraEye+::g_cameraFront, ::g_upVector);
        }
        GLint eyeLocation_UniLoc = glGetUniformLocation(shaderID, "eyeLocation");

        glUniform4f(eyeLocation_UniLoc, ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z, 1.0f);

        matProjection = glm::perspective(glm::radians(fov), ratio, 0.1f, 10000.0f);
        //matProjection = glm::perspective(0.6f, ratio, 0.1f, 10000.0f);
        //glUniformMatrix4fv(mView_location, 1, GL_FALSE, glm::value_ptr(matView));
        //glUniformMatrix4fv(mProjection_location, 1, GL_FALSE, glm::value_ptr(matProjection));

        updateInstanceObj(pShaderManager, pVAOManager, matView, matProjection);

        gui_->ImGUICreateFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();

        //set window title
        //std::stringstream 

    }

    gui_->ImGUI_shutdown();

    glfwDestroyWindow(window);

    delete pVAOManager;
    delete pShaderManager;
    delete g_Brain;
    //delete p_Beholder1;
    //delete p_Beholder2;
    //delete p_Beholder3;
    delete g_pTextureManager;
    delete g_pTheLightManager;

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void updateInstanceObj(cShaderManager* pShaderManager, cVAOManager* pVAOManager, glm::mat4x4 matView, glm::mat4x4 matProjection)
{
    glm::mat4x4 matModel;

    for (std::map<std::string, cMeshObj* >::iterator itCurrentMesh = pVAOManager->mapInstanceNametoMeshObj.begin();
        itCurrentMesh != pVAOManager->mapInstanceNametoMeshObj.end();
        itCurrentMesh++)
    {
        cMeshObj* pCurrentMeshObject = (itCurrentMesh->second);        // * is the iterator access thing

        //pCurrentMeshObject->textureRatios[0] -= 0.001f;
        //pCurrentMeshObject->textureRatios[1] += 0.001f;

        if (!pCurrentMeshObject->isVisible)
        {
            // Skip the rest of the loop
            continue;
        }
        if (pCurrentMeshObject->instanceName == "boss1")
        {
            g_pTheLightManager->plight[7]->position = glm::vec4(pCurrentMeshObject->position, 1);
            //g_pTheLightManager->plight[7]->direction.x =    g_pTheLightManager->plight[7]->direction.x * cos(pCurrentMeshObject->rotation.y) + 
            //                                                g_pTheLightManager->plight[7]->direction.z * sin(pCurrentMeshObject->rotation.y);
            //g_pTheLightManager->plight[7]->direction.z =    g_pTheLightManager->plight[7]->direction.x * sin(pCurrentMeshObject->rotation.y) +
            //                                                g_pTheLightManager->plight[7]->direction.z * cos(pCurrentMeshObject->rotation.y);


        }
        if (pCurrentMeshObject->instanceName == "boss2")
        {
            g_pTheLightManager->plight[8]->position = glm::vec4(pCurrentMeshObject->position, 1);
            //g_pTheLightManager->plight[8]->direction.x =    g_pTheLightManager->plight[8]->direction.x * cos(pCurrentMeshObject->rotation.y) +
            //                                                g_pTheLightManager->plight[8]->direction.z * sin(pCurrentMeshObject->rotation.y);
            //g_pTheLightManager->plight[8]->direction.z =    g_pTheLightManager->plight[8]->direction.x * sin(pCurrentMeshObject->rotation.y) +
            //                                                g_pTheLightManager->plight[8]->direction.z * cos(pCurrentMeshObject->rotation.y);


        }
        if (pCurrentMeshObject->instanceName == "boss3")
        {
            g_pTheLightManager->plight[9]->position = glm::vec4(pCurrentMeshObject->position, 1);
            //g_pTheLightManager->plight[9]->direction.x =    g_pTheLightManager->plight[9]->direction.x * cos(pCurrentMeshObject->rotation.y) +
            //                                                g_pTheLightManager->plight[9]->direction.z * sin(pCurrentMeshObject->rotation.y);
            //g_pTheLightManager->plight[9]->direction.z =    g_pTheLightManager->plight[9]->direction.x * sin(pCurrentMeshObject->rotation.y) +
            //                                                g_pTheLightManager->plight[9]->direction.z * cos(pCurrentMeshObject->rotation.y);


        }
        if (pCurrentMeshObject->isIslandModel)
        {
            pShaderManager->setShaderUniform1f("bIsIlandModel", (GLfloat)GL_TRUE);
        }
        if (pCurrentMeshObject->isSkybox)
        {
            pShaderManager->setShaderUniform1f("bIsSkyboxObject", (GLfloat)GL_TRUE);
            pCurrentMeshObject->position = ::g_cameraEye;
            pCurrentMeshObject->scale = 7500.f;
        }
        matModel = glm::mat4x4(1.0f);

        drawObj(pCurrentMeshObject, matModel, pShaderManager, pVAOManager, matView, matProjection);
        if (pCurrentMeshObject->isSkybox)
        {
            pShaderManager->setShaderUniform1f("bIsSkyboxObject", (GLfloat)GL_FALSE);
        }
        if (pCurrentMeshObject->isIslandModel)
        {
            pShaderManager->setShaderUniform1f("bIsIlandModel", (GLfloat)GL_FALSE);
        }
    }
}

void drawObj(cMeshObj* pCurrentMeshObject, glm::mat4x4 mat_PARENT_Model, cShaderManager* pShaderManager, cVAOManager* pVAOManager, glm::mat4x4 matView, glm::mat4x4 matProjection)
{
    // Don't draw any "back facing" triangles
    glCullFace(GL_BACK);
    //glEnable(GL_CULL_FACE);
    // Turn on depth buffer test at draw time
    glEnable(GL_DEPTH_TEST);

    //matModel = glm::mat4x4(1.0f);  // identity matrix

#if 0   //flame effect
    pShaderManager->setShaderUniform1f("bIsFlameObject", (GLfloat)GL_TRUE);
    glDepthMask(GL_FALSE);
#endif

    if (pCurrentMeshObject->meshName == "flame")
    {
        pShaderManager->setShaderUniform1f("bUseDiscardTexture", (GLfloat)GL_TRUE);

        if ((pCurrentMeshObject->scale < 8.5))
        {
            pCurrentMeshObject->scale += 0.05f;
            g_pTheLightManager->plight[1]->attenuation -= glm::vec4( 0.001,  0.0001,  0.00002, 0);
            g_pTheLightManager->plight[2]->attenuation -= glm::vec4( 0.001,  0.0001,  0.00002, 0);
            g_pTheLightManager->plight[3]->attenuation -= glm::vec4( 0.001,  0.0001,  0.00002, 0);
            g_pTheLightManager->plight[4]->attenuation -= glm::vec4( 0.001,  0.0001,  0.00002, 0);
            g_pTheLightManager->plight[5]->attenuation -= glm::vec4( 0.001,  0.0001,  0.00002, 0);
            g_pTheLightManager->plight[6]->attenuation -= glm::vec4( 0.001,  0.0001,  0.00002, 0);
        }
        else
        {
            pCurrentMeshObject->scale = 7;
            g_pTheLightManager->plight[1]->attenuation = glm::vec4(0.7f, 0.1f, 0.2f, 1.0f);
            g_pTheLightManager->plight[2]->attenuation = glm::vec4(0.7f, 0.1f, 0.2f, 1.0f);
            g_pTheLightManager->plight[3]->attenuation = glm::vec4(0.7f, 0.1f, 0.2f, 1.0f);
            g_pTheLightManager->plight[4]->attenuation = glm::vec4(0.7f, 0.1f, 0.2f, 1.0f);
            g_pTheLightManager->plight[5]->attenuation = glm::vec4(0.7f, 0.1f, 0.2f, 1.0f);
            g_pTheLightManager->plight[6]->attenuation = glm::vec4(0.7f, 0.1f, 0.2f, 1.0f);
        }
        
    }
    else
    {
        pShaderManager->setShaderUniform1f("bUseDiscardTexture", (GLfloat)GL_FALSE);
    }

    GLuint texture07_Number = g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[7]);
    GLuint texture07_Unit = 7;
    glActiveTexture(texture07_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture07_Number);
    pShaderManager->setShaderUniform1i("texture7", texture07_Unit);

    glm::mat4x4 matModel = mat_PARENT_Model;
    // Move the object 
    glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), pCurrentMeshObject->position);

    //std::cout << pCurrentMeshObject->instanceName << " position x = " << pCurrentMeshObject->position.x << " y = " << pCurrentMeshObject->position.y << " z = " << pCurrentMeshObject->position.z << std::endl;

    //rotate
    glm::mat4 matRoationZ = glm::rotate(glm::mat4(1.0f), pCurrentMeshObject->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 matRoationY = glm::rotate(glm::mat4(1.0f), pCurrentMeshObject->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 matRoationX = glm::rotate(glm::mat4(1.0f), pCurrentMeshObject->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));

    // Scale the object
    float uniformScale = pCurrentMeshObject->scale;
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f), glm::vec3(uniformScale, uniformScale, uniformScale));

    matModel = matModel * matTranslation;

    matModel = matModel * matRoationX;
    matModel = matModel * matRoationY;
    matModel = matModel * matRoationZ;

    matModel = matModel * matScale;

    pShaderManager->setShaderUniformM4fv("mModel", matModel);
    pShaderManager->setShaderUniformM4fv("mView", matView);
    pShaderManager->setShaderUniformM4fv("mProjection", matProjection);

    glm::mat4 mModelInverseTransform = glm::inverse(glm::transpose(matModel));
    pShaderManager->setShaderUniformM4fv("mModelInverseTranspose", mModelInverseTransform);

    // Wireframe
    if (pCurrentMeshObject->isWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      // GL_POINT, GL_LINE, GL_FILL
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    pShaderManager->setShaderUniform4f("RGBA_Color",
        pCurrentMeshObject->color_RGBA.r,
        pCurrentMeshObject->color_RGBA.g,
        pCurrentMeshObject->color_RGBA.b,
        pCurrentMeshObject->color_RGBA.w);

    if (pCurrentMeshObject->bUse_RGBA_colour)
    {
        pShaderManager->setShaderUniform1f("bUseRGBA_Color", (GLfloat)GL_TRUE);
    }
    else
    {
        pShaderManager->setShaderUniform1f("bUseRGBA_Color", (GLfloat)GL_FALSE);
    }
    pShaderManager->setShaderUniform4f("specularColour",
        pCurrentMeshObject->specular_colour_and_power.r,
        pCurrentMeshObject->specular_colour_and_power.g,
        pCurrentMeshObject->specular_colour_and_power.b,
        pCurrentMeshObject->specular_colour_and_power.w);

    //uniform bool bDoNotLight;	
    if (pCurrentMeshObject->bDoNotLight)
    {
        pShaderManager->setShaderUniform1f("bDoNotLight", (GLfloat)GL_TRUE);
    }
    else
    {
        pShaderManager->setShaderUniform1f("bDoNotLight", (GLfloat)GL_FALSE);
    }

    //set texture0
    GLuint texture00_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[0]);
    GLuint texture00_Unit = 0;
    glActiveTexture(texture00_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture00_Number);
    pShaderManager->setShaderUniform1i("texture0", texture00_Unit);
    //set texture1
    GLuint texture01_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[1]);
    GLuint texture01_Unit = 1;
    glActiveTexture(texture01_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture01_Number);
    pShaderManager->setShaderUniform1i("texture1", texture01_Unit);
    //set texture2
    GLuint texture02_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[2]);
    GLuint texture02_Unit = 2;
    glActiveTexture(texture02_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture02_Number);
    pShaderManager->setShaderUniform1i("texture2", texture02_Unit);
    //set texture3
    GLuint texture03_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[3]);
    GLuint texture03_Unit = 3;
    glActiveTexture(texture03_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture03_Number);
    pShaderManager->setShaderUniform1i("texture3", texture03_Unit);
    //set texture4
    GLuint texture04_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[4]);
    GLuint texture04_Unit = 4;
    glActiveTexture(texture04_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture04_Number);
    pShaderManager->setShaderUniform1i("texture4", texture04_Unit);
    //set texture5
    GLuint texture05_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[5]);
    GLuint texture05_Unit = 5;
    glActiveTexture(texture05_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture05_Number);
    pShaderManager->setShaderUniform1i("texture5", texture05_Unit);
    //set texture6
    GLuint texture06_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[6]);
    GLuint texture06_Unit = 6;
    glActiveTexture(texture06_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture06_Number);
    pShaderManager->setShaderUniform1i("texture6", texture06_Unit);

    pShaderManager->setShaderUniform4f("texRatio_0_3",
        pCurrentMeshObject->textureRatios[0],
        pCurrentMeshObject->textureRatios[1],
        pCurrentMeshObject->textureRatios[2],
        pCurrentMeshObject->textureRatios[3]);

    //cube map texture
    GLuint cubeMapTextureNumber = g_pTextureManager->getTexttureID("SpaceBox");
    GLuint texture30Unit = 30;
    glActiveTexture(texture30Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureNumber);
    pShaderManager->setShaderUniform1i("skyboxTexture", texture30Unit);

    cModelDrawInfo drawingInformation;
    if (pVAOManager->FindDrawInfo(pCurrentMeshObject->meshName, drawingInformation))
    {
        glBindVertexArray(drawingInformation.VAO_ID);

        glDrawElements(GL_TRIANGLES, drawingInformation.numberOfIndices, GL_UNSIGNED_INT, (void*)0);

        glBindVertexArray(0);

    }
    else
    {
        // Didn't find that model
        std::cout << "Error: didn't find model to draw." << std::endl;

    }

    for (std::vector<cMeshObj* >::iterator itCurrentMesh = pCurrentMeshObject->vecChildMesh.begin();
        itCurrentMesh != pCurrentMeshObject->vecChildMesh.end();
        itCurrentMesh++)
    {
        cMeshObj* pCurrentChildMeshObject = *itCurrentMesh;
        drawObj(pCurrentChildMeshObject, matModel, pShaderManager, pVAOManager, matView, matProjection);
    }
}

void light0Setup() //lamp
{
    //Directional light
    cDirLight* pDirLight = new cDirLight(*::g_pTheLightManager->plight[0]);
    *pDirLight->pDirection = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    *pDirLight->pDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    *pDirLight->pSpecular = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
    *pDirLight->pTurnON = 1;


    //::g_pTheLightManager->plight[3]->position = glm::vec4(1.95f, 2.7f, -0.75f, 1.f);
    //::g_pTheLightManager->plight[3]->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //::g_pTheLightManager->plight[3]->attenuation = glm::vec4(0.19f, 0.003f, 0.072f, 1.0f);
    //::g_pTheLightManager->plight[3]->type = cLight::LightType::LIGHT_SPOT;
    //::g_pTheLightManager->plight[3]->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    //// inner and outer angles
    //::g_pTheLightManager->plight[3]->angle.x = 10.0f;     // Degrees
    //::g_pTheLightManager->plight[3]->angle.y = 20.0f;     // Degrees
    //::g_pTheLightManager->plight[3]->turnON = 1;
}

void light1Setup(cVAOManager* pVAOManager)
{
    glm::vec4 atten = glm::vec4(0.7f, 0.1f, 0.2f, 1.0f);
    glm::vec4 diffuse = glm::vec4(20.0f, 7.5f, 0.0f, 1.0f);
    
    ::g_pTheLightManager->plight[1]->type = cLight::LightType::LIGHT_POINT;
    ::g_pTheLightManager->plight[1]->diffuse = diffuse;
    ::g_pTheLightManager->plight[1]->position = pVAOManager->getInstanceObjPosition("debug_light1");
    ::g_pTheLightManager->plight[1]->attenuation = atten;
    ::g_pTheLightManager->plight[1]->turnON = 1;

    ::g_pTheLightManager->plight[2]->type = cLight::LightType::LIGHT_POINT;
    ::g_pTheLightManager->plight[2]->diffuse = diffuse;
    ::g_pTheLightManager->plight[2]->position = pVAOManager->getInstanceObjPosition("debug_light2");
    ::g_pTheLightManager->plight[2]->attenuation = atten;
    ::g_pTheLightManager->plight[2]->turnON = 1;

    ::g_pTheLightManager->plight[3]->type = cLight::LightType::LIGHT_POINT;
    ::g_pTheLightManager->plight[3]->diffuse = diffuse;
    ::g_pTheLightManager->plight[3]->position = pVAOManager->getInstanceObjPosition("debug_light3");
    ::g_pTheLightManager->plight[3]->attenuation = atten;
    ::g_pTheLightManager->plight[3]->turnON = 1;

    ::g_pTheLightManager->plight[4]->type = cLight::LightType::LIGHT_POINT;
    ::g_pTheLightManager->plight[4]->diffuse = diffuse;
    ::g_pTheLightManager->plight[4]->position = pVAOManager->getInstanceObjPosition("debug_light4");
    ::g_pTheLightManager->plight[4]->attenuation = atten;
    ::g_pTheLightManager->plight[4]->turnON = 1;

    ::g_pTheLightManager->plight[5]->type = cLight::LightType::LIGHT_POINT;
    ::g_pTheLightManager->plight[5]->diffuse = diffuse;
    ::g_pTheLightManager->plight[5]->position = pVAOManager->getInstanceObjPosition("debug_light5");
    ::g_pTheLightManager->plight[5]->attenuation = atten;
    ::g_pTheLightManager->plight[5]->turnON = 1;

    ::g_pTheLightManager->plight[6]->type = cLight::LightType::LIGHT_POINT;
    ::g_pTheLightManager->plight[6]->diffuse = diffuse;
    ::g_pTheLightManager->plight[6]->position = pVAOManager->getInstanceObjPosition("debug_light6");
    ::g_pTheLightManager->plight[6]->attenuation = atten;
    ::g_pTheLightManager->plight[6]->turnON = 1;
}
void light2Setup(cVAOManager* pVAOManager)
{
    //cDirLight* pDirLight = new cDirLight(*::g_pTheLightManager->plight[5]);
    //*pDirLight->pDirection = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    //*pDirLight->pDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //*pDirLight->pSpecular = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
    //*pDirLight->pTurnON = 1;

    ::g_pTheLightManager->plight[7]->position = glm::vec4(15.6f, 0.6f, 8.7f, 1.0f);
    ::g_pTheLightManager->plight[7]->diffuse = glm::vec4(2.0f, 0.f, 0.f, 1.0f);
    ::g_pTheLightManager->plight[7]->attenuation = glm::vec4(1.19f, 0.1f, 0.2f, 1.0f);
    ::g_pTheLightManager->plight[7]->type = cLight::LightType::LIGHT_SPOT;
    ::g_pTheLightManager->plight[7]->direction = glm::vec4(0.f, 1.5f, -100.f, 1.0f);
    // inner and outer angles
    ::g_pTheLightManager->plight[7]->angle.x = 5.0f;     // Degrees
    ::g_pTheLightManager->plight[7]->angle.y = 5.0f;     // Degrees
    ::g_pTheLightManager->plight[7]->turnON = 1;

    ::g_pTheLightManager->plight[8]->position = glm::vec4(15.6f, 0.6f, 8.7f, 1.0f);
    ::g_pTheLightManager->plight[8]->diffuse = glm::vec4(0.f, 1.f, 0.f, 1.0f);
    ::g_pTheLightManager->plight[8]->attenuation = glm::vec4(1.19f, 0.1f, 0.2f, 1.0f);
    ::g_pTheLightManager->plight[8]->type = cLight::LightType::LIGHT_SPOT;
    ::g_pTheLightManager->plight[8]->direction = glm::vec4(0.f, 1.5f, -100.f, 1.0f);
    // inner and outer angles
    ::g_pTheLightManager->plight[8]->angle.x = 5.0f;     // Degrees
    ::g_pTheLightManager->plight[8]->angle.y = 5.0f;     // Degrees
    ::g_pTheLightManager->plight[8]->turnON = 1;

    ::g_pTheLightManager->plight[9]->position = glm::vec4(15.6f, 0.6f, 8.7f, 1.0f);
    ::g_pTheLightManager->plight[9]->diffuse = glm::vec4(0.f, 0.f, 1.0f, 1.0f);
    ::g_pTheLightManager->plight[9]->attenuation = glm::vec4(1.19f, 0.1f, 0.2f, 1.0f);
    ::g_pTheLightManager->plight[9]->type = cLight::LightType::LIGHT_SPOT;
    ::g_pTheLightManager->plight[9]->direction = glm::vec4(0.f, 1.5f, -100.f, 1.0f);
    // inner and outer angles
    ::g_pTheLightManager->plight[9]->angle.x = 5.0f;     // Degrees
    ::g_pTheLightManager->plight[9]->angle.y = 5.0f;     // Degrees
    ::g_pTheLightManager->plight[9]->turnON = 1;
}


bool deathEvent = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    //move camera
    // AWSD AD-Left, Right
    //      WS-Forward, Back
    const float CAMERA_MOVE_SPEED = 5.f;
    if (key == GLFW_KEY_A)
    {
        //::g_cameraEye.x -= CAMERA_MOVE_SPEED;
        ::g_cameraEye += (glm::normalize(glm::cross(g_upVector, (::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED)));
    }
    if (key == GLFW_KEY_D)
    {
        //::g_cameraEye.x += CAMERA_MOVE_SPEED;
        ::g_cameraEye -= (glm::normalize(glm::cross(g_upVector, (::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED)));
    }
    if (key == GLFW_KEY_W)
    {
        //::g_cameraEye.z -= CAMERA_MOVE_SPEED;
        ::g_cameraEye += ((::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED);
    }
    if (key == GLFW_KEY_S)
    {
        //::g_cameraEye.z += CAMERA_MOVE_SPEED;
        ::g_cameraEye -= ((::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED);
    }
    if (key == GLFW_KEY_Q)
    {
        ::g_cameraEye.y -= CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_E)
    {
        ::g_cameraEye.y += CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        ::g_cameraEye = glm::vec3(-18.f, 115.f, 145.0f);
        //::g_cameraEye = glm::vec3(0.0, 100.0, 300.0f);
        g_cameraTarget = glm::vec3(-16.0f, 4.0f, 0.0f);
        bIsWalkAround = false;

    }
    if (key == GLFW_KEY_F6 && action == GLFW_RELEASE)
    {
        bIsWalkAround = true;
        
        toggle++;
        if (toggle > 3)
        {
            toggle = 1;
        }
    }

    if (key == GLFW_KEY_F7 && action == GLFW_RELEASE)
    {
        cmdA.clear();
        cmdB.clear();
        cmdC.clear();
        cmdA.push_back("setgoto(50,-2.5,  -2.5)");
        cmdC.push_back("setgoto(52,  55, -12.5)");
        cmdC.push_back("setrotate(52, 1.57)");
        cmdC.push_back("setgoto(52,22.5, -12.5)");
        cmdC.push_back("setrotate(52,3.14)");
        cmdC.push_back("setgoto(52,22.5,  -2.5)");
        cmdC.push_back("setrotate(52,1.57)");
        cmdC.push_back("setgoto(52,2.5,-2.5)");
        cmdB.push_back("setgoto(51,-57.5,-40)");
        cmdB.push_back("setrotate(51,3.14)");
        cmdB.push_back("setgoto(51,-57.5,-22.5)");
        cmdB.push_back("setrotate(51,4.71)");
        cmdB.push_back("setgoto(51,-27.5,-22.5)");
        cmdB.push_back("setrotate(51,3.14)");
        cmdB.push_back("setgoto(51,-27.5,-2.5)");
        cmdB.push_back("setrotate(51,4.71)");
        cmdB.push_back("setgoto(51,-5,-2.5)");
        p_Beholder1->cmdIndex = 0;
        p_Beholder2->cmdIndex = 0;
        p_Beholder3->cmdIndex = 0;

        deathEvent = true;
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    if (bIsWalkAround)
    {


        //float xoffset = xpos - lastX;
        //float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        //lastX = xpos;
        //lastY = ypos;

        //float sensitivity = 0.1f; // change this value to your liking
        //xoffset *= sensitivity;
        //yoffset *= sensitivity;

        //yaw += xoffset;
        //pitch += yoffset;

        //// make sure that when pitch is out of bounds, screen doesn't get flipped
        //if (pitch > 89.0f)
        //    pitch = 89.0f;
        //if (pitch < -89.0f)
        //    pitch = -89.0f;

        //glm::vec3 front;
        //front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        //front.y = sin(glm::radians(pitch));
        //front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        //::g_cameraFront = glm::normalize(front);
    }
    else
    {
        ::g_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void updateByFrameRate()
{
    cTime::update();
    double deltaTime = cTime::getDeltaTime();
    g_CurrentTime += deltaTime;

    if (g_CurrentTime >= g_LastCall + FRAME_RATE)
    {
        double elapsedTime = g_CurrentTime - g_LastCall;
        g_LastCall = g_CurrentTime;

        
        patrol(p_Beholder1, cmdA);
        patrol(p_Beholder2, cmdB);
        patrol(p_Beholder3, cmdC);
        if (bIsWalkAround)
        {
            switch (toggle)
            {
            case 1:
                g_cameraEye+= (p_Beholder1->pMeshObj->position - g_cameraEye)* glm::vec3(0.1f)+glm::vec3(2,2,2) ;
                g_cameraTarget = p_Beholder1->pMeshObj->position;
                break;
            case 2:
                g_cameraEye += (p_Beholder2->pMeshObj->position - g_cameraEye) * glm::vec3(0.1f) + glm::vec3(2, 2, 2);
                g_cameraTarget = p_Beholder2->pMeshObj->position;
                break;
            case 3:
                g_cameraEye += (p_Beholder3->pMeshObj->position - g_cameraEye) * glm::vec3(0.1f) + glm::vec3(2, 2, 2);
                g_cameraTarget = p_Beholder3->pMeshObj->position;
                break;
            default:
                break;
            }
        }
        //if (deathEvent)
        //{
        //   
        //    if (p_Beholder1->position.x - p_Beholder3->position.x < 2.5)
        //    {//{
        //    //    cmdA.clear();
        //    //    cmdB.clear();
        //    //    //cmdC.clear();
        //    //    cmdA.push_back("deathscene(50)");
        //    //    cmdC.push_back("deathscene(52)");
        //    //    p_Beholder1->cmdIndex = 0;
        //    //    p_Beholder3->cmdIndex = 0;
        //        cmdB.clear();
        //        cmdB.push_back("setgoto(51,-5,-2.5)");
        //        p_Beholder1->cmdIndex = 0;
        //    }
        //}
    }
    if (g_CurrentTime >= g_LastCall5s + SEC_UPDATE)
    {
        double elapsedTime = g_CurrentTime - g_LastCall5s;
        g_LastCall5s = g_CurrentTime;

        //g_physicSys.gameUpdate();
    }
}

void patrol(cGameObj* obj,std::vector<std::string> cmd)
{
    if (obj->position != obj->pMeshObj->position)
    {
        if (abs(obj->position.x - obj->pMeshObj->position.x) > 0.1)
        {
            obj->pMeshObj->position.x += obj->direction.x * 0.01;
        }
        if (abs(obj->position.z - obj->pMeshObj->position.z) > 0.1)
        {
            obj->pMeshObj->position.z += obj->direction.z * 0.01;
        }
        if (abs(obj->position.x - obj->pMeshObj->position.x) < 0.1 && abs(obj->position.z - obj->pMeshObj->position.z) < 0.1)
        {
            obj->pMeshObj->position = obj->position;
        }


    }
    else if (obj->rotation != obj->pMeshObj->rotation)
    {
        obj->pMeshObj->rotation.y += 0.1;
        if (obj->rotation.y < obj->pMeshObj->rotation.y)
        {
            obj->pMeshObj->rotation = obj->rotation;
            if (obj->pMeshObj->rotation.y >= 6.28)
            {
                obj->pMeshObj->rotation.y = 0;
                obj->rotation.y = 0;
            }
        }
        
    }
    else
    {
        g_Brain->RunScriptImmediately(cmd[obj->cmdIndex]);
        if (++obj->cmdIndex == cmd.size())
        {
            obj->cmdIndex = 0;
        }
    }
}

void death(cGameObj* obj)
{

}