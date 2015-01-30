////////////////////////////////////////////////////
// anim.cpp version 4.1
// Template code for drawing an articulated figure.
// CS 174A 
////////////////////////////////////////////////////

#ifdef _WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GLUT/glut.h>
#endif

#include "Ball.h"
#include "FrameSaver.h"
#include "Timer.h"
#include "Shapes.h"
#include "tga.h"

#include "Angel/Angel.h"

#ifdef __APPLE__
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#endif

FrameSaver FrSaver ;
Timer TM ;

BallData *Arcball = NULL ;
int Width = 800;
int Height = 800 ;
int Button = -1 ;
float Zoom = 1 ;
int PrevY = 0 ;

int Animate = 0 ;
int Recording = 0 ;

void resetArcball() ;
void save_image();
void instructions();
void set_colour(float r, float g, float b) ;

const int STRLEN = 100;
typedef char STR[STRLEN];

#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2

//texture

GLuint texture_stone;
GLuint texture_ground;
GLuint texture_leaf;

// Structs that hold the Vertex Array Object index and number of vertices of each shape.
ShapeData triangleData;
ShapeData cubeData;
ShapeData sphereData;
ShapeData coneData;
ShapeData cylData;
ShapeData pyramidData;


// Matrix stack that can be used to push and pop the modelview matrix.
class MatrixStack {
    int    _index;
    int    _size;
    mat4*  _matrices;

   public:
    MatrixStack( int numMatrices = 32 ):_index(0), _size(numMatrices)
        { _matrices = new mat4[numMatrices]; }

    ~MatrixStack()
	{ delete[]_matrices; }

    void push( const mat4& m ) {
        assert( _index + 1 < _size );
        _matrices[_index++] = m;
    }

    mat4& pop( void ) {
        assert( _index - 1 >= 0 );
        _index--;
        return _matrices[_index];
    }
};

MatrixStack  mvstack;
mat4         model_view;
GLint        uModelView, uProjection, uView;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uEnableTex;

// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
Angel::vec4 eye(0, 0.0, 50.0,1.0);
Angel::vec4 ref(0.0, 0.0, 0.0,1.0);
Angel::vec4 up(0.0,1.0,0.0,0.0);

double TIME = 0.0 ;
double prevTime = 0;
double fps;

double scene1Time = 2 * M_PI;
double scene1t2Time = 7;
double scene2Time = 10;
double scene2t3Time = 11;
double scene3Time = 15;
double scene4Time = 17;
double scene5Time = 21;
double scene6Time = 24;
double scene7Time = 25;
double scene8Time = 28;
double scene9Time = 30;
double scene10Time = 32;
double scene11Time = 33;
double scene12Time = 36;
double scene13Time = 39;
double scene14Time = 41;

/////////////////////////////////////////////////////
//    PROC: drawCylinder()
//    DOES: this function 
//          render a solid cylinder  oriented along the Z axis. Both bases are of radius 1. 
//          The bases of the cylinder are placed at Z = 0, and at Z = 1.
//
//          
// Don't change.
//////////////////////////////////////////////////////
void drawCylinder(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
}

//////////////////////////////////////////////////////
//    PROC: drawCone()
//    DOES: this function 
//          render a solid cone oriented along the Z axis with base radius 1. 
//          The base of the cone is placed at Z = 0, and the top at Z = 1. 
//         
// Don't change.
//////////////////////////////////////////////////////
void drawCone(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, coneData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawCube()
//    DOES: this function draws a cube with dimensions 1,1,1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawCube(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
}

void drawGroundCube(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_ground );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawStoneCube(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_stone );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawLeafSphere(void)
{
	glBindTexture( GL_TEXTURE_2D, texture_leaf );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
	glUniform1i( uEnableTex, 0 );
}

void drawPyramid(void)
{
	glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( pyramidData.vao );
    glDrawArrays( GL_TRIANGLES, 0, pyramidData.numVertices );

}

void drawTriangle(void)
{
	glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( triangleData.vao );
    glDrawArrays( GL_TRIANGLES, 0, triangleData.numVertices );

}

//////////////////////////////////////////////////////
//    PROC: drawSphere()
//    DOES: this function draws a sphere with radius 1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawSphere(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
}


void resetArcball()
{
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}


//////////////////////////////////////////////////////
//    PROC: myKey()
//    DOES: this function gets caled for any keypresses
// 
//////////////////////////////////////////////////////

void myKey(unsigned char key, int x, int y)
{
    float time ;
    switch (key) {
        case 'q':
        case 27:
            exit(0); 
        case 's':
            FrSaver.DumpPPM(Width,Height) ;
            break;
        case 'r':
            resetArcball() ;
            glutPostRedisplay() ;
            break ;
        case 'a': // togle animation
            Animate = 1 - Animate ;
            // reset the timer to point to the current time		
            time = TM.GetElapsedTime() ;
            TM.Reset() ;
            //printf("Elapsed time %f\n", time) ;
            break ;
        case '0':
            //reset your object
            break ;
        case 'm':
            if( Recording == 1 )
            {
                printf("Frame recording disabled.\n") ;
                Recording = 0 ;
            }
            else
            {
                printf("Frame recording enabled.\n") ;
                Recording = 1  ;
            }
            FrSaver.Toggle(Width);
            break ;
        case 'h':
        case '?':
            instructions();
            break;
    }
    glutPostRedisplay() ;

}

/*********************************************************
    PROC: myinit()
    DOES: performs most of the OpenGL intialization
     -- change these with care, if you must.

**********************************************************/

void myinit(void)
{
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram(program);

    // Generate vertex arrays for geometric shapes
	generatePyramid(program, &pyramidData);
    generateCube(program, &cubeData);
    generateSphere(program, &sphereData);
    generateCone(program, &coneData);
    generateCylinder(program, &cylData);
	generateTriangle(program, &triangleData);

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView       = glGetUniformLocation( program, "View"       );

    glClearColor( 0.6, 0.8, 1.0, 1.0 ); // light blue background

    uAmbient   = glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse   = glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular  = glGetUniformLocation( program, "SpecularProduct" );
    uLightPos  = glGetUniformLocation( program, "LightPosition"   );
    uShininess = glGetUniformLocation( program, "Shininess"       );
    uTex       = glGetUniformLocation( program, "Tex"             );
    uEnableTex = glGetUniformLocation( program, "EnableTex"       );

    glUniform4f(uAmbient,    0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uDiffuse,    0.6f,  0.6f,  0.6f, 1.0f);
    glUniform4f(uSpecular,   0.3f,  0.3f,  0.3f, 1.0f);
    glUniform4f(uLightPos,  -20.0f, 20.0f, 20.0f, 0.0f);
    glUniform1f(uShininess, 110.0f);

    glEnable(GL_DEPTH_TEST);
 
 

	TgaImage grassImage;
    if (!grassImage.loadTGA("grass-texture.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }

	TgaImage leafImage;
    if (!leafImage.loadTGA("leaf-texture.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
    TgaImage stoneImage;
    if (!stoneImage.loadTGA("stone-texture.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    

	glGenTextures( 1, &texture_ground );
    glBindTexture( GL_TEXTURE_2D, texture_ground );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, grassImage.width, grassImage.height, 0,
                 (grassImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, grassImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glGenTextures( 1, &texture_stone );
    glBindTexture( GL_TEXTURE_2D, texture_stone );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, stoneImage.width, stoneImage.height, 0,
                 (stoneImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, stoneImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );


	glGenTextures( 1, &texture_leaf );
    glBindTexture( GL_TEXTURE_2D, texture_leaf );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, leafImage.width, leafImage.height, 0,
                 (leafImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, leafImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	
    // Set texture sampler variable to texture unit 0
    // (set in glActiveTexture(GL_TEXTURE0))
    
    glUniform1i( uTex, 0);

    Arcball = new BallData;
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}

/*********************************************************
    PROC: set_colour();
    DOES: sets all material properties to the given colour
    -- don't change
**********************************************************/

void set_colour(float r, float g, float b)
{
    float ambient  = 0.2f;
    float diffuse  = 0.6f;
    float specular = 0.2f;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1.0f);
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1.0f);
}

/*********************************************************
 PROC: drawGround(mat4);
 DOES: use the input matrix as view transformation, set
 model transformation and draw the ground object.
 **********************************************************/

void drawGround(mat4 view_trans){
    mat4 model_trans(1.0f);
    //model a ground
    model_trans *= Translate(0, -10, 0);
    set_colour(0.0f, 0.8f, 0.0f);
    model_trans *= Scale(250, 1, 250);
    model_view = view_trans * model_trans;
    drawGroundCube();
    
    model_trans = mvstack.pop();//pop
    
}

void drawFlyingBird(mat4 view_trans, mat4 model_trans, float theta, float toggle) {
	model_trans *= RotateY(theta);
	model_trans *= Translate(0, toggle, 8);
	set_colour(0.8f, 0.0f, 0.0f);

	//body
	mvstack.push(model_trans);
	model_trans *= Scale(1.1, 0.8, 0.8);
	model_view = view_trans * model_trans;
	drawSphere();

	model_trans = mvstack.pop();
	 
	//head
	mvstack.push(model_trans);
	model_trans *= Translate (1.5, 0, 0);
	model_trans *= Scale(0.7);
	model_view = view_trans * model_trans;
	drawSphere();
	model_trans = mvstack.pop();

	//beak
	mvstack.push(model_trans);
	model_trans *= Translate(2.4, 0, 0);
	model_trans *= RotateY(-90);
	model_trans *= Scale(0.3);
	set_colour(0.97, 0.82, 0.23);
	model_view = view_trans * model_trans;
	drawCone();
	model_trans = mvstack.pop();

	//tail
	mvstack.push(model_trans);
	model_trans *= Translate(-1.8, -0.2, 0);
	model_trans *= RotateY(90);
	model_trans *= RotateX(-20);
	set_colour(1.0f, 0.4f, 0.4f);
	model_view = view_trans * model_trans;
	drawTriangle();
	model_trans = mvstack.pop();

	//right wing
	double rtheta = 35.0*sin(5.0*TIME);
	mvstack.push(model_trans);
	 
	model_trans *= Translate(0, 0.2, 0.4);
	model_trans *= RotateX(rtheta);
	model_trans *= Translate(0, 0, 1.5);
	model_trans *= Scale(1, 0.1, 3);
	set_colour(1.0f, 0.4f, 0.4f);
	model_view = view_trans * model_trans;
	drawCube();

	 model_trans = mvstack.pop();

	 //left wing
	 double ltheta = -35.0*sin(5.0*TIME);
	 mvstack.push(model_trans);

	 model_trans *= Translate(0, 0.2, -0.4);
	 model_trans *= RotateX(ltheta);
	 model_trans *= Translate(0, 0, -1.5);
	 model_trans *= Scale(1, 0.1, 2.5);
	 set_colour(1.0f, 0.4f, 0.4f);
	 model_view = view_trans * model_trans;
	 drawCube();

	 model_trans = mvstack.pop();

	 model_trans = mvstack.pop();
}

/*********************************************************
**********************************************************
**********************************************************

    PROC: display()
    DOES: this gets called by the event handler to draw
          the scene, so this is where you need to build
          your ROBOT --  
      
        MAKE YOUR CHANGES AND ADDITIONS HERE

    Add other procedures if you like.

**********************************************************
**********************************************************
**********************************************************/
void display(void)
{
   
	//NOTE TO SELF: Call glUniform here to change shader parameters
	//Can update eye, ref, and up here to move the camera 


     // Clear the screen with the background colour (set in myinit)
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
     mat4 model_trans(1.0f);
     mat4 view_trans(1.0f);
	 int i;
	 
	 
/*if (Recording == 1)
		 TIME += 0.033;
	 else
	 {
		fps = TIME/1.0;
		printf("Frames per second: %f\n", fps);
	 }
     */

	 if(TIME <= scene1Time) {
		eye = Angel::vec4(72*sin(TIME), 40, 72*cos(TIME), 1);
	 }
	 else if (TIME <= scene1t2Time)
	 {
		 eye = Angel::vec4(0, 40, 72, 1) ;
	 }
	 else if (TIME <= scene2Time)
	 {
		 eye = Angel::vec4(27, 15, 50-(10*(TIME-scene1t2Time)), 1);
		 ref = Angel::vec4(27, 10, 0, 1);
	 }
	 else if (TIME <= scene2t3Time)
	 {
		 eye = Angel::vec4(27, 15, 20, 1);
		 ref = Angel::vec4(27, 10, 0, 1);
	 }
	 else if (TIME <= scene3Time)
	 {
		 eye = Angel::vec4(45, 15, -16, 1);
		 ref = Angel::vec4(27, 12, 0, 1);
	 }
	 else if (TIME <= scene4Time)
	 {
		eye = Angel::vec4(28, 15, 7, 1);
		ref = Angel::vec4(28, 13, 0, 1);
	 }
	 else if (TIME <= scene5Time)
	 {
		 eye = Angel::vec4(28, 13-(5*sin(5*TIME)), -2, 1);
		 ref = Angel::vec4(28, 0, 0, 1);
		 up = Angel::vec4(-1, 0, 0, 0);
	 }
	 else  if (TIME <= scene7Time)
	 {
		eye = Angel::vec4(28, 15, 7, 1);
		ref = Angel::vec4(28, 13, 0, 1);
		up = Angel::vec4(0, 1, 0, 0);
	 }
	 else if (TIME <= scene8Time)
	 {
		 eye = Angel::vec4(28, 15, 7+(3*(TIME-scene7Time)), 1);
	 }
	 else if (TIME <= scene9Time)
	 {
		 eye = Angel::vec4(28, 15, 16, 1);
	 }
	 else if (TIME <= scene10Time)
	 {
		 eye = Angel::vec4(3, 60, 5, 1);
		 ref = Angel::vec4(3, 60, 0, 1);
	 }
	 else if (TIME <= scene11Time)
	 {
		 eye = Angel::vec4(3, 60, 5+(7*(TIME-scene10Time)), 1);
		 ref = Angel::vec4(3, 60, 0, 1);
	 }
	 else
	 {
		 eye = Angel::vec4(3, 60, 12, 1);
	 }
	 
     view_trans = LookAt(eye, ref, up);
	 
	 /*view_trans *= Translate(20.0f, 0.0f, -40.0f);
     HMatrix r;
     Ball_Value(Arcball,r);
     
     mat4 mat_arcball_rot(
     r[0][0], r[0][1], r[0][2], r[0][3],
     r[1][0], r[1][1], r[1][2], r[1][3],
     r[2][0], r[2][1], r[2][2], r[2][3],
     r[3][0], r[3][1], r[3][2], r[3][3]);
     view_trans *= mat_arcball_rot;
     view_trans *= Scale(Zoom);
     */
     glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );
     
     mvstack.push(model_trans);//push
     drawGround(view_trans);

	 //main bird///////////////////////////////////////////////////
	 if (TIME <= scene3Time) 
	 {
		 mvstack.push(model_trans);
		 model_trans *= Translate (30.5, 12.3, 0);
		 model_trans *= RotateZ(-45);
	 
		 //body
		 mvstack.push(model_trans);
		 model_trans *= Scale(1.1, 0.8, 0.8);
		 set_colour(0.8f, 0.0f, 0.0f);
		 model_view = view_trans * model_trans;
		 drawSphere();

		 model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, 0.6);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(15);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, -0.6);
		model_trans *= RotateY(10);
		model_trans *= RotateZ(10);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();
		//eyes end

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		 model_trans = mvstack.pop();
	 }
	 else if (TIME <= scene4Time)
	 {
		mvstack.push(model_trans);
		model_trans *= Translate (30.5, 12.3, 0);
		model_trans *= RotateZ(-45);
	 
		//body
		mvstack.push(model_trans);
		model_trans *= Scale(1.1, 0.8, 0.8);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();

		model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);
		model_trans *= RotateZ(35*(TIME-scene3Time));

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, 0.6);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(15);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, -0.6);
		model_trans *= RotateY(10);
		model_trans *= RotateZ(10);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();
		//eyes end

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	 }
	 else if (TIME <= scene5Time)
	 {
		 mvstack.push(model_trans);
		 model_trans *= Translate (30.5, 12.3, 0);
		 model_trans *= RotateZ(-45);
	 
		 //body
		 mvstack.push(model_trans);
		 model_trans *= Scale(1.1, 0.8, 0.8);
		 set_colour(0.8f, 0.0f, 0.0f);
		 model_view = view_trans * model_trans;
		 drawSphere();

		 model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);
		model_trans *= RotateZ(20);

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, 0.6);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(15);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, -0.6);
		model_trans *= RotateY(10);
		model_trans *= RotateZ(15);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();
		//eyes end

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	 }
	 else if (TIME <= scene6Time)
	 {
		 mvstack.push(model_trans);
		 model_trans *= Translate (30.5, 12.3, 0);
		 model_trans *= RotateZ(-45);
	 
		 //body
		 mvstack.push(model_trans);
		 model_trans *= Scale(1.1, 0.8, 0.8);
		 set_colour(0.8f, 0.0f, 0.0f);
		 model_view = view_trans * model_trans;
		 drawSphere();

		 model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);
		model_trans *= RotateZ(20);
		model_trans *= RotateY(30*sin(7*TIME));

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, 0.07, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, -0.07, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();


		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, 0.07, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, -0.07, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();
		//eyes end

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	 }
	 else if (TIME <= scene7Time)
	 {
		mvstack.push(model_trans);
		model_trans *= Translate (30.5, 12.3, 0);
		model_trans *= RotateZ(-45);
	 
		//body
		mvstack.push(model_trans);
		model_trans *= Scale(1.1, 0.8, 0.8);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();

		model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);
		model_trans *= RotateZ(20-(20*(TIME-scene6Time)));

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, 0.6);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(15);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, -0.6);
		model_trans *= RotateY(10);
		model_trans *= RotateZ(10);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();
		//eyes end

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	 }
	 else if (TIME <= scene8Time)
	 {
		 mvstack.push(model_trans);
		 model_trans *= Translate (30.5, 12.3, 0);
		 model_trans *= RotateZ(-45);
	 
		 //body
		 mvstack.push(model_trans);
		 model_trans *= Scale(1.1, 0.8, 0.8);
		 set_colour(0.8f, 0.0f, 0.0f);
		 model_view = view_trans * model_trans;
		 drawSphere();

		 model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, 0.07, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, -0.07, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, 0.07, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, -0.07, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();
		//eyes end

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
	 
		model_trans *= Translate(0, 0.2, 0.4);
		model_trans *= RotateX(35.0*sin(11.0*TIME));
		model_trans *= Translate(0, 0, 1.5);
		model_trans *= Scale(1, 0.1, 3);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		//left wing
		mvstack.push(model_trans);

		model_trans *= Translate(0, 0.2, -0.4);
		model_trans *= RotateX(-35.0*sin(11.0*TIME));
		model_trans *= Translate(0, 0, -1.5);
		model_trans *= Scale(1, 0.1, 2.5);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	}
	else if (TIME <= scene9Time)
	{
		mvstack.push(model_trans);
		model_trans *= Translate (30.5-(10*(TIME-scene8Time)), 12.3+(5*(TIME-scene8Time)), 0);
		model_trans *= RotateZ(-45);
	 
		//body
		mvstack.push(model_trans);
		model_trans *= Scale(1.1, 0.8, 0.8);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();

		model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, 0.07, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, -0.07, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, 0.07, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, -0.07, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();
		//eyes end

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
	 
		//right wing
		model_trans *= Translate(0, 0.2, 0.4);
		model_trans *= RotateX(35.0*sin(11.0*TIME));
		model_trans *= Translate(0, 0, 1.5);
		model_trans *= Scale(1, 0.1, 3);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		//left wing
		mvstack.push(model_trans);

		model_trans *= Translate(0, 0.2, -0.4);
		model_trans *= RotateX(-35.0*sin(11.0*TIME));
		model_trans *= Translate(0, 0, -1.5);
		model_trans *= Scale(1, 0.1, 2.5);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	}
	else if (TIME <= scene11Time)
	{
		mvstack.push(model_trans);
		model_trans *= Translate (5, 60, 0);
		model_trans *= RotateZ(-5);
	 
		//body
		mvstack.push(model_trans);
		model_trans *= Scale(1.1, 0.8, 0.8);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();

		model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, 0.07, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, -0.07, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, 0.07, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.22, -0.07, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-30);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();
		//eyes end

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
	 
		//right wing
		model_trans *= Translate(0, 0.2, 0.4);
		model_trans *= RotateX(35.0*sin(6.0*TIME));
		model_trans *= Translate(0, 0, 1.5);
		model_trans *= Scale(1, 0.1, 3);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		//left wing
		mvstack.push(model_trans);

		model_trans *= Translate(0, 0.2, -0.4);
		model_trans *= RotateX(-35.0*sin(6.0*TIME));
		model_trans *= Translate(0, 0, -1.5);
		model_trans *= Scale(1, 0.1, 2.5);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	}
	else if (TIME <= scene12Time)
	{
		mvstack.push(model_trans);
		model_trans *= Translate (5, 60, 0);
		model_trans *= RotateZ(-5);
	 
		//body
		mvstack.push(model_trans);
		model_trans *= Scale(1.1, 0.8, 0.8);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();

		model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, 0.6);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(15);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.2, 0.1, -0.6);
		model_trans *= RotateY(10);
		model_trans *= RotateZ(10);
		model_trans *= Scale(0.1, 0.2, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCylinder();
		model_trans = mvstack.pop();
		//eyes end

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
	 
		model_trans *= Translate(0, 0.2, 0.4);
		model_trans *= RotateX(35.0*sin(6.0*TIME));
		model_trans *= Translate(0, 0, 1.5);
		model_trans *= Scale(1, 0.1, 3);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		//left wing
		mvstack.push(model_trans);

		model_trans *= Translate(0, 0.2, -0.4);
		model_trans *= RotateX(-35.0*sin(6.0*TIME));
		model_trans *= Translate(0, 0, -1.5);
		model_trans *= Scale(1, 0.1, 2.5);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	}
	else if (TIME <= scene13Time)
	{
		mvstack.push(model_trans);
		model_trans *= Translate (5, 60, 0);
		model_trans *= RotateZ(-5);
	 
		//body
		mvstack.push(model_trans);
		model_trans *= Scale(1.1, 0.8, 0.8);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();

		model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.3, 0.2, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(40);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.1, 0.2, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-40);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.3, 0.2, -0.7);
		model_trans *= RotateY(10);
		model_trans *= RotateZ(40);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.1, 0.2, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-40);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
	 
		model_trans *= Translate(0, 0.2, 0.4);
		model_trans *= RotateX(35.0*sin(6.0*TIME));
		model_trans *= Translate(0, 0, 1.5);
		model_trans *= Scale(1, 0.1, 3);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		//left wing
		mvstack.push(model_trans);

		model_trans *= Translate(0, 0.2, -0.4);
		model_trans *= RotateX(-35.0*sin(6.0*TIME));
		model_trans *= Translate(0, 0, -1.5);
		model_trans *= Scale(1, 0.1, 2.5);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	}
	else if (TIME <= scene14Time)
	{
		mvstack.push(model_trans);
		model_trans *= Translate (5-(6*(TIME-scene13Time)), 60, 0);
		model_trans *= RotateZ(-5);
	 
		//body
		mvstack.push(model_trans);
		model_trans *= Scale(1.1, 0.8, 0.8);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();

		model_trans = mvstack.pop();
	 
		//head start
		mvstack.push(model_trans);
		model_trans *= Translate (-1.6, 0, 0);

		mvstack.push(model_trans);
		model_trans *= Scale(0.7);
		set_colour(0.8f, 0.0f, 0.0f);
		model_view = view_trans * model_trans;
		drawSphere();
		model_trans = mvstack.pop();

		//eyes start
		mvstack.push(model_trans);
		model_trans *= Translate(-0.3, 0.2, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(40);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.1, 0.2, 0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-40);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.3, 0.2, -0.7);
		model_trans *= RotateY(10);
		model_trans *= RotateZ(40);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
		model_trans *= Translate(-0.1, 0.2, -0.7);
		model_trans *= RotateY(-10);
		model_trans *= RotateZ(-40);
		model_trans *= Scale(0.3, 0.1, 0.08);
		set_colour(0, 0, 0);
		model_view = view_trans * model_trans;
		drawCube();
		model_trans = mvstack.pop();

		//beak
		mvstack.push(model_trans);
		model_trans *= Translate(-0.8, -0.35, 0);
		model_trans *= RotateY(90);
		model_trans *= RotateX(-20);
		model_trans *= Scale(0.23);
		set_colour(0.97, 0.82, 0.23);
		model_view = view_trans * model_trans;
		drawCone();
		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
		//head end

		//tail
		mvstack.push(model_trans);
		model_trans *= Translate(1.8, 0.2, 0);
		model_trans *= RotateY(-90);
		model_trans *= RotateX(-10);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawTriangle();
		model_trans = mvstack.pop();

		mvstack.push(model_trans);
	 
		model_trans *= Translate(0, 0.2, 0.4);
		model_trans *= RotateX(35.0*sin(6.0*TIME));
		model_trans *= Translate(0, 0, 1.5);
		model_trans *= Scale(1, 0.1, 3);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		//left wing
		mvstack.push(model_trans);

		model_trans *= Translate(0, 0.2, -0.4);
		model_trans *= RotateX(-35.0*sin(6.0*TIME));
		model_trans *= Translate(0, 0, -1.5);
		model_trans *= Scale(1, 0.1, 2.5);
		set_colour(1.0f, 0.4f, 0.4f);
		model_view = view_trans * model_trans;
		drawCube();

		model_trans = mvstack.pop();

		model_trans = mvstack.pop();
	}
	 /////////////////////////////////////////////////////////////////


	 //static objects////////////////////////////////////////////////

	 /////tree start
	 //j
	 mvstack.push(model_trans);

	 //b
	 model_trans *= Translate (35, 8, 0);
	 mvstack.push(model_trans);

	 //a
	 mvstack.push(model_trans);
	 model_trans *= RotateX(90);
	 model_trans *= Scale(3, 3, 18);
	 set_colour(0.52, 0.35, 0.14);
	 model_view = view_trans * model_trans;
	 drawCylinder();

	 model_trans = mvstack.pop();
	 //a

	 ///branch start
	 //d
	 mvstack.push(model_trans);
	 model_trans *= Translate (-6, 3, 0);
	 model_trans *= RotateY(90);
	 model_trans *= RotateX(15);

	 //c
	 mvstack.push(model_trans);
	 model_trans *= Scale(0.8, 0.8, 4);
	 set_colour(0.52, 0.35, 0.14);
	 model_view = view_trans * model_trans;
	 drawCylinder();

	 model_trans = mvstack.pop();
	 //c

	 mvstack.push(model_trans);
	 model_trans *= Translate (0, 0, -6);
	 model_trans *= Scale(0.8, 0.8, 2);
	 model_view = view_trans * model_trans;
	 drawCone();
	 model_trans = mvstack.pop();
	 //branch end

	 model_trans = mvstack.pop();
	 //d

	 //leaves start

	 model_trans *= Translate(0, 17, 0);
	 model_trans *= Scale(12, 8, 10);
	 set_colour(0.34, 0.53, 0.17);
	 model_view = view_trans * model_trans;
	 //drawSphere();
	 drawLeafSphere();
	 model_trans = mvstack.pop();
	 //b
	 //leaves end

	 model_trans = mvstack.pop();
	 //j
	 //tree end

	 //house start
	 //k
	 mvstack.push(model_trans);

	 //a
	 model_trans *= Translate (-35, 0, 0);
	 
	 //base
	 //b
	 mvstack.push(model_trans);
	 model_trans *= Scale(30, 20, 30);
	 set_colour(1.0, 1.0, 0.8);
	 model_view = view_trans * model_trans;
	 drawStoneCube();
	 model_trans = mvstack.pop();
	 //b
	
	 //door
	 //f
	 mvstack.push(model_trans);
	 model_trans *= Translate (15, -5, 0);
	 model_trans *= Scale (2, 12, 9);
	 set_colour(0.8, 0.52, 0.25);
	 model_view = view_trans * model_trans;
	 drawCube();
	 model_trans = mvstack.pop();
	 //f

	 //window
	 mvstack.push(model_trans);
	 model_trans *= Translate (14.3, 5, 8);
	 model_trans *= Scale (2, 6, 7);
	 set_colour(0.94, 0.97, 1.0);
	 model_view = view_trans * model_trans;
	 drawCube();
	 model_trans = mvstack.pop();

	 mvstack.push(model_trans);
	 model_trans *= Translate (14.3, 5, -8);
	 model_trans *= Scale (2, 6, 7);
	 set_colour(0.94, 0.97, 1.0);
	 model_view = view_trans * model_trans;
	 drawCube();
	 model_trans = mvstack.pop();

	 //roof
	 //c
	 mvstack.push(model_trans);
	 model_trans *= Translate (0, 10, 0);
	 model_trans *= Scale(18, 18, 18);
	 set_colour(0.5, 0.24, 0.1);
	 model_view = view_trans * model_trans;
	 drawPyramid();
	 model_trans = mvstack.pop();
	 //c

	 //chimney
	 model_trans *= Translate(0, 20, 9);
	 model_trans *= Scale(4, 8, 4);
	 set_colour(0.34, 0.24, 0.18);
	 model_view = view_trans * model_trans;
	 drawCube();
	 //a

	 model_trans = mvstack.pop();
	 //house end
	 //k
	 

	 //static objects end/////////////////////////////////////////

	 //moving objects begin//////////////////////////////////////

	 //draw birds
	 mvstack.push(model_trans);
	 model_trans *= Translate(-10, 10, 12);

	 drawFlyingBird(view_trans, model_trans, 80*TIME, (9+3*sin(2.0*TIME)));

	 mvstack.push(model_trans);
	 model_trans *= Translate (-5, 6, 7);
	 drawFlyingBird(view_trans, model_trans, 100*TIME, (5+3*sin(4.0*TIME)));


	 //light ref
	 mvstack.push(model_trans);
	 model_trans *= Translate (0, -50, 0);
	 set_colour(0.0f, 1.0f, 0.0f);
	 model_trans *= Scale(2.0);
	 model_view = view_trans * model_trans;
	 drawSphere();

	 model_trans = mvstack.pop();


    glutSwapBuffers();
    if(Recording == 1)
        FrSaver.DumpPPM(Width, Height);
}

/**********************************************
    PROC: myReshape()
    DOES: handles the window being resized 
    
      -- don't change
**********************************************************/
void myReshape(int w, int h)
{
    Width = w;
    Height = h;

    glViewport(0, 0, w, h);

    mat4 projection = Perspective(50.0f, (float)w/(float)h, 1.0f, 1000.0f);
    glUniformMatrix4fv( uProjection, 1, GL_TRUE, projection );
}

void instructions() 
{
    printf("Press:\n");
    printf("  s to save the image\n");
    printf("  r to restore the original view.\n") ;
    printf("  0 to set it to the zero state.\n") ;
    printf("  a to toggle the animation.\n") ;
    printf("  m to toggle frame dumping.\n") ;
    printf("  q to quit.\n");
}

// start or end interaction
void myMouseCB(int button, int state, int x, int y)
{
    Button = button ;
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width-1.0;
        arcball_coords.y = -2.0*(float)y/(float)Height+1.0;
        Ball_Mouse(Arcball, arcball_coords) ;
        Ball_Update(Arcball);
        Ball_BeginDrag(Arcball);

    }
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {
        Ball_EndDrag(Arcball);
        Button = -1 ;
    }
    if( Button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        PrevY = y ;
    }


    // Tell the system to redraw the window
    glutPostRedisplay() ;
}

// interaction (mouse motion)
void myMotionCB(int x, int y)
{
    if( Button == GLUT_LEFT_BUTTON )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width - 1.0 ;
        arcball_coords.y = -2.0*(float)y/(float)Height + 1.0 ;
        Ball_Mouse(Arcball,arcball_coords);
        Ball_Update(Arcball);
        glutPostRedisplay() ;
    }
    else if( Button == GLUT_RIGHT_BUTTON )
    {
        if( y - PrevY > 0 )
            Zoom  = Zoom * 1.03 ;
        else 
            Zoom  = Zoom * 0.97 ;
        PrevY = y ;
        glutPostRedisplay() ;
    }
}


void idleCB(void)
{
    if( Animate == 1 )
    {
        // TM.Reset() ; // commenting out this will make the time run from 0
        // leaving 'Time' counts the time interval between successive calls to idleCB
        if( Recording == 0 )
            TIME = TM.GetElapsedTime() ;
        else
            TIME += 0.033 ; // save at 30 frames per second.

		if (TIME - prevTime >= 1)
		{
			prevTime = TIME;
			printf("Frames per second: %f\n", fps);
			fps = 0;
		}
		else
		{
			fps++;
		}
        //printf("TIME %f\n", TIME) ;
        glutPostRedisplay() ; 
    }
}
/*********************************************************
     PROC: main()
     DOES: calls initialization, then hands over control
           to the event handler, which calls 
           display() whenever the screen needs to be redrawn
**********************************************************/

int main(int argc, char** argv) 
{
    glutInit(&argc, argv);
    // If your code fails to run, uncommenting these lines may help.
    //glutInitContextVersion(3, 2);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (50, 50);
    glutInitWindowSize(Width,Height);
    glutCreateWindow("My cool bee");
    printf("GL version %s\n", glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    glewInit();
    
    myinit();

    glutIdleFunc(idleCB) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCB) ;
    glutMotionFunc(myMotionCB) ;
    instructions();

    glutDisplayFunc(display);
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}




