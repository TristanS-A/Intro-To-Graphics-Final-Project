#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../ew/external/stb_image.h"
#include "../tsa/assimpMesh.h"
#include "../ew/shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <iomanip>

namespace tsa
{
	class AssimpModel
	{
	public:
		AssimpModel(char* path)
		{
			loadModel(path);
		}
		void Draw(ew::Shader& shader);
	private:
		std::vector<AssimpMesh> meshes;
		std::vector<Texture> textures_loaded;
		std::string directory;

		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
		unsigned int TextureFromFile(const char* path, const std::string& directory, bool gama = false);
	};
}