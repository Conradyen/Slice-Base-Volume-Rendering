#include "main.h"

Texture::Texture(const std::string& path,glm::vec3 dimension)
    : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr)
{
    // inintialize 3D texture from file
    m_LocalBuffer = load_3d_raw_data(path.c_str(), dimension);

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_3D, m_RendererID); // Bind without slot selection

    // glBindTexture(GL_TEXTURE_3D, g_volTexObj);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    // pixel transfer happens here from client to OpenGL server
    // glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, dimension.x, dimension.y, dimension.z, 0, GL_RED, GL_UNSIGNED_BYTE,m_LocalBuffer);

    Unbind();
    delete []m_LocalBuffer;
    // if (m_LocalBuffer)
    //     stbi_image_free(m_LocalBuffer);
};

Texture::~Texture(){
  glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(unsigned int slot) const{
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const{
  glBindTexture(GL_TEXTURE_2D, 0);
}
