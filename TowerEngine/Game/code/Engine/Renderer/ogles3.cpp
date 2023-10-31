#pragma once

#ifndef openglcpp
#define openglcpp

#include "../Profiler.h"

namespace ogles3 {

	void CheckError(const char* funcName)
	{
		GLint err = glGetError();
		if (err != GL_NO_ERROR) {
			//LOGI("GL error after %s(): 0x%08x\n", funcName, err);
			int x = 0;
		}
	}

	int32 GetAttribLocation(shader* Shader, char* Attribute)
	{
		//PlatformApi.Print("GetAttribLocation");
		//PlatformApi.Print(Shader->VertPath.Array());
		return glGetAttribLocation(Shader->Program, Attribute);
	}

	void MakeProgram(shader* Shader)
	{
		// Delete old shader info if it exists
		if (Shader->Program != 0) {
			glDeleteShader(Shader->VertID);
			glDeleteShader(Shader->FragID);
			glDeleteProgram(Shader->Program);

			Shader->VertID = 0;
			Shader->FragID = 0;
			Shader->Program = 0;
		}

		int success;

		ClearMemory((uint8*)&Shader->ErrorInfo[0], ArrayCount(Shader->ErrorInfo));

		Shader->VertError = false;
		Shader->FragError = false;

		char* GLSLVersion =
		    "#version 100\n"
		    "#define LightType_Point 0\n"
		    "#define LightType_Directional 1\n"
		    "#define Expose\n";


		int VersionLength = CharArrayLength(GLSLVersion);
		int VertLength = CharArrayLength(Shader->VertSource);
		int FragLength = CharArrayLength(Shader->FragSource);

		// !!!!!!!!!!!!!! this is the ONLY the second time we call malloc!! So far.
		char* Vert = (char*)malloc(sizeof(char) * (VersionLength + VertLength + 1));
		char* Frag = (char*)malloc(sizeof(char) * (VersionLength + FragLength + 1));

		// Prepend the glsl version
		ConcatCharArrays(GLSLVersion, Shader->VertSource, Vert);
		ConcatCharArrays(GLSLVersion, Shader->FragSource, Frag);

		// vertex shader
		Shader->VertID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(Shader->VertID, 1, &Vert, GameNull);
		glCompileShader(Shader->VertID);
		glGetShaderiv(Shader->VertID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(Shader->VertID, ArrayCount(Shader->ErrorInfo), GameNull, Shader->ErrorInfo);
			Shader->Valid = false;
			Shader->VertError = true;

			//LOGI("Vertex Error \n%s", Shader->ErrorInfo);
			PlatformApi.Print("Vertex Shader Error");
			PlatformApi.Print(Shader->ErrorInfo);

			free(Vert);
			free(Frag);

			return;
		}

		// fragment shader
		Shader->FragID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(Shader->FragID, 1, &Frag, GameNull);
		glCompileShader(Shader->FragID);
		glGetShaderiv(Shader->FragID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(Shader->FragID, ArrayCount(Shader->ErrorInfo), GameNull, Shader->ErrorInfo);
			Shader->Valid = false;
			Shader->FragError = true;

			//LOGI("Fragment Error \n%s", Shader->ErrorInfo);
			PlatformApi.Print("Fragment Shader Error");
			PlatformApi.Print(Shader->ErrorInfo);

			free(Vert);
			free(Frag);

			return;
		}

		// link
		Shader->Program = glCreateProgram();
		glAttachShader(Shader->Program, Shader->VertID);
		glAttachShader(Shader->Program, Shader->FragID);
		glLinkProgram(Shader->Program);
		glGetProgramiv(Shader->Program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(Shader->Program, ArrayCount(Shader->ErrorInfo), GameNull, Shader->ErrorInfo);
			Shader->Valid = false;

			//LOGI("Link Error %s\n", Shader->ErrorInfo);
			PlatformApi.Print("Linking Shader Error");
			PlatformApi.Print(Shader->ErrorInfo);

			free(Vert);
			free(Frag);

			return;
		}

		// Get uniform locations
		for (int x = 0; x < Shader->Uniforms.Count; x++) {
			shader_uniform* Uni = Shader->Uniforms.Array[x];
			Uni->Location = glGetUniformLocation(Shader->Program, &Uni->Name.CharArray[0]);

			if (Uni->Location > 1000) {
				//Assert(0); // Issue
			}
		}


		//PlatformApi.Print("Shader compiled and linked successfully");

		free(Vert);
		free(Frag);

		Shader->Valid = true;
	}

	void GetFramebufferCubeMap(camera* Cam, bool32 GenerateMips, bool32 Trilinear)
	{
		unsigned int captureRBO;
		glGenFramebuffers(1, &Cam->Framebuffer);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, Cam->Framebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (uint32)Cam->Resolution.X, (uint32)Cam->Resolution.Y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		glGenTextures(1, &Cam->TextureColorbuffers[0]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Cam->TextureColorbuffers[0]);
		for (unsigned int i = 0; i < 6; ++i) {
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8,
			             (uint32)Cam->Resolution.X, (uint32)Cam->Resolution.Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		if (GenerateMips || Trilinear) {
			// Use trilinear filtering for mipmaps
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (GenerateMips) {
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}
	}

	void GetFramebufferDepth(camera* Cam, uint32 Width, uint32 Height)
	{
		// generate texture
		glGenTextures(1, Cam->TextureColorbuffers);
		glBindTexture(GL_TEXTURE_2D, Cam->TextureColorbuffers[0]);

		glTexStorage2D(GL_TEXTURE_2D,
		               1,
		               GL_DEPTH_COMPONENT24,
		               Width,
		               Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// bind to fbo
		glGenFramebuffers(1, &Cam->Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, Cam->Framebuffer);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Cam->TextureColorbuffers[0], 0);

		Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Cam is modified. Results of this are put into Cam
	void GetFramebuffer(camera* Cam, int32 ColorElementsCount)
	{
		return;

		int32 Height = (int32)Cam->Resolution.Y;
		int32 Width = (int32)Cam->Resolution.X;

		// fbo
		glGenFramebuffers(1, &Cam->Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, Cam->Framebuffer);

		// color attachment
		glGenTextures(3, Cam->TextureColorbuffers);
		for (unsigned int i = 0; i < 3; i++) {
			glBindTexture(GL_TEXTURE_2D, Cam->TextureColorbuffers[i]);

			if (ColorElementsCount == 0) {
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			} else {
				Assert(false);
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, Width, Height, 0, GL_RG, GL_FLOAT, NULL);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// attach texture to framebuffer
			glFramebufferTexture2D(
			    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, Cam->TextureColorbuffers[i], 0
			);
		}

		// rbo for depth and stencil. Means we can't read these.
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void MakeTexture(loaded_image *Image, uint32 *BitmapPixels, bool32 CorrectGamma)
	{
		glGenTextures(1, (GLuint *)&Image->GLID);
		glBindTexture(GL_TEXTURE_2D, Image->GLID);

		if (Image->BlendType == gl_blend_type::nearest) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		} else if (Image->BlendType == gl_blend_type::linear) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		GLint ColorSpace = GL_RGBA;
		if (CorrectGamma) {
			ColorSpace = GL_SRGB8_ALPHA8;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, ColorSpace,
		             Image->Width, Image->Height,
		             0, GL_RGBA, GL_UNSIGNED_BYTE, BitmapPixels);
	}

	// Sides needs to be in order of the opengl enum x, -x, y, -y, z, -z
	// SideWidth is the width of one image.
	// Assume gamma correction
	void MakeCubeMap(cube_map* CubeMap, int32 SideWidth, int32 SideHeight, bitmap_pixels* Sides)
	{
		glGenTextures(1, (GLuint *)&CubeMap->GLID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap->GLID);

		for (uint32 i = 0; i < 6; i++) {
			glTexImage2D(
			    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			    0, GL_SRGB8_ALPHA8, SideWidth, SideHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Sides[i].Pixels
			);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void MakeHDRTexture(loaded_image* Image, float* Data, int32 Width, int32 Height)
	{
		glGenTextures(1, (GLuint *)&Image->GLID);
		glBindTexture(GL_TEXTURE_2D, Image->GLID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, Data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	vao CreateVAO()
	{
		vao VAO = {};
		glGenVertexArrays(1, &VAO.ID);
		return VAO;
	}

	void VAOBind_v3(vao* VAO, v3* Data, uint32 DataCount, int32 Location)
	{
		glBindVertexArray(VAO->ID);

		glGenBuffers(1, &VAO->Buffers[VAO->BuffersCount]);

		glBindBuffer(GL_ARRAY_BUFFER, VAO->Buffers[VAO->BuffersCount]);
		glBufferData(GL_ARRAY_BUFFER, DataCount * sizeof(real32) * 3, Data, GL_STATIC_DRAW);
		glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(real32), (void*)0);
		glEnableVertexAttribArray(Location);

		VAO->BuffersCount++;
		Assert(VAO->BuffersCount < ArrayCount(VAO->Buffers));

		glBindVertexArray(0);
	}

	void VAOBind_v2(vao* VAO, v2* Data, uint32 DataCount, int32 Location)
	{
		glBindVertexArray(VAO->ID);

		glGenBuffers(1, &VAO->Buffers[VAO->BuffersCount]);

		glBindBuffer(GL_ARRAY_BUFFER, VAO->Buffers[VAO->BuffersCount]);
		glBufferData(GL_ARRAY_BUFFER, DataCount * sizeof(real32) * 2, Data, GL_STATIC_DRAW);
		glVertexAttribPointer(Location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(real32), (void*)0);
		glEnableVertexAttribArray(Location);

		VAO->BuffersCount++;
		Assert(VAO->BuffersCount < ArrayCount(VAO->Buffers));

		glBindVertexArray(0);
	}

	void RenderRenderCommand(camera* Camera, render_command* Command, shader* Shader, window_info WindowInfo, bool32 Scissor)
	{
		if (Command->Wireframe) {
			SetWireframeFillMode();
		} else {
			SetPolygonFillMode();
		}

		glUseProgram(Shader->Program);

		if (Scissor) {
			vector2 clip_off = vector2{0, 0};
			vector2 clip_scale = vector2{1, 1};

			vector4 clip_rect = {0, 0, (real64)WindowInfo.Width, (real64)WindowInfo.Height};
			if (Command->ClipRect.X != 0 || Command->ClipRect.Y != 0 || Command->ClipRect.Z != 0 || Command->ClipRect.W != 0) {
				clip_rect.X = (Command->ClipRect.X - clip_off.X) * clip_scale.X;
				clip_rect.Y = (Command->ClipRect.Y - clip_off.Y) * clip_scale.Y;
				clip_rect.Z = (Command->ClipRect.Z - clip_off.X) * clip_scale.X;
				clip_rect.W = (Command->ClipRect.W - clip_off.Y) * clip_scale.Y;
			}

			glScissor((int)clip_rect.X, (int)(WindowInfo.Height - clip_rect.W), (int)(clip_rect.Z - clip_rect.X), (int)(clip_rect.W - clip_rect.Y));
		}

		bool32 CreateVAO = (Command->VAO < 0);

		// Bind VAO. Make one if needed.
		uint32 VAO = Command->VAO;
		if (CreateVAO) {
			glGenVertexArrays(1, &VAO);
		}
		glBindVertexArray(VAO);

		if (CreateVAO) {

			for (uint32 i = 0; i < Command->LayoutsDataCount; i++) {
				layout_data* Layout = &Command->LayoutsData[i];

				glGenBuffers(1, &Layout->BufferID);
				glBindBuffer(GL_ARRAY_BUFFER, Layout->BufferID);

				switch (Layout->LayoutInfo->Type) {
					case glsl_type::gl_vec4:
						glBufferData(GL_ARRAY_BUFFER, Command->BufferCapacity * Layout->DataSize, Layout->Data.Vec4, GL_STATIC_DRAW);
						break;
					case glsl_type::gl_vec3:
						glBufferData(GL_ARRAY_BUFFER, Command->BufferCapacity * Layout->DataSize, Layout->Data.Vec3, GL_STATIC_DRAW);
						break;
					case glsl_type::gl_vec2:
						glBufferData(GL_ARRAY_BUFFER, Command->BufferCapacity * Layout->DataSize, Layout->Data.Vec2, GL_STATIC_DRAW);
						break;
					case glsl_type::gl_float:
						glBufferData(GL_ARRAY_BUFFER, Command->BufferCapacity * Layout->DataSize, Layout->Data.Float, GL_STATIC_DRAW);
						break;
					case glsl_type::gl_int:
						glBufferData(GL_ARRAY_BUFFER, Command->BufferCapacity * Layout->DataSize, Layout->Data.Int, GL_STATIC_DRAW);
						break;
					default:
						// That layout type isn't supported... yet
						Assert(0);
						break;
				};

				glVertexAttribPointer(Layout->LayoutInfo->Loc, Layout->DataStride, GL_FLOAT, GL_FALSE, Layout->DataSize, (void*)0);
				glEnableVertexAttribArray(Layout->LayoutInfo->Loc);
			}
		}

		// Uniforms
		Command->Uniforms.SetMat4("projection", m4y4Transpose(Camera->ProjectionMatrix));
		Command->Uniforms.SetMat4("view", m4y4Transpose(Camera->ViewMatrix));
		Command->Uniforms.SetVec3("viewPosition", v3::From(Camera->Center));

		if (Command->Skybox) {
			m4y4 Matrix = Camera->ViewMatrix;

			Matrix.E[0][3] = 0;
			Matrix.E[1][3] = 0;
			Matrix.E[2][3] = 0;

			Matrix.E[3][0] = 0;
			Matrix.E[3][1] = 0;
			Matrix.E[3][2] = 0;

			Command->Uniforms.SetMat4("view", m4y4Transpose(Matrix));
		}

		int32 TexCount = 0;
		for (int x = 0; x < Command->Uniforms.Count; x++) {

			shader_uniform* Uni = Command->Uniforms.Array[x];
			Uni->Location = glGetUniformLocation(Shader->Program, &Uni->Name.CharArray[0]);

			switch (Uni->Type) {

				case glsl_type::gl_int: {

					int32 Count = 0;
					int32 * Data = {};
					if (Uni->ArrayCount == 0) {
						Count = 1;
						Data = &Uni->Data.Int;
					} else {
						Count = Uni->ArrayCount;
						Data = &Uni->Data.IntArray[0];
					}

					glUniform1iv(Uni->Location, Count, (GLint*)Data);
				} break;

				case glsl_type::gl_bool: {
					// tood
				} break;

				case glsl_type::gl_float: {

					int32 Count = 0;
					float* Data = {};

					if (!Uni->IsArray) {
						Count = 1;
						Data = &Uni->Data.Float;		
					} else {
						Count = Uni->ArrayCount;
						Data = &Uni->Data.FloatArray[0];
					}

					//glUniform1f(Uni->Location, (GLfloat)Uni->Data.Float);
					glUniform1fv(Uni->Location, Count, (GLfloat*)Data);
				} break;

				case glsl_type::gl_vec3: {

					v3* Data = {};
					int32 Count = Uni->ArrayCount;
					if (!Uni->IsArray) {
						Count = 1;
						Data = &Uni->Data.Vec3;
					} else {
						Data = &Uni->Data.Vec3Array[0];
					}

					glUniform3fv(Uni->Location, Count, (GLfloat*)Data);
				} break;

				case glsl_type::gl_vec4: {
					glUniform4f(Uni->Location, Uni->Data.Vec4.X, Uni->Data.Vec4.Y, Uni->Data.Vec4.Z, Uni->Data.Vec4.W);
				} break;

				case glsl_type::gl_mat4: {

					real32* Data = {};
					int32 Count = Uni->ArrayCount;
					if (!Uni->IsArray) {
						Count = 1;
						Data = &Uni->Data.Mat4.E[0][0];
					} else {
						Data = &Uni->Data.Mat4Array->E[0][0];
					}

					if (Data != GameNull) {
						glUniformMatrix4fv(Uni->Location, Count, GL_FALSE, (GLfloat*)Data);
					} else {
						// Maybe put logging here. This will probably be a bug someday.
						// This means you're not setting the data on this uniform.
					}
				} break;

				case glsl_type::gl_sampler2D:
				case glsl_type::gl_samplerCube: {
					glUniform1i(Uni->Location, TexCount);

					glActiveTexture(GL_TEXTURE0 + TexCount);

					if (Uni->Type == glsl_type::gl_sampler2D) {
						if (Uni->Data.ImageID == GameNull) {
							glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)Command->DefaultWhiteImage->GLID);
						} else {
							glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)Uni->Data.ImageID);
						}
					} else if (Uni->Type == glsl_type::gl_samplerCube) {
						if (Uni->Data.ImageID != GameNull) {
							glBindTexture(GL_TEXTURE_CUBE_MAP, (GLuint)(intptr_t)Uni->Data.ImageID);
						}
					}

					TexCount++;
				} break;

				default: {
					// Unsupported uniform type
					Assert(0);
				}
			}
		}

		glDrawElements(GL_TRIANGLES, Command->BufferCapacity, GL_UNSIGNED_INT, Command->IndexBuffer);
		CheckError("testing");

		GlobalProfilerData->DrawCallCount++;

		// Reset texture binds
		for (int x = 0; x < TexCount; x++) {
			glActiveTexture(GL_TEXTURE0 + x);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Delete VAO
		if (CreateVAO) {
			glDeleteVertexArrays(1, &VAO);

			for (uint32 i = 0; i < Command->LayoutsDataCount; i++) {
				layout_data* Layout = &Command->LayoutsData[i];
				glDeleteBuffers(1, &Layout->BufferID);
			}
		}
	}

	// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
	// https://stackoverflow.com/questions/11863416/read-texture-bytes-with-glreadpixels
	int32 DoPickRender(camera* Camera, vector2 MousePos, window_info WindowInfo)
	{
		uint32 PickFrameBufffer;
		glGenFramebuffers(1, &PickFrameBufffer);
		glBindFramebuffer(GL_FRAMEBUFFER, PickFrameBufffer);

		//Bind the texture to your FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Camera->TextureColorbuffers[2], 0);

		//Test if anything failed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {

			unsigned char data[4];
			glReadPixels((int32)MousePos.X, (int32)(WindowInfo.Height - MousePos.Y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			int pickedID =
			    data[0] +
			    data[1] * 256 +
			    data[2] * 256 * 256;
			return pickedID;
		}

		return 0;
	}

	void RenderCamera(camera * Camera, renderer * Renderer, window_info WindowInfo, bool32 Scissor, bool32 DynamicBatch)
	{
		list_link* CurrentLink = Renderer->RenderCommands->TopLink;
		while (CurrentLink != GameNull) {

			render_command* Command = (render_command*)CurrentLink->Data;


			// How many render commands were rendering. 1 means we're doing no batching at all
			uint32 BatchCount = 1;
#if 0
			render_command BatchedCommand = {};

			// dynamic batching
			int32 FinalSize = Command->BufferCapacity;


			// Dynamic batching. Maybe we want this? It'll help some but probably manual forced batching will have better results.
			// We'll need to optimize the batching anyways so don't waste time on dynamic
			if (DynamicBatch) {
				// How far can we batch?
				{
					list_link* LinkChecking = CurrentLink->NextLink;
					while (LinkChecking != GameNull) {
						render_command* NextCommand = (render_command*)LinkChecking->Data;
						LinkChecking = LinkChecking->NextLink;

						bool32 Batchable = true;

						if (NextCommand->LayoutsDataCount != Command->LayoutsDataCount) {
							Batchable = false;
							break;
						}


						if (NextCommand->Shader.Program != Command->Shader.Program) {
							Batchable = false;
							break;
						}

						if (NextCommand->Uniforms.Count != Command->Uniforms.Count) {
							Batchable = false;
							break;
						}

						// This assumes the uniforms are in the same order, which is probably mostly true? Is it always true?
						for (int c = 0; c < NextCommand->Uniforms.Count; c++) {
							if (NextCommand->Uniforms.Array[c] != Command->Uniforms.Array[c] &&
							        !NextCommand->Uniforms.Array[c]->Equals(Command->Uniforms.Array[c])) {
								Batchable = false;
								break;
							}

						}

						if (Batchable) {
							BatchCount++;
							FinalSize += NextCommand->BufferCapacity;
						} else {
							break;
						}
					}
				}

				// Combine into one render command
				if (BatchCount > 1) {

					// Copy values from the original command, then write over them with the full batched data
					BatchedCommand = *Command;

					BatchedCommand.BufferCapacity = FinalSize;
					BatchedCommand.DefaultWhiteImage = Command->DefaultWhiteImage;
					BatchedCommand.LayoutsDataCount = Command->LayoutsDataCount;
					BatchedCommand.VAO = -1;
					BatchedCommand.Shader = Command->Shader;

					BatchedCommand.IndexBuffer = (uint32*)ArenaAllocate(GlobalTransMem, sizeof(uint32) * FinalSize);
					for (int k = 0; k < BatchedCommand.BufferCapacity; k++) {
						BatchedCommand.IndexBuffer[k] = k;
					}

					// Copy uniforms
					BatchedCommand.Uniforms = Command->Uniforms;

					// Collate all layouts
					for (uint32 k = 0; k < Command->LayoutsDataCount; k++) {
						layout_data* Layout = &BatchedCommand.LayoutsData[k];
						Layout->Allocate(Command->LayoutsData[k].LayoutInfo, FinalSize, GlobalTransMem);

						int DestI = 0;

						list_link* Link = CurrentLink;

						for (uint32 l = 0; l < BatchCount; l++) {
							render_command* SourceCommand = (render_command*)Link->Data;
							layout_data::data* DataSource = &SourceCommand->LayoutsData[k].Data;

							switch (Layout->LayoutInfo->Type) {
								case glsl_type::gl_vec4:
									for (int n = 0; n < SourceCommand->LayoutsData[k].DataCount; n++) {
										Layout->Data.Vec4[DestI++] = DataSource->Vec4[n];
									}
									break;
								case glsl_type::gl_vec3:
									for (int n = 0; n < SourceCommand->LayoutsData[k].DataCount; n++) {
										Layout->Data.Vec3[DestI++] = DataSource->Vec3[n];
									}
									break;
								case glsl_type::gl_vec2:
									for (int n = 0; n < SourceCommand->LayoutsData[k].DataCount; n++) {
										Layout->Data.Vec2[DestI++] = DataSource->Vec2[n];
									}
									break;
								case glsl_type::gl_float:
									for (int n = 0; n < SourceCommand->LayoutsData[k].DataCount; n++) {
										Layout->Data.Float[DestI++] = DataSource->Float[n];
									}
									break;
								case glsl_type::gl_int:
									for (int n = 0; n < SourceCommand->LayoutsData[k].DataCount; n++) {
										Layout->Data.Int[DestI++] = DataSource->Int[n];
									}
									break;
								default:
									// That layout type isn't supported.
									Assert(0);
									break;
							};

							Link = Link->NextLink;
						}
					}

					Command = &BatchedCommand;
				}
			}
#endif

			// Render finally
			RenderRenderCommand(Camera, Command, &Command->Shader, WindowInfo, Scissor);

			if (BatchCount > 1) {
				for (uint32 j = 0; j < BatchCount; j++) {
					CurrentLink = CurrentLink->NextLink;
				}
			} else {
				CurrentLink = CurrentLink->NextLink;
			}
		}
	}

	void RenderCameraToBuffer(camera * Camera, renderer * Renderer, window_info WindowInfo)
	{
		glViewport(0, 0, (int)Camera->Resolution.X, (int)Camera->Resolution.Y);
		glScissor(0, 0, (int)Camera->Resolution.X, (int)Camera->Resolution.X);

		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, Camera->Framebuffer);

		// make sure we clear the framebuffer's content
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCamera(Camera, Renderer, WindowInfo, false, true);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, (int)WindowInfo.Width, (int)WindowInfo.Height);
	}

	void BakeIBL(renderer* EquiRenderer, renderer* ConvRenderer, renderer* PreFilterRenderer, window_info* WindowInfo)
	{
		// TODO store this in the camera data, it should know its framebuffer size
		uint32 EquiToCubeRes = 512;
		uint32 ConvRes = 32;
		uint32 PreFilterRes = 128;

		vector3 Rotations[] = {
			vector3{0, 0, 0}, // 0
			vector3{PI, 0, 0}, // 1
			vector3{0, -PI / 2, 0}, // 2
			vector3{0, PI / 2, 0}, // 3
			vector3{PI / 2, 0, 0}, // 4
			vector3{ -PI / 2, 0, 0}, // 5
		};

		vector3 UpDirections[] = {
			vector3{0, 0, 1}, // 0
			vector3{0, 0, 1}, // 1
			vector3{0, 1, 0}, // 2
			vector3{0, -1, 0}, // 3
			vector3{0, 0, 1}, // 4
			vector3{0, 0, 1}, // 5
		};

		// ---------------------------------------------------------------------
		// 1. Convert the equirectangular texture into a cubemap into
		// ---------------------------------------------------------------------
		glViewport(0, 0, EquiToCubeRes, EquiToCubeRes);

		glBindFramebuffer(GL_FRAMEBUFFER, EquiRenderer->Camera->Framebuffer);
		for (int x = 0; x < ArrayCount(Rotations); x++) {
			EquiRenderer->Camera->EulerRotation = Rotations[x];
			EquiRenderer->Camera->UpdateMatricies(UpDirections[x]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + x,
			                       EquiRenderer->Camera->TextureColorbuffers[0], 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderCamera(EquiRenderer->Camera, EquiRenderer, *WindowInfo, false, true);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, EquiRenderer->Camera->TextureColorbuffers[0]);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// ---------------------------------------------------------------------


		// ---------------------------------------------------------------------
		// 2. Diffuse convolude the cubemap from 1
		// ---------------------------------------------------------------------
		glViewport(0, 0, ConvRes, ConvRes);

		glBindFramebuffer(GL_FRAMEBUFFER, ConvRenderer->Camera->Framebuffer);
		for (int x = 0; x < ArrayCount(Rotations); x++) {

			ConvRenderer->Camera->EulerRotation = Rotations[x];
			ConvRenderer->Camera->UpdateMatricies(UpDirections[x]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + x,
			                       ConvRenderer->Camera->TextureColorbuffers[0], 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderCamera(ConvRenderer->Camera, ConvRenderer, *WindowInfo, false, true);
		}

		glViewport(0, 0, WindowInfo->Width, WindowInfo->Height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// ---------------------------------------------------------------------


		// ---------------------------------------------------------------------
		// 3. PreFilter / Specular convolude
		// ---------------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, PreFilterRenderer->Camera->Framebuffer);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {

			unsigned int mipWidth  = (uint32)(PreFilterRes * pow(0.5f, mip));
			unsigned int mipHeight = (uint32)(PreFilterRes * pow(0.5f, mip));

			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);

			render_command* Command = (render_command*)PreFilterRenderer->RenderCommands->TopLink->Data;
			Command->Uniforms.SetFloat("roughness", roughness);

			for (int x = 0; x < ArrayCount(Rotations); x++) {

				PreFilterRenderer->Camera->EulerRotation = Rotations[x];
				PreFilterRenderer->Camera->UpdateMatricies(UpDirections[x]);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + x,
				                       PreFilterRenderer->Camera->TextureColorbuffers[0], mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				RenderCamera(PreFilterRenderer->Camera, PreFilterRenderer, *WindowInfo, false, true);
			}
		}

		glViewport(0, 0, WindowInfo->Width, WindowInfo->Height);
	}

	void Render(render::api * API,
	             camera * ActiveCam, window_info * WindowInfo,
	             renderer * DebugUIRenderer, renderer* UIRenderer, renderer * GameRenderer, shader * GaussianBlurShader
	            )
	{
		glViewport(0, 0, WindowInfo->Width, WindowInfo->Height);

		glScissor(0, 0, (int)WindowInfo->Width, (int)WindowInfo->Height);
		glClearStencil(0x00);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Render scene
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// make sure we clear the framebuffer's content
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderCamera(ActiveCam, GameRenderer, *WindowInfo, false, true);
		}

		// Render UI
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderCamera(UIRenderer->Camera, UIRenderer, *WindowInfo, true, true);

		glClear(GL_DEPTH_BUFFER_BIT);
		RenderCamera(DebugUIRenderer->Camera, DebugUIRenderer, *WindowInfo, true, true);

		glFlush();
		glFinish();
	}

	render::api Initialize(window_info WindowInfo, profiler_data* ProfilerData, memory_arena* PermMem, memory_arena* TransMem)
	{
		render::api Ret = {};

		GlobalPermMem = PermMem;
		GlobalTransMem = TransMem;

		const char* versionStr = (const char*)glGetString(GL_VERSION);

		Ret.MakeProgram = &MakeProgram;
		Ret.MakeTexture = &MakeTexture;
		Ret.MakeCubeMap = &MakeCubeMap;
		Ret.MakeHDRTexture = &MakeHDRTexture;
		Ret.GetFramebuffer = &GetFramebuffer;
		Ret.GetFramebufferDepth = &GetFramebufferDepth;
		Ret.GetFramebufferCubeMap = &GetFramebufferCubeMap;
		Ret.Render = &Render;
		Ret.CreateVAO = &CreateVAO;
		Ret.VAOBind_v2 = &VAOBind_v2;
		Ret.VAOBind_v3 = &VAOBind_v3;
		Ret.DoPickRender = &DoPickRender;
		Ret.BakeIBL = &BakeIBL;
		Ret.RenderCameraToBuffer = &RenderCameraToBuffer;
		Ret.GetAttribLocation = &GetAttribLocation;


		// This must happen after all TIME_BEGIN calls so that COUNTER is exactly the number of TIME_BEGIN calls
		// COPIED FROM ENGINE
		{
			GlobalProfilerData = ProfilerData;

			GlobalProfilerData->TimesCount = COUNTER;
			GlobalProfilerData->AccumulatedMethods = (accum_method*)ArenaAllocate(PermMem, GlobalProfilerData->TimesCount * sizeof(accum_method));
			GlobalProfilerData->AccumulatedMethodsGraphs = (graph_data*)ArenaAllocate(PermMem, GlobalProfilerData->TimesCount * sizeof(graph_data));
		}

		// set state
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDisable(GL_CULL_FACE);

			glEnable(GL_BLEND);
			glEnable(GL_SCISSOR_TEST);
			glEnable(GL_DEPTH_TEST);

			glDepthFunc(GL_LEQUAL);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		}

		return Ret;
	};

}

#endif
