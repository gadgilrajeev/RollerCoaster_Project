#include <stdlib.h>
//#include <GL/glut.h>
#include <glut.h>
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
#include <math.h>
#include <fstream>
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
int FrameRate = 15;
int help = 0;
int moveRoller 	 = 0;
int moveCarousel = 0;
int MoveCarouselFree = 0;
STVector3 mPosition;
STVector3 mLookAt;
STVector3 mRight;
STVector3 mUp;

GLint skybox[6], grass,x_r=0, y_r=0, z_r=0;
GLfloat viewer[3] = {1.0f, 0.0f, 0.0f},camera[3] = {0.0f, 0.0, 0.0};
GLdouble movcord[3]={-150,-10,200};
int coasterMesh, coasterBarsMesh,carouselMesh, carouselHorses, carouselOthers, tent, text, shops, fence;
int show_menu = 1;
GLuint Texture1, Texture2, Texture3, Texture4, Texture5, Texture6, Texture7, Texture8, Texture9;

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
	mUp		  = STVector3(0.0,-1.0,0.0);
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
    mLookAt=STVector3(1.f,0.1f,0.f);
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
//		printf("A = %f, B = %f, C = %f\n",tmpx, tmpy, tmpz);
	}

}

/*GLuint LoadTexture(char* fileName){
	GLuint ID;
	unsigned char *img;
	int height, width;


	img = NULL;

	tagBITMAPFILEHEADER fh;
	tagBITMAPINFOHEADER infoh;
	FILE *fp = fopen(fileName, "rb");

	if(!fp){
		printf("\nFailed to read Bitmap");
		return -1;
	}

	fread(&fh, sizeof(fh), 1, fp);
	fread(&infoh, sizeof(infoh), 1, fp);

	if(infoh.biBitCount != 24){
		printf("\n File not 24 bit BMP file");
		return -1;
	}

	width = infoh.biWidth;
	height = infoh.biHeight;

	img = new unsigned char(infoh.biSizeImage);

	fread(img, infoh.biSizeImage, 1, fp);

	unsigned char temp;

	for(int i = 0; i < infoh.biSizeImage; i++){
		temp = img[i];
		img[i] = img[i+2];
		img[i+2] = temp;
		i += 2;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, img);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	return ID;
}*/

int loadMyObject(const char* filename, GLuint Texture){
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
		glShadeModel(GL_SMOOTH);
		if (faces[i]->four){
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, Texture);
			glBegin(GL_QUADS);
			//	glColor3f(0.5, 0.5, 0.5);
				glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);

				glTexCoord2f(0.0, 0.0);
				glVertex3f(vertex[faces[i]->faces[0] - 1]->x, vertex[faces[i]->faces[0] - 1]->y, vertex[faces[i]->faces[0] - 1]->z);

				glTexCoord2f(1.0, 0.0);
				glVertex3f(vertex[faces[i]->faces[1] - 1]->x, vertex[faces[i]->faces[1] - 1]->y, vertex[faces[i]->faces[1] - 1]->z);

				glTexCoord2f(1.0, 1.0);
				glVertex3f(vertex[faces[i]->faces[2] - 1]->x, vertex[faces[i]->faces[2] - 1]->y, vertex[faces[i]->faces[2] - 1]->z);

				glTexCoord2f(0.0, 1.0);
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

GLuint LoadSkyboxFile(const char *fileName, GLuint retTexture)
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
	glGenTextures(retTexture, &texture);
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
	skybox[SKY_DOWN] = LoadSkyboxFile("SKYBOX/down.bmp", 1);
	skybox[SKY_FRONT] = LoadSkyboxFile("SKYBOX/front.bmp", 1);
	skybox[SKY_BACK] = LoadSkyboxFile("SKYBOX/back.bmp", 1);
	skybox[SKY_RIGHT] = LoadSkyboxFile("SKYBOX/right.bmp", 1);
	skybox[SKY_LEFT] = LoadSkyboxFile("SKYBOX/left.bmp", 1);
	skybox[SKY_UP] = LoadSkyboxFile("SKYBOX/up.bmp", 1);
	grass=LoadSkyboxFile("SKYBOX/grass_1.bmp", 1);
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
	glTexCoord2f(0.0f, 0.0f); glVertex3f(5000,0,5000);
	glTexCoord2f(800.0f, 0.0f); glVertex3f(5000,0,-5000);
	glTexCoord2f(800.0f, 800.0f); glVertex3f(-5000,0,-5000);
	glTexCoord2f(0.0f, 800.0f); glVertex3f(-5000,0,5000);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glLineWidth(5.0);
//	glTranslatef(0.0, -10, 0.0);
//	glTranslatef(0.0, 2, 0.0);
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

void initLights()
{

	GLfloat whiteSpecularMaterial[] = {1.0,1.0,1.0},light_post0[]={0.0,0.0,10.0,1.0},whiteSpecularLight[] = {1.0, 1.0, 1.0},blackAmbientLight[] = {0.3, 0.3, 0.3},whiteDiffuseLight[] = {1.0, 1.0, 1.0},mShininess[] = {50},twoModel[]={GL_TRUE};
	glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
   	glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, light_post0);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, twoModel);
 	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
}

void drawText(char *string,float x,float y,float z)
{
	char *c;
	glRasterPos3f(x,y,z);
	for (c=string; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
	}
}

void display(){
	static int i = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0,1.0,1.0,1.0);
	glLoadIdentity();

	if(help)
	{
		glPushMatrix();
		glColor4f(1.0,1.0,0.0,1.0);
		glDisable(GL_LIGHTING);
		resetCamera();
		resetUp();
		SetUpAndRight();
		gluLookAt(mLookAt.x,mLookAt.y,mLookAt.z,
	    			mPosition.x,mPosition.y,mPosition.z,
	              mUp.x,mUp.y,mUp.z);
		drawText((char*)" Amusement Park", 0,0.35,0.82);
		drawText((char*)" Welcome to the Amusement Park.", 0.001,0.3,0.82);
		drawText((char*)" h: Help", 0.005,0.25,0.82);
		drawText((char*)" v: Moving Carousel", 0.01,0.20,0.82);
		drawText((char*)" + and -: Sit on Carousel and Sit on the Roller Coaster", 0.02,0.15,0.82);
		drawText((char*)" w and x: Move forward and backward", 0.02,0.10,0.82);
		drawText((char*)" e and f: Move up and down", 0.02,0.05,0.82);
		drawText((char*)" y and z: Move left and right", 0.03,0.0,0.82);
		drawText((char*)" s: Take a screen shot", 0.03,-0.05,0.82);
		drawText((char*)" i: Increase Interpolating Frames", 0.04,-0.10,0.82);
		drawText((char*)" d: Decrease Interpolating Frames", 0.04,-0.15,0.82);
		drawText((char*)" Developed by Sai and Rajeev", 0.05,-0.2,0.82);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
	else
	{
	    glPushMatrix();
		//glColor4f(1.0,1.0,0.0,1.0);
		glDisable(GL_LIGHTING);
		gluLookAt(mLookAt.x,mLookAt.y,mLookAt.z,
		mPosition.x,mPosition.y,mPosition.z,
		mUp.x,mUp.y,mUp.z);
		Draw_Skybox(viewer[0]+(0.05*movcord[0]),viewer[1]+(0.05*movcord[1]),viewer[2]+(0.05*movcord[2]),250,250,250);
		draw_ground();

		glCallList(tent);
		glCallList(text);
		glCallList(shops);
		glCallList(fence);
		glCallList(coasterBarsMesh);
		//drawText((char*)"FrameRate", mLookAt.x-40,mLookAt.y+40,mLookAt.z);
		glCallList(coasterMesh);
		//glCallList(coasterBarsMesh);
		glTranslatef(-47.65778,0.5, 20.34678);
		glRotatef(15*i/FrameRate,0,1,0);
		glTranslatef(47.65778,-0.5, -20.34678);
		glCallList(carouselHorses);
		glCallList(carouselOthers);
		glCallList(carouselMesh);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
	glutSwapBuffers();
	i++;
}

void interpolateFrames(coordinate init, coordinate fin, coordinate viewInit, coordinate viewFin){
	for (int i = 0; i < FrameRate; i++){
		float alpha = (float)i / FrameRate;
		mPosition.x = init.x * alpha + fin.x *(1-alpha);
		mPosition.y = init.y * alpha + fin.y *(1-alpha);
		mPosition.z = init.z * alpha + fin.z *(1-alpha);

		mLookAt.x = viewInit.x * alpha + viewFin.x * (1-alpha);
		mLookAt.y = viewInit.y * alpha + viewFin.y * (1-alpha);
		mLookAt.z = viewInit.z * alpha + viewFin.z * (1-alpha);
		
		if(moveRoller){
			SetUpAndRight();
		}
		display();
	}
}

void MoveRollerCoaster(void){
	coordinate *current, *next, *further;
	static int i = 0;
	if(i < (coast_vertex.size()-2)){
		further = coast_vertex.at(i);
		next = coast_vertex.at(i+1);
		current = coast_vertex.at(i+2);
	}
	else{
		i = 0;
		further = coast_vertex.at(i);
		next = coast_vertex.at(i+1);
		current = coast_vertex.at(i+2);
		resetUp();
	}

	coordinate view(further->x, further->y, further->z);
	coordinate init(current->x, current->y, current->z);
	coordinate fin(next->x, next->y, next->z);

	interpolateFrames(init, fin, fin, view);

	/*mLookAt.x = next->x;
	mLookAt.y = next->y;
	mLookAt.z = next->z;

	mPosition.x = current->x;
	mPosition.y = current->y;
	mPosition.z = current->z;*/

	i++;

//	printf("Keyframe : %f %f %f --- %f %f %f\n",mLookAt.x,mLookAt.y,mLookAt.z,mPosition.x,mPosition.y,mPosition.z);
	//while(1);
}

void PlaceCameraAtCarousel(void){
	mPosition = STVector3(-47.64,0.5, 40.62);
	mLookAt	  = STVector3(-47.64,0.5, 20.31);
}

void MoveCarousel(void){
	coordinate *center(new coordinate(-47.64, 0.5, 20.31)), *viewInit(new coordinate(0.0, 0.0, 0.0)), *viewFin(new coordinate(0.0, 0.0, 0.0)), *init(new coordinate(0.0, 0.0, 0.0)), *fin(new coordinate(0.0, 0.0, 0.0));
	float radius = 6.30;
	float pi = 3.1415;

	float z1 = -0.7;
	float z2 = -1.7;

	static int i = 0;

	if(-1*i >= 24){
		i = 0;
	}

	init->x = center->x + radius * cos(15 * i * pi/180);
	init->y = -z1;
	init->z = center->z + radius * sin(15 * i * pi/180);
	

	if (-1*i % 2 == 1){
		init->y = -z2;
	}

	fin->x = center->x + radius * cos(15 * (i+1) * pi/180);
	fin->z = center->z + radius * sin(15 * (i+1) * pi/180);
	fin->y = -z2;

	if (-1*i % 2 == 1){
		fin->y = -z1;
	}

	viewInit->x = -47.64;
	viewInit->y = 0.5;
	viewInit->z = 20.31;

	viewFin->x = -47.64;
	viewFin->y = 0.5;
	viewFin->z = 20.31;
	/*	viewInit->x = center->x + 2 * radius * cos(15 * i * pi/180);
	viewInit->z = center->z + 2 * radius * sin(15 * i * pi/180);
	viewInit->y = -z1;*/

	/*viewInit->x = center->x + radius * cos(15 * (i+1) * pi/180);
	viewInit->z = center->z + radius * sin(15 * (i+1) * pi/180);
	viewInit->y = -z1;


	if (-1*i % 2 == 1){
		viewInit->y = -z2;
	}

	viewFin->x = center->x + radius * cos(15 * (i+2) * pi/180);
	viewFin->z = center->z + radius * sin(15 * (i+2) * pi/180);
	viewFin->y = -z2;

	if (-1*i % 2 == 1){
		viewFin->y = -z1;
	}*/

	//std::cout << i << "\n";
	//std::cout << "View: " << viewInit->x << " " << viewInit->y << " " << viewInit->z << "\n";
	//std::cout << "View Final: " << viewFin->x << " " << viewFin->y << " " << viewFin->z << "\n";

	interpolateFrames(*viewInit, *viewFin, *init, *fin);

	i--;
}

void KeyCallback(unsigned char key, int x, int y)
{
    // TO DO: Any new key press events must be added to this function
    switch(key) {
    case 'h':
    	if(help == 1)
    		help = 0;
    	else
    		help = 1;
    	break;
    case 'v':
    	if(MoveCarouselFree == 1){
    		moveCarousel = 1;
    		MoveCarouselFree = 0;
    	}
    	else if(MoveCarouselFree == 0){
    		moveCarousel = 0;
    		MoveCarouselFree = 1;
    		//MoveCarousel();
    	}
    	else{
    		moveCarousel = 0;
    		MoveCarouselFree = 0;
    	}
		moveRoller = 0;
		resetUp();
    	break;
    case '+':
    	moveRoller = 1;
    	moveCarousel = 0;
    	MoveCarouselFree = 0;
    	MoveRollerCoaster();
    	break;
    case '-':
    	moveRoller = 0;
		resetUp();
    	moveCarousel = 1;
    	MoveCarouselFree = 0;
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
    case 'i':
    		FrameRate++;
    		if(FrameRate > 100)
    			FrameRate = 100;
    	break;
    case 'd':
        	FrameRate--;
        	if(FrameRate < 1)
        		FrameRate = 1;
        break;
	case 'q':
		exit(0);
    default:
        break;
    }

    glutPostRedisplay();
}

void idle()
{
	if(help == 1 || MoveCarouselFree == 1)
		display();
	else{
		if(moveRoller == 1)
			MoveRollerCoaster();
		if(moveCarousel == 1)
			MoveCarousel();
	}
}

void displayReshape(int width,int height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65, (GLfloat)width/(GLfloat)height, 0.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void addMouseClickStuff()
{
	int submenu1,submenu2,submenu21, submenu22,submenu3, submenu31, submenu32, submenu4, submenu41;
	//submenu1 = glutCreateMenu(PlaceCameraAtCarousel);
	glutAddMenuEntry("Free Movement",0);
	glutAddMenuEntry("Inside Giant Wheel",1);
	glutAddMenuEntry("On Columbus ship",2);
	glutAddMenuEntry("On Roller Coaster",3);
	glutAddMenuEntry("Change Background",0);
	glutAddMenuEntry("Show/hide Help",1);
	glutAddMenuEntry("Quit",2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}


int main(int argc, char** argv)
{
		glutInit(&argc,argv);
		glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
		glutInitWindowSize(800,600);
		glutCreateWindow("CAP5705 Project");
		BuildCoasterPosition("OBJ/Barlocations.txt");
	    resetCamera();
	    //initLights();
		initEnvironment();

		Texture1 = LoadSkyboxFile("../../data/images/wood.bmp", 2);
		Texture2 = LoadSkyboxFile("../../data/images/metal.bmp", 3);
		Texture3 = LoadSkyboxFile("../../data/images/red.bmp", 4);
		Texture4 = LoadSkyboxFile("../../data/images/brown.bmp", 5);
		Texture5 = LoadSkyboxFile("../../data/images/black.bmp", 6);

		tent = loadMyObject("OBJ/Tents.obj", Texture3);
		text = loadMyObject("OBJ/Text.obj", Texture5);
		shops = loadMyObject("OBJ/Shops.obj", Texture2);
		fence = loadMyObject("OBJ/Fence.obj", Texture1);
		carouselMesh = loadMyObject("OBJ/CarouselUmbrella.obj", Texture3);
		carouselHorses = loadMyObject("OBJ/CarouselHorses.obj", Texture4);
		carouselOthers = loadMyObject("OBJ/CarouselOthers.obj", Texture5);
		coasterMesh = loadMyObject("OBJ/rollerCoaster.obj", Texture2);
		coasterBarsMesh = loadMyObject("OBJ/CoasterBars.obj", Texture1);

  		glutDisplayFunc(display);
	 	glutReshapeFunc(displayReshape);
	    glutSpecialFunc(SpecialKeyCallback);
	    glutKeyboardFunc(KeyCallback);
  		glutDisplayFunc(display);
  		glutIdleFunc(idle);
  		addMouseClickStuff();
		glutMainLoop();
		return 0;
}
