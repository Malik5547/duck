#include "duck.h"

//#include <stdio.h>

using namespace tema;

Duck::Duck(const std::string& name, glm::vec3 leftBottomPosition) {
	this->name = name;
    this->position = leftBottomPosition;

    float angle = 0.3 + (rand() / RAND_MAX) * 3;

    float xDir = cos(angle);
    float yDir = sin(angle);

    this->dir = glm::normalize(glm::vec3(xDir, yDir, 0));

	CreateMeshes();
}

Duck::~Duck() {
}

void Duck::CreateMeshes() {
    glm::vec3 corner = glm::vec3(0, 0, 0);

    glm::vec3 color = glm::vec3(1, 0, 0);
    glm::vec3 head_color = glm::vec3(0, 1, 0);
    glm::vec3 wing_color = glm::vec3(1, 0, 0);
    float length = 1;
    bool fill = true;

    std::vector<VertexFormat> body_vertices =
    {
        VertexFormat(corner + glm::vec3(DUCK_BODY_START, 0, 0), color),
        VertexFormat(corner + glm::vec3(DUCK_BODY_END, 0, 0), color),
        VertexFormat(corner + glm::vec3(DUCK_BODY_START + DUCK_BODY_WIDTH / 2, DUCK_BODY_HEIGHT, 0), color),
    };

    // create body mesh
    this->body_mesh = new Mesh(this->name + "_body");
    std::vector<unsigned int> indices = { 0, 1, 2 };

    body_mesh->InitFromData(body_vertices, indices);

    // create the head

    std::vector<VertexFormat> head_vertices =
    {
        VertexFormat(corner + glm::vec3(DUCK_BODY_START + DUCK_BODY_WIDTH / 2, DUCK_HEIGHT, 1), head_color),
    };

    this->head_mesh = new Mesh(this->name + "_head");
    std::vector<unsigned int> head_indices = { 0 };

    head_mesh->SetDrawMode(GL_POINTS);

    head_mesh->InitFromData(head_vertices, head_indices);


    // create the wings (1)

    std::vector<VertexFormat> l_wing_vertices =
    {
        VertexFormat(corner + glm::vec3(DUCK_BODY_START + DUCK_BODY_WIDTH / 2, DUCK_WING_START, 0), wing_color),
        VertexFormat(corner + glm::vec3(DUCK_BODY_START + DUCK_BODY_WIDTH / 2, DUCK_WING_END, 0), wing_color),
        VertexFormat(corner + glm::vec3(DUCK_BODY_START + DUCK_BODY_WIDTH / 2 - DUCK_WING_LENGTH, DUCK_WING_MID, 0), wing_color),
    };

    l_wing_mesh = new Mesh(this->name + "_wing_1");
    std::vector<unsigned int> l_wing_indices = { 0, 1, 2 };

    l_wing_mesh->InitFromData(l_wing_vertices, l_wing_indices);

    // create the wings (2)

    std::vector<VertexFormat> r_wing_vertices =
    {
        VertexFormat(corner + glm::vec3(DUCK_BODY_START + DUCK_BODY_WIDTH / 2, DUCK_WING_START, 0), wing_color),
        VertexFormat(corner + glm::vec3(DUCK_BODY_START + DUCK_BODY_WIDTH / 2, DUCK_WING_END, 0), wing_color),
        VertexFormat(corner + glm::vec3(DUCK_BODY_START + DUCK_BODY_WIDTH / 2 + DUCK_WING_LENGTH, DUCK_WING_MID, 0), wing_color),
    };

    this->r_wing_mesh = new Mesh(this->name + "_wing_2");
    std::vector<unsigned int> r_wing_indices = { 0, 1, 2 };

    this->r_wing_mesh->InitFromData(r_wing_vertices, r_wing_indices);

}



void Duck::Update(float deltaTime) {
    UpdateAnimation(deltaTime);

    CheckCollision();

    // move duck
    this->position.x += this->dir.x * DUCK_SPEED * deltaTime;
    this->position.y += this->dir.y * DUCK_SPEED * deltaTime;

    modelMatrix = glm::mat3(
        1, 0, 0,
        0, 1, 0,
        this->position.x, this->position.y, 1);
}

void Duck::UpdateAnimation(float deltaTime) {
    // update wings animation. Animation are based on rotation with sin function

    timeElpsed += deltaTime;
    angularStep = sin(timeElpsed * 8) / 4;

    glm::mat3 translateMatrix = glm::mat3(
        1, 0, 0,
        0, 1, 0,
        -(DUCK_WING_LENGTH), -(DUCK_WING_MID), 1);

    glm::mat3 lRotationMatrix = glm::mat3(
        cos(angularStep), sin(angularStep), 0,
        -sin(angularStep), cos(angularStep), 0,
        0, 0, 1);

    glm::mat3 rRotationMatrix = glm::mat3(
        cos(-angularStep), sin(-angularStep), 0,
        -sin(-angularStep), cos(-angularStep), 0,
        0, 0, 1);

    glm::mat3 reverseTranslateMatrix = glm::mat3(
        1, 0, 0,
        0, 1, 0,
        +(DUCK_WING_LENGTH), +(DUCK_WING_MID), 1);

    this->lWingModel = reverseTranslateMatrix * lRotationMatrix * translateMatrix;
    this->rWingModel = reverseTranslateMatrix * rRotationMatrix * translateMatrix;
}

void Duck::CheckCollision() {

    //Check collision with edges
    bool edgeCollision = false;

    if (this->position.x + DUCK_MESH_OFFSET_X < -SPACE_BORDER_X) {
        this->dir.x = abs(this->dir.x);
    }
    else if ((this->position.x + (DUCK_WIDTH - DUCK_MESH_OFFSET_X)) > SPACE_BORDER_X) {
        this->dir.x = -abs(this->dir.x);
    }

    if (this->position.y + DUCK_MESH_OFFSET_Y < 0) {
        this->dir.y = abs(this->dir.y);
    }
    else if ((this->position.y + (DUCK_HEIGHT - DUCK_MESH_OFFSET_Y)) > SPACE_BORDER_Y) {
        this->dir.y = -abs(this->dir.y);
    }

}