#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace gsl
{
const std::string assetFilePath{"../Bezier-updated/assets/"};
const std::string shaderFilePath{"../Bezier-updated/"};
const std::string valuesFilePath{"../Bezier-updated/values/"};

enum ShaderTypes
{
    COLOR,
    TEXTURE,
    COLORLIGHT,
    TEXTURELIGHT
};
}

#endif // CONSTANTS_H
