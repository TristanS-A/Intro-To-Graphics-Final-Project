//
// Created by tropi on 10/25/2023.
//
#pragma once
#include "../ew/mesh.h"

namespace tsa {
    ew::MeshData createTorus(float innerRadius, float outerRadius, int stackSubDiv, int sliceSubDiv);

    ew::MeshData createSphere(float radius, int numSegments);

    ew::MeshData createCylinder(float height, float radius, int numSegments);

    void makeRingWithSideNormals(ew::MeshData& meshData, float yPos, float radius, int numSegments, float yUV);

    void makeRingWithInputNormals(ew::MeshData& meshData, ew::Vec3 normalVec, float yPos, float radius, int numSegments);

    ew::MeshData createPlane(float size, int numSegments);

    ew::MeshData createCone(float height, float radius, int numSegments);

    ew::MeshData createSpring(float height, int numCoils, float outerRadius, float innerRadius, int stackSubDiv, int sliceSubDiv);
}