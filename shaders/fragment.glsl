#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec4 VertexColor;
in float TexIndex;

uniform sampler2D uTextures[8];
uniform vec4 spriteColor;

vec4 sample_from_slot(int slot, vec2 uv)
{
    if (slot == 0) return texture(uTextures[0], uv);
    if (slot == 1) return texture(uTextures[1], uv);
    if (slot == 2) return texture(uTextures[2], uv);
    if (slot == 3) return texture(uTextures[3], uv);
    if (slot == 4) return texture(uTextures[4], uv);
    if (slot == 5) return texture(uTextures[5], uv);
    if (slot == 6) return texture(uTextures[6], uv);
    return texture(uTextures[7], uv);
}

void main()
{
    int slot = int(TexIndex + 0.5);
    FragColor = sample_from_slot(slot, TexCoord) * spriteColor * VertexColor;
}
