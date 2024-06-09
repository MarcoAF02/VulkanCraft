
rem 编译 Shader 使用相对路径

rem 基础着色器
glslc ../shaders/base_render_shader.vert -o ../shaders/base_render_shader.vert.spv
glslc ../shaders/base_render_shader.frag -o ../shaders/base_render_shader.frag.spv

rem 灯光着色器
glslc ../shaders/point_light.vert -o ../shaders/point_light.vert.spv
glslc ../shaders/point_light.frag -o ../shaders/point_light.frag.spv
