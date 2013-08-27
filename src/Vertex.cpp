#include "Vertex.hpp"

void drawVertArray(GLenum shape,const VertexArray& varr) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3,GL_FLOAT,sizeof(Vertex),&varr[0].loc[0]);
    glColorPointer(3,GL_FLOAT,sizeof(Vertex),&varr[0].color[0]);
    glNormalPointer(GL_FLOAT,sizeof(Vertex),&varr[0].normal[0]);

    glDrawArrays(shape,0,varr.size());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
