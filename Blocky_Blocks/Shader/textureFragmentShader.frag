#version 330

//this is the texture
uniform sampler2D tex;

//this is the texture coord
in vec2 fragTexCoord;

//this is the output color of the pixel
out vec4 finalColor;

void main() {
    finalColor = texture(tex, fragTexCoord);
}

