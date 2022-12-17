#include "cGUI.h"


cGUI::cGUI(glm::vec3* camPos, glm::vec3* camTar)
{
    this->camPos = camPos;
    this->camTar = camTar;

}

cGUI::~cGUI()
{
}

bool cGUI::ImGUI_init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    static ImGuiIO& io = ImGui::GetIO();

    //platform / render bindings
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 460"))
    {
        return false;
    }

    //imgui style (dark mode for the win)
    ImGui::StyleColorsDark();
}

void cGUI::ImGUI_shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool cGUI::ImGUICreateFrame()
{
    bool result = false;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    result = ImGUI_render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return result;
}

bool cGUI::ImGUI_render()
{
    ImGui::Begin("Model Settings");
    //create tabs
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Object", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Model"))
        {
            ImGui::BeginGroup();
            //ImGui::BeginChild("##MainWindow", ImVec2(450, 65), true);

            for (int i = 0; i < pVecInstanceMeshObj->size(); i++)
            {
                cMeshObj* pCurrentMeshObject = pVecInstanceMeshObj->at(i);

                if ((pCurrentMeshObject->meshName != "floorA") && (pCurrentMeshObject->meshName != "floorB") && (pCurrentMeshObject->meshName != "floorC") &&
                    (pCurrentMeshObject->meshName != "wall") && (pCurrentMeshObject->meshName != "door") && (pCurrentMeshObject->meshName != "skybox") && 
                    (pCurrentMeshObject->meshName != "terrain") && (pCurrentMeshObject->meshName != "moon") && (pCurrentMeshObject->meshName != "torch") && 
                    (pCurrentMeshObject->meshName != "flame") && (pCurrentMeshObject->meshName != "water"))
                {
                    //ImGui::Text(pCurrentMeshObject->instanceName.c_str());
                    if (ImGui::TreeNode((void*)(intptr_t)i, pCurrentMeshObject->instanceName.c_str()))
                    {
                        ImGui::BeginGroup();
                        ImGui::DragFloat("X pos##", &pCurrentMeshObject->position.x, 0.1f);
                        ImGui::DragFloat("Y pos##", &pCurrentMeshObject->position.y, 0.1f);
                        ImGui::DragFloat("Z pos##", &pCurrentMeshObject->position.z, 0.1f);
                        ImGui::EndGroup();
                        ImGui::NewLine();
                        ImGui::BeginGroup();
                        ImGui::DragFloat("X Rotate##", &pCurrentMeshObject->rotation.x, 0.1f);
                        ImGui::DragFloat("Y Rotate##", &pCurrentMeshObject->rotation.y, 0.1f);
                        ImGui::DragFloat("Z Rotate##", &pCurrentMeshObject->rotation.z, 0.1f);
                        ImGui::EndGroup();
                        ImGui::NewLine();
                        ImGui::BeginGroup();
                        ImGui::DragFloat("R ##", &pCurrentMeshObject->color_RGBA.r, 0.01f, 0.f, 1.f);
                        ImGui::DragFloat("G ##", &pCurrentMeshObject->color_RGBA.g, 0.01f, 0.f, 1.f);
                        ImGui::DragFloat("B ##", &pCurrentMeshObject->color_RGBA.b, 0.01f, 0.f, 1.f);
                        ImGui::EndGroup();
                        ImGui::NewLine();
                        ImGui::BeginGroup();
                        ImGui::DragFloat("powerR ##", &pCurrentMeshObject->specular_colour_and_power.r, 0.01f);
                        ImGui::DragFloat("powerG ##", &pCurrentMeshObject->specular_colour_and_power.g, 0.01f);
                        ImGui::DragFloat("powerB ##", &pCurrentMeshObject->specular_colour_and_power.b, 0.01f);
                        ImGui::EndGroup();
                        ImGui::NewLine();
                        ImGui::DragFloat("scale##", &pCurrentMeshObject->scale, 0.1f, 0.f, 100.f);
                        ImGui::Checkbox("wireframe##", &pCurrentMeshObject->isWireframe);
                        ImGui::SameLine();
                        ImGui::Checkbox("visible##", &pCurrentMeshObject->isVisible);
                        //ImGui::Text(pCurrentMeshObject->instanceName.c_str());
                        ImGui::TreePop();
                    }

                    //ImGui::SameLine();
                    //pCurrentMeshObject->position;
                    //ImGui::InputText("Pos.x",)
                }
            }
            //ImGui::Text("test");
            //ImGui::EndChild();
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Lighting"))
        {
            ImGui::BeginGroup();
            
            for (int i = 0; i < MAX_LIGHT_SOURCE; i++)
            {
                if (ImGui::TreeNode((void*)(intptr_t)i, "light%d", i))
                {
                    ImGui::Text("light%d", i);
                    cLight* pCurrentLgiht = pLight[i];
                    switch (pCurrentLgiht->type)
                    {
                    case (cLight::LIGHT_POINT):
                        ImGui::Checkbox("enable ##", (bool*) &pCurrentLgiht->turnON);
                        ImGui::DragFloat("Position x##", &pCurrentLgiht->position.x, 1.f);
                        ImGui::DragFloat("Position y##", &pCurrentLgiht->position.y, 1.f);
                        ImGui::DragFloat("Position z##", &pCurrentLgiht->position.z, 1.f);
                        ImGui::NewLine();
                        ImGui::DragFloat("Diffuse x##", &pCurrentLgiht->diffuse.x, 0.1f);
                        ImGui::DragFloat("Diffuse y##", &pCurrentLgiht->diffuse.y, 0.1f);
                        ImGui::DragFloat("Diffuse z##", &pCurrentLgiht->diffuse.z, 0.1f);
                        ImGui::NewLine();
                        ImGui::DragFloat("Specualar R##", &pCurrentLgiht->specular.r, 0.01f);
                        ImGui::DragFloat("Specualar G##", &pCurrentLgiht->specular.g, 0.01f);
                        ImGui::DragFloat("Specualar B##", &pCurrentLgiht->specular.b, 0.01f);
                        ImGui::NewLine();
                        ImGui::DragFloat("constant ##", &pCurrentLgiht->attenuation.x, 0.01f);
                        ImGui::DragFloat("linear ##", &pCurrentLgiht->attenuation.y, 0.001f);
                        ImGui::DragFloat("quadratic ##", &pCurrentLgiht->attenuation.z, 0.0001f);

                        break;
                    case (cLight::LIGHT_SPOT):
                        ImGui::Checkbox("enable ##", (bool*)&pCurrentLgiht->turnON);
                        ImGui::DragFloat("Position x##", &pCurrentLgiht->position.x, 1.f);
                        ImGui::DragFloat("Position y##", &pCurrentLgiht->position.y, 1.f);
                        ImGui::DragFloat("Position z##", &pCurrentLgiht->position.z, 1.f);
                        ImGui::NewLine();
                        ImGui::DragFloat("Dirction x##", &pCurrentLgiht->direction.x, 0.01f);
                        ImGui::DragFloat("Dirction y##", &pCurrentLgiht->direction.y, 0.01f);
                        ImGui::DragFloat("Dirction z##", &pCurrentLgiht->direction.z, 0.01f);
                        ImGui::NewLine();
                        ImGui::DragFloat("Diffuse x##", &pCurrentLgiht->diffuse.x, 0.1f);
                        ImGui::DragFloat("Diffuse y##", &pCurrentLgiht->diffuse.y, 0.1f);
                        ImGui::DragFloat("Diffuse z##", &pCurrentLgiht->diffuse.z, 0.1f);
                        ImGui::NewLine();
                        ImGui::DragFloat("Specualar R##", &pCurrentLgiht->specular.r, 0.01f);
                        ImGui::DragFloat("Specualar G##", &pCurrentLgiht->specular.g, 0.01f);
                        ImGui::DragFloat("Specualar B##", &pCurrentLgiht->specular.b, 0.01f);
                        ImGui::NewLine();
                        ImGui::DragFloat("constant ##", &pCurrentLgiht->attenuation.x, 0.01f);
                        ImGui::DragFloat("linear ##", &pCurrentLgiht->attenuation.y, 0.001f);
                        ImGui::DragFloat("quadratic ##", &pCurrentLgiht->attenuation.z, 0.0001f);
                        ImGui::NewLine();
                        ImGui::DragFloat("inner angle ##", &pCurrentLgiht->angle.x, 0.01f);
                        ImGui::DragFloat("outer angle ##", &pCurrentLgiht->angle.y, 0.01f);
                        break;
                    case (cLight::LIGHT_DIRECTION):
                        ImGui::Checkbox("enable ##", (bool*)&pCurrentLgiht->turnON);
                        ImGui::DragFloat("Dirction x##", &pCurrentLgiht->direction.x, 0.01f);
                        ImGui::DragFloat("Dirction y##", &pCurrentLgiht->direction.y, 0.01f);
                        ImGui::DragFloat("Dirction z##", &pCurrentLgiht->direction.z, 0.01f);
                        ImGui::NewLine();
                        ImGui::DragFloat("Diffuse x##", &pCurrentLgiht->diffuse.x, 0.1f);
                        ImGui::DragFloat("Diffuse y##", &pCurrentLgiht->diffuse.y, 0.1f);
                        ImGui::DragFloat("Diffuse z##", &pCurrentLgiht->diffuse.z, 0.1f);
                        ImGui::NewLine();
                        ImGui::DragFloat("Specualar R##", &pCurrentLgiht->specular.r, 0.01f);
                        ImGui::DragFloat("Specualar G##", &pCurrentLgiht->specular.g, 0.01f);
                        ImGui::DragFloat("Specualar B##", &pCurrentLgiht->specular.b, 0.01f);
                        break;
                    default:
                        break;
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Camera"))
        {
            ImGui::BeginGroup(); ::
            ImGui::DragFloat("camera x##", &camPos->x, 1.f);
            ImGui::DragFloat("camera y##", &camPos->y, 1.f);
            ImGui::DragFloat("camera z##", &camPos->z, 1.f);
            ImGui::NewLine();
            ImGui::DragFloat("target x##", &camTar->x, 1.f);
            ImGui::DragFloat("target y##", &camTar->y, 1.f);
            ImGui::DragFloat("target z##", &camTar->z, 1.f);
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    //ImGui::Separator();
    ImGui::End();
    return true;
}

bool cGUI::ImGUI_update()
{
    
    return true;
}
