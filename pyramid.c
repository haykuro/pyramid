#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
         #define FOV 30
      #define ZOOM 100
   float v[5][3]={{0,2,0},
{-2,-2,2},{2,-2,2},
 {2,-2,-2},{-2,-2,-2}};
int e[8][2]={{0,1},{0,2},
 {0,3},{0,4},{1,2},
  {2,3},{3,4},{4,1}};
  void gts(int*w,int*h)
   {struct winsize ws;
 ioctl(STDOUT_FILENO,
TIOCGWINSZ,&ws);*w=ws.ws_col;
        *h=ws.ws_row;}
     void r(float x,float y,
    float z,float*a,float*b,
   float*c,float ax,float ay,
  float az){float cosa=cos(ax)
   ,sina=sin(ax),cosb=cos(ay),
   sinb=sin(ay),cosc=cos(az),
sinc=sin(az);float nx=x,ny=y,
 nz=z,ty=cosa*ny-sina*nz;nz=
sina*ny+cosa*nz;ny=ty;float tx=
 cosb*nx+sinb*nz;nz=-sinb*nx+
 cosb*nz;nx=tx;tx=cosc*nx-sinc
*ny;ny=sinc*nx+cosc*ny;nx=tx;
*a=nx;*b=ny;*c=nz;}void p(
float x,float y,float z,int*scx,
 int*scy,int w,int h){*scx=(int)
(w/2+ZOOM*x/(z+FOV));*scy=(int)
(h/2-ZOOM*y/(z+FOV));}void rf(
 float ax,float ay,float az,int w,
  int h){char s[h][w];for(int i=0;
 i<h;++i)for(int j=0;j<w;++j)
 s[i][j]=' ';for(int i=0;i<8;++i)
 {float x1=v[e[i][0]][0],y1=v[
 e[i][0]][1],z1=v[e[i][0]][2],
x2=v[e[i][1]][0],y2=v[e[i][1]][1],
z2=v[e[i][1]][2];float nx1,ny1,
 nz1,nx2,ny2,nz2;r(x1,y1,z1,&nx1
,&ny1,&nz1,ax,ay,az);r(x2,y2,z2,
&nx2,&ny2,&nz2,ax,ay,az);int scx1,
scy1,scx2,scy2;p(nx1,ny1,nz1,&scx1,
&scy1,w,h);p(nx2,ny2,nz2,&scx2,&scy2,
w,h);int dx=abs(scx2-scx1),sx=scx1<
scx2?1:-1,dy=-abs(scy2-scy1),sy=scy1<
scy2?1:-1,err=dx+dy,e2;while(1){if(
scx1>=0&&scx1<w&&scy1>=0&&scy1<h)
 s[scy1][scx1]='#';if(scx1==scx2&&
 scy1==scy2)break;e2=2*err;if(e2>=
dy){err+=dy;scx1+=sx;}if(e2<=dx){
err+=dx;scy1+=sy;}}}printf("\033[H");
for(int i=0;i<h;++i){for(int j=0;
j<w;++j)putchar(s[i][j]);putchar
('\n');}}int main(){float ax=0,ay=0,
az=0;int w,h;while(1){gts(&w,&h);rf(
 ax,ay,az,w,h);ax+=0.04;ay+=0.05;az+=
0.03;usleep(50000);}}
