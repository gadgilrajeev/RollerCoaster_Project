#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include "tiny_obj_loader.h"

/*#include "stglew.h"
#include "glut.h"
#include <iostream>
#include "tiny_obj_loader.h"
*/


enum{
	SKY_FRONT=0,
	SKY_RIGHT=1,
	SKY_LEFT=2,
	SKY_BACK=3,
	SKY_UP=4,
	SKY_DOWN=5,
};

GLint skybox[6], grass,x_r=0, y_r=0, z_r=0;
GLfloat viewer[3] = {1.0f, 0.0f, 0.0f},camera[3] = {0.0f, 0.0, 0.0};
GLdouble movcord[3]={-150,-10,200};


/*void renderScene(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.3, 0.3, 1.0);
	glutSwapBuffers();
}

void InitDisplay(){
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
}
void LoadFromObj(){
	std::string inputfile = "RollerCoasterTrack.obj";
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string materialBase = "";

	std::string err = tinyobj::LoadObj(shapes, materials, materialBase.c_str(), inputfile.c_str());

	if (!err.empty()) {
	  std::cerr << err << std::endl;
	  exit(1);
	}

	std::cout << "# of shapes : " << shapes.size() << std::endl;

	for (size_t i = 0; i < shapes.size(); i++) {
	  printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
	  printf("shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
	  assert((shapes[i].mesh.indices.size() % 3) == 0);
	  for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) {
		printf("  idx[%ld] = %d\n", f, shapes[i].mesh.indices[f]);
	  }

	  printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
	  assert((shapes[i].mesh.positions.size() % 3) == 0);
	  for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
		printf("  v[%ld] = (%f, %f, %f)\n", v,
		  shapes[i].mesh.positions[3*v+0],
		  shapes[i].mesh.positions[3*v+1],
		  shapes[i].mesh.positions[3*v+2]);
	  }

	  printf("shape[%ld].material.name = %s\n", i, shapes[i].material.name.c_str());
	  printf("  material.Ka = (%f, %f ,%f)\n", shapes[i].material.ambient[0], shapes[i].material.ambient[1], shapes[i].material.ambient[2]);
	  printf("  material.Kd = (%f, %f ,%f)\n", shapes[i].material.diffuse[0], shapes[i].material.diffuse[1], shapes[i].material.diffuse[2]);
	  printf("  material.Ks = (%f, %f ,%f)\n", shapes[i].material.specular[0], shapes[i].material.specular[1], shapes[i].material.specular[2]);
	  printf("  material.Tr = (%f, %f ,%f)\n", shapes[i].material.transmittance[0], shapes[i].material.transmittance[1], shapes[i].material.transmittance[2]);
	  printf("  material.Ke = (%f, %f ,%f)\n", shapes[i].material.emission[0], shapes[i].material.emission[1], shapes[i].material.emission[2]);
	  printf("  material.Ns = %f\n", shapes[i].material.shininess);
	  printf("  material.map_Ka = %s\n", shapes[i].material.ambient_texname.c_str());
	  printf("  material.map_Kd = %s\n", shapes[i].material.diffuse_texname.c_str());
	  printf("  material.map_Ks = %s\n", shapes[i].material.specular_texname.c_str());
	  printf("  material.map_Ns = %s\n", shapes[i].material.normal_texname.c_str());
	  std::map<std::string, std::string>::iterator it(shapes[i].material.unknown_parameter.begin());
	  std::map<std::string, std::string>::iterator itEnd(shapes[i].material.unknown_parameter.end());
	  for (; it != itEnd; it++) {
		printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
	  }
	  printf("\n");
}

	return;
}*/

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



void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0,1.0,1.0,1.0);
	glLoadIdentity();
	gluLookAt(viewer[0], viewer[1], viewer[2],camera[0], camera[1], camera[2],0, 1, 0);
	glRotatef(x_r, 0, 1, 0);
	Draw_Skybox(viewer[0]+(0.05*movcord[0]),viewer[1]+(0.05*movcord[1]),viewer[2]+(0.05*movcord[2]),250,250,250);
	glTranslatef(movcord[0],movcord[1],movcord[2]);
	draw_ground();
	glPushMatrix();
	glTranslatef(80,0,165);
	glutSwapBuffers();
}

void displayReshape(int width,int height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65,(GLfloat)width/(GLfloat)height,0.01f,1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


int main(int argc, char** argv)
{
		glutInit(&argc,argv);
		glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
		glutInitWindowSize(800,600);
		glutCreateWindow("CAP5705 Project");
		initEnvironment();
  		glutDisplayFunc(display);
	 	glutReshapeFunc(displayReshape);
		glutMainLoop();
		return 0;
}


