#include "assimpModel.h"
#include "assimpModel.h"
#include "assimpModel.h"
#include "assimpModel.h"
#include "assimpMesh.h"

void tsa::AssimpModel::Draw(ew::Shader & shader)
{
	for (auto i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void tsa::AssimpModel::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void tsa::AssimpModel::processNode(aiNode* node, const aiScene* scene)
{
	for (auto i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

AssimpMesh tsa::AssimpModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (auto i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex pos, normals, and texture cords [...]
		vertices.push_back(vertex);
	}
	// process indices [...]

	//process material
	if (mesh->mMaterialIndex >=0)
	{
		[...]
	}

	return AssimpMesh(vertices, indices, textures);
}

std::vector<Texture> tsa::AssimpModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	return std::vector<Texture>();
}
