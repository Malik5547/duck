#include "tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace tema;


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
    delete duck;
}


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;       // logic x
    logicSpace.y = 0;       // logic y
    logicSpace.width = 4;   // logic width
    logicSpace.height = 4;  // logic height

    glm::vec3 corner = glm::vec3(0.001, 0.001, 0);
    length = 0.99f;

    duck = new Duck("duck1", glm::vec3(0, 1, 0));
   
    // background
    glm::vec3 bColor = glm::vec3(0.1, 0.1, 0.1);
    std::vector<VertexFormat> backVertices =
    {
        VertexFormat(glm::vec3(-SPACE_BORDER_X, 0, 0), bColor),
        VertexFormat(glm::vec3(SPACE_BORDER_X, 0, 0), bColor),
        VertexFormat(glm::vec3(-SPACE_BORDER_X, SPACE_BORDER_Y, 0), bColor),
    };

    this->backMesh = new Mesh("background");
    std::vector<unsigned int> indices = { 0, 1, 2 };

    backMesh->InitFromData(backVertices, indices);
    AddMeshToList(backMesh);



    AddMeshToList(duck->GetBodyMesh());
    AddMeshToList(duck->GetHeadMesh());
    AddMeshToList(duck->GetLWingMesh());
    AddMeshToList(duck->GetRWingMesh());

    // Create a shader program for out 2D meshed
    {
        Shader* shader = new Shader("DuckShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema1", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema1", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty, smin, tsx, tsy;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x;
    ty = viewSpace.y - smin * logicSpace.y;

    tsx = (viewSpace.width - smin * (logicSpace.width));
    tsy = (viewSpace.height - smin * (logicSpace.height));

    return glm::transpose(glm::mat3(
        smin, 0.0f, tsx,
        0.0f, smin, tsy,
        0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();

    // set point size
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(70);
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw

    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);

    // Compute uniform 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
    DrawScene(visMatrix);

    // Update world
    UpdateWorld(deltaTimeSeconds);
}

void Tema1::FrameEnd()
{
}


void Tema1::DrawScene(glm::mat3 visMatrix)
{
    RenderMesh2D(this->backMesh, shaders["DuckShader"], visMatrix);


    modelMatrix = visMatrix * duck->GetModelMatrix();

    RenderSimpleMesh(duck->GetBodyMesh(), shaders["DuckShader"], modelMatrix);
    RenderSimpleMesh(duck->GetHeadMesh(), shaders["DuckShader"], modelMatrix);

    modelMatrix = visMatrix * duck->GetModelMatrix() * duck->GetLWingModel();
    RenderSimpleMesh(duck->GetLWingMesh(), shaders["DuckShader"], modelMatrix);

    modelMatrix = visMatrix * duck->GetModelMatrix() * duck->GetRWingModel();
    RenderSimpleMesh(duck->GetRWingMesh(), shaders["DuckShader"], modelMatrix);
}

void Tema1::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // TODO(student): Get shader location for uniform mat4 "Model"
    int uModelLocation = glGetUniformLocation(shader->GetProgramID(), "Model");

    // TODO(student): Set shader uniform "Model" to modelMatrix
    glm::mat3 mm = modelMatrix;
    glm::mat4 model = glm::mat4(
        mm[0][0], mm[0][1], mm[0][2], 0.f,
        mm[1][0], mm[1][1], mm[1][2], 0.f,
        0.f, 0.f, mm[2][2], 0.f,
        mm[2][0], mm[2][1], 0.f, 1.f);
    glUniformMatrix4fv(uModelLocation, 1, GL_FALSE, glm::value_ptr(model));

    // TODO(student): Get shader location for uniform mat4 "View"
    int uViewLocation = glGetUniformLocation(shader->GetProgramID(), "View");

    // TODO(student): Set shader uniform "View" to viewMatrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    glUniformMatrix4fv(uViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // TODO(student): Get shader location for uniform mat4 "Projection"
    int uProjectionLocation = glGetUniformLocation(shader->GetProgramID(), "Projection");

    // TODO(student): Set shader uniform "Projection" to projectionMatrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    glUniformMatrix4fv(uProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Set isPoint uniform
    int uPointLocation = glGetUniformLocation(shader->GetProgramID(), "IsPoint");
    bool isPoint = mesh->GetDrawMode() == GL_POINTS ? true : false;
    glUniform1i(uPointLocation, isPoint);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema1::UpdateWorld(float deltaTime) {
    duck->Update(deltaTime);
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema1::OnKeyPress(int key, int mods)
{
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}
