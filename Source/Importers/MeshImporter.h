#pragma once

#include <vector>

#include "ResourceManager.h"

#include "Math/Point2.h"
#include "Math/Point3.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include "Renderer/Mesh.h"

class MeshImporter : public ResourceImporter
{
public:
	bool canHandleFileType(const std::string &fileExtension) const;

    // Imports a source file and saves the binary mesh to the specified output file.
    // Returns true if successful.
	bool importFile(const std::string &sourceFile, const std::string &outputFile) const override;

private:
    // Imports a mesh in the .obj file format
    bool importObjFile(const std::string &sourceFile, const std::string &outputFile) const;

    // Imports a file in the .mesh file format
    bool importDotMeshFile(const std::string &sourceFile, const std::string &outputFile) const;

    // Writes the mesh attributes to an output file.
    void writeBinaryMesh(const std::string &outputFile,
        const std::vector<Point3> &positionAttributes,
        const std::vector<Vector3> &normalAttributes,
        const std::vector<Vector4> &tangentAttributes,
        const std::vector<Point2> &texcoordAttributes,
        const std::vector<MeshElementIndex> &elementIndexes) const;
};