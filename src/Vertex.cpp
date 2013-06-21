#include "Vertex.hpp"

void drawVertArrayGL(GLenum shape,const VertexArray& varr) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3,GL_FLOAT,sizeof(Vertex),&varr[0].loc);
    glColorPointer(3,GL_FLOAT,sizeof(Vertex),&varr[0].color);

    glDrawArrays(shape,0,varr.size());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}
