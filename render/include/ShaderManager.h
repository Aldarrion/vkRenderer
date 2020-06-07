#pragma once

#include "Config.h"
#include "Enums.h"

#include "Array.h"

#include "VkTypes.h"
#include <unordered_map>

struct shaderc_compiler;
struct shaderc_compile_options;


namespace vkr
{

class Shader;

//------------------------------------------------------------------------------
class ShaderManager
{
public:
    RESULT Init();

    Shader* GetOrCreateShader(const char* name);
    RESULT ReloadShaders();

private:
    template<typename>
    struct StrCmpEq
    {
        constexpr bool operator()(const char* lhs, const char* rhs) const
        {
            return strcmp(lhs, rhs) == 0;
        }
    };

    template<typename>
    struct StrHash
    {
        uint64 operator()(const char* key) const
        {
            uint64 hash = 9909453657034508789;
            uint len = strlen(key);
            for (uint i = 0; i < len; ++i)
            {
                hash = hash * 1525334644490293591 + key[i];
            }
            return hash;
        }
    };

    std::unordered_map<const char*, Shader*, StrHash<const char*>, StrCmpEq<const char*>> cache_;
    
    Array<VkShaderModule>       toDestroy_;
    shaderc_compiler*           shadercCompiler_{};
    shaderc_compile_options*    opts_{};

    uint16 shaderId_[PS_COUNT]{};

    RESULT CompileShader(const char* file, PipelineStage type, Shader& shader) const;
    RESULT CreateShader(const char* name, Shader* shader);
};

}
