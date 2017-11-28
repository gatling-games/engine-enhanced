#include "MeshImporter.h"

#include "Math\Vector3.h"
#include "Math\Point3.h"
#include "Math\Point2.h"
#include "Renderer\Mesh.h"

#include <assert.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include <filesystem>
namespace fs = std::experimental::filesystem::v1;

bool MeshImporter::canHandleFileType(const std::string& fileExtension) const
{
    // Support .obj files and .mesh only.
    return (fileExtension == ".obj" || fileExtension == ".mesh");
}

bool MeshImporter::importFile(const std::string& sourceFile, const std::string& outputFile) const
{
    // Determine the file extension that we are importing.
    const std::string fileExtension = fs::path(sourceFile).extension().string();

    // Use the obj importer for .obj files
    if (fileExtension == ".obj")
    {
        return importObjFile(sourceFile, outputFile);
    }

    // Use the .mesh importer for .mesh files
    if (fileExtension == ".mesh")
    {
        return importDotMeshFile(sourceFile, outputFile);
    }

    printf("MeshImporter cannot handle %s files \n", fileExtension.c_str());
    return false;
}

int attributeSearch(const std::vector<Point3> &positionAttributes, const std::vector<Vector3> &normalAttributes,
    const std::vector<Point2> &texCoordAttributes, const Point3 &position, const Vector3 &normal, const Point2 &texCoord)
{
    assert(positionAttributes.size() == normalAttributes.size());
    assert(positionAttributes.size() == texCoordAttributes.size());

    for (unsigned int i = 0; i < positionAttributes.size(); i++)
    {
        if (position == positionAttributes[i] && normal == normalAttributes[i] && texCoord == texCoordAttributes[i])
        {
            return i;
        }
    }

    return -1;
}

struct objVertex
{
    int position;
    int texCoord;
    int normal;
};

objVertex parseVertex(std::string vertex)
{
    std::string token;
    std::string delimiter = "/";

    objVertex objV;

    // Find position of delimiting character
    size_t position = vertex.find(delimiter);

    // Get substring from start of string until delimiting character
    token = vertex.substr(0, position);
    // Convert substring to integer and store as vertex index
    objV.position = std::stoi(token);
    // Erase substring and delimiting character from string
    vertex.erase(0, position + delimiter.length());

    //Repeat process until string has been consumed
    position = vertex.find(delimiter);

    token = vertex.substr(0, position);
    objV.texCoord = std::stoi(token);
    vertex.erase(0, position + delimiter.length());

    position = vertex.find(delimiter);

    token = vertex.substr(0, position);
    objV.normal = std::stoi(token);
    vertex.erase(0, position + delimiter.length());

    // Push data stored in f onto vector
    return objV;
}

bool MeshImporter::importObjFile(const std::string& sourceFile, const std::string& outputFile) const
{
    // Vectors to store mesh attributes
    std::vector<Point3> positions;
    std::vector<Vector3> normals;
    std::vector<Point2> texCoords;
    std::vector<objVertex> vertices;

    // Open source mesh file as binary to avoid position discrepancy
    std::ifstream file(sourceFile.c_str());

    // Loop through every line in file, looking for vertex attributes
    while (file.is_open() && !file.eof() && !file.fail())
    {
        // Read each value from file stream
        std::string type;
        file >> type;

        // If match with known type, process accordingly
        if (type == "v")
        {
            Point3 v;
            file >> v;
            positions.push_back(v);
        }
        else if (type == "vn")
        {
            Vector3 vn;
            file >> vn;
            normals.push_back(vn);
        }
        else if (type == "vt")
        {
            Point2 vt;
            file >> vt;
            texCoords.push_back(vt);
        }
        else if (type == "f")
        {
            // An f line contains 3 or more vertices forming a face.
            // eg. f 1/1/1 2/1/2 3/1/3 4/1/4

            // Read the entire face into a string syream.
            std::string face;
            std::getline(file, face);
            std::stringstream faceStream(face);

            // Read each vertex from the face stream in turn.
            std::vector<objVertex> faceVertices;
            while (faceStream.eof() == false)
            {
                // Read the vert from the stream
                std::string vert;
                faceStream >> vert;

                // Parse the vertex and add it to the list.
                faceVertices.push_back(parseVertex(vert));
            }

            // We need to trianglulate the face.
            // For now, assume the face is convex and use a triangle fan.
            for (unsigned int i = 2; i < faceVertices.size(); i++)
            {
                vertices.push_back(faceVertices[0]);
                vertices.push_back(faceVertices[i]);
                vertices.push_back(faceVertices[i - 1]);
            }
        }
    }

    // Create vectors to hold finalised vertex data using size
    std::vector<MeshElementIndex> vertexIndices;
    std::vector<Point3> positionAttributes;
    std::vector<Vector3> normalAttributes;
    std::vector<Vector4> tangentAttributes;
    std::vector<Point2> texCoordAttributes;

    // Count through data indices and assign to arrays in desired order
    assert(vertices.size() % 3 == 0);
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        objVertex vertex = vertices[i];

        // Get attributes for next vertex
        Point3 position = positions[vertex.position - 1];
        Vector3 normal = normals[vertex.normal - 1];
        Point2 texCoord = texCoords[vertex.texCoord - 1];

        // Determine which location in the attributes list has those values
        int attributeIndex = attributeSearch(positionAttributes, normalAttributes, texCoordAttributes, position, normal, texCoord);

        if (attributeIndex == -1)
        {
            // Not found, insert the values
            positionAttributes.push_back(position);
            normalAttributes.push_back(normal);
            texCoordAttributes.push_back(texCoord);

            // Use the index of the new attributes
            attributeIndex = (int)positionAttributes.size() - 1;
        }

        vertexIndices.push_back((MeshElementIndex)attributeIndex);
    }

    // Apply obj file scale.
    // From maya, files are imported as centimeters, so convert to meters.
    for (unsigned int i = 0; i < positionAttributes.size(); ++i)
    {
        positionAttributes[i] *= 0.01f;
    }

    // Output the obj data as a binary file
    writeBinaryMesh(outputFile, positionAttributes, normalAttributes, tangentAttributes, texCoordAttributes, vertexIndices);
    return true;
}

bool MeshImporter::importDotMeshFile(const std::string &sourceFile, const std::string &outputFile) const
{
    std::vector<Point3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector4> tangents;
    std::vector<Point2> texcoords;
    std::vector<MeshElementIndex> elements;

    std::ifstream file(sourceFile);

    // Read lines from the mesh file
    while (file.is_open() && !file.fail() && !file.eof())
    {
        std::string type;
        file >> type;

        if (type == "vertex")
        {
            Point3 v;
            file >> v;
            positions.push_back(v);
        }
        else if (type == "normal")
        {
            Vector3 n;
            file >> n;
            normals.push_back(n);
        }
        else if (type == "tangent")
        {
            Vector4 t;
            file >> t;
            tangents.push_back(t);
        }
        else if (type == "texcoord")
        {
            Point2 t;
            file >> t;
            texcoords.push_back(t);
        }
        else if (type == "triangle")
        {
            MeshElementIndex a, b, c;
            file >> a >> b >> c;
            elements.push_back(c);
            elements.push_back(b);
            elements.push_back(a);
        }
    }

    // Check for file reading errors
    if (file.fail())
    {
        printf("Error reading mesh file \n");
        return false;
    }

    // Success. Output the binary mesh.
    writeBinaryMesh(outputFile, positions, normals, tangents, texcoords, elements);
    return true;
}

void MeshImporter::writeBinaryMesh(const std::string &outputFile,
    const std::vector<Point3> &positionAttributes,
    const std::vector<Vector3> &normalAttributes,
    const std::vector<Vector4> &tangentAttributes,
    const std::vector<Point2> &texcoordAttributes,
    const std::vector<MeshElementIndex> &elementIndexes) const
{
    // Populate mesh settings object with appropriate data
    MeshSettings settings;
    settings.vertexCount = (int)positionAttributes.size();
    settings.elementsCount = (int)elementIndexes.size();
    settings.hasNormals = (normalAttributes.size() == positionAttributes.size());
    settings.hasTangents = (tangentAttributes.size() == positionAttributes.size());
    settings.hasTexcoords = (texcoordAttributes.size() == positionAttributes.size());

    // Pack mesh settings and vertex data into binary file
    std::ofstream outputStream(outputFile.c_str(), std::ofstream::binary);
    outputStream.write((const char*)&settings, sizeof(MeshSettings));
    outputStream.write((const char*)&positionAttributes[0], sizeof(Point3) * positionAttributes.size());
    if (settings.hasNormals) outputStream.write((const char*)&normalAttributes[0], sizeof(Vector3) * normalAttributes.size());
    if (settings.hasTangents) outputStream.write((const char*)&tangentAttributes[0], sizeof(Vector4) * tangentAttributes.size());
    if (settings.hasTexcoords)outputStream.write((const char*)&texcoordAttributes[0], sizeof(Point2) * texcoordAttributes.size());
    outputStream.write((const char*)&elementIndexes[0], sizeof(MeshElementIndex) * elementIndexes.size());
    outputStream.close();
}