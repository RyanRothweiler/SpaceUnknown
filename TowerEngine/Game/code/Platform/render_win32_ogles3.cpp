// This can be swapped for other platforms. Such as render_android_opengl
// Only purpose of this is to include the necessary platform opengl headers

#include <dsound.h>
#include <Rpc.h>
#include <stdio.h>

#include <gl/gl.h>


// -----------------------------------------------------------------------------
// opengl extensionts. These are found in glext.h. Google it.
//typedef void gl_buffer_data (GLenum target, GLsizeiptr size, const void *data, GLenum usage);

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

typedef unsigned int GLenum;
typedef char GLchar;


typedef void (APIENTRY  *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

typedef void gl_gen_vertex_arrays (GLsizei n, GLuint *arrays);
typedef void gl_delete_vertex_arrays (GLsizei n, GLuint *arrays);
typedef void gl_bind_vertex_array (GLuint array);
typedef void gl_bind_buffer (GLenum target, GLuint buffer);
typedef void gl_buffer_data (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void gl_enable_vertex_attrib_array (GLuint index);
typedef void gl_vertex_attrib_pointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void gl_disable_vertex_attrib_array (GLuint index);
typedef void gl_gen_buffers (GLsizei n, GLuint *buffers);
typedef GLuint gl_create_shader (GLenum type);
typedef void gl_compile_shader (GLuint shader);
typedef void gl_shader_source (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);;
typedef void gl_get_shader_iv (GLuint shader, GLenum pname, GLint *params);
typedef void gl_get_shader_infolog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void gl_attach_shader (GLuint program, GLuint shader);
typedef void gl_link_program (GLuint program);
typedef void gl_get_programiv (GLuint program, GLenum pname, GLint *params);
typedef void gl_get_program_infolog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void gl_use_program (GLuint program);
typedef void gl_delete_shader (GLuint shader);
typedef void gl_validate_program (GLuint program);
typedef GLuint gl_create_program (void);
typedef GLint gl_get_uniform_location (GLuint program, const GLchar *name);
typedef void gl_uniform_matrix_4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void gl_delete_buffers (GLsizei n, const GLuint *buffers);
typedef void gl_debug_message_callback (GLDEBUGPROC callback, const void *userParam);
typedef void gl_uniform_4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void gl_uniform_3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void gl_uniform_1f (GLint location, GLfloat v0);
typedef void gl_uniform_1iv (GLint location, GLsizei count, const GLint *value);
typedef void gl_active_texture (GLenum texture);
typedef void gl_uniform_1i (GLint location, GLint v0);
typedef void gl_gen_framebuffers (GLsizei n, GLuint *framebuffers);
typedef void gl_bind_framebuffer (GLenum target, GLuint framebuffer);
typedef GLenum gl_check_framebuffer_status (GLenum target);
typedef void gl_delete_framebuffers (GLsizei n, const GLuint *framebuffers);
typedef void gl_framebuffer_texture2d (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void gl_gen_renderbuffers (GLsizei n, GLuint *renderbuffers);
typedef void gl_bind_renderbuffer (GLenum target, GLuint renderbuffer);
typedef void gl_renderbuffer_storage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void gl_framebuffer_renderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void gl_delete_program (GLuint program);
typedef void gl_draw_buffers (GLsizei n, const GLenum *bufs);
typedef void gl_uniform_3fv (GLint location, GLsizei count, const GLfloat *value);
typedef void gl_tex_storage_2d (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void gl_generate_mipmap (GLenum target);

gl_gen_vertex_arrays* glGenVertexArrays;
gl_delete_vertex_arrays* glDeleteVertexArrays;
gl_bind_vertex_array* glBindVertexArray;
gl_bind_buffer* glBindBuffer;
gl_buffer_data* glBufferData;
gl_enable_vertex_attrib_array* glEnableVertexAttribArray;
gl_vertex_attrib_pointer* glVertexAttribPointer;
gl_disable_vertex_attrib_array* glDisableVertexAttribArray;
gl_gen_buffers* glGenBuffers;
gl_create_shader* glCreateShader;
gl_compile_shader* glCompileShader;
gl_shader_source* glShaderSource;
gl_get_shader_iv* glGetShaderiv;
gl_get_shader_infolog* glGetShaderInfoLog;
gl_get_program_infolog* glGetProgramInfoLog;
gl_create_program* glCreateProgram;
gl_attach_shader* glAttachShader;
gl_link_program* glLinkProgram;
gl_get_programiv* glGetProgramiv;
gl_use_program* glUseProgram;
gl_delete_shader* glDeleteShader;
gl_validate_program* glValidateProgram;
gl_get_uniform_location* glGetUniformLocation;
gl_uniform_matrix_4fv* glUniformMatrix4fv;
gl_delete_buffers* glDeleteBuffers;
gl_debug_message_callback* glDebugMessageCallback;
gl_uniform_4f* glUniform4f;
gl_uniform_3f* glUniform3f;
gl_uniform_1f* glUniform1f;
gl_uniform_1iv* glUniform1iv;
gl_active_texture* glActiveTexture;
gl_uniform_1i* glUniform1i;
gl_gen_framebuffers* glGenFramebuffers;
gl_bind_framebuffer* glBindFramebuffer;
gl_check_framebuffer_status* glCheckFramebufferStatus;
gl_delete_framebuffers* glDeleteFramebuffers;
gl_framebuffer_texture2d* glFramebufferTexture2D;
gl_gen_renderbuffers* glGenRenderbuffers;
gl_bind_renderbuffer* glBindRenderbuffer;
gl_renderbuffer_storage* glRenderbufferStorage;
gl_framebuffer_renderbuffer* glFramebufferRenderbuffer;
gl_delete_program* glDeleteProgram;
gl_draw_buffers* glDrawBuffers;
gl_uniform_3fv* glUniform3fv;
gl_tex_storage_2d* glTexStorage2D;
gl_generate_mipmap* glGenerateMipmap;

#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_TEXTURE_CUBE_MAP_SEAMLESS      0x884F

#define GL_TEXTURE_CUBE_MAP               0x8513

#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A

#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_VALIDATE_STATUS                0x8B83
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_RGB16F                         0x881B
#define GL_RGBA16F                        0x881A
#define GL_RG                             0x8227
#define GL_RG16F                          0x822F
#define GL_FRAMEBUFFER_SRGB               0x8DB9

#define GL_SRGB                           0x8C40
#define GL_SRGB8                          0x8C41
#define GL_SRGB_ALPHA                     0x8C42
#define GL_SRGB8_ALPHA8                   0x8C43

#define GL_SRGB                           0x8C40
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_RENDERBUFFER                   0x8D41
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_CLAMP_TO_EDGE                  0x812F

#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF

#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_COLOR_ATTACHMENT16             0x8CF0
#define GL_COLOR_ATTACHMENT17             0x8CF1
#define GL_COLOR_ATTACHMENT18             0x8CF2
#define GL_COLOR_ATTACHMENT19             0x8CF3
#define GL_COLOR_ATTACHMENT20             0x8CF4
#define GL_COLOR_ATTACHMENT21             0x8CF5
#define GL_COLOR_ATTACHMENT22             0x8CF6
#define GL_COLOR_ATTACHMENT23             0x8CF7
#define GL_COLOR_ATTACHMENT24             0x8CF8
#define GL_COLOR_ATTACHMENT25             0x8CF9
#define GL_COLOR_ATTACHMENT26             0x8CFA
#define GL_COLOR_ATTACHMENT27             0x8CFB
#define GL_COLOR_ATTACHMENT28             0x8CFC
#define GL_COLOR_ATTACHMENT29             0x8CFD
#define GL_COLOR_ATTACHMENT30             0x8CFE
#define GL_COLOR_ATTACHMENT31             0x8CFF
#define GL_DEPTH_ATTACHMENT               0x8D00

#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
// -----------------------------------------------------------------------------

void SetWireframeFillMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void SetPolygonFillMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void EnableSeamlessCubeMap()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

#include "../Engine/EngineCore.h"
#include "../Engine/Renderer/ogles3.cpp"

WIN_EXPORT render::api Init(window_info WindowInfo, platform::api* PlatApi, profiler_data* ProfilerData, memory_arena* PermMem, memory_arena* TransMem)
{
	glGenVertexArrays = 			(gl_gen_vertex_arrays*)				PlatApi->GetProcAddress("glGenVertexArrays");
	glDeleteVertexArrays = 			(gl_delete_vertex_arrays*)			PlatApi->GetProcAddress("glDeleteVertexArrays");
	glBindVertexArray = 			(gl_bind_vertex_array*)				PlatApi->GetProcAddress("glBindVertexArray");
	glBufferData = 					(gl_buffer_data*)					PlatApi->GetProcAddress("glBufferData");
	glEnableVertexAttribArray = 	(gl_enable_vertex_attrib_array*)	PlatApi->GetProcAddress("glEnableVertexAttribArray");
	glBindBuffer = 					(gl_bind_buffer*)					PlatApi->GetProcAddress("glBindBuffer");
	glVertexAttribPointer = 		(gl_vertex_attrib_pointer*)			PlatApi->GetProcAddress("glVertexAttribPointer");
	glDisableVertexAttribArray = 	(gl_disable_vertex_attrib_array*)	PlatApi->GetProcAddress("glDisableVertexAttribArray");
	glGenBuffers = 					(gl_gen_buffers*)					PlatApi->GetProcAddress("glGenBuffers");
	glCreateShader = 				(gl_create_shader*)					PlatApi->GetProcAddress("glCreateShader");
	glCompileShader = 				(gl_compile_shader*)				PlatApi->GetProcAddress("glCompileShader");
	glShaderSource = 				(gl_shader_source*)					PlatApi->GetProcAddress("glShaderSource");
	glGetShaderiv = 				(gl_get_shader_iv*)					PlatApi->GetProcAddress("glGetShaderiv");
	glGetShaderInfoLog = 			(gl_get_shader_infolog*)			PlatApi->GetProcAddress("glGetShaderInfoLog");
	glCreateProgram = 				(gl_create_program*)				PlatApi->GetProcAddress("glCreateProgram");
	glAttachShader = 				(gl_attach_shader*)					PlatApi->GetProcAddress("glAttachShader");
	glLinkProgram = 				(gl_link_program*)					PlatApi->GetProcAddress("glLinkProgram");
	glGetProgramiv = 				(gl_get_programiv*)					PlatApi->GetProcAddress("glGetProgramiv");
	glGetProgramInfoLog = 			(gl_get_program_infolog*)			PlatApi->GetProcAddress("glGetProgramInfoLog");
	glUseProgram =	 				(gl_use_program*)					PlatApi->GetProcAddress("glUseProgram");
	glDeleteShader =	 			(gl_delete_shader*)					PlatApi->GetProcAddress("glDeleteShader");
	glDeleteProgram =	 			(gl_delete_program*)				PlatApi->GetProcAddress("glDeleteProgram");
	glValidateProgram =	 			(gl_validate_program*)				PlatApi->GetProcAddress("glValidateProgram");
	glGetUniformLocation =	 		(gl_get_uniform_location*)			PlatApi->GetProcAddress("glGetUniformLocation");
	glUniformMatrix4fv =	 		(gl_uniform_matrix_4fv*)			PlatApi->GetProcAddress("glUniformMatrix4fv");
	glDeleteBuffers =	 			(gl_delete_buffers*)				PlatApi->GetProcAddress("glDeleteBuffers");
	glDebugMessageCallback =	 	(gl_debug_message_callback*)		PlatApi->GetProcAddress("glDebugMessageCallback");
	glUniform4f =	 				(gl_uniform_4f*)					PlatApi->GetProcAddress("glUniform4f");
	glUniform3f =	 				(gl_uniform_3f*)					PlatApi->GetProcAddress("glUniform3f");
	glUniform1f =	 				(gl_uniform_1f*)					PlatApi->GetProcAddress("glUniform1f");
	glUniform1iv =	 				(gl_uniform_1iv*)					PlatApi->GetProcAddress("glUniform1iv");
	glActiveTexture =	 			(gl_active_texture*)				PlatApi->GetProcAddress("glActiveTexture");
	glUniform1i =	 				(gl_uniform_1i*)					PlatApi->GetProcAddress("glUniform1i");
	glGenFramebuffers =	 			(gl_gen_framebuffers*)				PlatApi->GetProcAddress("glGenFramebuffers");
	glBindFramebuffer =	 			(gl_bind_framebuffer*)				PlatApi->GetProcAddress("glBindFramebuffer");
	glCheckFramebufferStatus = 		(gl_check_framebuffer_status*) 		PlatApi->GetProcAddress("glCheckFramebufferStatus");
	glDeleteFramebuffers = 			(gl_delete_framebuffers*) 			PlatApi->GetProcAddress("glDeleteFramebuffers");
	glFramebufferTexture2D = 		(gl_framebuffer_texture2d*) 		PlatApi->GetProcAddress("glFramebufferTexture2D");
	glGenRenderbuffers = 			(gl_gen_renderbuffers*) 			PlatApi->GetProcAddress("glGenRenderbuffers");
	glRenderbufferStorage = 		(gl_renderbuffer_storage*) 			PlatApi->GetProcAddress("glRenderbufferStorage");
	glFramebufferRenderbuffer = 	(gl_framebuffer_renderbuffer*) 		PlatApi->GetProcAddress("glFramebufferRenderbuffer");
	glBindRenderbuffer = 			(gl_bind_renderbuffer*) 			PlatApi->GetProcAddress("glBindRenderbuffer");
	glDrawBuffers = 				(gl_draw_buffers*) 					PlatApi->GetProcAddress("glDrawBuffers");
	glUniform3fv = 					(gl_uniform_3fv*) 					PlatApi->GetProcAddress("glUniform3fv");
	glTexStorage2D = 				(gl_tex_storage_2d*) 				PlatApi->GetProcAddress("glTexStorage2D");
	glGenerateMipmap = 				(gl_generate_mipmap*) 				PlatApi->GetProcAddress("glGenerateMipmap");

	return ogles3::Initialize(WindowInfo, ProfilerData, PermMem, TransMem);
}