#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/CameraController.h>
#include <ituGL/utils/DearImGui.h>

class Material;

class RaymarchingApplication : public Application
{
public:
    RaymarchingApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeCamera();
    void InitializeMaterial();
    void InitializeRenderer();
    void SetViewMatrixRelation();

    std::shared_ptr<Material> CreateRaymarchingMaterial(const char* fragmentShaderPath);

    void RenderGUI();

private:
    // Helper object for debug GUI
    DearImGui m_imGui;

    // Camera controller
    CameraController m_cameraController;

    // Renderer
    Renderer m_renderer;

    // Materials
    std::shared_ptr<Material> m_material;


    
    glm::vec3 cylinderTranslation = glm::vec3(0, 1, -10);
    glm::vec3 cylinderRotation = glm::vec3(0.0f);
    bool cylinderBlend = true;
    bool cylinderEnabled = true;

    glm::vec3 sphereCenter = glm::vec3(-2, 1, -10);
    bool sphereBlend = true;
    bool sphereEnabled = true;

    glm::vec3 boxTranslation = glm::vec3(-4, 1, -10);
    glm::vec3 boxRotation = glm::vec3(0.0f);
    bool boxBlend = true;
    bool boxEnabled = true;

    glm::vec3 triPrismTranslation = glm::vec3(2, 1, -10);
    glm::vec3 triPrismRotation = glm::vec3(0.0f);
    glm::vec2 triPrismHeight = glm::vec2(0.8f);
    bool triPrismBlend = true;
    bool triPrismEnabled = true;

    int combinationType = 0;
};
