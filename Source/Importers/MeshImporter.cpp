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

    bool countFlag = false;
    size_t vertexCount;

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

            if (countFlag == false)
            {
                // Preserve current position in file stream
                int positionInFile = file.tellg();

                // Get line of file and count number of vertices per face
                std::getline(file, line);
                vertexCount = std::count(line.begin(), line.end(), ' ');

                if (vertexCount == 4)
                {
                    std::cout << "WARNING: Importing quads" << std::endl;
                }

                // Return to previously recorded file position
                file.seekg(positionInFile);

                countFlag = true;
            }

            //Process accordingly based on number of vertices per face
            if (vertexCount == 3)
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
            else if (vertexCount == 4)
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

                    // push data onto vector
                    vertices.push_back(f);
                }
            }
        }
    }

    // STEP 1 DONE
    // We now have a list of all vertex attributes,
    // and a list of how they are combined

    // STEP 2
    // Make 3 new lists for the final vertex positions, texcoords and normals
    // make them the same size as the objvertex list
    // Assign 
    
	return true;
}
//If normals present, require same number as vertices
//If texcoords present, need same number as vertices
