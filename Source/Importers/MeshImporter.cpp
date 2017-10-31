#include "MeshImporter.h"

#include "Math\Vector3.h"
#include "Math\Point3.h"
#include "Math\Point2.h"
#include "Renderer\Mesh.h"

#include <assert.h>
#include <vector>
#include <fstream>
#include <iostream>

bool MeshImporter::canHandleFileType(const std::string& fileExtension) const
{
    // Support .obj files only.
    return (fileExtension == ".obj");
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

bool MeshImporter::importFile(const std::string& sourceFile, const std::string& outputFile) const
{
    struct objVertex
    {
        int position;
        int texCoord;
        int normal;
    };

    // Vectors to store mesh attributes
    std::vector<Point3> positions;
    std::vector<Vector3> normals;
    std::vector<Point2> texCoords;
    std::vector<objVertex> vertices;

    // For counting number of vertices in face
    size_t verticesPerFace;

    // Open source mesh file as binary to avoid position discrepancy
    std::ifstream file(sourceFile.c_str(), std::ios::binary);

    // Loop through every line in file, looking for vertex attributes
    while (file.is_open() && !file.eof())
    {
        // Return false if source file read fails
        if (file.fail())
        {
            printf(" - ERROR: Failed to read source file \n");
            return false;
        }

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
            // Parse face data using delimiter
            std::string line;
            std::string delimiter = "/";
            // Preserve current position in file stream
            int positionInFile = (int)file.tellg();

            // Get line of file and count number of vertices per face
            std::getline(file, line);
            verticesPerFace = std::count(line.begin(), line.end(), ' ');

            // Return to previously recorded file position
            file.seekg(positionInFile);

            //Process accordingly based on number of vertices per face
            if (verticesPerFace == 3)
            {
                // Create array to hold line data
                std::string v[3];
                file >> v[0];
                file >> v[1];
                file >> v[2];

                objVertex f;

                // Count through each vertex in line
                for (int i = 0; i < 3; i++)
                {
                    std::string token;
                    size_t position = 0;

                    // Find position of delimiting character
                    position = v[i].find(delimiter);

                    // Get value from string using delimiter and shorten string once stored + repeat
                    token = v[i].substr(0, position);
                    f.position = std::stoi(token);
                    v[i].erase(0, position + delimiter.length());

                    position = v[i].find(delimiter);

                    token = v[i].substr(0, position);
                    f.texCoord = std::stoi(token);
                    v[i].erase(0, position + delimiter.length());

                    position = v[i].find(delimiter);

                    token = v[i].substr(0, position);
                    f.normal = std::stoi(token);
                    v[i].erase(0, position + delimiter.length());

                    // Push data onto vector
                    vertices.push_back(f);
                }
            }
            else if (verticesPerFace == 4)
            {
                // Create array to hold line data
                std::string v[4];
                file >> v[0];
                file >> v[1];
                file >> v[2];
                file >> v[3];

                objVertex f;

                // Count through each vertex in line
                for (int i = 0; i < 4; i++)
                {
                    std::string token;
                    size_t position = 0;

                    // Find position of delimiting character
                    position = v[i].find(delimiter);

                    // Get value from string using delimiter and shorten string once stored + repeat
                    token = v[i].substr(0, position);
                    f.position = std::stoi(token);
                    v[i].erase(0, position + delimiter.length());

                    position = v[i].find(delimiter);

                    token = v[i].substr(0, position);
                    f.texCoord = std::stoi(token);
                    v[i].erase(0, position + delimiter.length());

                    position = v[i].find(delimiter);

                    token = v[i].substr(0, position);
                    f.normal = std::stoi(token);
                    v[i].erase(0, position + delimiter.length());

                    // Push data onto vector
                    vertices.push_back(f);
                }
            }
        }
    }

    // Create vectors to hold finalised vertex data using size
    std::vector<MeshElementIndex> vertexIndices;
    std::vector<Point3> positionAttributes;
    std::vector<Vector3> normalAttributes;
    std::vector<Point2> texCoordAttributes;

    // Count through data indices and assign to arrays in desired order
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

    // Populate mesh settings object with appropriate data
    MeshSettings settings;
    settings.vertexCount = vertices.size();
    settings.elementsCount = vertexIndices.size();
    settings.hasNormals = true;
    settings.hasTangents = true;
    settings.hasTexcoords = true;

    // Pack mesh settings and vertex data into binary file
    std::ofstream outputStream(outputFile.c_str());
    outputStream.write((const char*)&settings, sizeof(MeshSettings));
    outputStream.write((const char*)&positionAttributes[0], sizeof(Point3) * positionAttributes.size());
    outputStream.write((const char*)&normalAttributes[0], sizeof(Vector3) * normalAttributes.size());
    outputStream.write((const char*)&texCoordAttributes[0], sizeof(Point2) * texCoordAttributes.size());
    outputStream.write((const char*)&vertexIndices[0], sizeof(MeshElementIndex) * vertexIndices.size());
    outputStream.close();

    return true;
}
