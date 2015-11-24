#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include "tiny_obj_loader.h"
#include <iostream>
#include <assert.h>
/*#include <GL\freeglut.h>
#include <GL\GL.h>
#include <GL\GLU.h>*/
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include "../libst/include/STVector3.h"
#include "../libst/include/STImage.h"
#include "../libst/include/STMatrix4.h"

enum{
	SKY_FRONT=0,
	SKY_RIGHT=1,
	SKY_LEFT=2,
	SKY_BACK=3,
	SKY_UP=4,
	SKY_DOWN=5,
};
int gPreviousMouseX = -1;
int gPreviousMouseY = -1;
int gMouseButton = -1;



STVector3 mPosition;
STVector3 mLookAt;
STVector3 mRight;
STVector3 mUp;

GLint skybox[6], grass,x_r=0, y_r=0, z_r=0;
GLfloat viewer[3] = {1.0f, 0.0f, 0.0f},camera[3] = {0.0f, 0.0, 0.0};
GLdouble movcord[3]={-150,-10,200};
int coasterMesh, coasterBarsMesh,carouselMesh;
int show_menu = 1;
struct coordinate{
	float x, y, z;
	coordinate(float a, float b, float c): x(a),y(b),z(c) {};
};

struct face{
	int facenum;
	bool four;
	int faces[4];
	face(int facen, int f1, int f2, int f3):facenum(facen){
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		four = false;
	}
	face(int facen, int f1, int f2, int f3, int f4):facenum(facen){
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		faces[3] = f4;
		four = true;
	}
};

std::vector<std::string*> coast_coord;
std::vector<coordinate*> coast_vertex;

void PlaceCameraAtRollerCoaster(void){
	mPosition = STVector3(7.5,0.5,25);
	mLookAt	  = STVector3(7.5,0.5,23);
	//mUp		  = STVector3(0.0,0.0,1.0);
}

void SetUpAndRight()
{
    mRight = STVector3::Cross(mLookAt - mPosition, mUp);
    mRight.Normalize();
    mUp = STVector3::Cross(mRight, mLookAt - mPosition);
    mUp.Normalize();
}

void resetCamera()
{
    mLookAt=STVector3(1.f,0.f,0.f);
    mPosition=STVector3(0.f,0.f,0.f);
    mUp=STVector3(0.f,1.f,0.f);

    //SetUpAndRight();
}

void resetUp()
{
    mUp = STVector3(0.f,1.f,0.f);
    mRight = STVector3::Cross(mLookAt - mPosition, mUp);
    mRight.Normalize();
    mUp = STVector3::Cross(mRight, mLookAt - mPosition);
    mUp.Normalize();
}


void renderScene(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.3, 0.3, 1.0);
	glutSwapBuffers();
}



/*void InitDisplay(){
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(500, 500);
	glutInitWindowSize(800, 600);
	glutCreateWindow("OpenGL First Window");
	glewInit();
	if (glewIsSupported("GL_VERSION_2_0")){
		std::cout << " GLEW Version is 2.0\n ";
	}
	else{
	   std::cout << "GLEW 2.0 not supported\n ";
	}
	glEnable(GL_DEPTH_TEST);
	// register callbacks
	glutDisplayFunc(renderScene);
	glutMainLoop();
	return;
}*/
void BuildCoasterPosition(const char* filename){
	std::ifstream in(filename);

	if(!in.is_open()){
		printf("Unable to open file!\n");
		return;
	}

	char c[256];
	while(!in.eof()){
		in.getline(c, 256);
		coast_coord.push_back(new std::string(c));
	}
	for (int i = 0; i < coast_coord.size(); i++){
		float tmpx, tmpy, tmpz;
		sscanf(coast_coord[i]->c_str(), "%f %f %f", &tmpx, &tmpy, &tmpz);
		coast_vertex.push_back(new coordinate(tmpx, tmpy, tmpz));
		printf("A = %f, B = %f, C = %f\n",tmpx, tmpy, tmpz);
	}

}

void MoveRollerCoaster(void){
	coordinate *current, *next;
	static int i = 0;
	if(i < (coast_vertex.size()-1)){
		next = coast_vertex.at(i);
		current 	= coast_vertex.at(i+1);
	}
	else{
		i = 0;
		next = coast_vertex.at(i);
		current 	= coast_vertex.at(i+1);
	}
	mLookAt.x = next->x;
	mLookAt.y = next->y;
	mLookAt.z = next->z;

	mPosition.x = current->x;
	mPosition.y = current->y;
	mPosition.z = current->z;

	i++;

	printf("%f %f %f --- %f %f %f\n",mLookAt.x,mLookAt.y,mLookAt.z,mPosition.x,mPosition.y,mPosition.z);
	//while(1);
}

int loadMyObject(const char* filename){
	std::vector<std::string*> coord;
	std::vector<coordinate*> vertex;
	std::vector<face*> faces;
	std::vector<coordinate*> normals;
	std::ifstream in(filename);

	if(!in.is_open()){
		printf("Unable to open file!\n");
		return -1;
	}

	char c[256];
	while(!in.eof()){
		in.getline(c, 256);
		coord.push_back(new std::string(c));
	}
	for (int i = 0; i < coord.size(); i++){
		if((*coord[i])[0] == '#'){
			continue;
		} else if ((*coord[i])[0] =='v' && (*coord[i])[1] == ' '){
			float tmpx, tmpy, tmpz;
			sscanf(coord[i]->c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);
			vertex.push_back(new coordinate(tmpx, tmpy, tmpz));
		} else if((*coord[i])[0] == 'v' && (*coord[i])[1] == 'n'){
			float tmpx, tmpy, tmpz;
			sscanf(coord[i]->c_str(), "vn %f %f %f", &tmpx, &tmpy, &tmpz);
			normals.push_back(new coordinate(tmpz, tmpy, tmpz));
		} else if((*coord[i])[0] == 'f'){
			int a, b, c, d, e;
			if (count(coord[i]->begin(), coord[i]->end(), ' ') == 4){
				sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d %d//%d", &a, &b, &c, &b, &d, &b, &e, &b);
				faces.push_back(new face(b, a, c, d, e));
			} else{
				sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d", &a, &b, &c, &b, &d, &b);
				faces.push_back(new face(b, a, c, d));
			}
		}
	}

	//Drawing the faces, vertices
	int num;
	num = glGenLists(1);
	glNewList(num, GL_COMPILE);

	for(int i = 0; i < faces.size(); i++){
		if (faces[i]->four){
			glBegin(GL_QUADS);
				glColor3f(0.5, 0.5, 0.5);
				glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
				glVertex3f(vertex[faces[i]->faces[0] - 1]->x, vertex[faces[i]->faces[0] - 1]->y, vertex[faces[i]->faces[0] - 1]->z);
				glVertex3f(vertex[faces[i]->faces[1] - 1]->x, vertex[faces[i]->faces[1] - 1]->y, vertex[faces[i]->faces[1] - 1]->z);
				glVertex3f(vertex[faces[i]->faces[2] - 1]->x, vertex[faces[i]->faces[2] - 1]->y, vertex[faces[i]->faces[2] - 1]->z);
				glVertex3f(vertex[faces[i]->faces[3] - 1]->x, vertex[faces[i]->faces[3] - 1]->y, vertex[faces[i]->faces[3] - 1]->z);
			glEnd();
		} else {
			glBegin(GL_TRIANGLES);
				glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
				glVertex3f(vertex[faces[i]->faces[0] - 1]->x, vertex[faces[i]->faces[0] - 1]->y, vertex[faces[i]->faces[0] - 1]->z);
				glVertex3f(vertex[faces[i]->faces[1] - 1]->x, vertex[faces[i]->faces[1] - 1]->y, vertex[faces[i]->faces[1] - 1]->z);
				glVertex3f(vertex[faces[i]->faces[2] - 1]->x, vertex[faces[i]->faces[2] - 1]->y, vertex[faces[i]->faces[2] - 1]->z);
			glEnd();
		}
	}

	glEndList();

	for(int i = 0; i < coord.size(); i++){
		delete coord[i];
	}
	for (int i = 0; i < vertex.size(); i++){
		delete vertex[i];
	}
	for (int i = 0; i < normals.size(); i++){
		delete normals[i];
	}
	for (int i = 0; i < faces.size(); i++){
		delete faces[i];
	}

	return num;
}

GLuint LoadSkyboxFile(const char *fileName)
{
	FILE *file;
	unsigned char header[54],*data;
	unsigned int dataPos,size,width, height;
	file = fopen(fileName, "rb");
	fread(header, 1, 54, file);				//Parsing BMP 54 byte header
	dataPos		= *(int*)&(header[0x0A]);	//Actual BMP data
	size		= *(int*)&(header[0x22]);	//BMP Size
	width		= *(int*)&(header[0x12]);	//Image Width
	height		= *(int*)&(header[0x16]);	//Image Height
	if (size == 0)
		size = width * height * 3;
	if (dataPos == 0)
		dataPos = 54;
	data = new unsigned char[size];
	fread(data, 1, size, file);
	fclose(file);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	return texture;
}

void initEnvironment()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	skybox[SKY_DOWN] = LoadSkyboxFile("SKYBOX/down.bmp");
	skybox[SKY_FRONT] = LoadSkyboxFile("SKYBOX/front.bmp");
	skybox[SKY_BACK] = LoadSkyboxFile("SKYBOX/back.bmp");
	skybox[SKY_RIGHT] = LoadSkyboxFile("SKYBOX/right.bmp");
	skybox[SKY_LEFT] = LoadSkyboxFile("SKYBOX/left.bmp");
	skybox[SKY_UP] = LoadSkyboxFile("SKYBOX/up.bmp");
	grass=LoadSkyboxFile("SKYBOX/grass_1.bmp");
}

void Draw_Skybox(float x, float y, float z, float width, float height, float length){

	glDisable(GL_DEPTH_TEST);
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,skybox[SKY_UP]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y+height, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y+height, z+length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z+length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,skybox[SKY_FRONT]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z+length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height,	z+length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height,	z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,skybox[SKY_BACK]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z+length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z+length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,skybox[SKY_RIGHT]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,skybox[SKY_LEFT]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height, z+length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height, z+length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z+length);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,skybox[SKY_DOWN]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z+length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y,		z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y,		z+length);
	glEnd();
 	glDisable(GL_TEXTURE_2D);

}



void draw_ground()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,grass);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(5000,-10,5000);
	glTexCoord2f(800.0f, 0.0f); glVertex3f(5000,-10,-5000);
	glTexCoord2f(800.0f, 800.0f); glVertex3f(-5000,-10,-5000);
	glTexCoord2f(0.0f, 800.0f); glVertex3f(-5000,-10,5000);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glLineWidth(5.0);
	glTranslatef(0.0, -2, 0.0);
	glTranslatef(0.0, 2, 0.0);
}

/**
 * Camera adjustment methods
 */
void RotateCamera(float delta_x, float delta_y)
{
    float yaw_rate=1.f;
    float pitch_rate=1.f;

    mPosition -= mLookAt;
    STMatrix4 m;
    m.EncodeR(-1*delta_x*yaw_rate, mUp);
    mPosition = m * mPosition;
    m.EncodeR(-1*delta_y*pitch_rate, mRight);
    mPosition = m * mPosition;

    mPosition += mLookAt;
}

void ZoomCamera(float delta_y)
{
    STVector3 direction = mLookAt - mPosition;
    float magnitude = direction.Length();
    direction.Normalize();
    float zoom_rate = 0.1f*magnitude < 0.5f ? .1f*magnitude : .5f;
    if(delta_y * zoom_rate + magnitude > 0)
    {
        mPosition += (delta_y * zoom_rate) * direction;
    }
}

void StrafeCamera(float delta_x, float delta_y)
{
    float strafe_rate = 0.05f;

    mPosition -= strafe_rate * delta_x * mRight;
    mLookAt   -= strafe_rate * delta_x * mRight;
    mPosition += strafe_rate * delta_y * mUp;
    mLookAt   += strafe_rate * delta_y * mUp;
}

void SpecialKeyCallback(int key, int x, int y)
{
    switch(key) {
        case GLUT_KEY_LEFT:
            StrafeCamera(10,0);
            break;
        case GLUT_KEY_RIGHT:
            StrafeCamera(-10,0);
            break;
        case GLUT_KEY_DOWN:
            StrafeCamera(0,-10);
            break;
        case GLUT_KEY_UP:
            StrafeCamera(0,10);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}



void MoveCarousel(void){

}
void KeyCallback(unsigned char key, int x, int y)
{
    // TO DO: Any new key press events must be added to this function
    switch(key) {
    case '+':
    	MoveRollerCoaster();
    	break;
    case '-':
    	MoveCarousel();
    	break;
    case 'b':
    	PlaceCameraAtRollerCoaster();

        break;
    case 'w':
    	mLookAt.x +=1;
    	break;
    case 'x':
    	mLookAt.x -=1;
    	break;
    case 'y':
    	mLookAt.y += 1;
    	break;
    case 'z':
    	mLookAt.y -= 1;
    	break;
    case 'e':
     	mLookAt.z += 1;
     	break;
    case 'f':
     	mLookAt.z -= 1;
     	break;
    case 's': {
            //
            // Take a screenshot, and save as screenshot.jpg
            //
            STImage* screenshot = new STImage(800, 600);
            screenshot->Read(0,0);
            screenshot->Save("../../data/images/screenshot.jpg");
            delete screenshot;
        }
        break;
    case 'r':
        resetCamera();
        break;
    case 'u':
        resetUp();
        break;
	case 'q':
		exit(0);
    default:
        break;
    }

    glutPostRedisplay();
}



void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0,1.0,1.0,1.0);
	glLoadIdentity();
	//gluLookAt(viewer[0], viewer[1], viewer[2],camera[0], camera[1], camera[2],0, 1, 0);
	//SetUpAndRight();
    gluLookAt(mLookAt.x,mLookAt.y,mLookAt.z,
    			mPosition.x,mPosition.y,mPosition.z,
              mUp.x,mUp.y,mUp.z);

	Draw_Skybox(viewer[0]+(0.05*movcord[0]),viewer[1]+(0.05*movcord[1]),viewer[2]+(0.05*movcord[2]),250,250,250);
	//Draw_Skybox(mLookAt.x+(0.05*movcord[0]),mLookAt.y+(0.05*movcord[1]),mLookAt.z+(0.05*movcord[2]),250,250,250);

	draw_ground();
	glCallList(coasterMesh);
	glCallList(carouselMesh);
	glCallList(coasterBarsMesh);
	glPushMatrix();

	glutSwapBuffers();
}


void idle()
{
	MoveRollerCoaster();
	display();

}

void displayReshape(int width,int height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluPerspective(65,(GLfloat)width/(GLfloat)height,0.01f,1000.0f);
	gluPerspective(65, (GLfloat)width/(GLfloat)height, 0.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}




int main(int argc, char** argv)
{
		glutInit(&argc,argv);
		glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
		glutInitWindowSize(800,600);
		glutCreateWindow("CAP5705 Project");
		BuildCoasterPosition("OBJ/Barlocations.txt");
	    resetCamera();
		initEnvironment();
		coasterMesh = loadMyObject("OBJ/rollerCoaster.obj");
		carouselMesh = loadMyObject("OBJ/merryGoRound.obj");
		coasterBarsMesh = loadMyObject("OBJ/CoasterBars.obj");
  		glutDisplayFunc(display);
	 	glutReshapeFunc(displayReshape);
	    glutSpecialFunc(SpecialKeyCallback);
	    glutKeyboardFunc(KeyCallback);
  		glutDisplayFunc(display);
  		glutIdleFunc(idle);
		glutMainLoop();
		return 0;
}
