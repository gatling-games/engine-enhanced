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
    glCreateVertexArrays(1, &vertexArray_);

    // Load the positions attribute buffer.
    {
        // First read in the attribute data
        const int positionsSize = sizeof(Point3) * vertexCount();
        std::unique_ptr<Point3[]> positionsData(new Point3[vertexCount()]);
        file.read((char*)positionsData.get(), positionsSize);

        // Create a buffer to hold the data.
        glCreateBuffers(1, &attributeBuffers_[PositionsBuffer]);      
        glNamedBufferData(attributeBuffers_[PositionsBuffer], positionsSize, positionsData.get(), GL_STATIC_DRAW);

        // Now link the buffer to the vertex array attribute
        glVertexArrayVertexBuffer(vertexArray_, PositionsBuffer, attributeBuffers_[PositionsBuffer], 0, 3 * sizeof(float));
        glVertexArrayAttribFormat(vertexArray_, PositionsBuffer, 3, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(vertexArray_, PositionsBuffer, PositionsBuffer);
        glEnableVertexArrayAttrib(vertexArray_, PositionsBuffer);
    }

    // Load the normals attribute buffer
    if (hasNormals())
    {
        // First read in the attribute data
        const int normalsSize = sizeof(Vector3) * vertexCount();
        std::unique_ptr<Vector3[]> normalsData(new Vector3[vertexCount()]);
        file.read((char*)normalsData.get(), normalsSize);

        // Create a buffer to hold the data.
        glCreateBuffers(1, &attributeBuffers_[NormalsBuffer]);
        glNamedBufferData(attributeBuffers_[NormalsBuffer], normalsSize, normalsData.get(), GL_STATIC_DRAW);

        // Now link the buffer to the vertex array attribute
        glVertexArrayVertexBuffer(vertexArray_, NormalsBuffer, attributeBuffers_[NormalsBuffer], 0, 3 * sizeof(float));
        glVertexArrayAttribFormat(vertexArray_, NormalsBuffer, 3, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(vertexArray_, NormalsBuffer, NormalsBuffer);
        glEnableVertexArrayAttrib(vertexArray_, NormalsBuffer);
    }

    // Load the tangents attribute buffer
    if (hasTangents())
    {
        // First read in the attribute data
        const int tangentsSize = sizeof(Vector4) * vertexCount();
        std::unique_ptr<Vector4[]> tangentsData(new Vector4[vertexCount()]);
        file.read((char*)tangentsData.get(), tangentsSize);

        // Create a buffer to hold the data.
        glCreateBuffers(1, &attributeBuffers_[TangentsBuffer]);
        glNamedBufferData(attributeBuffers_[TangentsBuffer], tangentsSize, tangentsData.get(), GL_STATIC_DRAW);

        // Now link the buffer to the vertex array attribute
        glVertexArrayVertexBuffer(vertexArray_, TangentsBuffer, attributeBuffers_[TangentsBuffer], 0, 4 * sizeof(float));
        glVertexArrayAttribFormat(vertexArray_, TangentsBuffer, 4, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(vertexArray_, TangentsBuffer, TangentsBuffer);
        glEnableVertexArrayAttrib(vertexArray_, TangentsBuffer);
    }

    // Load the texcoords attribute buffer
    if (hasTexcoords())
    {
        // First read in the attribute data
        const int texcoordsSize = sizeof(Vector2) * vertexCount();
        std::unique_ptr<Vector2[]> texcoordsData(new Vector2[vertexCount()]);
        file.read((char*)texcoordsData.get(), texcoordsSize);

        // Create a buffer to hold the data.
        glCreateBuffers(1, &attributeBuffers_[TexcoordsBuffer]);
        glNamedBufferData(attributeBuffers_[TexcoordsBuffer], texcoordsSize, texcoordsData.get(), GL_STATIC_DRAW);

        // Now link the buffer to the vertex array attribute
        glVertexArrayVertexBuffer(vertexArray_, TexcoordsBuffer, attributeBuffers_[TexcoordsBuffer], 0, 2 * sizeof(float));
        glVertexArrayAttribFormat(vertexArray_, TexcoordsBuffer, 2, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(vertexArray_, TexcoordsBuffer, TexcoordsBuffer);
        glEnableVertexArrayAttrib(vertexArray_, TexcoordsBuffer);
    }

    // Load the elements buffer
    {
        // Read in the elements list
        const int elementsSize = sizeof(MeshElementIndex) * elementsCount();
        std::unique_ptr<MeshElementIndex[]> elementsData(new MeshElementIndex[elementsCount()]);
        file.read((char*)elementsData.get(), elementsSize);

        // Create a buffer to hold the elements
        glCreateBuffers(1, &elementsBuffer_);
        glNamedBufferData(elementsBuffer_, elementsSize, elementsData.get(), GL_STATIC_DRAW);

        // Now attack the elements buffer to the vertex array.
        glVertexArrayElementBuffer(vertexArray_, elementsBuffer_);
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