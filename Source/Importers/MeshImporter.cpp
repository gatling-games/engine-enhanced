#include "MeshImporter.h"

#include "Math\Vector3.h"
#include "Math\Point3.h"
#include "Math\Point2.h"
#include "Renderer\Mesh.h"

#include <vector>
#include <fstream>
#include <iostream>

bool MeshImporter::canHandleFileType(const std::string& fileExtension) const
{
    // Support .obj files only.
    return (fileExtension == ".obj");
}

bool MeshImporter::importFile(const std::string& sourceFile, const std::string& outputFile) const
{
    struct objVertex
    {
        objVertex()
        {
            position = 0;
            texCoord = 0;
            normal = 0;
        }
        int position;
        int texCoord;
        int normal;
    };

    // Vectors to store mesh attributes
    std::vector<Point3> positions;
    std::vector<Vector3> normals;
    std::vector<Point2> texCoords;
    std::vector<objVertex> vertices;

    // Flag so we only count number of vertices in face once
    bool countFlag = false;
    size_t verticesPerFace;

    // Open source mesh file as binary to avoid position discrepancy
    std::ifstream file(sourceFile.c_str(), std::ios::binary);
    std::cout << "File: " << sourceFile << std::endl;

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
            //vertexCount += 1;
        }
        else if (type == "vn")
        {
            Vector3 vn;
            file >> vn;
            normals.push_back(vn);
            //normalCount += 1;
        }
        else if (type == "vt")
        {
            Point2 vt;
            file >> vt;
            texCoords.push_back(vt);
            //texCoordCount += 1;
        }
        else if (type == "f")
        {
            // Parse face data using delimiter
            std::string line;
            std::string delimiter = "/";

            if (countFlag == false)
            {
                // Preserve current position in file stream
                int positionInFile = file.tellg();

                // Get line of file and count number of vertices per face
                std::getline(file, line);
                verticesPerFace = std::count(line.begin(), line.end(), ' ');

                if (verticesPerFace == 4)
                {
                    std::cout << "WARNING: Importing quads" << std::endl;
                }

                // Return to previously recorded file position
                file.seekg(positionInFile);

                countFlag = true;
            }

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

    // Record face vertex vector size
    int size = vertices.size();

    // Create arrays to hold finalised vertex data using size
    unsigned short* vertexIndices = new unsigned short[size];
    Point3* vertexList = new Point3[size];
    Vector3* normalList = new Vector3[size];
    Point2* texCoordList = new Point2[size];

    // Count through data indices and assign to arrays in desired order
    for (int i = 0; i < size; i++)
    {
        vertexIndices[i] = (unsigned short)vertices[i].position;

        vertexList[i] = positions[vertices[i].position - 1];
        normalList[i] = normals[vertices[i].normal - 1];
        texCoordList[i] = texCoords[vertices[i].texCoord - 1];
    }

    // Don't forget to delete arrays!

    return true;
}
