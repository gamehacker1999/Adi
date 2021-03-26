// This OpenGL project demonstrates a simple interactive camera implementation and a few vertex and fragment shaders. 
// Two modes of camera controls (press c to switch between two modes): 
// 1. Focus Mode: Holding ALT and Left Mouse Button (LMB) and moving the mouse will rotate the camera about the LookAt Position
//                Holding ALT and MMB and moving the mouse will pan the camera.
//                Holding ALT and RMB and moving the mouse will zoom the camera.
// 2. First-Person Mode: Pressing W, A, S, or D will move the camera
//                       Holding LMB and moving the mouse will roate the camera.
// Basic shader - demonstrate the basic use of vertex and fragment shader
// Displacement shader - a special fireball visual effects with Perlin noise function
// Toon shading shader - catoonish rendering effects
// Per-vertex shading v.s. per-fragment shading = visual comparison between two types of shading 

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Camera.h"
#include "Text.h"
#include "Mesh.h"

#include "PointLight.h"

#include <iostream>
using namespace std;
using namespace glm;

int g_winWidth  = 1024;
int g_winHeight = 512;

Camera g_cam;
Text g_text;

unsigned char g_keyStates[256];

char v_shader_file[] =
//".\\shaders\\basic.vert";
//".\\shaders\\displacement.vert"; // vertex displacement shader with perlin noise
//".\\shaders\\perVert_lambert.vert"; // basic lambert lighting  
// ".\\shaders\\perFrag_lambert.vert"; // basic lambert lighting with per-fragment implementation
// ".\\shaders\\toon_shading.vert"; // basic toon shading with per-fragment implementation
".\\shaders\\test.vert";

char f_shader_file[] =
//".\\shaders\\basic.frag";
// ".\\shaders\\displacement.frag"; // vertex displacement shader with perlin noise
// ".\\shaders\\perVert_lambert.frag"; // basic lambert shading 
// ".\\shaders\\perFrag_lambert.frag"; // basic lambert shading with per-fragment implementation
// ".\\shaders\\toon_shading.frag"; // basic toon shading with per-fragment implementation
".\\shaders\\test.frag";

const char meshFile[128] = 
//"Mesh/sphere.obj";
//"Mesh/bunny2K.obj";
"Mesh/teapot.obj";
//"Mesh/teddy.obj";

Mesh g_mesh;

vec3 g_lightPos = vec3(3, 3, 3);

vec3 pointLightPositions[] = {
	vec3(3.0f,3.0f,3.0f),
	vec3(1.0f,0.0f,-2.0f)
};

vec3 diffuseValues[] = {
	vec3(1.0f,1.0f,0.0f),
	vec3(1.0f,0.0f,1.0f)
};

vec3 ambientValues[] = {
	vec3(0.0f,0.15f,0.0f),
	vec3(0.0f,0.0f,0.15f)
};

vec3 specularValues[] = {
	vec3(1.0f,0.0f,0.0f),
	vec3(1.0f,0.0f,0.0f)
};

int coeffValues[] = {
	64,
	64
};



float g_time = 0.0f;

void initialization() 
{    
    g_cam.set(1.0f, 2.0f, 4.0f, 0.0f, 1.0f, -0.5f, g_winWidth, g_winHeight);
	g_text.setColor(0.0f, 0.0f, 0.0f);

	g_mesh.create(meshFile, v_shader_file, f_shader_file);
	// add any stuff you want to initialize ...
}

/****** GL callbacks ******/
void initialGL()
{    
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

void idle()
{
    // add any stuff to update at runtime ....

    g_cam.keyOperation(g_keyStates, g_winWidth, g_winHeight);

	glutPostRedisplay();
}

void display()
{	 
	glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	// add any stuff you'd like to draw	

	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	g_cam.drawGrid();
    g_cam.drawCoordinateOnScreen(g_winWidth, g_winHeight);
    g_cam.drawCoordinate();

	// display the text
	string str;
	if(g_cam.isFocusMode()) {
        str = "Cam mode: Focus";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	} else if(g_cam.isFPMode()) {
        str = "Cam mode: FP";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	}
	str = "vertex count: " + std::to_string(g_mesh.vert_num);
	g_text.draw(10, 45, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	str = "triangle count: " + std::to_string(g_mesh.tri_num);
	g_text.draw(10, 60, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	
	PointLight lights[2];

	lights[0] = {};
	lights[0].ambient = vec3(1, 1, 1);
	lights[0].diffuse = vec3(0.5, 0.5, 0);
	lights[0].specular = vec3(0.5, 0.5, 0.5);
	lights[0].position = vec3(0, 0, 0);
	lights[0].coeff = 20;

	lights[1].ambient = vec3(1, 1, 1);
	lights[1].diffuse = vec3(0.5, 0.5, 0);
	lights[1].specular = vec3(0.5, 0.5, 0.5);
	lights[1].position = vec3(-2, 0, 0);
	lights[1].coeff = 20;

	g_time = (float)glutGet(GLUT_ELAPSED_TIME)/1000.0f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutSolidSphere(1, 50, 50);

	//g_mesh.draw(g_cam.viewMat, g_cam.projMat, g_lightPos, g_time);
	g_mesh.draw(g_cam.viewMat, g_cam.projMat, lights, vec3(0.0, 2.0, 0.0), vec3(0.5, 0.5, 0.5), vec3(g_cam.eye.x, g_cam.eye.y, g_cam.eye.z));
	g_mesh.draw(g_cam.viewMat, g_cam.projMat, lights, vec3(3.0, 2.0, 0.0), vec3(0.5, 0.5, 0.5), vec3(g_cam.eye.x, g_cam.eye.y, g_cam.eye.z));


	
    glutSwapBuffers();
}

void reshape(int w, int h)
{
	g_winWidth = w;
	g_winHeight = h;
	if (h == 0) {
		h = 1;
	}
	g_cam.setProjectionMatrix(g_winWidth, g_winHeight);
    g_cam.setViewMatrix();
    glViewport(0, 0, w, h);
}

void mouse(int button, int state, int x, int y)
{
    g_cam.mouseClick(button, state, x, y, g_winWidth, g_winHeight);
}

void motion(int x, int y)
{
    g_cam.mouseMotion(x, y, g_winWidth, g_winHeight);
}

void keyup(unsigned char key, int x, int y)
{
    g_keyStates[key] = false;
}

void keyboard(unsigned char key, int x, int y)
{
    g_keyStates[key] = true;
	switch(key) { 
		case 27:
			exit(0);
			break;
        case 'c': // switch cam control mode
            g_cam.switchCamMode();
			glutPostRedisplay();
            break;
        case ' ':
            g_cam.PrintProperty();
            break;
		case '+':
			g_mesh.normal_offset += 0.01;
			break;
		case'-':
			g_mesh.normal_offset -= 0.01;
	}
}

int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_winWidth, g_winHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("VertFrag Shader Example");
	
	glewInit();
	initialGL();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	initialization();
	
    glutMainLoop();
    return EXIT_SUCCESS;
}