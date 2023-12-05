#pragma once
#include "../ew/external/glad.h"
#include "../ew/ewMath/ewMath.h"
#include "../ew/shader.h"
#include <string>
#include <vector>
namespace tsa 
{
	struct Vertex 
	{
		ew::Vec3 Position;
		ew::Vec3 Normal;
		ew::Vec2 TexCoords;
		ew::Vec3 Tangent;
		ew::Vec3 Bitangent;
	};

	struct Texture
	{
		unsigned int id;
		std::string type;
		std::string path;
	};

	class AssimpMesh
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		AssimpMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
		void AssimpDraw(ew::Shader& shader);
	private:
		unsigned int VAO, VBO, EBO;

		void setupAssimpMesh();
	};
}