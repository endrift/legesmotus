#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#include "GL/glu.h"
#ifdef __WIN32
#include "GL/glext.h"
#define GL_SRC0_RGB GL_SOURCE0_RGB
#define GL_SRC1_RGB GL_SOURCE1_RGB
#define GL_SRC2_RGB GL_SOURCE2_RGB
#define GL_SRC0_ALPHA GL_SOURCE0_ALPHA
#define GL_SRC1_ALPHA GL_SOURCE1_ALPHA
#define GL_SRC2_ALPHA GL_SOURCE2_ALPHA
#endif
#endif
