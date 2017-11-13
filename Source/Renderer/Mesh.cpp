#include "Mesh.h"

#include <memory>

#include "Math/Point3.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

Mesh::Mesh(ResourceID id)
    : Resource(id),
    loaded_(false),
    settings_(),
    vertexArray_(0),
    attributeBuffers_(),
    elementsBuffer_(0)
{

}

Mesh::~Mesh()
{
    // Ensure any remaining resource data gets unloaded
    if(loaded_)
    {
        Mesh::unload();
    }
}

void Mesh::load(std::ifstream& file)
{
    // Unload any existing buffers
    if(loaded_)
    {
        unload();
    }

    // Read the mesh settings from the file
    file.read((char*)&settings_, sizeof(MeshSettings));

    // First, create the vertex array object.
    glGenVertexArrays(1, &vertexArray_);
    glBindVertexArray(vertexArray_);

    // Load the positions attribute buffer.
    {
        const int positionsSize = sizeof(Point3) * vertexCount();
        std::unique_ptr<Point3[]> positionsData(new Point3[vertexCount()]);
        file.read((char*)positionsData.get(), positionsSize);
        glGenBuffers(1, &attributeBuffers_[PositionsBuffer]);
        glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers_[PositionsBuffer]);
        glBufferData(GL_ARRAY_BUFFER, positionsSize, positionsData.get(), GL_STATIC_DRAW);
        glVertexAttribPointer(PositionsBuffer, 3, GL_FLOAT, false, 0, (void*)0);
    }

    // Load the normals attribute buffer
    if (hasNormals())
    {
        const int normalsSize = sizeof(Vector3) * vertexCount();
        std::unique_ptr<Vector3[]> normalsData(new Vector3[vertexCount()]);
        file.read((char*)normalsData.get(), normalsSize);
        glGenBuffers(1, &attributeBuffers_[NormalsBuffer]);
        glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers_[NormalsBuffer]);
        glBufferData(GL_ARRAY_BUFFER, normalsSize, normalsData.get(), GL_STATIC_DRAW);
        glVertexAttribPointer(NormalsBuffer, 3, GL_FLOAT, false, 0, (void*)0);
    }

    // Load the tangents attribute buffer
    if (hasTangents())
    {
        const int tangentsSize = sizeof(Vector4) * vertexCount();
        std::unique_ptr<Vector4[]> tangentsData(new Vector4[vertexCount()]);
        file.read((char*)tangentsData.get(), tangentsSize);
        glGenBuffers(1, &attributeBuffers_[TangentsBuffer]);
        glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers_[TangentsBuffer]);
        glBufferData(GL_ARRAY_BUFFER, tangentsSize, tangentsData.get(), GL_STATIC_DRAW);
        glVertexAttribPointer(TangentsBuffer, 4, GL_FLOAT, false, 0, (void*)0);
    }

    // Load the texcoords attribute buffer
    if (hasTexcoords())
    {
        const int texcoordsSize = sizeof(Vector2) * vertexCount();
        std::unique_ptr<Vector2[]> texcoordsData(new Vector2[vertexCount()]);
        file.read((char*)texcoordsData.get(), texcoordsSize);
        glGenBuffers(1, &attributeBuffers_[TexcoordsBuffer]);
        glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers_[TexcoordsBuffer]);
        glBufferData(GL_ARRAY_BUFFER, texcoordsSize, texcoordsData.get(), GL_STATIC_DRAW);
        glVertexAttribPointer(TexcoordsBuffer, 2, GL_FLOAT, false, 0, (void*)0);
    }

    // Load the elements buffer
    {
        const int elementsSize = sizeof(MeshElementIndex) * elementsCount();
        std::unique_ptr<MeshElementIndex[]> elementsData(new MeshElementIndex[elementsCount()]);
        file.read((char*)elementsData.get(), elementsSize);
        glGenBuffers(1, &elementsBuffer_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuffer_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementsSize, elementsData.get(), GL_STATIC_DRAW);
    }

    // Now loaded
    loaded_ = true;
}

void Mesh::unload()
{
    // Check there is a mesh to unload
    if(loaded_ == false)
    {
        return;
    }

    // Delete the vertex array object
    glDeleteVertexArrays(1, &vertexArray_);

    // Delete all vertex attribute buffers that were in use
    glDeleteBuffers(1, &attributeBuffers_[PositionsBuffer]);
    if (hasNormals()) glDeleteBuffers(1, &attributeBuffers_[NormalsBuffer]);
    if (hasTangents()) glDeleteBuffers(1, &attributeBuffers_[TangentsBuffer]);
    if (hasTexcoords()) glDeleteBuffers(1, &attributeBuffers_[TexcoordsBuffer]);

    // Delete the elements buffer
    glDeleteBuffers(1, &elementsBuffer_);

    // Now unloaded
    loaded_ = false;
}

void Mesh::bind() const
{
    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuffer_);
}