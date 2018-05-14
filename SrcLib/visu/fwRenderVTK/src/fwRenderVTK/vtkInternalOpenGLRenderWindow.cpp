#include "fwRenderVTK/vtkInternalOpenGLRenderWindow.hpp"
#include "fwRenderVTK/FrameBufferItem.hpp"

#include <chrono>

namespace fwRenderVTK
{

vtkInternalOpenGLRenderWindow::vtkInternalOpenGLRenderWindow() :
    m_qtParentRenderer(0)
{
}

vtkInternalOpenGLRenderWindow::~vtkInternalOpenGLRenderWindow()
{
     this->OffScreenRendering = 0;
}

void    vtkInternalOpenGLRenderWindow::OpenGLInitState()
{
    Superclass::OpenGLInitState();

    // Before any of the gl* functions in QOpenGLFunctions are called for a
    // given OpenGL context, an initialization must be run within that context
    this->MakeCurrent();
    initializeOpenGLFunctions();
    glUseProgram(0); // Shouldn't Superclass::OpenGLInitState() handle this?
    glDisable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
    glDisable(GL_BLEND); // doesn't seem crucial (?) but it is one of the differnces that showed up in apitrace analysis
    glClearColor(1.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    GLfloat texcoords[] =
    {
        0,  0,
        0,  1,
        1,  1,
        1,  0
    };
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
}

void    vtkInternalOpenGLRenderWindow::internalRender()
{
    Superclass::Render();

    if (this->m_qtParentRenderer)
    {
        this->m_qtParentRenderer->update();
    }

}

void    vtkInternalOpenGLRenderWindow::Render()
{
    if (this->m_qtParentRenderer)
    {
        this->m_qtParentRenderer->update();
    }
}

void    vtkInternalOpenGLRenderWindow::setRenderer(FrameBufferRenderer *renderer)
{
    this->m_qtParentRenderer = renderer;
}

FrameBufferRenderer    *vtkInternalOpenGLRenderWindow::getRenderer() const
{
    return m_qtParentRenderer;
}

void    vtkInternalOpenGLRenderWindow::setFrameBufferObject(QOpenGLFramebufferObject *frameBuffer)
{
    this->BackLeftBuffer = this->FrontLeftBuffer = this->BackBuffer = this->FrontBuffer =
          static_cast<unsigned int>(GL_COLOR_ATTACHMENT0);

    // Save GL objects by static casting to standard C types. GL* types
    // are not allowed in VTK header files.
    QSize fboSize = frameBuffer->size();
    this->SetSize(static_cast<int>(fboSize.width()), static_cast<int>(fboSize.height()));
    this->NumberOfFrameBuffers = 1;
    this->FrameBufferObject       = static_cast<unsigned int>(frameBuffer->handle());
    this->DepthRenderBufferObject = 0; // static_cast<unsigned int>(depthRenderBufferObject);
    this->TextureObjects[0]       = static_cast<unsigned int>(frameBuffer->texture());
    this->OffScreenRendering = 1;
    this->OffScreenUseFrameBuffer = 1;
    this->SetAlphaBitPlanes(1);
    this->SetMultiSamples(0);
    this->Modified();

}

vtkInternalOpenGLRenderWindow   *vtkInternalOpenGLRenderWindow::New()
{
    return new vtkInternalOpenGLRenderWindow;
}

}
