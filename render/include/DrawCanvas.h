#pragma once

#include "Config.h"
#include "Enums.h"
#include "Types.h"

namespace vkr
{

class DrawCanvas
{
public:
    RESULT Init();
    void Draw();

private:
    VertexBuffer*   lines_{};
    Shader*         lineVert_{};
    Shader*         lineFrag_{};
    uint            lineVertType_{};

    uint            lineCount_{};
};

}
