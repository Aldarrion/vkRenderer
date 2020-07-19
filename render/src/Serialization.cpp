#include "Serialization.h"

#include "Logging.h"

#include "cjson/cJSON.h"

#include "vkr_Windows.h"

namespace vkr
{

#define LOG_AND_FAIL(msg, ...)      do { Log(LogLevel::Error, msg, __VA_ARGS__);\
                                    return R_FAIL; } while(false)

//------------------------------------------------------------------------------
RESULT SerializationManager::FillObject(cJSON* json, PropertyContainer& container)
{
    const cJSON* defJson = cJSON_GetObjectItemCaseSensitive(json, "Def");
    if (!cJSON_IsString(defJson))
        LOG_AND_FAIL("JSON: Invalid Def");

    const cJSON* versionJson = cJSON_GetObjectItemCaseSensitive(json, "Version");
    if (!cJSON_IsNumber(versionJson))
        LOG_AND_FAIL("JSON: Invalid Version");

    auto def = defs_.find(defJson->valuestring);
    if (def == defs_.end())
        LOG_AND_FAIL("JSON: Def %s not found", defJson->valuestring);

    uint version = static_cast<uint>(versionJson->valueint);
    ContainerDef containerDef = def->second->GetDef(version);

    for (int i = 0; i < containerDef.props_.Count(); ++i)
    {
        const PropertyDefinition& propDef = containerDef.props_[i];
        const cJSON* propJson = cJSON_GetObjectItemCaseSensitive(json, propDef.name_);
        
        PropertyContainer::PropertyPair prop;
        prop.Idx = i;
        prop.Value.Type = propDef.type_;
        
        switch (propDef.type_) 
        {
            case PropertyType::Int:
            {
                if (!cJSON_IsNumber(propJson))
                    LOG_AND_FAIL("JSON: Invalid value of property %s", propDef.name_);
                prop.Value.I = propJson->valueint;
                break;
            }
            case PropertyType::Float:
            {
                if (!cJSON_IsNumber(propJson))
                    LOG_AND_FAIL("JSON: Invalid value of property %s", propDef.name_);
                prop.Value.F = (float)propJson->valuedouble;
                break;
            }
            case PropertyType::Vec2:
            {
                if (!cJSON_IsArray(propJson))
                    LOG_AND_FAIL("JSON: Invalid value of property %s", propDef.name_);

                cJSON* element;
                int elementIdx = 0;
                cJSON_ArrayForEach(element, propJson)
                {
                    if (elementIdx > 1)
                        LOG_AND_FAIL("JSON: Invalid value of property %s", propDef.name_);
                    prop.Value.V2.v[elementIdx++] = (float)element->valuedouble;
                }

                if (elementIdx != 2)
                    LOG_AND_FAIL("JSON: Invalid value of property %s", propDef.name_);

                break;
            }
            case PropertyType::Vec3:
            {
                if (!cJSON_IsArray(propJson))
                    LOG_AND_FAIL("JSON: Invalid value of property %s", propDef.name_);

                cJSON* element;
                int elementIdx = 0;
                cJSON_ArrayForEach(element, propJson)
                {
                    if (elementIdx > 2)
                        LOG_AND_FAIL("JSON: Invalid value of property %s", propDef.name_);
                    prop.Value.V2.v[elementIdx++] = (float)element->valuedouble;
                }

                if (elementIdx != 3)
                    LOG_AND_FAIL("JSON: Invalid value of property %s", propDef.name_);

                break;
            }
            case PropertyType::String:
            {
                if (!cJSON_IsString(propJson))
                    LOG_AND_FAIL("JSON: Invalid value of property %s", propDef.name_);

                size_t len = strlen(propJson->valuestring);
                prop.Value.Str = (char*)malloc(len);
                strcpy(prop.Value.Str, propJson->valuestring);

                break;
            }
            default:
            {
                LOG_AND_FAIL("Unknown property type");
            }
        }
        container.Insert(prop);
    }

    // Always ensure we use the latest version
    const uint latestVersion = def->second->GetLatestVersion();
    if (version == latestVersion)
        return R_OK;

    while(true)
    {
        def->second->Upgrade(containerDef, container, version);
        if (version == latestVersion)
            break;
        containerDef = def->second->GetDef(version);
    }

    return R_OK;
}

//------------------------------------------------------------------------------
RESULT SerializationManager::ReadConfig(const char* fileName, PropertyContainer& container)
{
    FILE* f = fopen(fileName, "r");
    if (!f)
    {
        Log(LogLevel::Error, "Failed to open config %s", fileName);
        return R_FAIL;
    }

    fseek(f , 0 , SEEK_END);
    auto size = ftell(f);
    rewind(f);

    char* buffer = (char*)malloc(size);
    if (!buffer)
    {
        free(buffer);
        fclose(f);
        Log(LogLevel::Error, "Failed to alloc space for config data");
        return R_FAIL;
    }

    auto readRes = fread(buffer, 1, size, f);
    auto eof = feof(f);
    if (readRes != size && !eof)
    {
        free(buffer);
        fclose(f);
        Log(LogLevel::Error, "Failed to read config file %s, error %d", fileName, ferror(f));
        return R_FAIL;
    }
    fclose(f);

    // Json parsing
    cJSON* root = cJSON_Parse(buffer);
    free(buffer);
    if (!root)
    {
        const char* jsonError = cJSON_GetErrorPtr();
        Log(LogLevel::Error, "Failed to parse config file %s, error %s", fileName, jsonError);
        return R_FAIL;
    }

    // Json parsed, fill the container
    if (FAILED(FillObject(root, container)))
    {
        cJSON_Delete(root);
        Log(LogLevel::Error, "Failed to decode config file %s", fileName);
        return R_FAIL;
    }

    cJSON_Delete(root);
    return R_OK;
}

//------------------------------------------------------------------------------
RESULT SerializationManager::Init()
{
    defs_.emplace("CameraDef", new CameraDef());

    char cwd[512];
    GetCurrentDirectory(512, cwd);

    PropertyContainer container;
    RESULT r = ReadConfig("configs/Camera.json", container);

    return R_OK;
}

//------------------------------------------------------------------------------
PropertyContainer Deserialize(/*data*/)
{
    /*
    const char* name = data.GetName();
    DataDef def = GetDef(name);

    PropertyContainer container;
    for (auto field : data)
    {
        findField in def
        container.Properties.Add({ propIdx, field.Value });
    }

    */

    return {};
}

#undef LOG_AND_FAIL

}

