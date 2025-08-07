#version 440

// Pixel input structure
layout(location = 0) in vec2 texCoord; // TEXCOORD0
layout(location = 1) in vec3 normal; // NORMAL

// Pixel output structure
layout(location = 0) out vec4 outputColor; // SV_TARGET

// Texture and sampler declarations
layout(binding = 1) uniform sampler2D shaderTexture; // 在描述符集 0 的绑定点 1 上，定义了一个名为 shaderTexture 的 uniform 变量，它是一个 2D 纹理采样器。
layout(std140, binding = 2) uniform DiffuseLightBuffer{
    vec4 diffuseLightColor; // Diffuse color
    vec4 ambientLight;
    vec3 lightDirection; // Light direction
}UBO;

void main() {
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
	vec4 textureColor = texture(shaderTexture, texCoord);
    
    // Set the default output color to the ambient light value for all pixels.
    vec4 color = UBO.ambientLight;
    
    // Invert the light direction for calculations.
    vec3 lightDir = -UBO.lightDirection;

    // Calculate the amount of light on this pixel.
    float lightIntensity = clamp(dot(normal, lightDir), 0.0, 1.0);
    
    if(lightIntensity > 0.0f)
    {
        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
        color += (UBO.diffuseLightColor * lightIntensity);
    }

    // Clamp the final light color.
    color = clamp(color, 0.0f, 1.0f);

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    outputColor = textureColor * color;
}