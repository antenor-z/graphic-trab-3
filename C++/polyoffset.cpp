#include "polyoffset.h"
#include "state.h"

#ifdef _WIN32
#include <glad/glad.h>
#elif __linux__
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#else
#include <OpenGL/gl3.h>
#endif

PolygonOffsetPtr PolygonOffset::Make (float factor, float units)
{
  return PolygonOffsetPtr(new PolygonOffset(factor,units));
}

PolygonOffset::PolygonOffset (float factor, float units)
: m_factor(factor), m_units(units) 
{
}

PolygonOffset::~PolygonOffset ()
{
}

void PolygonOffset::Load (StatePtr )
{
  glPolygonOffset(m_factor,m_units);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_POLYGON_OFFSET_LINE);
}
void PolygonOffset::Unload (StatePtr )
{
  glDisable(GL_POLYGON_OFFSET_LINE);
  glDisable(GL_POLYGON_OFFSET_FILL);
}