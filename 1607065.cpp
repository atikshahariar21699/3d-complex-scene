#include<GL/gl.h>
#include<windows.h>
#include <GL/glut.h>
#include<stdlib.h>
#include<stdio.h>
#include<bits/stdc++.h>
using namespace std;



float cx=0,cy=12.8,cz=5,lx=0,ly=12.8,lz=0; ///ly=cy=0/12.8//cz=5
int s1=0,s2=0,s3=0;
int p1=0,p2=0,p3=0,sp=0,q1=0,q2=0,q3=0,fc=0,fc2=0,mv=0,bb=0,nn=0;
float angle2=0;int pp=1,qq=0,zz=0;



///
const double PI = 3.14159265389;
int anglex= 0, angley = 0, anglez = 0;          //rotation angles
int window;
int wired=0;
int shcpt=1;
int animat = 0;
const int L=22;
const int dgre=3;
int ncpt=L+1;
int clikd=0;
const int nt = 40;				//number of slices along x-direction
const int ntheta = 20;


///


GLfloat ctrlpoints[L+1][3] =
{
    {-0.827,7.325, 0.0},
    {-0.728,6.947, 0.0},
    {-0.370, 6.729, 0.0},
    {-0.132,6.511, 0.0},
    {0.125, 6.213,0.0},
    {0.383, 6.034,0.0},
    {0.681 ,5.856 ,0.0},
    {1.177, 5.558,0.0},
    {1.157 ,5.558,0.0},
    {1.296, 5.121,0.0},
    {1.514, 4.704,0.0},
    {1.951, 4.287,0.0},
    {2.408, 4.129,0.0},
    {2.904, 4.029,0.0},
    {3.281, 3.811,0.0},
    {3.5, 3.255,0.0},
    {3.758, 2.759,0.0},
    {4.075, 2.481,0.0},
    {4.591, 2.203,0.0},
    {4.790, 1.667,0.0},
    {4.968, 1.151,0.0},
    {5.088, 0.615,0.0},
    {5.187, 0.198,0.0}


};
long long nCr(int n, int r)
{
    if(r > n / 2) r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for(i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}
void BezierCurve ( double t,  float xy[2])
{
    double y=0;
    double x=0;
    t=t>1.0?1.0:t;
    for(int i=0; i<=L; i++)
    {
        int ncr=nCr(L,i);
        double oneMinusTpow=pow(1-t,double(L-i));
        double tPow=pow(t,double(i));
        double coef=oneMinusTpow*tPow*ncr;
        x+=coef*ctrlpoints[i][0];
        y+=coef*ctrlpoints[i][1];

    }
    xy[0] = float(x);
    xy[1] = float(y);

    //return y;
}




float p=0,z0=0;///zoom value p=0/35
float rot = 0;

float angle=0.0,z=0,y=0,xy=0;
float x;

void display();
void timer (int);


unsigned int ID,ID2;

class BmpLoader
{
public:
    unsigned char* textureData;
    int iWidth, iHeight;

    BmpLoader(const char* filename)
    {
        FILE *file=0;
        file=fopen(filename, "rb");
        if(!file)
            std::cout<<"File not found"<<std::endl;
        fread(&bfh, sizeof(BITMAPFILEHEADER),1,file);
        if(bfh.bfType != 0x4D42)
            std::cout<<"Not a valid bitmap"<<std::endl;
        fread(&bih, sizeof(BITMAPINFOHEADER),1,file);
        if(bih.biSizeImage==0)
            bih.biSizeImage=bih.biHeight*bih.biWidth*3;
        textureData = new unsigned char[bih.biSizeImage];
        fseek(file, bfh.bfOffBits, SEEK_SET);
        fread(textureData, 1, bih.biSizeImage, file);
        unsigned char temp;
        for(int i=0; i<bih.biSizeImage; i+=3)
        {
            temp = textureData[i];
            textureData[i] = textureData[i+2];
            textureData[i+2] = temp;

        }

        iWidth = bih.biWidth;
        iHeight = bih.biHeight;
        fclose(file);
    }

    ~BmpLoader()
    {
        delete [] textureData;
    }

private:
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;


};




void bush();

void reshape(int, int);


GLfloat v[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte quadIndices[6][4] =
{
    {0,1,2,3},//front
    {4,5,6,7},//back
    {0,3,6,5},//right
    {1,4,7,2},//left
    {1,0,5,4},
    {2,3,6,7}
};

static void getNormal3p
(GLfloat x1, GLfloat y1,GLfloat z1, GLfloat x2, GLfloat y2,GLfloat z2, GLfloat x3, GLfloat y3,GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}







void Bezier()
{
    int i, j;
    float x, y, z, r;				//current coordinates
    float x1, y1, z1, r1;			//next coordinates
    float theta;

    const float startx = 0, endx = ctrlpoints[L][0];
    //number of angular slices
    const float dx = (endx - startx) / nt;	//x step size
    const float dtheta = 2*PI / ntheta;		//angular step size

    float t=0;
    float dt=1.0/nt;
    float xy[2];
    BezierCurve( t,  xy);
    x = xy[0];
    r = xy[1];
    //rotate about z-axis
    float p1x,p1y,p1z,p2x,p2y,p2z;
    for ( i = 0; i < nt; ++i )  			//step through x
    {
        theta = 0;
        t+=dt;
        BezierCurve( t,  xy);
        x1 = xy[0];
        r1 = xy[1];

        //draw the surface composed of quadrilaterals by sweeping theta

    GLfloat curve_no[] = {0, 0, 0, 1.0};
    GLfloat curve_amb[] = {1.0,1.0,1.0,1};
    GLfloat curve_dif[] = {1.0,1.0,1.0,1};
    GLfloat curve_spec[] = {1,1,1,1};
    GLfloat curve_sh[] = {50};

    // GLfloat m_em[] = {1,1,1,1};

    glMaterialfv(GL_FRONT, GL_AMBIENT, curve_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, curve_dif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, curve_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, curve_sh);


        glBegin( GL_QUAD_STRIP );
        for ( j = 0; j <= ntheta; ++j )
        {
            theta += dtheta;
            double cosa = cos( theta );
            double sina = sin ( theta );
            y = r * cosa;
            y1 = r1 * cosa;	//current and next y
            z = r * sina;
            z1 = r1 * sina;	//current and next z

            //edge from point at x to point at next x
            glVertex3f (x, y, z);

            if(j>0)
            {
                getNormal3p(p1x,p1y,p1z,p2x,p2y,p2z,x, y, z);
            }
            else
            {
                p1x=x;
                p1y=y;
                p1z=z;
                p2x=x1;
                p2y=y1;
                p2z=z1;

            }
            glVertex3f (x1, y1, z1);

            //forms quad with next pair of points with incremented theta value
        }
        glEnd();
        x = x1;
        r = r1;
    } //for i




}




void cube(GLfloat colr1, GLfloat colr2, GLfloat colr3)
{



    GLfloat cube_no[] = {0, 0, 0, 1.0};
    GLfloat cube_amb[] = {colr1,colr2,colr3,1};
    GLfloat cube_dif[] = {colr1,colr2,colr3,1};
    GLfloat cube_spec[] = {1,1,1,1};
    GLfloat cube_sh[] = {20};

    // GLfloat m_em[] = {1,1,1,1};

    glMaterialfv(GL_FRONT, GL_AMBIENT, cube_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cube_dif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, cube_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, cube_sh);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {
        getNormal3p(v[quadIndices[i][0]][0], v[quadIndices[i][0]][1], v[quadIndices[i][0]][2],
                    v[quadIndices[i][1]][0], v[quadIndices[i][1]][1], v[quadIndices[i][1]][2],
                    v[quadIndices[i][2]][0], v[quadIndices[i][2]][1], v[quadIndices[i][2]][2]);


        glVertex3fv(&v[quadIndices[i][0]][0]);glTexCoord2f(1,1);
        glVertex3fv(&v[quadIndices[i][1]][0]);glTexCoord2f(1,0);
        glVertex3fv(&v[quadIndices[i][2]][0]);glTexCoord2f(0,0);
        glVertex3fv(&v[quadIndices[i][3]][0]);glTexCoord2f(0,1);

    }
    glEnd();


}





void update(float x,float y, float z, int c )
{
    v[c][0]=x;
    v[c][1]=y;
    v[c][2]=z;

}



void  face(GLfloat A[],GLfloat B[],GLfloat C[],GLfloat D[])
{
    glBegin(GL_POLYGON);
    glVertex3fv(A);
    glVertex3fv(B);
    glVertex3fv(C);
    glVertex3fv(D);
    glEnd();
}





// GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT
void light(int s1,int s2,int s3, int p1, int p2, int p3 , int q1, int q2, int q3)
{
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient1[]  = {0.5, 0.5, 0.5, 1.0};//environment light
    GLfloat light_ambient2[]  = {0.5, 0.5, 0.5, 1.0};//environment light
    GLfloat light_ambient3[]  = {0.6, 0.6, 0.6, 1.0};//environment light
    GLfloat light_diffuse1[]  = { 1.0, 1.0, 1.0, 1.0 };//reflects from objects
    GLfloat light_diffuse2[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_diffuse3[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };//incident light angle same angle reflection from object
    GLfloat light_position[] = { -62+q1, 68+q2, 202+q3, 1.0 };
    //GLfloat light_position1[] = { 2.45+q1, 10.4+q2, -3.0+q3, 1.0 };
    GLfloat light_position1[] = { -50, 80, 100, 1.0 };
    //GLfloat light_position2[] = { 0.5+q1, 100+q2, 20.0+q3, 1.0 };
    GLfloat light_position2[] = { -76.5, 41, 127, 1.0 };

    //GLfloat light_position3[] = { 0.0+p1, 7+p2, -6.0+p3, 1.0 };





    if(s1%2==0)
    {
        glEnable( GL_LIGHT0);
    }
    else
    {
        glDisable( GL_LIGHT0);
    }
    if(s2%2==0)
    {
        glEnable( GL_LIGHT1);
    }
    else
    {
        glDisable( GL_LIGHT1);
    }

     if(s3%2==0)
    {
        glEnable( GL_LIGHT2);
    }
    else
    {
        glDisable( GL_LIGHT2);
    }
    if(p1%2==1)
    {
        glLightfv( GL_LIGHT0, GL_AMBIENT, no_light);
        glLightfv( GL_LIGHT1, GL_AMBIENT, no_light);
        glLightfv( GL_LIGHT2, GL_AMBIENT, no_light);
    }
    else
    {
        glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient1);
        glLightfv( GL_LIGHT1, GL_AMBIENT, light_ambient2);
        glLightfv( GL_LIGHT2, GL_AMBIENT, light_ambient3);
    }
    if(p2%2==1)
    {
        glLightfv( GL_LIGHT0, GL_SPECULAR, no_light);
        glLightfv( GL_LIGHT1, GL_SPECULAR, no_light);
        glLightfv( GL_LIGHT2, GL_SPECULAR, no_light);

    }
    else
    {
        glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular);
        glLightfv( GL_LIGHT1, GL_SPECULAR, light_specular);
        glLightfv( GL_LIGHT2, GL_SPECULAR, light_specular);
    }
    if(p3%2==1)
    {
        glLightfv( GL_LIGHT0, GL_DIFFUSE, no_light);
        glLightfv( GL_LIGHT1, GL_DIFFUSE, no_light);
        glLightfv( GL_LIGHT2, GL_DIFFUSE, no_light);
    }
    else
    {
        glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
        glLightfv( GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
        glLightfv( GL_LIGHT2, GL_DIFFUSE, light_diffuse3);
    }


    //if


    glLightfv( GL_LIGHT0, GL_POSITION, light_position);


    glLightfv( GL_LIGHT1, GL_POSITION, light_position1);


    glLightfv( GL_LIGHT2, GL_POSITION, light_position2);


    GLfloat spot_direction[] = { 0.0, -1.0, 0.0 };
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightf( GL_LIGHT2, GL_SPOT_CUTOFF, 20.0);


}

void floor()
{
      update(1.0f, 1.0f, 1.0f,0);
      update(-1.0f, 1.0f, 1.0f,1);
      update(-1.0f,-1.0f, 1.0f,2);
      update(1.0f, -1.0f, 1.0f,3);
      update(-1.0f, 1.0f, -1.0f,4);
      update(1.0f, 1.0f, -1.0f,5);
      update(1.0f, -1.0f, -1.0f,6);
      update(-1.0f, -1.0f, -1.0f,7);
      //top
       cube(1.0f, 1.0f, 1.0f);
}


static void key_input(unsigned char input, int x, int y)
{
    switch (input)
    {

    case '6':
        cx+=1;
        break;
    case '4':
        cx-=1;
        break;
    case '8':
        cy+=2;
        break;
    case '5':
        cy-=2;
        break;
    case '+':
        cz=cz-1;
        lz=lz-1;
        break;
    case '-':
        cz=cz+1;
        lz=lz+1;
        break;
     case '*':
        z0=z0+.1;
        break;
    case '/':
        z0=z0-.1;
        break;
    case 'j':
        lx-=1;
        break;
    case 'l':
        lx+=1;
        break;
    case 'i':
        ly+=2;
        break;
    case 'k':
        ly-=2;
        break;
    case '1':
        s1++;
        break;
    case '2':
        s2++;
        break;
    case '3':
        s3++;
        break;
      case 'a':
        p1++;
        break;
    case 's':
        p2++;
        break;
    case 'd':
        p3++;
        break;
     case '.':
        sp=sp+10;
        break;
    case ',':
        sp--;
        break;
    case 'D':
        q1++;
        break;
    case 'A':
        q1--;
        break;
    case 'W':
        q2++;
        break;
      case 'S':
        q2--;
        break;
    case 'J':
        q3--;
        break;
    case 'L':
        q3++;
        break;
    case 'p':
        fc++;
        if(fc>=5)
        fc=0;
        break;
    case 'm':
        mv++;
        break;
     case 'o':
        fc2++;
        break;
     case 'b':
        bb++;
        break;
    case 'n':
        nn++;
        break;



    }

    glutPostRedisplay();
}

void windowbackground()
{
    glClearColor(0.0,0.0,0.0,1.0);/// window background color black
    glEnable(GL_DEPTH_TEST);
}











void LoadTexture(const char*filename, int i)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}




int main(int argc, char**argv)
{

    printf("\n\nFOR CHANGING CAMERA POSITION USE 4(for left),5(for down),6(for right),8(for up)\n\n\n");
    printf("FOR CHANGING CAMERA LOOKAT POSITION USE j(for left),k(for down),l(for right),i(for up)\n\n\n");
    printf(" USE + or *(for ZOOM IN),- or /(for ZOOM OUT)\n\n\n");
    printf("press 1 for light 1 \n press 2 for  light 2 \n press 3 for light 3 control\n press a for ambient\npress d for diffuse\npress s for specular light control\npresss m for play pause ");
    printf(" USE m (for waterfall control),\n\n");
    printf(" USE n (for updown control),\n\n");
    printf(" USE b (for round cradle control),\n\n");

    //push0();
    //char n;
    //scanf("%c",&n);
    //printf("Proceed");
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    //   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutInitWindowPosition(50,50);
    glutInitWindowSize(1000,1000);

    glutCreateWindow("1607065");
    LoadTexture("D:\\grafixlab4\\court.bmp", 1);
    LoadTexture("D:\\grafixlab4\\green.bmp", 2);
    LoadTexture("D:\\grafixlab4\\water.bmp", 3);
    LoadTexture("D:\\grafixlab4\\mountain.bmp", 4);
    LoadTexture("D:\\grafixlab4\\treebody.bmp",5);
    LoadTexture("D:\\grafixlab4\\sky.bmp",6);
    LoadTexture("D:\\grafixlab4\\leaf.bmp",7);
     LoadTexture("D:\\grafixlab4\\yellow.bmp",8);
    LoadTexture("D:\\grafixlab4\\red.bmp",9);
    LoadTexture("D:\\grafixlab4\\gray.bmp",10);
    LoadTexture("D:\\grafixlab4\\black.bmp",11);

    glutDisplayFunc(display);
    glutKeyboardFunc(key_input);
    glutReshapeFunc(reshape);
    glutTimerFunc(0,timer,0);


    windowbackground();///change background colour
    glEnable(GL_DEPTH_TEST);
    glShadeModel( GL_SMOOTH );
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    //light();
    //glEnable(GL_BLEND);



    glutMainLoop();
}
void hilltexture()
{


        //glRotatef(90,0,0,1);
        for(int i=0;i<=360;i=i+22.5){
          glPushMatrix();
          glRotatef(i,0,1,0);

         glPushMatrix();
          glTranslatef(0,6.78,0);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();

          glPushMatrix();
          glTranslatef(0,6.75,0.6);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();

          glPushMatrix();
          glTranslatef(0,6.55,1.2);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();

          glPushMatrix();
          glTranslatef(0,6.35,1.8);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();


          glPushMatrix();
          glTranslatef(0,6.05,2.4);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();


          glPushMatrix();
          glTranslatef(0,5.5,3);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();

          glPushMatrix();
          glTranslatef(0,5,3.6);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();


         glPushMatrix();
          glTranslatef(0,4.0,4.2);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();

          glPushMatrix();
          glTranslatef(0,3.35,4.8);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();


          glPushMatrix();
          glTranslatef(0,3,5.4);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();

          glPushMatrix();
          glTranslatef(0,2.25,6);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();

          glPushMatrix();
          glTranslatef(0,1.75,6.6);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();



          glPushMatrix();
          glTranslatef(0,1.25,7.2);
          glScalef(0.3,0.5,0.3);
         glBindTexture(GL_TEXTURE_2D,7);
         floor();
          glPopMatrix();


          glPopMatrix();

        }

}





void hill()

{
     glPushMatrix();
    glTranslatef(0.0,2,0);
     glRotatef(90,0,0,1);
    glScalef(6,2,2);
    glBindTexture(GL_TEXTURE_2D,2);
    Bezier();
    glPopMatrix();



     glPushMatrix();
    glTranslatef(9.0,2,0);
     glRotatef(90,0,0,1);
    glScalef(4,2,2);
    glBindTexture(GL_TEXTURE_2D,2);
    Bezier();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(-9.0,2,0);
     glRotatef(90,0,0,1);
    glScalef(4,2,2);
    glBindTexture(GL_TEXTURE_2D,2);
    Bezier();
    glPopMatrix();

         glPushMatrix();
    glTranslatef(0.0,2,9.0);
     glRotatef(90,0,0,1);
    glScalef(4,2,2);
    glBindTexture(GL_TEXTURE_2D,2);
    Bezier();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(0.0,2,-9.0);
     glRotatef(90,0,0,1);
    glScalef(4,2,2);
    glBindTexture(GL_TEXTURE_2D,2);
    Bezier();
    glPopMatrix();



/// bottom row
    ///bush on the hill

    glPushMatrix();
    glTranslatef(0.0,0,23.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0,0,-23.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(23.0,0,0.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-23.0,0,0.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

   ///

     ///bush on the hill

    glPushMatrix();
    glTranslatef(2.0,0,23.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.0,0,-23.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(23.0,0,2.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-23.0,0,2.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-2.0,0,23.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.0,0,-23.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(23.0,0,-2.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-23.0,0,-2.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



   ///

     ///bush on the hill

    glPushMatrix();
    glTranslatef(4.0,0,22.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.0,0,-22.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22.5,0,4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-22.5,0,4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-4.0,0,22.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4.0,0,-22.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22.5,0,-4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-22.5,0,-4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



   ///



     ///bush on the hill

    glPushMatrix();
    glTranslatef(6.0,0,22);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.0,0,-22);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22,0,6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-22,0,6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-6.0,0,22);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-6.0,0,-22);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22,0,-6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-22,0,-6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

   ///

    ///bush on the hill

    glPushMatrix();
    glTranslatef(8.0,0,21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.0,0,-21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21,0,8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21,0,8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-8.0,0,21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-8.0,0,-21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21,0,-8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21,0,-8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();
   ///


          ///bush on the hill

    glPushMatrix();
    glTranslatef(10.0,0,19.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10.0,0,-19.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19.5,0,10.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19.5,0,10.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-10.0,0,19.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10.0,0,-19.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19.5,0,-10.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19.5,0,-10.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();
   ///


      ///bush on the hill

    glPushMatrix();
    glTranslatef(12.0,0,17.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12.0,0,-17.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17.5,0,12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-17.5,0,12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-12.0,0,17.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12.0,0,-17.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17.5,0,-12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-17.5,0,-12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();
   ///

           ///bush on the hill

    glPushMatrix();
    glTranslatef(14.0,0,14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(14.0,0,-14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(14,0,14.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-14,0,14.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-14.0,0,14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-14.0,0,-14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(14,0,-14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-14,0,-14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///


///2nd row

        ///bush on the hill2

    glPushMatrix();
    glTranslatef(1,1,22.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1,1,-22.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22.0,1,1);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-22.0,1,1);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-1,1,22.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1,1,-22.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22.0,1,-1);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-22.0,1,-1);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

   ///


           ///bush on the hill2

    glPushMatrix();
    glTranslatef(3,1,22.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3,1,-22.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22.0,1,3);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-22.0,1,3);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();




    glPushMatrix();
    glTranslatef(-3,1,22.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3,1,-22.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22.0,1,-3);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-22.0,1,-3);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

   ///





           ///bush on the hill2

    glPushMatrix();
    glTranslatef(5,1,21.75);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5,1,-21.75);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.75,1,5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.75,1,5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();




    glPushMatrix();
    glTranslatef(-5,1,21.75);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-5,1,-21.75);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.75,1,-5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.75,1,-5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

   ///


              ///bush on the hill2

    glPushMatrix();
    glTranslatef(7,1,21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7,1,-21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21,1,7);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21,1,7);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();




    glPushMatrix();
    glTranslatef(-7,1,21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-7,1,-21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21,1,-7);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21,1,-7);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

   ///


             ///bush on the hill2

    glPushMatrix();
    glTranslatef(9,1,19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(9,1,-19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19,1,9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19,1,9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();




    glPushMatrix();
    glTranslatef(-9,1,19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-9,1,-19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19,1,-9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19,1,-9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

   ///


              ///bush on the hill2

    glPushMatrix();
    glTranslatef(11,1,17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(11,1,-17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17,1,11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-17,1,11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();




    glPushMatrix();
    glTranslatef(-11,1,17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-11,1,-17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17,1,-11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-17,1,-11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

   ///



              ///bush on the hill2

    glPushMatrix();
    glTranslatef(13,1,15);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(13,1,-15);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15,1,13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15,1,13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();




    glPushMatrix();
    glTranslatef(-13,1,15);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-13,1,-15);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15,1,-13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15,1,-13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

   ///
///3rd row

///hill bush

glPushMatrix();
    glTranslatef(0.0,2,21.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0,2,-21.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.5,2,0.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.5,2,0.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


///


///

glPushMatrix();
    glTranslatef(2.0,2,21.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.0,2,-21.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.5,2,2.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.5,2,2.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-2.0,2,21.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.0,2,-21.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.5,2,-2.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.5,2,-2.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();
///

///
glPushMatrix();
    glTranslatef(4.0,2,21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.0,2,-21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.0,2,4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.0,2,4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-4.0,2,21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4.0,2,-21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.0,2,-4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.0,2,-4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///



///
glPushMatrix();
    glTranslatef(6.0,2,20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.0,2,-20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20.5,2,6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20.5,2,6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-6.0,2,20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-6.0,2,-20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20.5,2,-6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20.5,2,-6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///



///
glPushMatrix();
    glTranslatef(8.0,2,19.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.0,2,-19.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19.5,2,8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19.5,2,8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-8.0,2,19.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-8.0,2,-19.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19.5,2,-8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19.5,2,-8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///



///
glPushMatrix();
    glTranslatef(10,2,18);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10,2,-18);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(18,2,10);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-18,2,10);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-10,2,18);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10,2,-18);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(18,2,-10);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-18,2,-10);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///



///
glPushMatrix();
    glTranslatef(12,2,16.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12,2,-16.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(16.5,2,12);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-16.5,2,12);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-12,2,16.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12,2,-16.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(16.5,2,-12);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-16.5,2,-12);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///



///

///
glPushMatrix();
    glTranslatef(14,2,13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(14,2,-13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(13,2,14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-13,2,14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-14,2,13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-14,2,-13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(13,2,-14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-13,2,-14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///
///4th row


///
glPushMatrix();
    glTranslatef(1,3,21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1,3,-21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21,3,1);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21,3,1);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-1,3,21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1,3,-21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21,3,-1);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21,3,-1);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///


///
glPushMatrix();
    glTranslatef(3,3,21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3,3,-21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21,3,3);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21,3,3);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-3,3,21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3,3,-21);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21,3,-3);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21,3,-3);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///


///
glPushMatrix();
    glTranslatef(5,3,20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5,3,-20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20.5,3,5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20.5,3,5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-5,3,20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-5,3,-20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20.5,3,-5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20.5,3,-5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///

///
glPushMatrix();
    glTranslatef(7,3,20);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7,3,-20);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20,3,7);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20,3,7);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-7,3,20);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-7,3,-20);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20,3,-7);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20,3,-7);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///
glPushMatrix();
    glTranslatef(9,3,19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(9,3,-19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19,3,9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19,3,9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-9,3,19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-9,3,-19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19,3,-9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19,3,-9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///
glPushMatrix();
    glTranslatef(11,3,17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(11,3,-17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17,3,11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-17,3,11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-11,3,17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-11,3,-17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17,3,-11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-17,3,-11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///


glPushMatrix();
    glTranslatef(13,3,15.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(13,3,-15.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15.5,3,13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15.5,3,13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-13,3,15.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-13,3,-15.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15.5,3,-13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15.5,3,-13);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///
///5th row
///
    glPushMatrix();
    glTranslatef(0,4,21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,4,-21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.0,4,0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.0,4,0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


///
    glPushMatrix();
    glTranslatef(2,4,21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2,4,-21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.0,4,2);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.0,4,2);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-2,4,21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2,4,-21.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(21.0,4,-2);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-21.0,4,-2);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


    ///
    glPushMatrix();
    glTranslatef(4.0,4,20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.0,4,-20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20.5,4,4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20.5,4,4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-4.0,4,20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4.0,4,-20.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20.5,4,-4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20.5,4,-4.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


///
    ///
    glPushMatrix();
    glTranslatef(6.0,4,20);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.0,4,-20);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20,4,6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20,4,6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-6.0,4,20);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-6.0,4,-20);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20,4,-6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20,4,-6.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


    ///

   ///
    glPushMatrix();
    glTranslatef(8.0,4,19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.0,4,-19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19,4,8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19,4,8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-8.0,4,19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-8.0,4,-19);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19,4,-8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19,4,-8.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///

///
    glPushMatrix();
    glTranslatef(10.0,4,17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10.0,4,-17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17,4,10.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-17,4,10.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-10.0,4,17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10.0,4,-17);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17,4,-10.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-17,4,-10.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///
///
    glPushMatrix();
    glTranslatef(12.0,4,14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12.0,4,-14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(14,4,12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-14,4,12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-12.0,4,14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12.0,4,-14);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(14,4,-12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-14,4,-12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///
///5th row

///
///
    glPushMatrix();
    glTranslatef(13.0,5,12);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(13.0,5,-12);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12,5,13.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12,5,13.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-13.0,5,12);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-13.0,5,-12);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12,5,-13.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12,5,-13.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///





///
    glPushMatrix();
    glTranslatef(11,5,13.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(11,5,-13.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(13.5,5,11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-13.5,5,11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-11,5,13.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-11,5,-13.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(13.5,5,-11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-13.5,5,-11);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///

///
    glPushMatrix();
    glTranslatef(9,5,15.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(9,5,-15.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15.5,5,9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15.5,5,9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-9,5,15.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-9,5,-15.5);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15.5,5,-9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15.5,5,-9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///
///



///6th row
glPushMatrix();
    glTranslatef(12.0,7,10);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12.0,7,-10);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10,7,12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10,7,12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-12.0,7,10);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12.0,7,-10);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10,7,-12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10,7,-12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///
///7th row

glPushMatrix();
    glTranslatef(12.0,9,9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12.0,9,-9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(9,9,12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-9,9,12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();


  glPushMatrix();
    glTranslatef(-12.0,9,9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12.0,9,-9);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(9,9,-12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-9,9,-12.0);
    glScalef(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,7);floor();
    glPopMatrix();

///





///
///
//

///



}



void bigmountain()
{
      ///bigblock

    for(int i=-48;i>=-144;i=i-8)
        for(int j=-47;j<=48;j=j+8)
    {

         glPushMatrix();
    glTranslatef(i,-3.3,j+5);
    glScalef(5,2,4);
    glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
    }


     for(int i=47;i<=144;i=i+8)
        for(int j=-47;j<=48;j=j+8)
    {
         glPushMatrix();
    glTranslatef(i,-3.3,j+5);
    glScalef(5,2,4);
    glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
    }

         for(int i=-144;i<=144;i=i+8)
        for(int j=48;j<=192;j=j+8)
    {
         glPushMatrix();
    glTranslatef(i,-3.3,j+5);
    glScalef(5,2,4);
    glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
    }



///left

///mountain

  glPushMatrix();
  glTranslatef(-55,-2,-35);
  hill();
  glPopMatrix();

   glPushMatrix();
  glTranslatef(-100,-2,-55);
  glScalef(1.5,1.5,1.5);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-145,-2,-35);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-145,-2,10);
  glScalef(1,1,1);
  hill();
  glPopMatrix();


  glPushMatrix();
  glTranslatef(-145,-2,40);
  glRotatef(45,0,1,0);
  glScalef(1,2,1);
  hill();
  glPopMatrix();


  glPushMatrix();
  glTranslatef(-145,-2,70);
  glRotatef(45,0,1,0);
  glScalef(1,1.5,1);
  hill();
  glPopMatrix();

   glPushMatrix();
  glTranslatef(-145,-2,100);
  glRotatef(45,0,1,0);
  glScalef(1.5,1,1.5);
  hill();
  glPopMatrix();

   glPushMatrix();
  glTranslatef(-145,-2,130);
  glRotatef(45,0,1,0);
  glScalef(1.5,0.5,1.5);
  hill();
  glPopMatrix();

   glPushMatrix();
  glTranslatef(-145,-2,160);
  glRotatef(45,0,1,0);
  glScalef(1,1,1);
  hill();
  glPopMatrix();

   glPushMatrix();
  glTranslatef(-145,-2,200);
  glRotatef(45,0,1,0);
  glScalef(2,2,2);
  hill();
  glPopMatrix();

 glPushMatrix();
  glTranslatef(-110,-2,200);
  glRotatef(45,0,1,0);
  glScalef(1,1,1);
  hill();
  glPopMatrix();


  glPushMatrix();
  glTranslatef(-80,-2,200);
  glRotatef(45,0,1,0);
  glScalef(1,0.5,1);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-50,-2,200);
  glRotatef(45,0,1,0);
  glScalef(1.5,1.5,1.5);
  hill();
  glPopMatrix();

    glPushMatrix();
  glTranslatef(-20,-2,200);
  glRotatef(45,0,1,0);
  glScalef(1,1,1);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(20,-2,200);
  glRotatef(45,0,1,0);
  glScalef(1.5,2,1.5);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(60,-2,200);
  glRotatef(45,0,1,0);
  glScalef(0.5,0.5,0.5);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(100,-2,200);
  glRotatef(45,0,1,0);
  glScalef(1.5,1.5,1.5);
  hill();
  glPopMatrix();

   glPushMatrix();
  glTranslatef(140,-2,200);
  glRotatef(45,0,1,0);
  glScalef(2,2,2);
  hill();
  glPopMatrix();


///going left to right
  glPushMatrix();
  glTranslatef(140,-2,170);
  glRotatef(45,0,1,0);
  glScalef(1,1,1);
  hill();
  glPopMatrix();

   glPushMatrix();
  glTranslatef(140,-2,130);
  glRotatef(45,0,1,0);
  glScalef(1.5,1.5,1.5);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(140,-2,100);
  glRotatef(45,0,1,0);
  glScalef(1,1,1);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(140,-2,70);
  glRotatef(45,0,1,0);
  glScalef(1,1,1);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(140,-2,40);
  glRotatef(45,0,1,0);
  glScalef(1.5,1,1.5);
  hill();
  glPopMatrix();

   glPushMatrix();
  glTranslatef(140,-2,10);
  glRotatef(45,0,1,0);
  glScalef(1.,1,1);
  hill();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(140,-2,-20);
  glRotatef(45,0,1,0);
  glScalef(2,1,2);
  hill();
  glPopMatrix();




///right
///mountain

  glPushMatrix();
  glTranslatef(62,-2,-29);
  glRotatef(45,0,1,0);
  glScalef(1.5,2,1.5);
  hill();
  glPopMatrix();


  ///mountain

  glPushMatrix();
  glTranslatef(95,-2,-35);
  hill();
  glPopMatrix();



}

void soil()
{
    glTranslatef(0.0,0.0,24.0);
    ///soil block1
    glPushMatrix();
    glTranslatef(-5.0,-3.3,-5.0);

    glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
///soil block2
    glPushMatrix();
    glTranslatef(-1.0,-3.3,-1.0);

    glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

 ///soil block3
    glPushMatrix();
    glTranslatef(-5.0,-3.3,-1.0);

    glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

    ///soil block3
    glPushMatrix();
    glTranslatef(-5.0,-3.3,-9.0);

    glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


    ///grass block4
        glPushMatrix();
    glTranslatef(-5.0,-3.3,-13.0);

    glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

       ///grass block5
        glPushMatrix();
    glTranslatef(-9,-3.3,-13.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

           ///grass block6
        glPushMatrix();
    glTranslatef(-9,-3.3,-9.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block7
        glPushMatrix();
    glTranslatef(-9,-3.3,-5.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block8
        glPushMatrix();
    glTranslatef(-9,-3.3,-1.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
               ///grass block9
        glPushMatrix();
    glTranslatef(-13,-3.3,-9.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block10
        glPushMatrix();
    glTranslatef(-13,-3.3,-5.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block11
        glPushMatrix();
    glTranslatef(-13,-3.3,-1.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();






                   ///grass block12
        glPushMatrix();
    glTranslatef(-17,-3.3,-9.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block13
        glPushMatrix();
    glTranslatef(-17,-3.3,-5.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block14
        glPushMatrix();
    glTranslatef(-17,-3.3,-1.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



                       ///grass block15
        glPushMatrix();
    glTranslatef(-17,-3.3,-13.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block16
        glPushMatrix();
    glTranslatef(-17,-3.3,-17.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block17
        glPushMatrix();
    glTranslatef(-17,-3.3,-21.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


                      ///grass block18
        glPushMatrix();
    glTranslatef(-21,-3.3,-9.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block19
        glPushMatrix();
    glTranslatef(-21,-3.3,-5.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block20
        glPushMatrix();
    glTranslatef(-21,-3.3,-1.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



                       ///grass block21
        glPushMatrix();
    glTranslatef(-21,-3.3,-13.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block22
        glPushMatrix();
    glTranslatef(-21,-3.3,-17.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block23
        glPushMatrix();
    glTranslatef(-21,-3.3,-21.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();




                          ///grass block24
        glPushMatrix();
    glTranslatef(-21,-3.3,-25.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block25
        glPushMatrix();
    glTranslatef(-21,-3.3,-29.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block26
        glPushMatrix();
    glTranslatef(-21,-3.3,-33.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



                       ///grass block27
        glPushMatrix();
    glTranslatef(-21,-3.3,-37.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block28
        glPushMatrix();
    glTranslatef(-21,-3.3,-41.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block29
        glPushMatrix();
    glTranslatef(-21,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


       ///grass block30
        glPushMatrix();
    glTranslatef(-17,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block31
        glPushMatrix();
    glTranslatef(-13,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block32
        glPushMatrix();
    glTranslatef(-9,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


           ///grass block33
        glPushMatrix();
    glTranslatef(-5,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


           ///grass block34
        glPushMatrix();
    glTranslatef(-1,-3.3,-41.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block35
        glPushMatrix();
    glTranslatef(3,-3.3,-41.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


           ///grass block36
        glPushMatrix();
    glTranslatef(7,-3.3,-41.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block37
        glPushMatrix();
    glTranslatef(11,-3.3,-41.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

    ///newly add



           ///grass block34
        glPushMatrix();
    glTranslatef(-1,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

               ///grass block35
        glPushMatrix();
    glTranslatef(3,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


           ///grass block36
        glPushMatrix();
    glTranslatef(7,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block37
        glPushMatrix();
    glTranslatef(11,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

   /// newly add



               ///grass block38
        glPushMatrix();
    glTranslatef(15,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



           ///grass block39
        glPushMatrix();
    glTranslatef(19,-3.3,-45.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block40
        glPushMatrix();
    glTranslatef(19,-3.3,-41.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
               ///grass block39
        glPushMatrix();
    glTranslatef(19,-3.3,-37.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block40
        glPushMatrix();
    glTranslatef(19,-3.3,-33.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


     ///grass block40.1
        glPushMatrix();
    glTranslatef(19,-3.3,-17.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


           ///grass block42
        glPushMatrix();
    glTranslatef(19,-3.3,-21.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
               ///grass block43
        glPushMatrix();
    glTranslatef(19,-3.3,-25.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block44
        glPushMatrix();
    glTranslatef(19,-3.3,-29.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


            ///grass block45
        glPushMatrix();
    glTranslatef(15,-3.3,-17.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block46
        glPushMatrix();
    glTranslatef(15,-3.3,-21.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();




               ///grass block47
        glPushMatrix();
    glTranslatef(15,-3.3,-13.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block48
        glPushMatrix();
    glTranslatef(15,-3.3,-9.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
               ///grass block49
        glPushMatrix();
    glTranslatef(15,-3.3,-5.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block50
        glPushMatrix();
    glTranslatef(15,-3.3,-1.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


               ///grass block47
        glPushMatrix();
    glTranslatef(19,-3.3,-13.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block48
        glPushMatrix();
    glTranslatef(19,-3.3,-9.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
               ///grass block49
        glPushMatrix();
    glTranslatef(19,-3.3,-5.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
           ///grass block50
        glPushMatrix();
    glTranslatef(19,-3.3,-1.0);

     glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

    ///soil block51
    glPushMatrix();
    glTranslatef(3.0,-3.3,-1.0);

    glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
   ///soil block52
    glPushMatrix();
    glTranslatef(7.0,-3.3,-1.0);

    glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

  ///soil block53
    glPushMatrix();
    glTranslatef(11.0,-3.3,-1.0);

    glScalef(2,2,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();





}

void teardrop()

{


   if(z>=1.25)
   {
       x=z-1.25;
   }
   else
   {
       x=z+1.25;
   }

      glPushMatrix();

   if(mv%2==0)
    {
        glTranslatef(3,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }
     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



          glPushMatrix();
   if(mv%2==0)
    {
        glTranslatef(2.75,18.5-(2*x),-47+x);
    }
    else
    {
        glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


          glPushMatrix();

   if(mv%2==0)
    {
        glTranslatef(2.5,18.5-(2*z),-47+z);
    }
    else
    {
        glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


  glPushMatrix();

   if(mv%2==0)
    {
        glTranslatef(2.25,18.5-(2*x),-47+x);
    }
    else
    {
        glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

    glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(2,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

   glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(1.75,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


    glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(1.5,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

    glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(1.25,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(3.25,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(3.5,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(3.75,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();




        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(4,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(4.25,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(4.5,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(4.75,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(5,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(5.25,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(5.5,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(5.75,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(6,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(6.25,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(6.5,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(6.75,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(7,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(7.25,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(7.5,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(7.75,18.5-(2*x),-47+x);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(8,18.5-(2*z),-47+z);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



   if(z>=0.675)
   {
       y=z-0.675;
   }
   else
   {
       y=x+0.675;
   }

    if(y>=1.25)
   {
       xy=y-1.25;
   }
   else
   {
       xy=y+1.25;
   }

      glPushMatrix();

   if(mv%2==0)
    {
        glTranslatef(3,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }
     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



          glPushMatrix();
   if(mv%2==0)
    {
        glTranslatef(2.75,18.5-(2*xy),-47+xy);
    }
    else
    {
        glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


          glPushMatrix();

   if(mv%2==0)
    {
        glTranslatef(2.5,18.5-(2*y),-47+y);
    }
    else
    {
        glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


  glPushMatrix();

   if(mv%2==0)
    {
        glTranslatef(2.25,18.5-(2*xy),-47+xy);
    }
    else
    {
        glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

    glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(2,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

   glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(1.75,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


    glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(1.5,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

    glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(1.25,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(3.25,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(3.5,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(3.75,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();




        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(4,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(4.25,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



        glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(4.5,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(4.75,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(5,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(5.25,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(5.5,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(5.75,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(6,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(6.25,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(6.5,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(6.75,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(7,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(7.25,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(7.5,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(7.75,18.5-(2*xy),-47+xy);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



            glPushMatrix();
       if(mv%2==0)
    {
        glTranslatef(8,18.5-(2*y),-47+y);
    }
    else
    {
       glTranslatef(5,10,-49.5);
    }

     glScalef(0.10,0.15,0.10);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



}

void waterdrop()
{
    glPushMatrix();
    glScalef(1,1.75,1);
    glTranslatef(2,1.2,-39.455);
    teardrop();
    glPopMatrix();


    glPushMatrix();
    glScalef(1.25,1.75,1);
    glTranslatef(1.25,-3.4,-37.75);
    teardrop();
    glPopMatrix();


    glPushMatrix();
    glScalef(1.25,1.75,1);
    glTranslatef(1.0,-8.5,-35.65);
    teardrop();
    glPopMatrix();

    glPushMatrix();
    glScalef(1.5,1.75,1);
    glTranslatef(0.0,-13.6,-33.55);
    teardrop();
    glPopMatrix();

     glPushMatrix();
     glScalef(1.75,1.75,1);
    glTranslatef(-0.5,-18.32,-31.45);
   teardrop();
    glPopMatrix();
}

void mountain()
{
    /// for(int i=-16;i<=15-j;i++)
     glPushMatrix();
    glTranslatef(0,0,-4);
    for(int j=-1;j<10;j=j+2)
    {
        for(int i=-16+j;i<=17+0.5*j;i++)

        {
            if(i<1||i>4)
            {


              glPushMatrix();

            glBindTexture(GL_TEXTURE_2D,4);
            glTranslatef(2*i,1.85*j,-(48+(j*0.75)));
            glScalef(1.01,3,3.01);
             floor();
             glPopMatrix();


              glPushMatrix();
            glBindTexture(GL_TEXTURE_2D,2);
            glTranslatef(2*i,(1.85*j)+3,-(48+(j*0.75)));
            glScalef(1,0.5,3);
             floor();
             glPopMatrix();

            }

        }

    }

    glPopMatrix();


    glPushMatrix();
    glRotatef(270,0,1,0);
    glTranslatef(-14,0,12);

    for(int j=-1;j<10;j=j+2)
    {
        for(int i=-17-0.5*j;i<=0-2*j;i++)

        {
              glPushMatrix();

            glBindTexture(GL_TEXTURE_2D,4);
            glTranslatef(2*i,1.85*j,-(48+(j*0.75)));
            glScalef(1.01,3,3.01);
             floor();
             glPopMatrix();


              glPushMatrix();
            glBindTexture(GL_TEXTURE_2D,2);
            glTranslatef(2*i,(1.85*j)+3,-(48+(j*0.75)));
            glScalef(1,0.5,3);
             floor();
             glPopMatrix();



        }

    }

    glPopMatrix();





}
void waterfall()
{

               ///grass block35
        glPushMatrix();
    glTranslatef(5,-3.2,-40.5);

     glScalef(5.5,2,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



                   ///grass block352
        glPushMatrix();
    glTranslatef(5,1.0,-43);

     glScalef(5,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();




            ///soil
        glPushMatrix();
    glTranslatef(8,3,-43);

     glScalef(1,1,3);
     glBindTexture(GL_TEXTURE_2D,4);
     glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

      ///soiltop
        glPushMatrix();
    glTranslatef(8,4,-43);

     glScalef(1,0.5,3);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();






                      ///grass block354
        glPushMatrix();
    glTranslatef(5,6.0,-45);

     glScalef(4.5,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

        ///soil
        glPushMatrix();
    glTranslatef(2.5,8,-45);

     glScalef(1,1,3);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

      ///soiltop
        glPushMatrix();
    glTranslatef(2.5,9,-45);

     glScalef(1,0.5,3);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


                          ///grass block355
        glPushMatrix();
    glTranslatef(5,11.0,-47);

     glScalef(4,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();


    ///soil
        glPushMatrix();
    glTranslatef(5,14,-47);

     glScalef(1,1,3);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

      ///soiltop
        glPushMatrix();
    glTranslatef(5,15,-47);

     glScalef(1,0.5,3);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


    ///grass block355.1
        glPushMatrix();
    glTranslatef(5,16.0,-49);

     glScalef(3.5,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



    ///grass block3551
        glPushMatrix();
    glTranslatef(2.5,16.5,-49);

     glScalef(1,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();
                              ///grass block3552
        glPushMatrix();
    glTranslatef(7.5,16.5,-49);

     glScalef(1,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

                                ///grass block3553
        glPushMatrix();
    glTranslatef(2,17,-49);

     glScalef(.5,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();
                              ///grass block3554
        glPushMatrix();
    glTranslatef(8,17,-49);

     glScalef(.5,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();



      ///grass block355.1
        glPushMatrix();
    glTranslatef(5,16.0,-53);

     glScalef(3.5,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

      ///grass block3551
        glPushMatrix();
    glTranslatef(2.5,16.5,-53);

     glScalef(1,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();
                              ///grass block3552
        glPushMatrix();
    glTranslatef(7.5,16.5,-53);

     glScalef(1,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();

                                ///grass block3553
        glPushMatrix();
    glTranslatef(2,17,-53);

     glScalef(.5,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();
                              ///grass block3554
        glPushMatrix();
    glTranslatef(8,17,-53);

     glScalef(.5,2.5,2);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();









  ///waterfall side left+right



        ///soil
        glPushMatrix();
    glTranslatef(-1.5,-3.1,-39.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();
         glPushMatrix();
    glTranslatef(11.5,-3.1,-39.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();



      ///soiltop
        glPushMatrix();
    glTranslatef(-1.5,-1.1,-39.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


            glPushMatrix();
    glTranslatef(11.5,-1.1,-39.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


           ///soil2bottom
        glPushMatrix();
    glTranslatef(-1,-3.1,-41.0);
     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(11,-3.1,-41.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


            ///soil2bottom
        glPushMatrix();
    glTranslatef(-1,-0.5,-41.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(11,-0.5,-41.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();



          ///soiltop
        glPushMatrix();
    glTranslatef(-1,1.5,-41.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(11,1.5,-41.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();






               ///soil3bottom
        glPushMatrix();
    glTranslatef(-3,-2.1,-40.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(13,-2.1,-40.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-3,0,-40.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(13,0,-40.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


                   ///soil4bottom
        glPushMatrix();
    glTranslatef(-2.5,2.3,-42.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


            glPushMatrix();
    glTranslatef(12.5,2.3,-42.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();



          ///soiltop
        glPushMatrix();
    glTranslatef(-2.5,4.3,-42.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


            glPushMatrix();
    glTranslatef(12.5,4.3,-42.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();





    ///soil5bottom
        glPushMatrix();
    glTranslatef(-0.5,4,-43);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


            glPushMatrix();
    glTranslatef(10.5,4,-43);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-0.5,6,-43);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(10.5,6,-43);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();




                  ///soil6bottom
        glPushMatrix();
    glTranslatef(-2,6.5,-44.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


            glPushMatrix();
    glTranslatef(12,6.5,-44.0);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-2,8.5,-44.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(12,8.5,-44.0);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


    ///soil7bottom
        glPushMatrix();
    glTranslatef(-0.5,8,-43.5);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(10.5,8,-43.5);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-0.5,10,-43.5);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

         glPushMatrix();
    glTranslatef(10.5,10,-43.5);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


        ///soil8bottom
        glPushMatrix();
    glTranslatef(0.0,10,-44.5);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

          glPushMatrix();
    glTranslatef(10.0,10,-44.5);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();




          ///soiltop
        glPushMatrix();
    glTranslatef(0.0,12,-44.5);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

         glPushMatrix();
    glTranslatef(10.0,12,-44.5);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();




            ///soil9bottom
        glPushMatrix();
    glTranslatef(0.5,14,-47);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

           glPushMatrix();
    glTranslatef(9.5,14,-47);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(0.5,16,-47);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

          glPushMatrix();
    glTranslatef(9.5,16,-47);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();




                ///soil10bottom
        glPushMatrix();
    glTranslatef(1.0,17.5,-49);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(9.0,17.5,-49);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(1,19.5,-49);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(9,19.5,-49);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();




                    ///soil10bottom
        glPushMatrix();
    glTranslatef(-.5,16,-48);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(10.5,16,-48);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-.5,18,-48);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(10.5,18,-48);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();




    ///soil11bottom

     glPushMatrix();
    glTranslatef(-1.5,12,-48);

     glScalef(2,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(11.5,12,-48);

     glScalef(2,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();







          ///soiltop
        glPushMatrix();
    glTranslatef(-1.5,14,-48);

     glScalef(2,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(11.5,14,-48);

     glScalef(2,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();






        ///soil11bottom

            glPushMatrix();
    glTranslatef(-1.5,10,-46.5);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


             glPushMatrix();
    glTranslatef(11.5,10,-46.5);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-1.5,12,-46.5);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

          glPushMatrix();
    glTranslatef(11.5,12,-46.5);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


     ///soil11bottom

     glPushMatrix();
    glTranslatef(-4.0,10,-49);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(14.0,10,-49);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-4.0,12,-49);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(14.0,12,-49);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



  ///soilbottom

     glPushMatrix();
    glTranslatef(-4.0,8,-47);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(14.0,8,-47);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-4.0,10,-47);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(14.0,10,-47);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();

     ///soilbottom

     glPushMatrix();
    glTranslatef(-3.5,6,-46);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(13.5,8,-46);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-3.5,8,-46);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(13.5,8,-46);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


    ///soilbottomp
     glPushMatrix();
    glTranslatef(-6.0,7,-50);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(16.0,7,-50);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-6.0,9,-50);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(16.0,9,-50);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


        ///soilbottomp
     glPushMatrix();
    glTranslatef(-6.0,6,-48);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(16.0,6,-48);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-6.0,8,-48);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(16.0,8,-48);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();






            ///soilbottomp
     glPushMatrix();
    glTranslatef(-5.0,4,-47);

     glScalef(2,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(15.0,4,-47);

     glScalef(2,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-5.0,6,-47);

     glScalef(2,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(15.0,6,-47);

     glScalef(2,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



                ///soilbottomp
     glPushMatrix();
    glTranslatef(-4.0,2,-45);

     glScalef(1,2,2);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(14.0,2,-45);

     glScalef(1,2,2);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-4.0,4,-45);

     glScalef(1,0.5,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(14.0,4,-45);

     glScalef(1,0.5,2);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



               ///soilbottomp
     glPushMatrix();
    glTranslatef(-4.0,0,-43);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(14.0,0,-43);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-4.0,2,-43);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(14.0,2,-43);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


    ///soilbottomp
         glPushMatrix();
    glTranslatef(-6,-2.0,-44);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
     floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(16.0,-2.0,-44);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
     floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-6.0,0,-44);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
   floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(16.0,0,-44);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();




        ///soilbottomp
         glPushMatrix();
    glTranslatef(-6,0.0,-45);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
     floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(16.0,0.0,-45);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
     floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-6.0,2.0,-45);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
   floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(16.0,2.0,-45);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



            ///soilbottomp
         glPushMatrix();
    glTranslatef(-7,2.0,-46);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
     floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(17.0,2.0,-46);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
     floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-7.0,4.0,-46);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
   floor();
    glPopMatrix();
          glPushMatrix();
    glTranslatef(17.0,4.0,-46);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();




                ///soil10bottom
        glPushMatrix();
    glTranslatef(1.0,17.5,-50);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(9.0,17.5,-50);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(1,19.5,-50);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(9,19.5,-50);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();





                    ///soil10bottom
        glPushMatrix();
    glTranslatef(1.0,17.5,-51);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(9.0,17.5,-51);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(1,19.5,-51);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(9,19.5,-51);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


                        ///soil10bottom
        glPushMatrix();
    glTranslatef(0.0,15.5,-51);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(10.0,15.5,-51);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(0,17.5,-51);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(10,17.5,-51);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



                        ///soil10bottom
        glPushMatrix();
    glTranslatef(1.0,17.5,-52);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(9.0,17.5,-52);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(1,19.5,-52);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(9,19.5,-52);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


    ///soil10bottom
        glPushMatrix();
    glTranslatef(1.0,17.5,-53);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(9.0,17.5,-53);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(1,19.5,-53);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(9,19.5,-53);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



    ///soil10bottom
        glPushMatrix();
    glTranslatef(-1.0,17.5,-52);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(11.0,17.5,-52);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-1,19.5,-52);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(11,19.5,-52);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


        ///soil10bottom
        glPushMatrix();
    glTranslatef(-1.0,17.5,-53);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(11.0,17.5,-53);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-1,19.5,-53);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(11,19.5,-53);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


            ///soil10bottom
        glPushMatrix();
    glTranslatef(-3.0,16.5,-53);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(13.0,16.5,-53);

     glScalef(1,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-3,18.5,-53);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(13,18.5,-53);

     glScalef(1,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



                ///soil10bottom
        glPushMatrix();
    glTranslatef(-3.0,14.5,-51);

     glScalef(1.5,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(13.0,14.5,-51);

     glScalef(1.5,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-3,16.5,-51);

     glScalef(1.5,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(13,16.5,-51);

     glScalef(1.5,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();


                    ///soil10bottom
        glPushMatrix();
    glTranslatef(-5.0,12.5,-51);

     glScalef(1.5,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(15.0,12.5,-51);

     glScalef(1.5,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-5,14.5,-51);

     glScalef(1.5,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
            glPushMatrix();
    glTranslatef(15,14.5,-51);

     glScalef(1.5,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



    ///soil10bottom
        glPushMatrix();
    glTranslatef(-5.0,13.5,-53);

     glScalef(2,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(13.0,13.5,-53);

     glScalef(2,2,1);
     glBindTexture(GL_TEXTURE_2D,4);glScalef(1.01 ,1,1.01);
    floor();
    glPopMatrix();


          ///soiltop
        glPushMatrix();
    glTranslatef(-5,15.5,-53);

     glScalef(2,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(13,15.5,-53);
     glScalef(2,0.5,1);
     glBindTexture(GL_TEXTURE_2D,2);
    floor();
    glPopMatrix();



    mountain();



///mountain
    ///soil10bottom

   /* for(int j=-1;j<10;j=j+2)
    {
        for(int i=-16;i<=18-j;i++)
        {
              glPushMatrix();

            glBindTexture(GL_TEXTURE_2D,4);
            glTranslatef(-2*i,1.85*j,-(48+(j*0.75)));
            glScalef(1.01,3,1.01);
             floor();
             glPopMatrix();


              glPushMatrix();
            glBindTexture(GL_TEXTURE_2D,2);
            glTranslatef(-2*i,(1.85*j)+3,-(48+(j*0.75)));
            glScalef(1,0.5,1);
             floor();
             glPopMatrix();

        }

    }*/

        ///soiltop








}






void water()
{

  ///water
   glPushMatrix();
    glTranslatef(0.0,-3.75,1.0);
    //glRotatef(90,0.0,1.0,0.0);
    glScalef(20,1.5,20);
     glBindTexture(GL_TEXTURE_2D,3);
    floor();
    glPopMatrix();
///water

}

void sky()

{
    glPushMatrix();

    GLUquadric* sky = gluNewQuadric();
    gluQuadricTexture(sky, GL_TRUE);

    glBindTexture(GL_TEXTURE_2D,6);
    //floor();//sky
    glTranslatef(0,0,0);
    glRotatef(90,0,1,0);
    gluSphere(sky,400,200,200);

    glPopMatrix();
}


void bush2()
{

      glPushMatrix();
    glTranslatef(0,0,0);
    glScalef(.05,1,2);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,0);
    glRotatef(45,0.0,1.0,0.0);
    glScalef(.05,1,2);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,2);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,0,0);
    glRotatef(135,0.0,1.0,0.0);
    glScalef(.05,1,2);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

          glPushMatrix();
    glTranslatef(0,0.5,0);
    glScalef(.05,1,1.75);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0.5,0);
    glRotatef(45,0.0,1.0,0.0);
    glScalef(.05,1,1.75);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0.5,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,1.75);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,0.5,0);
    glRotatef(135,0.0,1.0,0.0);
    glScalef(.05,1,1.75);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

          glPushMatrix();
    glTranslatef(0,1,0);
    glScalef(.05,1,1.5);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,1,0);
    glRotatef(45,0.0,1.0,0.0);
    glScalef(.05,1,1.5);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,1,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,1.5);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,1,0);
    glRotatef(135,0.0,1.0,0.0);
    glScalef(.05,1,1.5);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

          glPushMatrix();
    glTranslatef(0,1.5,0);
    glScalef(.05,1,1.25);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,1.5,0);
    glRotatef(45,0.0,1.0,0.0);
    glScalef(.05,1,1.25);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,1.5,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,1.25);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,1.5,0);
    glRotatef(135,0.0,1.0,0.0);
    glScalef(.05,1,1.25);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

          glPushMatrix();
    glTranslatef(0,2,0);
    glScalef(.05,1,1.0);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,2,0);
    glRotatef(45,0.0,1.0,0.0);
    glScalef(.05,1,1.0);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,2,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,1.0);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,2,0);
    glRotatef(135,0.0,1.0,0.0);
    glScalef(.05,1,1.0);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

          glPushMatrix();
    glTranslatef(0,2.5,0);
    glScalef(.05,1,0.75);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,2.5,0);
    glRotatef(45,0.0,1.0,0.0);
    glScalef(.05,1,0.75);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,2.5,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,0.75);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,2.5,0);
    glRotatef(135,0.0,1.0,0.0);
    glScalef(.05,1,0.75);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

          glPushMatrix();
    glTranslatef(0,3,0);
    glScalef(.05,1,0.5);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,3,0);
    glRotatef(45,0.0,1.0,0.0);
    glScalef(.05,1,0.5);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,3,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,0.5);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,3,0);
    glRotatef(135,0.0,1.0,0.0);
    glScalef(.05,1,0.5);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();


}

void bush()
{

    glPushMatrix();
    glTranslatef(0,0,0);
    glScalef(.05,1,2);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0,0,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,2);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();




          glPushMatrix();
    glTranslatef(0,0.5,0);
    glScalef(.05,1,1.75);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0,0.5,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,1.75);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();



          glPushMatrix();
    glTranslatef(0,1,0);
    glScalef(.05,1,1.5);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0,1,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,1.5);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();



          glPushMatrix();
    glTranslatef(0,1.5,0);
    glScalef(.05,1,1.25);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0,1.5,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,1.25);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();




          glPushMatrix();
    glTranslatef(0,2,0);
    glScalef(.05,1,1.0);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0,2,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,1.0);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();



          glPushMatrix();
    glTranslatef(0,2.5,0);
    glScalef(.05,1,0.75);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,2.5,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,0.75);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();




          glPushMatrix();
    glTranslatef(0,3,0);
    glScalef(.05,1,0.5);
     glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0,3,0);
    glRotatef(90,0.0,1.0,0.0);
    glScalef(.05,1,0.5);
    glBindTexture(GL_TEXTURE_2D,7);
    floor();
    glPopMatrix();





    //glPushMatrix();
    //glRotatef(22.5,0,1,0);
    //bush2();
    //glPopMatrix();



}

void tree()
{
    glPushMatrix();
    glTranslatef(0,0,0);
    //glRotatef(135,0.0,1.0,0.0);
    glScalef(1,7,1);
    glBindTexture(GL_TEXTURE_2D,5);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,4,0);
    //glRotatef(135,0.0,1.0,0.0);
    glScalef(3.5,1,3.5);
    glBindTexture(GL_TEXTURE_2D,7);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,5,0);
    glRotatef(11.25,0.0,1.0,0.0);
    glScalef(3.5,1,3.5);
    glBindTexture(GL_TEXTURE_2D,7);
    bush();
    glPopMatrix();


        glPushMatrix();
    glTranslatef(0,6,0);
    //glRotatef(135,0.0,1.0,0.0);
    glScalef(3,1.3,3);
    glBindTexture(GL_TEXTURE_2D,7);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,7,0);
    glRotatef(11.25,0.0,1.0,0.0);
    glScalef(3,1.3,3);
    glBindTexture(GL_TEXTURE_2D,7);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,8,0);
    //glRotatef(135,0.0,1.0,0.0);
    glScalef(2.5,1.5,2.5);
    glBindTexture(GL_TEXTURE_2D,7);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,9,0);
    glRotatef(11.25,0.0,1.0,0.0);
    glScalef(2.5,1.5,2.5);
    glBindTexture(GL_TEXTURE_2D,7);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,10,0);
    //glRotatef(135,0.0,1.0,0.0);
    glScalef(2,1.75,2);
    glBindTexture(GL_TEXTURE_2D,7);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,11,0);
    glRotatef(11.25,0.0,1.0,0.0);
    glScalef(2,1.75,2);
    glBindTexture(GL_TEXTURE_2D,7);
    bush();
    glPopMatrix();


}

void plantation()
{
    ///ground
    ///right
    glPushMatrix();
    glTranslatef(-0.5,-6.5,0.5);
    glPushMatrix();
    glTranslatef(24,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(28,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(32,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(34.5,6,-37);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


       glPushMatrix();
    glTranslatef(34.5,6,-33);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

       glPushMatrix();
    glTranslatef(34.5,6,-29);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

       glPushMatrix();
    glTranslatef(34.5,6,-25);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


       glPushMatrix();
    glTranslatef(34.5,6,-21);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();
           glPushMatrix();
    glTranslatef(34.5,6,-17);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

           glPushMatrix();
    glTranslatef(34.5,6,-13);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

           glPushMatrix();
    glTranslatef(34.5,6,-9);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

           glPushMatrix();
    glTranslatef(34.5,6,-5);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

           glPushMatrix();
    glTranslatef(34.5,6,-1);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

               glPushMatrix();
    glTranslatef(38.5,6,-1);
    glScalef(0.2,0.25,0.2);
   // glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();
    glPopMatrix();

    ///left
    glPushMatrix();
    glTranslatef(-2,-6.5,0.5);
    glPushMatrix();
    glTranslatef(-8,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-16,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(-20,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-24,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-28,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-32,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-32,6,-43);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
    glTranslatef(6,0,0);
    bush();
    glPopMatrix();
    glPopMatrix();




    ///first row

    ///right
    glPushMatrix();
    glTranslatef(24,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(28,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(32,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(34.5,6,-37);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


       glPushMatrix();
    glTranslatef(34.5,6,-33);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

       glPushMatrix();
    glTranslatef(34.5,6,-29);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

       glPushMatrix();
    glTranslatef(34.5,6,-25);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


       glPushMatrix();
    glTranslatef(34.5,6,-21);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();
           glPushMatrix();
    glTranslatef(34.5,6,-17);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

           glPushMatrix();
    glTranslatef(34.5,6,-13);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

           glPushMatrix();
    glTranslatef(34.5,6,-9);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

           glPushMatrix();
    glTranslatef(34.5,6,-5);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

           glPushMatrix();
    glTranslatef(34.5,6,-1);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

               glPushMatrix();
    glTranslatef(38.5,6,-1);
    glScalef(0.2,0.25,0.2);
   // glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();





    ///left
    glPushMatrix();
    glTranslatef(-8,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-16,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(-20,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-24,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-28,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-32,6,-39);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-32,6,-43);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

///2nd row
///right
    glPushMatrix();
    glTranslatef(24,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(28,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(32,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(36,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(36,12.5,-37);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(36,12.5,-33);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


     glPushMatrix();
    glTranslatef(36,12.5,-29);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


     glPushMatrix();
    glTranslatef(36,12.5,-25);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(36,12.5,-21);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(36,12.5,-17);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(36,12.5,-13);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


      glPushMatrix();
    glTranslatef(36,12.5,-9);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(40,12.5,-9);
    glScalef(0.2,0.25,0.2);
  //  glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();









///left
  glPushMatrix();
    glTranslatef(-8,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-16,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(-20,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-24,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-28,12.5,-41);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(-28,12.5,-45);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

  ///3rd row
  ///right
    glPushMatrix();
    glTranslatef(22,19,-43);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(26,19,-43);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(30,19,-43);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(34,19,-43);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(38,19,-41);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


      glPushMatrix();
    glTranslatef(38,19,-37);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(38,19,-33);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(38,19,-29);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


      glPushMatrix();
    glTranslatef(38,19,-25);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


      glPushMatrix();
    glTranslatef(38,19,-21);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


     glPushMatrix();
    glTranslatef(38,19,-17);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(42,19,-17);
    glScalef(0.2,0.25,0.2);
    //glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();





  ///left


    glPushMatrix();
    glTranslatef(-7,19,-43);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-11,19,-43);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-15,19,-43);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(-19,19,-43);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-23,19,-43);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();



glPushMatrix();
    glTranslatef(-23,19,-47);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    ///4th row

    ///right

     glPushMatrix();
    glTranslatef(22,25.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(26,25.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(30,25.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(34,25.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(38,25.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(39,25.5,-40);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(39,25.5,-36);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

         glPushMatrix();
    glTranslatef(39,25.5,-32);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

         glPushMatrix();
    glTranslatef(39,25.5,-28);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

         glPushMatrix();
    glTranslatef(39,25.5,-24);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


         glPushMatrix();
    glTranslatef(43,25.5,-28);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

         glPushMatrix();
    glTranslatef(43,25.5,-24);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();



    ///left
    glPushMatrix();
    glTranslatef(-7,25.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-11,25.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-15,25.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(-19,25.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


        glPushMatrix();
    glTranslatef(-19,25.5,-47);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    /// 5th row

    ///right
    glPushMatrix();
    glTranslatef(18,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(26,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(30,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(34,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(38,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(40.5,32,-44);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


      glPushMatrix();
    glTranslatef(40.5,32,-40);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


      glPushMatrix();
    glTranslatef(40.5,32,-36);
    glScalef(0.2,0.25,0.2);
    glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(44.5,32,-36);
    glScalef(0.2,0.25,0.2);
    //glRotatef(90,0,1,0);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();



    ///left
    glPushMatrix();
    glTranslatef(-3,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

         glPushMatrix();
    glTranslatef(-7,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-11,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-15,32,-46);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-15,32,-50);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

///6th row


///right
    glPushMatrix();
    glTranslatef(1,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


          glPushMatrix();
    glTranslatef(2,38.5,-42);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(13,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


              glPushMatrix();
    glTranslatef(12,38.5,-42);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();



    glPushMatrix();
    glTranslatef(17,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

        glPushMatrix();
    glTranslatef(21,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(25,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


            glPushMatrix();
    glTranslatef(29,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

            glPushMatrix();
    glTranslatef(33,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


            glPushMatrix();
    glTranslatef(37,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(41,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

   glPushMatrix();
    glTranslatef(43,38.5,-44);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(43,38.5,-40);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();


///left
    glPushMatrix();
    glTranslatef(-3,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

         glPushMatrix();
    glTranslatef(-7,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
    glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-11,38.5,-48);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();

  glPushMatrix();
    glTranslatef(-11,38.5,-52);
    glScalef(0.2,0.25,0.2);
    tree();
        glTranslatef(-4,-3,0);
    glScalef(1.5,1.5,1.5);
    bush();
     glTranslatef(6,0,0);
    bush();
    glPopMatrix();







}



void slide()
{

   glPushMatrix();

   glRotatef(0,0,1,0);
   for(int i=0;i<14;i=i+2)
   {


    glPushMatrix();


    glScalef(4,1,2);
    glTranslatef(0,0+2*i,0+i*0.5);
    glBindTexture(GL_TEXTURE_2D,10);
    floor();
    glPopMatrix();
   }


    glPushMatrix();
    glRotatef(26,1,0,0);
    glTranslatef(4,13,-10);
    glScalef(.5,16,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();

    glPushMatrix();
    glRotatef(26,1,0,0);
    glTranslatef(-4,13,-10);
    glScalef(.5,16,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();

     glPushMatrix();
    glRotatef(26,1,0,0);
    glTranslatef(4,13,0);
    glScalef(.5,16,.5);
    glBindTexture(GL_TEXTURE_2D,8);
    floor();
    glPopMatrix();

    glPushMatrix();
    glRotatef(26,1,0,0);
    glTranslatef(-4,13,0);
    glScalef(.5,16,.5);
    glBindTexture(GL_TEXTURE_2D,8);
    floor();
    glPopMatrix();

   glPushMatrix();
    glRotatef(0,1,0,0);
    glTranslatef(4,0,-10);
    glScalef(.5,4,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();

    glPushMatrix();
    glRotatef(0,1,0,0);
    glTranslatef(-4,0,-10);
    glScalef(.5,4,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();


       glPushMatrix();
       glTranslatef(4,26,7);
    glRotatef(-60,1,0,0);
    glScalef(.5,5,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();

      glPushMatrix();
       glTranslatef(-4,26,7);
    glRotatef(-60,1,0,0);
    glScalef(.5,5,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,10,22);
    glRotatef(60,1,0,0);
    glScalef(4,.5,17);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(4,10,22);
    glRotatef(60,1,0,0);
    glScalef(.5,4,17);
    glBindTexture(GL_TEXTURE_2D,10);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4,10,22);
    glRotatef(60,1,0,0);
    glScalef(.5,4,17);
    glBindTexture(GL_TEXTURE_2D,10);
    floor();
    glPopMatrix();




   glPopMatrix();
}


void updown()
{

    glPushMatrix();
   glRotatef(0,0,1,0);


    glPushMatrix();
    glTranslatef(0,2,0);
    if(nn%2==0)
    {
    glRotatef(angle2,1,0,0);
    }
    glScalef(3,.5,20);
    glBindTexture(GL_TEXTURE_2D,8);
    floor();
    glPopMatrix();


      glPushMatrix();
    glTranslatef(-3,0,0);
    //glRotatef(60,1,0,0);
    glScalef(1,4,1);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();

      glPushMatrix();
    glTranslatef(3,0,0);
    //glRotatef(60,1,0,0);
    glScalef(1,4,1);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();



    glPopMatrix();
}
void bench()
{
    glPushMatrix();
    glRotatef(-90,0,1,0);

    glPushMatrix();
    glTranslatef(0,2,-1.5);
    glScalef(5,3,0.5);
    glBindTexture(GL_TEXTURE_2D,10);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,0,0);
    glScalef(5.5,0.5,2.5);
    glBindTexture(GL_TEXTURE_2D,8);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-4.5,-2,1.5);
    glScalef(.5,2,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4.5,-2,-1.5);
    glScalef(.5,2,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.5,-2,1.5);
    glScalef(.5,2,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.5,-2,-1.5);
    glScalef(.5,2,.5);
    glBindTexture(GL_TEXTURE_2D,9);
    floor();
    glPopMatrix();


    glPopMatrix();
}

void lamp()
{

     glPushMatrix();
    glRotatef(180,0,1,0);


    glPushMatrix();
    glTranslatef(0,0,0);
    glScalef(.5,30,.5);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,25,2);
    glScalef(0.5,.5,2);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();


     glPushMatrix();
    glTranslatef(0,25,4);
    glScalef(2,.5,2);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,24,4);
    glScalef(2.1,.5,2.1);
    glBindTexture(GL_TEXTURE_2D,10);
    floor();
    glPopMatrix();





    glPopMatrix();

}


void childpark()
{
    glPushMatrix();

   // slide();

     //updown();

     //bench();

     //lamp();

    glPopMatrix();
}
void bench4()
{
     glPushMatrix();

    glTranslatef(30,1,10);
    glScalef(.5,.5,.5);
    bench();
    glPopMatrix();
    glPushMatrix();

    glTranslatef(30,1,20);
    glScalef(.5,.5,.5);
    bench();
    glPopMatrix();

       glPushMatrix();

    glTranslatef(30,1,30);
    glScalef(.5,.5,.5);
    bench();
    glPopMatrix();

      glPushMatrix();

    glTranslatef(30,1,40);
    glScalef(.5,.5,.5);
    bench();
    glPopMatrix();

}

void cradle()
{

     glPushMatrix();

     if(bb%2==0){
      glRotatef(angle*4,0,1,0);
     }

    glPushMatrix();

    GLUquadric* cd1 = gluNewQuadric();
    gluQuadricTexture(cd1, GL_TRUE);

    glBindTexture(GL_TEXTURE_2D,10);
    //floor();//sky
    glTranslatef(0,2,0);
    glRotatef(90,0,1,0);
    glScalef(1,.1,1);
    gluSphere(cd1,13,10,10);

    glPopMatrix();


    glPushMatrix();

    GLUquadric* cd2 = gluNewQuadric();
    gluQuadricTexture(cd2, GL_TRUE);

    glBindTexture(GL_TEXTURE_2D,8);
    //floor();//sky
    glTranslatef(0,20,0);
    glRotatef(90,0,1,0);
    glScalef(1,.1,1);
    gluSphere(cd2,13,10,10);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-8,6,0);
    glScalef(1,1,.3);
    bench();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8,6,0);
     glRotatef(180,0,1,0);
    glScalef(1,1,.3);
    bench();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,6,8);
    glRotatef(90,0,1,0);
    glScalef(1,1,.3);
    bench();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,6,-8);
    glRotatef(-90,0,1,0);
    glScalef(1,1,.3);
    bench();
    glPopMatrix();




    glPushMatrix();
    glTranslatef(0,11,0);
    glScalef(.3,11,.3);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12,11,0);
    glScalef(.3,11,.3);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-12,11,0);
    glScalef(.3,11,.3);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0,11,12);
    glScalef(.3,11,.3);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,11,-12);
    glScalef(.3,11,.3);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();



   glPopMatrix();



}


void court()
{
    glPushMatrix();
    glTranslatef(0,1,0);
    glScalef(30,1,50);
    glBindTexture(GL_TEXTURE_2D,1);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-30,10,0);
    glScalef(.5,20,.5);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(30,10,0);
    glScalef(.5,20,.5);
    glBindTexture(GL_TEXTURE_2D,11);
    floor();
    glPopMatrix();

     glPushMatrix();
    glTranslatef(0,20,0);
    glScalef(30,5,0.2);
    glBindTexture(GL_TEXTURE_2D,10);
    floor();
    glPopMatrix();
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);///clear previous frames
    glLoadIdentity();///resets any transformations from previous work
    glFrustum(-2, 2, -2, 2, 4.0+z0, 2000.0);

    gluLookAt(cx,cy,cz,lx,ly,lz,0,1,0);
    light(s1,s2,s3,p1,p2,p3,q1,q2,q3);

    ///cout<<"cx: "<<cx<<"cy: "<<cy<<"cz: "<<cz<<"lx: "<<lx<<"ly: "<<ly<<"lz: "<<lz<<"fs"<<z0<<"q1 : "<<q1<<"q2 : "<<q2<<"q3 : "<<q3<<"fc : "<<fc<<"fc2 : "<<fc2<<endl;

    glEnable(GL_TEXTURE_2D);


    if(fc==0&&fc2%2==0)
    {
        cx=0;cy=46.8;cz=69,lx=0;ly=44.8;lz=64;z0=0;
        glRotatef(angle,0.0,1.0,0.0);
        glTranslatef(0,0,-96);
    }
    else if(fc==0&&fc2%2==1)
    {
        glRotatef(0,0.0,1.0,0.0);
        glTranslatef(0,0,-96);
    }
    else if(fc==1&&fc2%2==0)
    {
        cx=-73;cy=24.8;cz=-23,lx=-73.4;ly=24.8;lz=-28;z0=0;///waterfall
        glRotatef(45,0.0,1.0,0.0);
        glTranslatef(0,0,-96);
    }
    else if(fc==1&&fc2%2==1)
    {
         glRotatef(45,0.0,1.0,0.0);
        glTranslatef(0,0,-96);
    }
    else if(fc==2&&fc2%2==0)
    {
        cx=72;cy=16.8;cz=22,lx=0;ly=12.8;lz=17;z0=8.1;///cx: 30cy: 16.8cz: 22lx: 0ly: 12.8lz: 17fs8.09999
        glRotatef(45,0.0,1.0,0.0);
        glTranslatef(0,0,-96);
    }
    else if(fc==2&&fc2%2==1)
    {
         glRotatef(45,0.0,1.0,0.0);
        glTranslatef(0,0,-96);
    }
      else if(fc==3&&fc2%2==0)
    {
        cx=-80;cy=10.8;cz=179,lx=-79;ly=10.8;lz=174;z0=0;
         // glTranslatef(0,0,-96);

    }
    else if(fc==3&&fc2%2==1)
    {
        //glRotatef(2,0.0,1.0,0.0);
        // glTranslatef(0,0,-96);
    }
      else if(fc==4&&fc2%2==0)
    {
        cx=-52;cy=16.8;cz=73,lx=-43;ly=14.8;lz=64;z0=0;///cx: 30cy: 16.8cz: 22lx: 0ly: 12.8lz: 17fs8.09999
        glRotatef(0,0.0,1.0,0.0);
        glTranslatef(0,0,-96);
    }
    else if(fc==4&&fc2%2==1)
    {
         glRotatef(0,0.0,1.0,0.0);
        glTranslatef(0,0,-96);
    }


  ///cx: -48cy: 16.8cz: 73lx: -43.0001ly: 14.8lz: 68fs0q1 : 0q2 : 0q3 : 0fc : 0fc2 : 1




  //
    ///water
    glPushMatrix();
    glScalef(2,1,2);
    water();
    soil();
    glPopMatrix();

    glPushMatrix();
    bigmountain();
    glPopMatrix();


    glPushMatrix();
    glScalef(1,1.75,1);
    glTranslatef(2,1.2,9.455);
     waterfall();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,48.89);
    waterdrop();
    glPopMatrix();

    glPushMatrix();
    glRotatef(angle,1,0,0);
    sky();
    glPopMatrix();



    glPushMatrix();
    plantation();

    glPopMatrix();



    glPushMatrix();

    glTranslatef(-70,-1,30);
    glScalef(.4,.4,.4);
    slide();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-90,-1,30);
    glScalef(.4,.4,.4);
    slide();
    glPopMatrix();

   glPushMatrix();
   glTranslatef(-70,0,50);
   bench4();

   glPopMatrix();


   glPushMatrix();
   glTranslatef(60,0,100);
   glRotatef(180,0,1,0);
   bench4();

   glPopMatrix();



   glPushMatrix();
    glTranslatef(-70,0,120);
    glRotatef(90,0,1,0);
    glScalef(.4,.4,.4);
    updown();
    glPopMatrix();


    glPushMatrix();

    glTranslatef(-70,0,130);
    glRotatef(270,0,1,0);
    glScalef(.4,.4,.4);
    updown();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-80,11,125);
    glRotatef(270,0,1,0);
    glScalef(.4,.4,.4);
    lamp();


    glPopMatrix();


    glPushMatrix();
    glTranslatef(-20,-3,125);
    glScalef(1,1,1);
    cradle();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20,-3,125);
    glScalef(1,1,1);
    cradle();
    glPopMatrix();
   ///cx: -48cy: 16.8cz: 73lx: -43.0001ly: 14.8lz: 68fs0q1 : 0q2 : 0q3 : 0fc : 0fc2 : 1

     glPushMatrix();
    glTranslatef(70,-3,70);
    glScalef(1,1,1);
    court();
    glPopMatrix();





    glDisable(GL_TEXTURE_2D);


    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();///resets any transformations from previous work
    //gluOrtho2D(-20,20,-20,20);
  // glFrustum(-1, 1, -1, 1, 5.0+p, 100.0);
    //gluPerspective(60,1.0,4.0,1000.0);   ///(field of view angle,aspect ratio, znear, z far)
    glMatrixMode(GL_MODELVIEW);

}



void timer (int)
{
    glutPostRedisplay();
    glutTimerFunc(1000/60,timer,0);

     z+=.2;
     if(z>=2.5)
     {
         z=z-2.5;
     }

   /*if(z>=1.25)
   {
       x=z-1.25;
   }
   else
   {
       x=z+1.25;
   }*/


    angle+=1;

    if(angle>=360.0)
    {
        angle=angle-360.0;
    }


    if(pp==1)
    {
        angle2+=1;
        if(angle2>=10)
        {
            pp=0;
            zz=1;

        }
    }
    else if(zz==1&&pp==0)
    {
         angle2-=1;
        if(angle2<=-10)
        {
            pp=1;
            zz=0;

        }
    }


}
