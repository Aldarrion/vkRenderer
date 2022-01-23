#include "Game/VkrGame.h"

#include "Render/Material.h"
#include "Render/Render.h"
#include "Render/RenderBufferCache.h"
#include "Render/Buffer.h"
#include "Render/TinyGltf.h"
#include "Render/Texture.h"

#include "Input/Input.h"

#include "Math/Math.h"
#include "Common/Logging.h"

#include "imgui/imgui.h"

#include <string>

namespace hs
{

//------------------------------------------------------------------------------
VkrGame* g_Game{};

//------------------------------------------------------------------------------
RESULT CreateGame()
{
    g_Game = new VkrGame;
    g_GameBase = g_Game;

    return R_OK;
}

//------------------------------------------------------------------------------
void DestroyGame()
{
    delete g_Game;
    g_Game = nullptr;
    g_GameBase = nullptr;
}

//------------------------------------------------------------------------------
// TODO better obviously
static VisualObject skybox;
static VisualObject pbrBox[4];
static RenderBuffer g_BoxVertexBuffer;
static RenderBuffer g_BoxIndexBuffer;

static VisualObject g_BoxModel;
static RenderBuffer g_BoxModelVertexBuffer;
static RenderBuffer g_BoxModelIndexBuffer;
static Texture      g_BoxModelAlbedo;

static Texture      g_Tex2DWhite;

static UniquePtr<Material> skyboxMaterial;
static UniquePtr<PBRMaterial> pbrMaterial[HS_ARR_LEN(pbrBox)];
static UniquePtr<PBRMaterial> boxModelMaterial;

void MakeVertexBufferBarrier(const RenderBuffer* buffer, VkBufferMemoryBarrier* barrier)
{
    barrier->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier->srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier->dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    barrier->buffer = buffer->GetBuffer();
    barrier->size = buffer->GetSize();
}

void MakeIndexBufferBarrier(const RenderBuffer* buffer, VkBufferMemoryBarrier* barrier)
{
    barrier->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier->srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier->dstAccessMask = VK_ACCESS_INDEX_READ_BIT;
    barrier->buffer = buffer->GetBuffer();
    barrier->size = buffer->GetSize();
}

void CreatePbrBoxBuffers()
{
    // Vertex buffer
    int boxBufferSize = 3 * 8 * sizeof(ObjectVertex);
    HS_CHECK(g_BoxVertexBuffer.Init(RenderBufferType::Vertex, RenderBufferMemory::DeviceLocal, boxBufferSize));

    ObjectVertex* verts;
    RenderBufferEntry stagingVerts = g_Render->GetVertexCache()->BeginAlloc(boxBufferSize, sizeof(ObjectVertex), (void**)&verts);

    verts[0] = ObjectVertex{ Vec3(-1.0, 1.0, -1.0),      Vec3(0.0, 1.0, 0.0) };
    verts[1] = ObjectVertex{ Vec3(1.0, 1.0, -1.0),       Vec3(0.0, 1.0, 0.0) };
    verts[2] = ObjectVertex{ Vec3(1.0, 1.0, 1.0),        Vec3(0.0, 1.0, 0.0) };
    verts[3] = ObjectVertex{ Vec3(-1.0, 1.0, 1.0),       Vec3(0.0, 1.0, 0.0) };
    verts[4] = ObjectVertex{ Vec3(-1.0, -1.0, -1.0),     Vec3(0.0, -1.0, 0.0) };
    verts[5] = ObjectVertex{ Vec3(1.0, -1.0, -1.0),      Vec3(0.0, -1.0, 0.0) };
    verts[6] = ObjectVertex{ Vec3(1.0, -1.0, 1.0),       Vec3(0.0, -1.0, 0.0) };
    verts[7] = ObjectVertex{ Vec3(-1.0, -1.0, 1.0),      Vec3(0.0, -1.0, 0.0) };
    verts[8] = ObjectVertex{ Vec3(-1.0, -1.0, 1.0),      Vec3(-1.0, 0.0, 0.0) };
    verts[9] = ObjectVertex{ Vec3(-1.0, -1.0, -1.0),     Vec3(-1.0, 0.0, 0.0) };
    verts[10] = ObjectVertex{ Vec3(-1.0, 1.0, -1.0),     Vec3(-1.0, 0.0, 0.0) };
    verts[11] = ObjectVertex{ Vec3(-1.0, 1.0, 1.0),      Vec3(-1.0, 0.0, 0.0) };
    verts[12] = ObjectVertex{ Vec3(1.0, -1.0, 1.0),      Vec3(1.0, 0.0, 0.0) };
    verts[13] = ObjectVertex{ Vec3(1.0, -1.0, -1.0),     Vec3(1.0, 0.0, 0.0) };
    verts[14] = ObjectVertex{ Vec3(1.0, 1.0, -1.0),      Vec3(1.0, 0.0, 0.0) };
    verts[15] = ObjectVertex{ Vec3(1.0, 1.0, 1.0),       Vec3(1.0, 0.0, 0.0) };
    verts[16] = ObjectVertex{ Vec3(-1.0, -1.0, -1.0),    Vec3(0.0, 0.0, -1.0) };
    verts[17] = ObjectVertex{ Vec3(1.0, -1.0, -1.0),     Vec3(0.0, 0.0, -1.0) };
    verts[18] = ObjectVertex{ Vec3(1.0, 1.0, -1.0),      Vec3(0.0, 0.0, -1.0) };
    verts[19] = ObjectVertex{ Vec3(-1.0, 1.0, -1.0),     Vec3(0.0, 0.0, -1.0) };
    verts[20] = ObjectVertex{ Vec3(-1.0, -1.0, 1.0),     Vec3(0.0, 0.0, 1.0) };
    verts[21] = ObjectVertex{ Vec3(1.0, -1.0, 1.0),      Vec3(0.0, 0.0, 1.0) };
    verts[22] = ObjectVertex{ Vec3(1.0, 1.0, 1.0),       Vec3(0.0, 0.0, 1.0) };
    verts[23] = ObjectVertex{ Vec3(-1.0, 1.0, 1.0),      Vec3(0.0, 0.0, 1.0) };

    g_Render->GetVertexCache()->EndAlloc();

    RenderBufferEntry vertexBuffer{ g_BoxVertexBuffer.GetBuffer(), 0, g_BoxVertexBuffer.GetSize() };
    RenderCopyBuffer(g_Render->CmdBuff(), vertexBuffer, stagingVerts);

    // Index buffer
    constexpr uint cubeIndices[] =
    {
        3, 1, 0,
        2, 1, 3,

        6, 4, 5,
        7, 4, 6,

        11, 9, 8,
        10, 9, 11,

        14, 12, 13,
        15, 12, 14,

        19, 17, 16,
        18, 17, 19,

        22, 20, 21,
        23, 20, 22
    };

    HS_CHECK(g_BoxIndexBuffer.Init(RenderBufferType::Index, RenderBufferMemory::DeviceLocal, sizeof(cubeIndices)));

    uint* indices;
    RenderBufferEntry stagingIndices = g_Render->GetIndexCache()->BeginAlloc<uint>(HS_ARR_LEN(cubeIndices), &indices);

    memcpy(indices, cubeIndices, sizeof(cubeIndices));
    g_Render->GetIndexCache()->EndAlloc();

    RenderBufferEntry indexBuffer{ g_BoxIndexBuffer.GetBuffer(), 0, g_BoxIndexBuffer.GetSize() };
    RenderCopyBuffer(g_Render->CmdBuff(), indexBuffer, stagingIndices);

    VkBufferMemoryBarrier barriers[2]{};
    MakeVertexBufferBarrier(&g_BoxVertexBuffer, &barriers[0]);
    MakeIndexBufferBarrier(&g_BoxIndexBuffer, &barriers[1]);

    vkCmdPipelineBarrier(g_Render->CmdBuff(),
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0,
        0, nullptr,
        HS_ARR_LEN(barriers), barriers,
        0, nullptr
    );
}

//------------------------------------------------------------------------------
static RESULT LoadBoxModel()
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, "models/Box/BoxTextured.glb");
    if (!warn.empty())
        LOG_WARN(err.c_str());

    if (!err.empty())
        LOG_ERR(err.c_str());

    if (!ret)
        return R_FAIL;

    tinygltf::Mesh mesh = model.meshes[0];
    tinygltf::Primitive primitive = mesh.primitives[0];

    // Vertices
    {
        const float* positionBuffer{};
        const float* normalBuffer{};
        const float* texcoordBuffer{};
        int positionStride{};
        int normalStride{};
        int texcoordStride{};
        int vertexCount{};

        auto posIter = primitive.attributes.find("POSITION");
        if (posIter != primitive.attributes.end())
        {
            const tinygltf::Accessor&  accessor = model.accessors[posIter->second];
            const tinygltf::BufferView& view    = model.bufferViews[accessor.bufferView];
            positionBuffer                      = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
            vertexCount                         = (int)accessor.count;
            positionStride                      = view.byteStride / sizeof(float);
            HS_ASSERT(view.byteStride % sizeof(float) == 0);
        }

        auto normalIter = primitive.attributes.find("NORMAL");
        if (normalIter != primitive.attributes.end())
        {
            const tinygltf::Accessor&  accessor = model.accessors[normalIter->second];
            const tinygltf::BufferView& view    = model.bufferViews[accessor.bufferView];
            normalBuffer                        = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
            normalStride                        = view.byteStride / sizeof(float);
            HS_ASSERT(view.byteStride % sizeof(float) == 0);
        }

        auto uvIter = primitive.attributes.find("TEXCOORD_0");
        if (uvIter != primitive.attributes.end())
        {
            const tinygltf::Accessor& accessor  = model.accessors[uvIter->second];
            const tinygltf::BufferView& view    = model.bufferViews[accessor.bufferView];
            texcoordBuffer                      = reinterpret_cast<const float *>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
            texcoordStride                      = view.byteStride / sizeof(float);
            HS_ASSERT(view.byteStride % sizeof(float) == 0);
        }

        ObjectVertex* verts;
        RenderBufferEntry stagingVerts = g_Render->GetVertexCache()->BeginAlloc<ObjectVertex>(vertexCount, &verts);

        const float* p = positionBuffer;
        const float* n = normalBuffer;
        const float* t = texcoordBuffer;
        for (int i = 0; i < vertexCount; ++i)
        {
            verts[i].position_ = Vec3(p);
            verts[i].normal_ = Vec3(n).Normalized();

            if (t)
            {
                verts[i].uv_ = Vec2(t);
                t += texcoordStride;
            }
            else
            {
                verts[i].uv_ = Vec2{};
            }

            p += positionStride;
            n += normalStride;
        }

        g_Render->GetVertexCache()->EndAlloc();

        HS_CHECK(g_BoxModelVertexBuffer.Init(RenderBufferType::Vertex, RenderBufferMemory::DeviceLocal, vertexCount * sizeof(ObjectVertex)));
        RenderBufferEntry vertexBuffer{ g_BoxModelVertexBuffer.GetBuffer(), 0, g_BoxModelVertexBuffer.GetSize() };
        RenderCopyBuffer(g_Render->CmdBuff(), vertexBuffer, stagingVerts);
    }

    // Indices
    {
        const tinygltf::Accessor&   accessor   = model.accessors[primitive.indices];
        const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer&     buffer     = model.buffers[bufferView.buffer];

        const int indexCount = (int)accessor.count;

        uint* indices;
        RenderBufferEntry stagingIndices = g_Render->GetIndexCache()->BeginAlloc<uint>(indexCount, &indices);

        switch (accessor.componentType)
        {
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                HS_ASSERT(bufferView.byteStride == 0 || bufferView.byteStride == 4);
                for (int i = 0; i < indexCount; ++i)
                    indices[i] = *(reinterpret_cast<const uint*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]) + i);
                break;
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                HS_ASSERT(bufferView.byteStride == 0 || bufferView.byteStride == 2);
                for (int i = 0; i < indexCount; ++i)
                    indices[i] = *(reinterpret_cast<const uint16*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]) + i);
                break;
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                HS_ASSERT(bufferView.byteStride == 0 || bufferView.byteStride == 1);
                for (int i = 0; i < indexCount; ++i)
                    indices[i] = *(reinterpret_cast<const uint8*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]) + i);
                break;
            default:
                HS_ASSERT(false);
        }

        g_Render->GetIndexCache()->EndAlloc();

        HS_CHECK(g_BoxModelIndexBuffer.Init(RenderBufferType::Index, RenderBufferMemory::DeviceLocal, indexCount * sizeof(uint)));
        RenderBufferEntry indexBuffer{ g_BoxModelIndexBuffer.GetBuffer(), 0, g_BoxModelIndexBuffer.GetSize() };
        RenderCopyBuffer(g_Render->CmdBuff(), indexBuffer, stagingIndices);
    }

    // Textures
    {
        const tinygltf::Material& material = model.materials[0];

        auto albedoIter = material.values.find("baseColorTexture");
        if (albedoIter != material.values.end())
        {
            int albedoIdx = albedoIter->second.TextureIndex();
            const tinygltf::Image& image = model.images[albedoIdx];

            int bufferSize = -1;
            const unsigned char* buffer{};
            bool deleteBuffer = false;
            if (image.component == 3)
            {
                bufferSize = image.width * image.height * 4;
                unsigned char* rgba = new unsigned char[bufferSize]; // TODO(pavel): Do not allocate for every texture;
                const unsigned char* rgb = &image.image[0];
                for (size_t i = 0; i < image.width * image.height; ++i) {
                    memcpy(rgba, rgb, sizeof(unsigned char) * 3);
                    rgba += 4;
                    rgb += 3;
                }
                buffer = rgba;
                deleteBuffer = true;
            }
            else
            {
                buffer = &image.image[0];
                bufferSize = image.image.size();
            }

            g_BoxModelAlbedo.Init(VK_FORMAT_R8G8B8A8_SRGB, VkExtent3D{ (uint)image.width, (uint)image.height, 1 }, Texture::Type::TEX_2D);
            if (HS_FAILED(g_BoxModelAlbedo.Allocate((const void**)&buffer, "BoxModelAlbedo")))
                return R_FAIL;

            if (deleteBuffer)
                delete buffer;
        }

        g_Tex2DWhite.Init(VK_FORMAT_R8G8B8A8_SRGB, VkExtent3D{ 2, 2, 1 }, Texture::Type::TEX_2D);
        unsigned char whiteTexData[2 * 2 * 4];
        memset(whiteTexData, 0xff, sizeof(whiteTexData));
        unsigned char* ptr = whiteTexData;
        if (HS_FAILED(g_Tex2DWhite.Allocate((const void**)&ptr, "DummyWhite")))
            return R_FAIL;
    }

    VkBufferMemoryBarrier barriers[2]{};
    MakeVertexBufferBarrier(&g_BoxModelVertexBuffer, &barriers[0]);
    MakeIndexBufferBarrier(&g_BoxModelIndexBuffer, &barriers[1]);

    vkCmdPipelineBarrier(g_Render->CmdBuff(),
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0,
        0, nullptr,
        HS_ARR_LEN(barriers), barriers,
        0, nullptr
    );

    return R_OK;
}

//------------------------------------------------------------------------------
RESULT VkrGame::Init()
{
    CameraInitAsPerspective(g_Render->GetCamera(), Vec3(0, 0, 0), Vec3(0, 0, 1));
    freeflyCamera_.camera_ = g_Render->GetCamera();

    if (HS_FAILED(LoadBoxModel()))
        return R_FAIL;

    boxModelMaterial = MakeUnique<PBRMaterial>();
    if (HS_FAILED(boxModelMaterial->Init()))
        return R_FAIL;

    boxModelMaterial->albedo_ = Vec3(1, 1, 1);
    boxModelMaterial->albedoTex_ = &g_BoxModelAlbedo;

    g_BoxModel.vertexBuffer_.buffer_ = RenderBufferEntry{ g_BoxModelVertexBuffer.GetBuffer(), 0, g_BoxModelVertexBuffer.GetSize() };
    g_BoxModel.vertexBuffer_.vertexSize_ = sizeof(ObjectVertex);
    g_BoxModel.indexBuffer_.buffer_ = RenderBufferEntry{ g_BoxModelIndexBuffer.GetBuffer(), 0, g_BoxModelIndexBuffer.GetSize() };
    g_BoxModel.transform_ = Mat44::Scale(2);
    g_BoxModel.transform_.SetPosition(Vec3(0, 0, 10));
    g_BoxModel.material_ = boxModelMaterial.Get();

    // Skybox
    skyboxMaterial = MakeUnique<SkyboxMaterial>();
    if (HS_FAILED(skyboxMaterial->Init()))
        return R_FAIL;

    skybox.material_ = skyboxMaterial.Get();

    // Pbr boxes
    CreatePbrBoxBuffers();

    for (uint i = 0; i < HS_ARR_LEN(pbrBox); ++i)
    {
        pbrMaterial[i] = MakeUnique<PBRMaterial>();
        if (HS_FAILED(pbrMaterial[i]->Init()))
            return R_FAIL;

        pbrMaterial[i]->albedoTex_ = &g_Tex2DWhite;

        pbrBox[i].material_ = pbrMaterial[i].Get();
        pbrBox[i].transform_ = Mat44::Scale(3);

        VertexBuffer vb;
        vb.buffer_.buffer_ = g_BoxVertexBuffer.GetBuffer();
        vb.buffer_.offset_ = 0;
        vb.buffer_.size_ = g_BoxVertexBuffer.GetSize();
        vb.vertexSize_ = sizeof(ObjectVertex);

        pbrBox[i].vertexBuffer_ = vb;

        IndexBuffer ib;
        ib.buffer_.buffer_ = g_BoxIndexBuffer.GetBuffer();
        ib.buffer_.offset_ = 0;
        ib.buffer_.size_ = g_BoxIndexBuffer.GetSize();

        pbrBox[i].indexBuffer_ = ib;
    }
    pbrBox[0].transform_.SetPosition(Vec3(5, -5, 20));
    pbrBox[1].transform_.SetPosition(Vec3(-5, -5, 20));
    pbrBox[2].transform_.SetPosition(Vec3(5, 5, 20));
    pbrBox[3].transform_.SetPosition(Vec3(-5, 5, 20));

    return R_OK;
}

//------------------------------------------------------------------------------
RESULT VkrGame::OnWindowResized()
{
    return R_OK;
}

int boxIdx = 0;
//------------------------------------------------------------------------------
void VkrGame::Update()
{
    CameraUpdateFreeFly(&freeflyCamera_);

    g_Render->RenderObject(&skybox);

    ImGui::Begin("PBR settings");
        ImGui::SliderInt("Box idx", &boxIdx, 0, HS_ARR_LEN(pbrBox) - 1);

        auto mat = ((PBRMaterial*)pbrBox[boxIdx].material_);
        Vec3 albedo = mat->albedo_;
        float roughness = mat->roughness_;
        float metallic = mat->metallic_;
        float ao = mat->ao_;

        ImGui::ColorPicker3("Albedo", albedo.v);
        ImGui::SliderFloat("Roughness", &roughness, 0, 1);
        ImGui::SliderFloat("Metellic", &metallic, 0, 1);
        ImGui::SliderFloat("Ambient", &ao, 0, 1);

        mat->albedo_ = albedo;
        mat->roughness_ = roughness;
        mat->metallic_ = metallic;
        mat->ao_ = ao;
    ImGui::End();

    g_Render->RenderObjects(MakeSpan(pbrBox));
    g_Render->RenderObject(&g_BoxModel);
}

//------------------------------------------------------------------------------
void VkrGame::Free()
{
    g_BoxVertexBuffer.Free();
    g_BoxIndexBuffer.Free();
    g_BoxModelVertexBuffer.Free();
    g_BoxModelIndexBuffer.Free();
    g_BoxModelAlbedo.Free();
}

}
