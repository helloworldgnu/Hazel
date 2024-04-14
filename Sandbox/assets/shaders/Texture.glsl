// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
// 增加纹理索引，表示当前点使用的是第几个纹理
layout(location = 3) in float a_TexIndex;
// 支持tiling缩放因子
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
// 输出纹理索引到片元着色器
out float v_TexIndex;
// 输出tiling缩放因子到片元着色器
out float v_TilingFactor;

void main()
{
   v_Color = a_Color;
	  v_TexCoord = a_TexCoord;
	  v_TexIndex = a_TexIndex;
	  v_TilingFactor = a_TilingFactor;
//	 gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	  gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;

// 纹理数组u_Textures只是存储[0~15]的索引
// 实际绑定到哪个纹理上是在外面的程序中实现
uniform sampler2D u_Textures[16];

void main()
{
	 color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;
}