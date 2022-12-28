#pragma once

#include <string>
#include <map>

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"

namespace tema {

#define SPACE_BORDER_X 4
#define SPACE_BORDER_Y 4

#define DUCK_WIDTH 1
#define DUCK_HEIGHT 1

#define DUCK_BODY_HEIGHT (DUCK_HEIGHT)
#define DUCK_BODY_WIDTH (DUCK_WIDTH * 0.5f)
#define DUCK_BODY_START (DUCK_WIDTH - DUCK_BODY_WIDTH) / 2
#define DUCK_BODY_END DUCK_BODY_START + DUCK_BODY_WIDTH

#define DUCK_WING_START (DUCK_HEIGHT * 0.3f)
#define DUCK_WING_END (DUCK_HEIGHT * 0.6f)
#define DUCK_WING_LENGTH (DUCK_WIDTH * 0.5f)
#define DUCK_WING_MID DUCK_WING_START + (DUCK_WING_END - DUCK_WING_START) / 2

#define DUCK_SPEED 1


#define DUCK_MESH_WIDTH 0.6f
#define DUCK_MESH_HEIGHT 0.6f

#define DUCK_MESH_OFFSET_X ((DUCK_WIDTH - DUCK_MESH_WIDTH) / 2)
#define DUCK_MESH_OFFSET_Y ((DUCK_HEIGHT - DUCK_MESH_HEIGHT) / 2)

	class Duck {
	public: 
		Duck(const std::string& name, glm::vec3 leftBottomPosition);
		~Duck();


		void Update(float deltaTime);

		inline Mesh* GetBodyMesh() { return body_mesh; }
		inline Mesh* GetHeadMesh() { return head_mesh; }
		inline Mesh* GetLWingMesh() { return l_wing_mesh; }
		inline Mesh* GetRWingMesh() { return r_wing_mesh; }

		inline glm::mat3 GetLWingModel() { return lWingModel; }
		inline glm::mat3 GetRWingModel() { return rWingModel; }

		inline glm::mat3 GetModelMatrix() { return modelMatrix; }

	private:
		void CreateMeshes();
		void UpdateAnimation(float timeDelta);
		void CheckCollision();

	protected:
		std::string name;
		glm::vec3 position;
		glm::vec3 dir;

		Mesh* body_mesh;
		Mesh* head_mesh;
		Mesh* l_wing_mesh;
		Mesh* r_wing_mesh;

		glm::mat3 lWingModel;
		glm::mat3 rWingModel;
		glm::mat3 modelMatrix;

		float timeElpsed = 0;
		float angularStep = 0;

	};

}  // namespace tema