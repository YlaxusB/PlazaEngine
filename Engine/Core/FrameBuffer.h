#pragma once
namespace Engine {
	class FrameBuffer
	{
	public:
		unsigned int buffer;
		unsigned int colorBuffer;
		unsigned int depthBuffer;
		unsigned int renderBufferObject;
		GLenum target;

		FrameBuffer(const GLenum target) : target(target) {
			Init();
		};

		void Init();
		void Bind();
		void Unbind();

		void InitRenderBufferObject(GLenum internalFormat, int width, int height);
		void InitColorAttachment(GLenum textureTarget, GLenum internalFormat, int width, int height, GLenum format, GLenum type, GLint param);
		void InitDepthAttachment(GLint level, GLenum internalFormat, int width, int height, GLenum format, GLenum type);

		void DrawAttachments(GLenum attachments[]);
		void UpdateSize();
		void Terminate();
	};
}