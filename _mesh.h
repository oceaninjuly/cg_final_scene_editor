#pragma once
#include <map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <IMG/stb_image.h>




#define INVALID_OGL_VALUE 0xffffffff
//#define ASSIMP_LOAD_FLAGS aiProcess_Triangulate | aiProcess_FlipUVs
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals |  aiProcess_JoinIdenticalVertices )
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define NOT_IMPLEMENTED printf("Not implemented case in %s:%d\n", __FILE__, __LINE__); exit(0);

/*

struct Vector2f
{
    union {
        float x;
        float u;
    };

    union {
        float y;
        float v;
    };

    Vector2f()
    {
    }

    Vector2f(float _x, float _y)
    {
        x = _x;
        y = _y;
    }

    void Print(bool endl = true) const
    {
        printf("(%f, %f)", x, y);

        if (endl) {
            printf("\n");
        }
    }

    float Length() const
    {
        float len = sqrtf(x * x + y * y);
        return len;
    }


    void Normalize()
    {
        float len = Length();

        assert(len != 0);

        x /= len;
        y /= len;
    }
};


inline Vector2f operator*(const Vector2f & l, float f)
{
    Vector2f Ret(l.x * f, l.y * f);

    return Ret;
}

struct Vector3f
{
    union {
        float x;
        float r;
    };

    union {
        float y;
        float g;
    };

    union {
        float z;
        float b;
    };

    Vector3f() {}

    Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f(const float* pFloat)
    {
        x = pFloat[0];
        y = pFloat[1];
        z = pFloat[2];
    }

    /*
    void InitBySphericalCoords(float Radius, float Pitch, float Heading)
    {
        x = Radius * cosf(ToRadian(Pitch)) * sinf(ToRadian(Heading));
        y = -Radius * sinf(ToRadian(Pitch));
        z = Radius * cosf(ToRadian(Pitch)) * cosf(ToRadian(Heading));
    }
    */

/*
    Vector3f(float f)
    {
        x = y = z = f;
    }

    

    Vector3f& operator+=(const Vector3f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;

        return *this;
    }

    Vector3f& operator-=(const Vector3f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;

        return *this;
    }

    Vector3f& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }

    bool operator==(const Vector3f& r)
    {
        return ((x == r.x) && (y == r.y) && (z == r.z));
    }

    bool operator!=(const Vector3f& r)
    {
        return !(*this == r);
    }

    operator const float* () const
    {
        return &(x);
    }


    float Distance(const Vector3f& v) const
    {
        float delta_x = x - v.x;
        float delta_y = y - v.y;
        float delta_z = z - v.z;
        float distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
        return distance;
    }

    float Length() const
    {
        float len = sqrtf(x * x + y * y + z * z);
        return len;
    }

    bool IsZero() const
    {
        return ((x + y + z) == 0.0f);
    }

    void Print(bool endl = true) const
    {
        printf("(%f, %f, %f)", x, y, z);

        if (endl) {
            printf("\n");
        }
    }
};

*/

class _Texture
{
public:
    _Texture(GLenum TextureTarget, const std::string& FileName);

    _Texture(GLenum TextureTarget);

    // Should be called once to load the texture
    bool Load();

    void Load(unsigned int BufferSize, void* pData);

    void Load(const std::string& Filename);

    void LoadRaw(int Width, int Height, int BPP, unsigned char* pData);

    // Must be called at least once for the specific texture unit
    void Bind(GLenum TextureUnit);

    void GetImageSize(int& ImageWidth, int& ImageHeight)
    {
        ImageWidth = m_imageWidth;
        ImageHeight = m_imageHeight;
    }

    GLuint GetTexture() const { return m_textureObj; }

private:
    void LoadInternal(void* image_data);

    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageBPP = 0;
};

struct _Vertex
{
    glm::vec3 m_pos;
    glm::vec2 m_tex;
    glm::vec3 m_normal;

    _Vertex() {}

    _Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = normal;
    }

    _Vertex(const glm::vec3& pos, const glm::vec2& tex)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};

class _Mesh
{
public:
    _Mesh();
    _Mesh(const std::string& Filename);
    ~_Mesh();

    bool LoadMesh(const std::string& Filename);

    void Render();

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        void Init(const std::vector<_Vertex>& Vertices,
            const std::vector<unsigned int>& Indices);

        GLuint VB;
        GLuint IB;
        unsigned int NumIndices;
        unsigned int MaterialIndex;
    };

    std::vector<MeshEntry> m_Entries;
    std::vector<_Texture*> m_Textures;
};


_Mesh::MeshEntry::MeshEntry()
{
    VB = INVALID_OGL_VALUE;
    IB = INVALID_OGL_VALUE;
    NumIndices = 0;
    MaterialIndex = INVALID_MATERIAL;
};

_Mesh::MeshEntry::~MeshEntry()
{
    if (VB != INVALID_OGL_VALUE)
    {
        glDeleteBuffers(1, &VB);
    }

    if (IB != INVALID_OGL_VALUE)
    {
        glDeleteBuffers(1, &IB);
    }
}

void _Mesh::MeshEntry::Init(const std::vector<_Vertex>& Vertices,
    const std::vector<unsigned int>& Indices)
{
    NumIndices = Indices.size();

    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * NumIndices, &Indices[0], GL_STATIC_DRAW);
}

_Mesh::_Mesh()
{
}

_Mesh::_Mesh(const std::string& Filename) {
    LoadMesh(Filename);
}

_Mesh::~_Mesh()
{
    Clear();
}


void _Mesh::Clear()
{
    for (unsigned int i = 0; i < m_Textures.size(); i++) {
        SAFE_DELETE(m_Textures[i]);
    }
}


bool _Mesh::LoadMesh(const std::string& Filename)
{
    // Release the previously loaded _Mesh (if it exists)
    Clear();

    bool Ret = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS);

    if (pScene) {
        Ret = InitFromScene(pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
    }

    return Ret;
}

bool _Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    return InitMaterials(pScene, Filename);
}

void _Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

    std::vector<_Vertex> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
        const aiVector3D* pPos = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        _Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
            glm::vec2(pTexCoord->x, pTexCoord->y),
            glm::vec3(pNormal->x, pNormal->y, pNormal->z));

        Vertices.push_back(v);
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}

bool _Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string FullPath = Dir + "/" + Path.data;
                m_Textures[i] = new _Texture(GL_TEXTURE_2D, FullPath.c_str());

                if (!m_Textures[i]->Load()) {
                    printf("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else {
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
            }
        }

        // Load a white texture in case the model does not include its own texture
        if (!m_Textures[i]) {
            m_Textures[i] = new _Texture(GL_TEXTURE_2D, "model/white.png");

            Ret = m_Textures[i]->Load();
        }
    }

    return Ret;
}

void _Mesh::Render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(_Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (const GLvoid*)20);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);

        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

_Texture::_Texture(GLenum TextureTarget, const std::string& FileName)
{
    m_textureTarget = TextureTarget;
    m_fileName = FileName;
}


_Texture::_Texture(GLenum TextureTarget)
{
    m_textureTarget = TextureTarget;
}


void _Texture::Load(unsigned int BufferSize, void* pData)
{
    void* image_data = stbi_load_from_memory((const stbi_uc*)pData, BufferSize, &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);

    LoadInternal(image_data);

    stbi_image_free(image_data);
}

bool _Texture::Load()
{
    stbi_set_flip_vertically_on_load(1);

    unsigned char* image_data = stbi_load(m_fileName.c_str(), &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);

    if (!image_data) {
        printf("Can't load texture from '%s' - %s\n", m_fileName.c_str(), stbi_failure_reason());
        exit(0);
    }

    printf("Width %d, height %d, bpp %d\n", m_imageWidth, m_imageHeight, m_imageBPP);

    LoadInternal(image_data);

    return true;
}


void _Texture::Load(const std::string& Filename)
{
    m_fileName = Filename;

    if (!Load()) {
        exit(0);
    }
}


void _Texture::LoadRaw(int Width, int Height, int BPP, unsigned char* pData)
{
    m_imageWidth = Width;
    m_imageHeight = Height;
    m_imageBPP = BPP;

    LoadInternal(pData);
}


void _Texture::LoadInternal(void* image_data)
{
    glGenTextures(1, &m_textureObj);
    glBindTexture(m_textureTarget, m_textureObj);

    if (m_textureTarget == GL_TEXTURE_2D) {
        switch (m_imageBPP) {
        case 1:
            glTexImage2D(m_textureTarget, 0, GL_RED, m_imageWidth, m_imageHeight, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
            break;

        case 2:
            glTexImage2D(m_textureTarget, 0, GL_RG, m_imageWidth, m_imageHeight, 0, GL_RG, GL_UNSIGNED_BYTE, image_data);
            break;

        case 3:
            glTexImage2D(m_textureTarget, 0, GL_RGB, m_imageWidth, m_imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
            break;

        case 4:
            glTexImage2D(m_textureTarget, 0, GL_RGBA, m_imageWidth, m_imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            break;

        default:
            NOT_IMPLEMENTED;
        }
    }
    else {
        printf("Support for texture target %x is not implemented\n", m_textureTarget);
        exit(1);
    }

    glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(m_textureTarget);

    glBindTexture(m_textureTarget, 0);
}

void _Texture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}

