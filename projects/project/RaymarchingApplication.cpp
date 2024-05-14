#include "RaymarchingApplication.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>
#include <ituGL/lighting/DirectionalLight.h>
#include <ituGL/shader/Material.h>
#include <ituGL/renderer/PostFXRenderPass.h>
#include <ituGL/scene/RendererSceneVisitor.h>
#include <imgui.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

RaymarchingApplication::RaymarchingApplication()
    : Application(1024, 1024, "Ray-marching demo")
    , m_renderer(GetDevice())
{
}

void RaymarchingApplication::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeCamera();
    InitializeMaterial();
    InitializeRenderer();
}

void RaymarchingApplication::Update()
{
    Application::Update();

    // Update camera controller
    m_cameraController.Update(GetMainWindow(), GetDeltaTime());

    // Set renderer camera
    const Camera& camera = *m_cameraController.GetCamera()->GetCamera();
    m_renderer.SetCurrentCamera(camera);

    // Update the material properties
    m_material->SetUniformValue("ProjMatrix", camera.GetProjectionMatrix());
    m_material->SetUniformValue("InvProjMatrix", glm::inverse(camera.GetProjectionMatrix()));

    //Update view relation
    SetViewMatrixRelation();
}

void RaymarchingApplication::Render()
{
    Application::Render();

    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Render the scene
    m_renderer.Render();

    // Render the debug user interface
    RenderGUI();
}

void RaymarchingApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void RaymarchingApplication::InitializeCamera()
{
    // Create the main camera
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->SetViewMatrix(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0), glm::vec3(0.0f, 1.0f, 0.0));
    float fov = 1.0f;
    camera->SetPerspectiveProjectionMatrix(fov, GetMainWindow().GetAspectRatio(), 0.1f, 100.0f);

    // Create a scene node for the camera
    std::shared_ptr<SceneCamera> sceneCamera = std::make_shared<SceneCamera>("camera", camera);

    // Set the camera scene node to be controlled by the camera controller
    m_cameraController.SetCamera(sceneCamera);
}

void RaymarchingApplication::InitializeMaterial()
{
    m_material = CreateRaymarchingMaterial("shaders/sdf-main.glsl");

    m_material->SetUniformValue("SphereCameraCenter", glm::vec3(0, 0, 0));
    m_material->SetUniformValue("SphereCameraRadius", 1.0f);
    m_material->SetUniformValue("SphereCameraColor", glm::vec3(0, 0, 1));
    m_material->SetUniformValue("SphereCameraSmoothness", 1.0f);
    m_material->SetUniformValue("SphereCameraBlend", GL_TRUE);
    m_material->SetUniformValue("SphereCameraEnabled", GL_TRUE);

    m_material->SetUniformValue("SphereCenter", glm::vec3(0, 0, 0));
    m_material->SetUniformValue("SphereRadius", 1.0f);
    m_material->SetUniformValue("SphereColor", glm::vec3(0, 0, 1));
    m_material->SetUniformValue("SphereSmoothness", 1.0f);
    m_material->SetUniformValue("SphereBlend", GL_TRUE);
    m_material->SetUniformValue("SphereEnabled", GL_TRUE);

    m_material->SetUniformValue("CylinderMatrix", glm::translate(glm::vec3(0, 0, 0)));
    m_material->SetUniformValue("CylinderRadius", 1.0f);
    m_material->SetUniformValue("CylinderHeight", 1.0f);
    m_material->SetUniformValue("CylinderColor", glm::vec3(0, 1, 0));
    m_material->SetUniformValue("CylinderSmoothness", 1.0f);
    m_material->SetUniformValue("CylinderBlend", GL_TRUE);
    m_material->SetUniformValue("CylinderEnabled", GL_TRUE);

    m_material->SetUniformValue("BoxMatrix", glm::translate(glm::vec3(0, 0, 0)));
    m_material->SetUniformValue("BoxSize", glm::vec3(1, 1, 1));
    m_material->SetUniformValue("BoxColor", glm::vec3(1, 0, 0));
    m_material->SetUniformValue("BoxSmoothness", 1.0f);
    m_material->SetUniformValue("BoxBlend", GL_TRUE);
    m_material->SetUniformValue("BoxEnabled", GL_TRUE);

    m_material->SetUniformValue("TriPrismMatrix", glm::translate(glm::vec3(0, 0, 0)));
    m_material->SetUniformValue("TriPrismHeight", glm::vec2(0.8, 0.8));
    m_material->SetUniformValue("TriPrismColor", glm::vec3(0, 1, 1));
    m_material->SetUniformValue("TriPrismSmoothness", 1.0f);
    m_material->SetUniformValue("TriPrismBlend", GL_TRUE);
    m_material->SetUniformValue("TriPrismEnabled", GL_TRUE);

    m_material->SetUniformValue("TorusMatrix", glm::translate(glm::vec3(0, 0, 0)));
    m_material->SetUniformValue("TorusMRadius", 0.7f);
    m_material->SetUniformValue("TorusSRadius", 0.7f);
    m_material->SetUniformValue("TorusColor", glm::vec3(0.5f, 0.5f, 0.5f));
    m_material->SetUniformValue("TorusSmoothness", 1.0f);
    m_material->SetUniformValue("TorusBlend", GL_TRUE);
    m_material->SetUniformValue("TorusEnabled", GL_TRUE);

    m_material->SetUniformValue("VerticalCapsuleMatrix", glm::translate(glm::vec3(0, 0, 0)));
    m_material->SetUniformValue("VerticalCapsuleRadius", 0.7f);
    m_material->SetUniformValue("VerticalCapsuleHeight", 0.7f);
    m_material->SetUniformValue("VerticalCapsuleColor", glm::vec3(0.2f, 0.9f, 0.5f));
    m_material->SetUniformValue("VerticalCapsuleSmoothness", 1.0f);
    m_material->SetUniformValue("VerticalCapsuleBlend", GL_TRUE);
    m_material->SetUniformValue("VerticalCapsuleEnabled", GL_TRUE);

    m_material->SetUniformValue("OctahedronMatrix", glm::translate(glm::vec3(0, 0, 0)));
    m_material->SetUniformValue("OctahedronWidth", 0.7f);
    m_material->SetUniformValue("OctahedronColor", glm::vec3(0.7f, 0.0f, 0.5f));
    m_material->SetUniformValue("OctahedronSmoothness", 1.0f);
    m_material->SetUniformValue("OctahedronBlend", GL_TRUE);
    m_material->SetUniformValue("OctahedronEnabled", GL_TRUE);

    m_material->SetUniformValue("PyramidMatrix", glm::translate(glm::vec3(0, 0, 0)));
    m_material->SetUniformValue("PyramidHeight", 0.7f);
    m_material->SetUniformValue("PyramidColor", glm::vec3(0.0f, 0.44f, 0.88f));
    m_material->SetUniformValue("PyramidSmoothness", 1.0f);
    m_material->SetUniformValue("PyramidBlend", GL_TRUE);
    m_material->SetUniformValue("PyramidEnabled", GL_TRUE);
    m_material->SetUniformValue("PyramidBrokenBase", GL_FALSE);

    m_material->SetUniformValue("CombinationType", combinationType);
}

void RaymarchingApplication::InitializeRenderer()
{
    m_renderer.AddRenderPass(std::make_unique<PostFXRenderPass>(m_material));
}

std::shared_ptr<Material> RaymarchingApplication::CreateRaymarchingMaterial(const char* fragmentShaderPath)
{
    // We could keep this vertex shader and reuse it, but it looks simpler this way
    std::vector<const char*> vertexShaderPaths;
    vertexShaderPaths.push_back("shaders/version330.glsl");
    vertexShaderPaths.push_back("shaders/renderer/fullscreen.vert");
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

    std::vector<const char*> fragmentShaderPaths;
    fragmentShaderPaths.push_back("shaders/version330.glsl");
    fragmentShaderPaths.push_back("shaders/utils.glsl");
    fragmentShaderPaths.push_back("shaders/sdflibrary.glsl");
    fragmentShaderPaths.push_back("shaders/sdflibrary-additions.glsl");
    fragmentShaderPaths.push_back("shaders/raymarcher.glsl");
    fragmentShaderPaths.push_back(fragmentShaderPath);
    fragmentShaderPaths.push_back("shaders/raymarching.frag");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

    std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
    shaderProgramPtr->Build(vertexShader, fragmentShader);

    // Create material
    std::shared_ptr<Material> material = std::make_shared<Material>(shaderProgramPtr);
    
    return material;
}

void RaymarchingApplication::SetViewMatrixRelation() {

    glm::mat4 viewMatrix = m_cameraController.GetCamera()->GetCamera()->GetViewMatrix();


    m_material->SetUniformValue("SphereCameraCenter", sphereCameraCenter);


    m_material->SetUniformValue("CylinderMatrix", viewMatrix * glm::translate(cylinderTranslation) * glm::eulerAngleXYZ(cylinderRotation.x, cylinderRotation.y, cylinderRotation.z));
    m_material->SetUniformValue("SphereCenter", glm::vec3(viewMatrix * glm::vec4(sphereCenter, 1.0f)));
    m_material->SetUniformValue("BoxMatrix", viewMatrix * glm::translate(boxTranslation) * glm::eulerAngleXYZ(boxRotation.x, boxRotation.y, boxRotation.z));
    m_material->SetUniformValue("TriPrismMatrix", viewMatrix * glm::translate(triPrismTranslation) * glm::eulerAngleXYZ(triPrismRotation.x, triPrismRotation.y, triPrismRotation.z));
    m_material->SetUniformValue("TriPrismHeight", triPrismHeight);
    m_material->SetUniformValue("TorusMatrix", viewMatrix * glm::translate(torusTranslation) * glm::eulerAngleXYZ(torusRotation.x, torusRotation.y, torusRotation.z));
    m_material->SetUniformValue("VerticalCapsuleMatrix", viewMatrix * glm::translate(verticalCapsuleTranslation) * glm::eulerAngleXYZ(verticalCapsuleRotation.x, verticalCapsuleRotation.y, verticalCapsuleRotation.z));
    m_material->SetUniformValue("OctahedronMatrix", viewMatrix * glm::translate(octahedronTranslation) * glm::eulerAngleXYZ(octahedronRotation.x, octahedronRotation.y, octahedronRotation.z));
    m_material->SetUniformValue("PyramidMatrix", viewMatrix * glm::translate(pyramidTranslation) * glm::eulerAngleXYZ(pyramidRotation.x, pyramidRotation.y, pyramidRotation.z));

}

void RaymarchingApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    // Draw GUI for camera controller
    //m_cameraController.DrawGUI(m_imGui);

    if (auto window = m_imGui.UseWindow("Scene parameters"))
    {
        glm::mat4 viewMatrix = m_cameraController.GetCamera()->GetCamera()->GetViewMatrix();

        const char* items[] = { "Union", "SmoothUnion", "Intersection", "Smoothintersection"};
        ImGui::ListBox("CombinationType", &combinationType, items, IM_ARRAYSIZE(items), 5);
        m_material->SetUniformValue("CombinationType", combinationType);

        if (ImGui::TreeNodeEx("Camera Sphere", ImGuiTreeNodeFlags_DefaultOpen))
        {

            ImGui::DragFloat("Smoothness", m_material->GetDataUniformPointer<float>("SphereCameraSmoothness"), 0.1f);

            ImGui::Checkbox("Blend", &sphereCameraBlend);
            if (sphereCameraBlend) m_material->SetUniformValue("SphereCameraBlend", GL_TRUE);
            else m_material->SetUniformValue("SphereCameraBlend", GL_FALSE);

            ImGui::Checkbox("Enabled", &sphereCameraEnabled);
            if (sphereCameraEnabled) m_material->SetUniformValue("SphereCameraEnabled", GL_TRUE);
            else m_material->SetUniformValue("SphereCameraEnabled", GL_FALSE);

            ImGui::DragFloat3("Center", &sphereCameraCenter[0], 0.1f);

            ImGui::DragFloat("Radius", m_material->GetDataUniformPointer<float>("SphereCameraRadius"), 0.1f);
            ImGui::ColorEdit3("Color", m_material->GetDataUniformPointer<float>("SphereCameraColor"));
            ImGui::TreePop();
        }


        if (ImGui::TreeNodeEx("Sphere", ImGuiTreeNodeFlags_OpenOnDoubleClick))
        {

            ImGui::DragFloat("Smoothness", m_material->GetDataUniformPointer<float>("SphereSmoothness"), 0.1f);

            ImGui::Checkbox("Blend", &sphereBlend);
            if (sphereBlend) m_material->SetUniformValue("SphereBlend", GL_TRUE);
            else m_material->SetUniformValue("SphereBlend", GL_FALSE);

            ImGui::Checkbox("Enabled", &sphereEnabled);
            if (sphereEnabled) m_material->SetUniformValue("SphereEnabled", GL_TRUE);
            else m_material->SetUniformValue("SphereEnabled", GL_FALSE);


            ImGui::DragFloat3("Center", &sphereCenter[0], 0.1f);
            
            ImGui::DragFloat("Radius", m_material->GetDataUniformPointer<float>("SphereRadius"), 0.1f);
            ImGui::ColorEdit3("Color", m_material->GetDataUniformPointer<float>("SphereColor"));
            ImGui::TreePop();
        } 
       
        
        if (ImGui::TreeNodeEx("Cylinder", ImGuiTreeNodeFlags_OpenOnDoubleClick))
        {

            ImGui::DragFloat("Smoothness", m_material->GetDataUniformPointer<float>("CylinderSmoothness"), 0.1f);

            ImGui::Checkbox("Blend", &cylinderBlend);
            if (cylinderBlend) m_material->SetUniformValue("CylinderBlend", GL_TRUE);
            else m_material->SetUniformValue("CylinderBlend", GL_FALSE);

            ImGui::Checkbox("Enabled", &cylinderEnabled);
            if (cylinderEnabled) m_material->SetUniformValue("CylinderEnabled", GL_TRUE);
            else m_material->SetUniformValue("CylinderEnabled", GL_FALSE);

            ImGui::DragFloat3("Translation", &cylinderTranslation[0], 0.1f);
            ImGui::DragFloat3("Rotation", &cylinderRotation[0], 0.1f);
            ImGui::DragFloat("Radius", m_material->GetDataUniformPointer<float>("CylinderRadius"), 0.1f);
            ImGui::DragFloat("Height", m_material->GetDataUniformPointer<float>("CylinderHeight"), 0.1f);
            ImGui::ColorEdit3("Color", m_material->GetDataUniformPointer<float>("CylinderColor"));
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Box", ImGuiTreeNodeFlags_OpenOnDoubleClick))
        {

            ImGui::DragFloat("Smoothness", m_material->GetDataUniformPointer<float>("BoxSmoothness"), 0.1f);
            
            ImGui::Checkbox("Blend", &boxBlend);
            if (boxBlend) m_material->SetUniformValue("BoxBlend", GL_TRUE);
            else m_material->SetUniformValue("BoxBlend", GL_FALSE);

            ImGui::Checkbox("Enabled", &boxEnabled);
            if (boxEnabled) m_material->SetUniformValue("BoxEnabled", GL_TRUE);
            else m_material->SetUniformValue("BoxEnabled", GL_FALSE);

            ImGui::DragFloat3("Translation", &boxTranslation[0], 0.1f);
            ImGui::DragFloat3("Rotation", &boxRotation[0], 0.1f);

            ImGui::DragFloat3("Size", m_material->GetDataUniformPointer<float>("BoxSize"), 0.1f);
            ImGui::ColorEdit3("Color", m_material->GetDataUniformPointer<float>("BoxColor"));

            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("TriPrism", ImGuiTreeNodeFlags_OpenOnDoubleClick))
        {

            ImGui::DragFloat("Smoothness", m_material->GetDataUniformPointer<float>("TriPrismSmoothness"), 0.1f);

            ImGui::Checkbox("Blend", &triPrismBlend);
            if (triPrismBlend) m_material->SetUniformValue("TriPrismBlend", GL_TRUE);
            else m_material->SetUniformValue("TriPrismBlend", GL_FALSE);

            ImGui::Checkbox("Enabled", &triPrismEnabled);
            if (triPrismEnabled) m_material->SetUniformValue("TriPrismEnabled", GL_TRUE);
            else m_material->SetUniformValue("TriPrismEnabled", GL_FALSE);

            ImGui::DragFloat3("Translation", &triPrismTranslation[0], 0.1f);
            ImGui::DragFloat3("Rotation", &triPrismRotation[0], 0.1f);

            ImGui::DragFloat2("Height", &triPrismHeight[0], 0.1f);
            m_material->SetUniformValue("TriPrismHeight", triPrismHeight);
            ImGui::ColorEdit3("Color", m_material->GetDataUniformPointer<float>("TriPrismColor"));

            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Torus", ImGuiTreeNodeFlags_OpenOnDoubleClick))
        {

            ImGui::DragFloat("Smoothness", m_material->GetDataUniformPointer<float>("TorusSmoothness"), 0.1f);

            ImGui::Checkbox("Blend", &torusBlend);
            if (torusBlend) m_material->SetUniformValue("TorusBlend", GL_TRUE);
            else m_material->SetUniformValue("TorusBlend", GL_FALSE);

            ImGui::Checkbox("Enabled", &triPrismEnabled);
            if (triPrismEnabled) m_material->SetUniformValue("TorusEnabled", GL_TRUE);
            else m_material->SetUniformValue("TorusEnabled", GL_FALSE);

            ImGui::DragFloat3("Translation", &torusTranslation[0], 0.1f);
            ImGui::DragFloat3("Rotation", &torusRotation[0], 0.1f);

            ImGui::DragFloat("MainRadius", m_material->GetDataUniformPointer<float>("TorusMRadius"), 0.1f);
            ImGui::DragFloat("SideRadius", m_material->GetDataUniformPointer<float>("TorusSRadius"), 0.1f);
            ImGui::ColorEdit3("Color", m_material->GetDataUniformPointer<float>("TorusColor"));

            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("VerticalCapsule", ImGuiTreeNodeFlags_OpenOnDoubleClick))
        {

            ImGui::DragFloat("Smoothness", m_material->GetDataUniformPointer<float>("VerticalCapsuleSmoothness"), 0.1f);

            ImGui::Checkbox("Blend", &verticalCapsuleBlend);
            if (verticalCapsuleBlend) m_material->SetUniformValue("VerticalCapsuleBlend", GL_TRUE);
            else m_material->SetUniformValue("VerticalCapsuleBlend", GL_FALSE);

            ImGui::Checkbox("Enabled", &verticalCapsuleEnabled);
            if (verticalCapsuleEnabled) m_material->SetUniformValue("VerticalCapsuleEnabled", GL_TRUE);
            else m_material->SetUniformValue("VerticalCapsuleEnabled", GL_FALSE);

            ImGui::DragFloat3("Translation", &verticalCapsuleTranslation[0], 0.1f);
            ImGui::DragFloat3("Rotation", &verticalCapsuleRotation[0], 0.1f);

            ImGui::DragFloat("Radius", m_material->GetDataUniformPointer<float>("VerticalCapsuleRadius"), 0.1f);
            ImGui::DragFloat("Height", m_material->GetDataUniformPointer<float>("VerticalCapsuleHeight"), 0.1f);
            ImGui::ColorEdit3("Color", m_material->GetDataUniformPointer<float>("VerticalCapsuleColor"));

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Octahedron", ImGuiTreeNodeFlags_OpenOnDoubleClick))
        {

            ImGui::DragFloat("Smoothness", m_material->GetDataUniformPointer<float>("OctahedronSmoothness"), 0.1f);

            ImGui::Checkbox("Blend", &octahedronBlend);
            if (octahedronBlend) m_material->SetUniformValue("OctahedronBlend", GL_TRUE);
            else m_material->SetUniformValue("OctahedronBlend", GL_FALSE);

            ImGui::Checkbox("Enabled", &octahedronEnabled);
            if (octahedronEnabled) m_material->SetUniformValue("OctahedronEnabled", GL_TRUE);
            else m_material->SetUniformValue("OctahedronEnabled", GL_FALSE);

            ImGui::DragFloat3("Translation", &octahedronTranslation[0], 0.1f);
            ImGui::DragFloat3("Rotation", &octahedronRotation[0], 0.1f);

            ImGui::DragFloat("Radius", m_material->GetDataUniformPointer<float>("OctahedronWidth"), 0.1f);
            ImGui::ColorEdit3("Color", m_material->GetDataUniformPointer<float>("OctahedronColor"));

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Pyramid", ImGuiTreeNodeFlags_OpenOnDoubleClick))
        {

            ImGui::DragFloat("Smoothness", m_material->GetDataUniformPointer<float>("PyramidSmoothness"), 0.1f);

            ImGui::Checkbox("Blend", &pyramidBlend);
            if (pyramidBlend) m_material->SetUniformValue("PyramidBlend", GL_TRUE);
            else m_material->SetUniformValue("PyramidBlend", GL_FALSE);

            ImGui::Checkbox("Enabled", &pyramidEnabled);
            if (pyramidEnabled) m_material->SetUniformValue("PyramidEnabled", GL_TRUE);
            else m_material->SetUniformValue("PyramidEnabled", GL_FALSE);

            ImGui::Checkbox("BrokenBase", &pyramidBrokenBase);
            if (pyramidBrokenBase) m_material->SetUniformValue("PyramidBrokenBase", GL_TRUE);
            else m_material->SetUniformValue("PyramidBrokenBase", GL_FALSE);

            ImGui::DragFloat3("Translation", &pyramidTranslation[0], 0.1f);
            ImGui::DragFloat3("Rotation", &pyramidRotation[0], 0.1f);

            ImGui::DragFloat("Radius", m_material->GetDataUniformPointer<float>("PyramidHeight"), 0.1f);
            ImGui::ColorEdit3("Color", m_material->GetDataUniformPointer<float>("PyramidColor"));

            ImGui::TreePop();
        }
    }

    m_imGui.EndFrame();
}
