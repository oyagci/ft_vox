#version 330 core

out vec4 frag_color;

in vec3 v_position;
in vec2 v_texcoord;

uniform sampler2D screen;

void main()
{
	// frag_color = vec4(v_texcoord.x, v_texcoord.y, 0, 1);
	frag_color = texture(screen, v_texcoord);
}

// const float offset = 1.0 / 300.0;  

// void main()
// {
//     vec2 offsets[9] = vec2[](
//         vec2(-offset,  offset), // top-left
//         vec2( 0.0f,    offset), // top-center
//         vec2( offset,  offset), // top-right
//         vec2(-offset,  0.0f),   // center-left
//         vec2( 0.0f,    0.0f),   // center-center
//         vec2( offset,  0.0f),   // center-right
//         vec2(-offset, -offset), // bottom-left
//         vec2( 0.0f,   -offset), // bottom-center
//         vec2( offset, -offset)  // bottom-right    
//     );

//     float kernel[9] = float[](
//         -1, -1, -1,
//         -1,  9, -1,
//         -1, -1, -1
//     );

//     vec3 col = vec3(0.0);
//     for(int i = 0; i < 9; i++)
//         col += vec3(texture(screen, v_texcoord.st + offsets[i])) * kernel[i];
    
//     frag_color = vec4(col, 1.0);
// }
