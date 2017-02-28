#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <list>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <FTGL/ftgl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
GLuint programID;

void createLaser();
struct VAO {
	GLuint VertexArrayID;
	GLuint VertexBuffer;
	GLuint ColorBuffer;

	GLenum PrimitiveMode;
	GLenum FillMode;
	int NumVertices;
};
typedef struct VAO VAO;

typedef struct COLOR {
	float r;
	float g;
	float b;
} COLOR;

struct Sprite{
	float rotation;
	float pos_x;
	float pos_y;
	float back_x;
	float back_y;
	float dx;
	float dy;
	float rdx;
	float limit_xn,limit_xp,limit_yn, limit_yp;
	float limit_rn,limit_rp;
	float width;
	float height;
	float length;
	float speed;
	bool active;
	bool choose;
	bool hover;
	int color;
	VAO* object;
	string name;
};
typedef struct Sprite Sprite;

void SpriteRotateUp(Sprite* sprite){
	sprite->rotation+=sprite->rdx;
	if(sprite->rotation>=sprite->limit_rp){

		sprite->rotation-=sprite->rdx;
	}
}
void SpriteRotateDown(Sprite* sprite){

	sprite->rotation-=sprite->rdx;
	if(sprite->rotation<=sprite->limit_rn){
		sprite->rotation+=sprite->rdx;
	}
}
void SpriteMoveUp(Sprite* sprite){

	sprite->pos_y+=sprite->dy;
	if(sprite->pos_y>=sprite->limit_yp){
		sprite->pos_y-=sprite->dy;
	}
}
void SpriteMoveDown(Sprite* sprite){

	sprite->pos_y-=sprite->dy;
	if(sprite->pos_y<=sprite->limit_yn){
		sprite->pos_y+=sprite->dy;
	}
}
void SpriteMoveRight(Sprite* sprite){ //1

	sprite->pos_x+=sprite->dx;
	if(sprite->pos_x + sprite->width>= sprite->limit_xp)
		sprite->pos_x-=sprite->dx;
}
void SpriteMoveLeft(Sprite* sprite){

	sprite->pos_x-=sprite->dx;
	if(sprite->pos_x <= sprite->limit_xn)
		sprite->pos_x+=sprite->dx;
}

struct Point{
	float x,y,z;
};
typedef struct Point Point;
struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

COLOR grey = {168.0/255.0,168.0/255.0,168.0/255.0};
COLOR gold = {218.0/255.0,165.0/255.0,32.0/255.0};
COLOR coingold = {255.0/255.0,223.0/255.0,0.0/255.0};
COLOR red = {255.0/255.0,51.0/255.0,51.0/255.0};
COLOR lightgreen = {57/255.0,230/255.0,0/255.0};
COLOR darkgreen = {51/255.0,102/255.0,0/255.0};
COLOR black = {30/255.0,30/255.0,21/255.0};
COLOR blue = {0,0,1};
COLOR darkbrown = {46/255.0,46/255.0,31/255.0};
COLOR lightbrown = {95/255.0,63/255.0,32/255.0};
COLOR brown1 = {117/255.0,78/255.0,40/255.0};
COLOR brown2 = {134/255.0,89/255.0,40/255.0};
COLOR brown3 = {46/255.0,46/255.0,31/255.0};
COLOR cratebrown = {153/255.0,102/255.0,0/255.0};
COLOR cratebrown1 = {121/255.0,85/255.0,0/255.0};
COLOR cratebrown2 = {102/255.0,68/255.0,0/255.0};
COLOR skyblue2 = {113/255.0,185/255.0,209/255.0};
COLOR skyblue1 = {123/255.0,201/255.0,227/255.0};
COLOR skyblue = {132/255.0,217/255.0,245/255.0};
COLOR cloudwhite = {229/255.0,255/255.0,255/255.0};
COLOR cloudwhite1 = {204/255.0,255/255.0,255/255.0};
COLOR lightpink = {255/255.0,122/255.0,173/255.0};
COLOR darkpink = {255/255.0,51/255.0,119/255.0};
COLOR white = {255/255.0,255/255.0,255/255.0};
COLOR scoreColor = {117/255.0,78/255.0,40/255.0};

struct Mice{
	int clicked=0;
	int right_clicked=0;
	double pos_x,pos_y;
	double back_x,back_y;
	double x_new,y_new;
};
typedef struct Mice Mice;
Mice mouse;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
	//    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;

	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

	glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
			0,                  // attribute 0. Vertices
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
	GLfloat* color_buffer_data = new GLfloat [3*numVertices];
	for (int i=0; i<numVertices; i++) {
		color_buffer_data [3*i] = red;
		color_buffer_data [3*i + 1] = green;
		color_buffer_data [3*i + 2] = blue;
	}

	return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/
GLFWwindow* window;
float zoom_camera = 1;
float window_width=400;
float window_height=400;
float x_change = 0; //For the camera pan
float y_change = 0; //For the camera pan
double cannon_last_time=0;
//bool button[100]={false};
map < string, bool>button;
map < int, Sprite> buckets;
list < Sprite> bricks;
list < Sprite> lasers;
vector < Sprite > mirrors;
Sprite cannon;
Sprite brickArea;
float fall_dy=1;
int score=0;
void createCannon();
bool shot=false;
float cannon_color_shot, cannon_back_x, cannon_back_y;


/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void mousescroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset==-1) { 
        zoom_camera /= 1.1; //make it bigger than current size
    }
    else if(yoffset==1){
        zoom_camera *= 1.1; //make it bigger than current size
    }
    if (zoom_camera<=0.5) {
        zoom_camera = 0.5;
    }
    if (zoom_camera>=4) {
        zoom_camera=4;
    }
    if(x_change-400.0f/zoom_camera<-400)
        x_change=-400+400.0f/zoom_camera;
    else if(x_change+400.0f/zoom_camera>400)
        x_change=400-400.0f/zoom_camera;
    if(y_change-400.0f/zoom_camera<-400)
        y_change=-400+400.0f/zoom_camera;
    else if(y_change+400.0f/zoom_camera>400)
        y_change=400-400.0f/zoom_camera;
    Matrices.projection = glm::ortho((float)(-400.0f/zoom_camera+x_change), (float)(400.0f/zoom_camera+x_change), (float)(-400.0f/zoom_camera+y_change), (float)(400.0f/zoom_camera+y_change), 0.1f, 500.0f);
}
void check_pan(){
    if(x_change-400.0f/zoom_camera<-400)
        x_change=-400+400.0f/zoom_camera;
    else if(x_change+400.0f/zoom_camera>400)
        x_change=400-400.0f/zoom_camera;
    if(y_change-400.0f/zoom_camera<-400)
        y_change=-400+400.0f/zoom_camera;
    else if(y_change+400.0f/zoom_camera>400)
        y_change=400-400.0f/zoom_camera;
    Matrices.projection = glm::ortho((float)(-400.0f/zoom_camera+x_change), (float)(400.0f/zoom_camera+x_change), (float)(-400.0f/zoom_camera+y_change), (float)(400.0f/zoom_camera+y_change), 0.1f, 500.0f);

}
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Function is called first on GLFW_PRESS.
	if (action == GLFW_RELEASE) {
		switch (key) {
			
			case GLFW_KEY_A:
				button["A"]=false;
				break;
			case GLFW_KEY_D:
				button["D"]=false;
				break;
			case GLFW_KEY_W:
				button["W"]=false;
				break;
			case GLFW_KEY_S:
				button["S"]=false;
				break;
			case GLFW_KEY_LEFT_CONTROL:
				button["CTRL"]=false;
				break;
			case GLFW_KEY_LEFT_ALT:
				button["ALT"]=false;
				break;
			case GLFW_KEY_LEFT:
				button["LEFT"]=false;
				break;
			case GLFW_KEY_RIGHT:
				button["RIGHT"]=false;
				break;
			case GLFW_KEY_UP:
				button["UP"]=false;
				break;
			case GLFW_KEY_DOWN:
				button["DOWN"]=false;
				break;

			case GLFW_KEY_SPACE:
				createLaser();
				break;
			default:
				break;
		}
	}
	else if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				quit(window);
				break;
			case GLFW_KEY_A:
				button["A"]=true;
				
				break;
			case GLFW_KEY_D:
				button["D"]=true;
				break;
			case GLFW_KEY_W:
				button["W"]=true;
				break;
			case GLFW_KEY_S:
				button["S"]=true;
				break;
			case GLFW_KEY_N:
				DecreaseLevel();
				break;
			case GLFW_KEY_M:
				IncreaseLevel();
				break;
			case GLFW_KEY_LEFT_CONTROL:
				button["CTRL"]=true;
				break;
			case GLFW_KEY_LEFT_ALT:
				button["ALT"]=true;
				break;
			case GLFW_KEY_LEFT:
				button["LEFT"]=true;
				if(!button["CTRL"]&&!button["ALT"]){
					x_change-=10;
                	check_pan();
				}
				break;
			case GLFW_KEY_RIGHT:
				button["RIGHT"]=true;
				if(!button["CTRL"]&&!button["ALT"]){
					x_change+=10;
                	check_pan();
				}
				break;
			case GLFW_KEY_UP:
				button["UP"]=true;
				if(!button["CTRL"]&&!button["ALT"]){
					mousescroll(window,0,+1);
                	check_pan();
				}
				break;
			case GLFW_KEY_DOWN:
				button["DOWN"]=true;
				if(!button["CTRL"]&&!button["ALT"]){
					mousescroll(window,0,-1);
                	check_pan();
				}
				break;
			case GLFW_KEY_SPACE:
				break;

			default:
				break;
		}
	}


}
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (action == GLFW_PRESS) {
				mouse.clicked=1;
			}
			else if (action == GLFW_RELEASE) {
				mouse.clicked=0;
			}
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (action == GLFW_PRESS) {
				mouse.right_clicked=1;
			}
			else if (action == GLFW_RELEASE) {
				mouse.right_clicked=0;
			}
			break;
		default:
			break;
	}
}
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
			quit(window);
			break;
		default:
			break;
	}
}
void reshapeWindow (GLFWwindow* window, int width, int height)
{
	int fbwidth=width, fbheight=height;
	/* With Retina display on Mac OS X, GLFW's FramebufferSize
	   is different from WindowSize */
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
	// Perspective projection for 3D views
	// Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

	// Ortho projection for 2D views
	Matrices.projection = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, 0.1f, 500.0f);
}
void dragCannon(){
	if(mouse.pos_y <=cannon.limit_yp && mouse.pos_y >= cannon.limit_yn){
		cannon.pos_y = mouse.pos_y;
		return;
	}
	else if(mouse.pos_y >=cannon.limit_yp){
		cannon.pos_y = cannon.limit_yp;
	}
	else if(mouse.pos_y <=cannon.limit_yn){
		cannon.pos_y = cannon.limit_yn;
	}
}
bool checkMouseCannon(){
	if(mouse.pos_x<=cannon.pos_x + cannon.width/2 && mouse.pos_x>=cannon.pos_x - cannon.width/2)
		if(mouse.pos_y<=cannon.pos_y + cannon.width/2 && mouse.pos_y>=cannon.pos_y - cannon.width/2){
			// cout << "Caught Cannon" << endl;
			return true;
		}
	return false;

}
void updateCannon(){
	if(checkMouseCannon()){
		cannon.hover=true;
	}
	else{
		cannon.hover=false;
	}
	if(cannon.hover && mouse.clicked)
		cannon.choose=true;
	if(cannon.choose && mouse.clicked){
		dragCannon();
	}
	else{
		cannon.choose=false;
	}

}
void dragBucket(int num){
	if(mouse.pos_x + buckets[num].width/2 <=buckets[num].limit_xp && mouse.pos_x - buckets[num].width/2 >= buckets[num].limit_xn){
		buckets[num].pos_x = mouse.pos_x - buckets[num].width/2;
		return;
	}
	else if(mouse.pos_x + buckets[num].width/2 >=buckets[num].limit_xp){
		buckets[num].pos_x = buckets[num].limit_xp - buckets[num].width;
	}
	else if(mouse.pos_x - buckets[num].width/2<=buckets[num].limit_xn){
		buckets[num].pos_x = buckets[num].limit_xn;
	}
}
bool checkMouseBucket(int num){
	if(mouse.pos_x<=buckets[num].pos_x + buckets[num].width && mouse.pos_x>=buckets[num].pos_x )
		if(mouse.pos_y<=buckets[num].pos_y + buckets[num].height && mouse.pos_y>=buckets[num].pos_y ){
			// cout << "Caught Bucket" <<num<< endl;
			return true;
		}
	return false;

}
void updateBucket(int num){
	if(checkMouseBucket(num)){
		buckets[num].hover=true;
	}
	else{
		buckets[num].hover=false;
	}
	if(buckets[num].hover && mouse.clicked && !buckets[1-num].choose)
		buckets[num].choose=true;
	if(buckets[num].choose && mouse.clicked){
		dragBucket(num);
	}
	else{
		buckets[num].choose=false;
	}

}
void dragNozzle(){
	float dx = cannon.pos_x-mouse.pos_x;
	float dy = cannon.pos_y-mouse.pos_y;
	cannon.rotation = atan(dy/dx)*180/M_PI;
}
bool checkMouseNozzle(){
	if(mouse.pos_x>=brickArea.pos_x )
		if(mouse.pos_y>=brickArea.pos_y){
			// cout << "Caught brickArea" << endl;
			return true;
		}
	return false;
}
void updateNozzle(){
	if(checkMouseNozzle() && mouse.clicked){
		dragNozzle(); 
		createLaser();

	}
}
void mouseChecker(){
	double mouse_x_cur, mouse_y_cur;
	glfwGetCursorPos(window,&mouse_x_cur,&mouse_y_cur);
	mouse.pos_x = mouse_x_cur/600*800 -400;
	mouse.pos_y = 400 -mouse_y_cur/600*800 ;
	// cout << mouse_x_cur/600*800 -400<< ' ' << -mouse_y_cur/600*800 + 400<<endl;
	updateCannon();
	updateBucket(0);
	updateBucket(1);
	updateNozzle();
    if(mouse.right_clicked){
        x_change+=mouse.pos_x - mouse.back_x;
        y_change+=mouse.pos_y - mouse.back_y;
        check_pan();
    }
    mouse.back_x = mouse.pos_x;
    mouse.back_y = mouse.pos_y;
}

/* Executed for character input (like in text boxes) */

/* Executed when a mouse button is pressed/released */


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */

float camera_rotation_angle = 90;

void createCannon(){

	cannon.rotation=0;
	cannon.pos_x=-370;
	cannon.pos_y=0;
	cannon.dx=10;
	cannon.dy=10;
	cannon.rdx=1;
	cannon.width=80;
	cannon.height=40;
	cannon.limit_rn=-90;
	cannon.limit_rp=90;
	cannon.limit_xn=-370;
	cannon.limit_xp=-370;
	cannon.limit_yn=-350;
	cannon.limit_yp=350;
	float body_width=cannon.width;
	float body_height=cannon.height;
	float noz_width=body_width/4;
	float noz_height=body_height/4;
	float offx=body_width/2;
	float offy=body_height/2;
	float noz_place_x=body_width-offx;
	float noz_place_y=body_height/2-noz_height/2-offy;

	Point body[5];
	Point noz[5];
	Point noz1,noz2,noz3,noz4;
	body[1].x =0,body[1].y=0;
	body[2].x =body_width,body[2].y=0;
	body[3].x =body_width,body[3].y=body_height;
	body[4].x =0,body[4].y=body_height;

	noz[1].x = 0,noz[1].y=0;
	noz[2].x =noz_width,noz[2].y=0;
	noz[3].x =noz_width,noz[3].y=noz_height;
	noz[4].x =0,noz[4].y=noz_height;

	for(int i=1;i<=4;i++){
		noz[i].x+=noz_place_x;
		noz[i].y+=noz_place_y;
	}

	for(int i=1;i<=4;i++){
		body[i].x-=offx;
		body[i].y-=offy;
	}

	//create3DObject creates and returns a handle to a VAO that can be used later
	static const GLfloat vertex_buffer_data [] = {
		body[1].x,body[1].y,0,
		body[2].x,body[2].y,0,
		body[3].x,body[3].y,0,

		body[3].x,body[3].y,0,
		body[4].x,body[4].y,0,
		body[1].x,body[1].y,0,

		noz[1].x,noz[1].y,0,
		noz[2].x,noz[2].y,0,
		noz[3].x,noz[3].y,0,

		noz[3].x,noz[3].y,0,
		noz[4].x,noz[4].y,0,
		noz[1].x,noz[1].y,0

	};
	COLOR colorA;
	if(cannon.choose){
		colorA.r=0,colorA.b=1,colorA.g=1;
	}
	else{
		colorA.r=0,colorA.b=1,colorA.g=0;
	}
	static const GLfloat color_buffer_data [] = {
		colorA.r,colorA.g,colorA.b,//1
		colorA.r,colorA.g,colorA.b,//2
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//4
		colorA.r,colorA.g,colorA.b,//1
		colorA.r,colorA.g,colorA.b,//2-1
		colorA.r,colorA.g,colorA.b,//2-2
		colorA.r,colorA.g,colorA.b,//2-3
		colorA.r,colorA.g,colorA.b,//2-3
		colorA.r,colorA.g,colorA.b,//2-4
		colorA.r,colorA.g,colorA.b,//2-1
	};
	cannon.object = create3DObject(GL_TRIANGLES, 12, vertex_buffer_data, color_buffer_data, GL_FILL);;
}

void createBucket(int number){
	buckets[number].rotation=0;
	buckets[number].pos_y=-350;
	if(number==1)
		buckets[number].pos_x=-200;
	else
		buckets[number].pos_x=200;
	buckets[number].dx=10;
	buckets[number].dy=10;
	buckets[number].width=100;
	buckets[number].height=100;
	buckets[number].color=number;
	buckets[number].limit_xp=400;
	buckets[number].limit_xn=-300;
	// number = 1 color = red
	// number = 0 color = green
	float body_width = buckets[number].width;
	float body_height = buckets[number].height;
	float offx=0;
	float offy=0;

	Point body[5];
	body[1].x =0,body[1].y=0;
	body[2].x =body_width,body[2].y=0;
	body[3].x =body_width,body[3].y=body_height;
	body[4].x =0,body[4].y=body_height;

	for(int i=1;i<=4;i++){
		body[i].x-=offx;
		body[i].y-=offy;
	}

	//create3DObject creates and returns a handle to a VAO that can be used later
	const GLfloat vertex_buffer_data [] = {
		body[1].x,body[1].y,0,
		body[2].x,body[2].y,0,
		body[3].x,body[3].y,0,

		body[3].x,body[3].y,0,
		body[4].x,body[4].y,0,
		body[1].x,body[1].y,0,

	};
	COLOR colorA;
	// cout << number << endl;
	colorA.r=1-number,colorA.b=0,colorA.g=number;
	const GLfloat color_buffer_data [] = {
		colorA.r,colorA.g,colorA.b,//1
		colorA.r,colorA.g,colorA.b,//2
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//4
		colorA.r,colorA.g,colorA.b,//1
	};
	buckets[number].object = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);

}
void createBrick(){
	Sprite current_brick;
	current_brick.rotation=0;
	current_brick.pos_y=400;
	current_brick.dy=1;
	current_brick.width=10;
	current_brick.height=20;
	current_brick.active=true;
	int a = rand()%700;
	a-=300;
	current_brick.pos_x=(float)a;


	float body_width = current_brick.width;
	float body_height = current_brick.height;
	float offx=0;
	float offy=0;

	Point body[5];
	body[1].x =0,body[1].y=0;
	body[2].x =body_width,body[2].y=0;
	body[3].x =body_width,body[3].y=body_height;
	body[4].x =0,body[4].y=body_height;

	for(int i=1;i<=4;i++){
		body[i].x-=offx;
		body[i].y-=offy;
	}

	//create3DObject creates and returns a handle to a VAO that can be used later
	const GLfloat vertex_buffer_data [] = {
		body[1].x,body[1].y,0,
		body[2].x,body[2].y,0,
		body[3].x,body[3].y,0,

		body[3].x,body[3].y,0,
		body[4].x,body[4].y,0,
		body[1].x,body[1].y,0,

	};
	a = rand()%3;
	COLOR colorA;
	if(a!=2){
		// a = 1 red=1 , green = 0
		// a = 0 red=0 , green = 1
		colorA.r =  a;
		colorA.b=0;
		colorA.g = 1 -a;
		current_brick.color = 1 - a;
	}
	else{
		colorA.r=colorA.b=colorA.g=0;
		current_brick.color = 2;
	}
	//cout << colorA.r<<colorA.g<<endl;
	const GLfloat color_buffer_data [] = {
		colorA.r,colorA.g,colorA.b,//1
		colorA.r,colorA.g,colorA.b,//2
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//4
		colorA.r,colorA.g,colorA.b,//1
	};
	current_brick.object = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
	try{
		bricks.push_back(current_brick);
	}
	catch(std::bad_alloc& eN ){
		cout << "cannot create brick"<<endl;
	}
}

void createMirror(float x,float y,float length,float rotation){
	Sprite newMirr;
	newMirr.pos_x=x;
	newMirr.pos_y=y;
	newMirr.length=length;
	newMirr.rotation=rotation;
	newMirr.width=length;
	newMirr.height=10;


	float body_width = newMirr.width;
	float body_height = newMirr.height;
	float offx=newMirr.width/2;
	float offy=0;

	Point body[5];
	Point body2[5];
	body2[1].x = body[1].x =0, body2[1].y = body[1].y=0;
	body2[2].x = body[2].x =body_width, body2[2].y =  body[2].y=0;
	body2[3].x = body[3].x =body_width, body2[3].y =  body[3].y=body_height;
	body2[4].x = body[4].x =0, body2[4].y =  body[4].y=body_height;

	for(int i=1;i<=4;i++){
		body[i].x-=offx;
		body[i].y-=offy;
		body2[i].x-=offx;
		body2[i].y-=newMirr.height;
	}

	//create3DObject creates and returns a handle to a VAO that can be used later
	const GLfloat vertex_buffer_data [] = {
		body[1].x,body[1].y,0,
		body[2].x,body[2].y,0,
		body[3].x,body[3].y,0,

		body[3].x,body[3].y,0,
		body[4].x,body[4].y,0,
		body[1].x,body[1].y,0,

		body2[1].x,body2[1].y,0,
		body2[2].x,body2[2].y,0,
		body2[3].x,body2[3].y,0,

		body2[3].x,body2[3].y,0,
		body2[4].x,body2[4].y,0,
		body2[1].x,body2[1].y,0,
	};
	COLOR colorA;
	colorA.r=(float)98/255;
	colorA.g=(float)114/255;
	colorA.b=(float)132/155;
	COLOR colorB;
	colorB.r=(float)138/255;
	colorB.g=(float)89/255;
	colorB.b=(float)45/155;
	//cout << colorA.r<<colorA.g<<endl;
	const GLfloat color_buffer_data [] = {
		colorA.r,colorA.g,colorA.b,//1
		colorA.r,colorA.g,colorA.b,//2
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//4
		colorA.r,colorA.g,colorA.b,//1
		colorB.r,colorB.g,colorB.b,//1
		colorB.r,colorB.g,colorB.b,//2
		colorB.r,colorB.g,colorB.b,//3
		colorB.r,colorB.g,colorB.b,//3
		colorB.r,colorB.g,colorB.b,//4
		colorB.r,colorB.g,colorB.b,//1
	};
	newMirr.object = create3DObject(GL_TRIANGLES, 12, vertex_buffer_data, color_buffer_data, GL_FILL);
	mirrors.push_back(newMirr);
}

void createLaser(){
	double time_now = glfwGetTime();
	if(time_now - cannon_last_time <= 1)
		return;
	shot=true;
	cannon_back_x = cannon.pos_x, cannon_back_y = cannon.pos_y;
	cannon_last_time= time_now;
	Sprite laser;
	laser.rotation=cannon.rotation;
	laser.pos_y=cannon.pos_y;
	laser.pos_x=cannon.pos_x;
	laser.dy=10;
	laser.width=cannon.height/4;
	laser.height=cannon.height/4;
	laser.speed=20;
	laser.active=true;

	float body_width = laser.width;
	float body_height = laser.height;
	float offx=laser.height/2;
	float offy=laser.height/2;

	Point body[5];
	body[1].x =0,body[1].y=0;
	body[2].x =body_width,body[2].y=0;
	body[3].x =body_width,body[3].y=body_height;
	body[4].x =0,body[4].y=body_height;

	for(int i=1;i<=4;i++){
		body[i].x-=offx;
		body[i].y-=offy;
	}

	//create3DObject creates and returns a handle to a VAO that can be used later
	const GLfloat vertex_buffer_data [] = {
		body[1].x,body[1].y,0,
		body[2].x,body[2].y,0,
		body[3].x,body[3].y,0,

		body[3].x,body[3].y,0,
		body[4].x,body[4].y,0,
		body[1].x,body[1].y,0,

	};
	COLOR colorA;
	colorA.r=colorA.b=colorA.g=1;
	//cout << colorA.r<<colorA.g<<endl;
	const GLfloat color_buffer_data [] = {
		colorA.r,colorA.g,colorA.b,//1
		colorA.r,colorA.g,colorA.b,//2
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//3
		colorA.r,colorA.g,colorA.b,//4
		colorA.r,colorA.g,colorA.b,//1
	};
	laser.object = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
	try{
		lasers.push_back(laser);
	}
	catch(std::bad_alloc& eN ){
		cout << "cannont create laser"<<endl;

	}
}

void createBrickArea(){
	brickArea.rotation=0;
	brickArea.pos_y=-250;
	brickArea.pos_x=-300;
	brickArea.dy=10;
	brickArea.width = 400 - brickArea.pos_x;
	brickArea.height= 400 - brickArea.pos_y;
	brickArea.speed=0;
}

void Laser(){
	for(list<Sprite>::iterator it=lasers.begin(); it!=lasers.end(); ++it){
		if(it->active==1){
			int dx=it->speed*cos((it->rotation*M_PI)/180);
			int dy=it->speed*sin((it->rotation*M_PI)/180);
			it->pos_x+=dx;
			it->pos_y+=dy;
		}
		if(it->pos_y>=400 || it->pos_y<=-400 || it->pos_x>=400 || it->pos_x <= -400){
			it->active=false;
			it->pos_x=2000;
			it->pos_y=2000;
		}
	}
}
/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw ()
{
	// clear the color and depth in the frame buffer


	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the loaded shader program
	// Don't change unless you know what you are doing
	glUseProgram (programID);

	// Eye - Location of camera. Don't change unless you are sure!!
	glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	// Target - Where is the camera looking at.  Don't change unless you are sure!!
	glm::vec3 target (0, 0, 0);
	// Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
	glm::vec3 up (0, 1, 0);

	// Compute Camera matrix (view)
	// Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
	//  Don't change unless you are sure!!
	Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	//  Don't change unless you are sure!!
	glm::mat4 VP = Matrices.projection * Matrices.view;

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	//  Don't change unless you are sure!!
	glm::mat4 MVP;	// MVP = Projection * View * Model



	// Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
	// glPopMatrix ();
	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translateCannon = glm::translate (glm::vec3(cannon.pos_x, cannon.pos_y, 0));        // glTranslatef
	glm::mat4 rotateCannon = glm::rotate((float)(cannon.rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
	Matrices.model *= (translateCannon * rotateCannon);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	draw3DObject(cannon.object);

	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translatebucketR = glm::translate (glm::vec3(buckets[0].pos_x, buckets[0].pos_y, 0));        // glTranslatef
	glm::mat4 rotatebucketR = glm::rotate((float)(buckets[0].rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
	Matrices.model *= (translatebucketR * rotatebucketR);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	draw3DObject(buckets[0].object);


	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translateBucketG = glm::translate (glm::vec3(buckets[1].pos_x, buckets[1].pos_y, 0));        // glTranslatef
	glm::mat4 rotateBucketG = glm::rotate((float)(buckets[1].rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
	Matrices.model *= (translateBucketG * rotateBucketG);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	draw3DObject(buckets[1].object);

	for(std::list<Sprite>::iterator it=lasers.begin(); it!=lasers.end() && !lasers.empty(); ++it){
		if(it->active){

			Matrices.model = glm::mat4(1.0f);

			glm::mat4 translateLaser = glm::translate (glm::vec3(it->pos_x, it->pos_y, 0));        // glTranslatef
			glm::mat4 rotateLaser = glm::rotate((float)(it->rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
			Matrices.model *= (translateLaser * rotateLaser);
			MVP = VP * Matrices.model;
			glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

			draw3DObject(it->object);
		}
	}
	for(std::vector<Sprite>::iterator it=mirrors.begin(); it!=mirrors.end(); ++it){
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 translateBucketG = glm::translate (glm::vec3(it->pos_x, it->pos_y, 0));        // glTranslatef
		glm::mat4 rotateBucketG = glm::rotate((float)(it->rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
		Matrices.model *= (translateBucketG * rotateBucketG);
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// draw3DObject draws the VAO given to it using current MVP matrix
		draw3DObject(it->object);
	}

	// draw3DObject draws the VAO given to it using current MVP matrix

	for(std::list<Sprite>::iterator it=bricks.begin(); it!=bricks.end(); ++it){
		if(it->pos_y<-400){
			it->active=false;
		}
		if(it->active){
			Matrices.model = glm::mat4(1.0f);
			glm::mat4 translateBucketG = glm::translate (glm::vec3(it->pos_x, it->pos_y, 0));        // glTranslatef
			glm::mat4 rotateBucketG = glm::rotate((float)(it->rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
			Matrices.model *= (translateBucketG * rotateBucketG);
			MVP = VP * Matrices.model;
			glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
			draw3DObject(it->object);
		}
	}
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
	// window desciptor/handle

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		//        exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

	if (!window) {
		glfwTerminate();
		//        exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );

	/* --- register callbacks with GLFW --- */

	/* Register function to handle window resizes */
	/* With Retina display on Mac OS X GLFW's FramebufferSize
	   is different from WindowSize */
	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);

	/* Register function to handle window close */
	glfwSetWindowCloseCallback(window, quit);

	/* Register function to handle keyboard input */
	glfwSetKeyCallback(window, keyboard);      // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

	/* Register function to handle mouse click */
	glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
	glfwSetScrollCallback(window, mousescroll); // mouse scroll

	return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height){
	/* Objects should be created before any other gl function and shaders */
	// Create the models
	//createTriangle (); // Generate the VAO, VBOs, vertices data & copy into the array buffer

	createCannon ();
	createBucket (0);
	createBucket (1);
	createMirror ( -200,-200,150,45);
	createMirror ( -100,200,150,45);
	createMirror ( 200,0,150,135);
	createBrickArea ();
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}
bool reflected=false;
int colliBuckBrick(Sprite buck,list<Sprite>::iterator it){
	if( it->pos_y + it->height <= buck.pos_y + buck.height && it->pos_y>= buck.pos_y + buck.height/5 ){

		if( it->pos_x + it->width <= buck.pos_x+buck.width && it->pos_x >= buck.pos_x ){
			if(it->color==2){
				return -1;
			}
			else if(it->color==buck.color){
				return 2;
			}
			else if(it->color!=buck.color){
				return 1;
			}
		}
	}
	return 0;
}
bool colliLaserBrick(list<Sprite>::iterator it, list<Sprite>::iterator laser)
{
	Point l1,l2,r1,r2;
	l1.x=laser->pos_x;
	l1.y=laser->pos_y + laser->height;
	r1.x = laser->pos_x + laser->width ;
	r1.y = laser->pos_y;
	l2.x = it->pos_x;
	l2.y=it->pos_y + it->height;
	r2.x = it->pos_x + it->width ;
	r2.y = it->pos_y;
	if (l1.x > r2.x || l2.x > r1.x)
		return false;
	if (l1.y < r2.y || l2.y < r1.y)
		return false;

	return true;
}


void LaserReflect(vector<Sprite>::iterator it, list<Sprite>::iterator laser){
	Point vectord, vectorn;
	float theta = it->rotation*M_PI/180;

	vectorn.x=-sin(theta);
	vectorn.y=cos(theta);

	theta = laser->rotation*M_PI/180;
	vectord.x=cos(theta);
	vectord.y=sin(theta);

	float  dot = vectord.x*vectorn.x + vectord.y*vectorn.y;

	Point final;
	final.x = vectord.x - 2*dot*vectorn.x;  
	final.y = vectord.y - 2*dot*vectorn.y;
	//  cout << laser->rotation <<endl;
	//laser->rotation = 180+atan(final.y/final.x)*180/M_PI; 
	laser->rotation = 2*it->rotation - laser->rotation;
	// cout << laser->rotation<<endl;

	// laser.pos_x += it->height*vectorn.x;
	// laser.pos_y += it->height*vectorn.y; 
	reflected=true;
	// cout <<"Set"<<endl;

}
bool colliLaserMirror(vector<Sprite>::iterator it, list<Sprite>::iterator laser)
{
	for(int i=-it->length/2+1 + it->height;i<it->length/2;i+=it->height){
		float d = (it->pos_y+i*sin(it->rotation*M_PI/180)-laser->pos_y)*(it->pos_y+i*sin(it->rotation*M_PI/180)-laser->pos_y)
			+(it->pos_x+i*cos(it->rotation*M_PI/180)-laser->pos_x)*(it->pos_x+i*cos(it->rotation*M_PI/180)-laser->pos_x);
		d = sqrt(d);
		float theta = it->rotation*M_PI/180;
		float cx = abs(d*cos(theta));
		float cy = abs(d*sin(theta));
		//float theta = laser.rotation*M_PI/180;
		float fi = laser->rotation;
		theta = it->rotation;

		if(cy <=it->height  && theta-fi>0){
			// cout << theta-fi<<endl;  
			return true;
		}
		else if(cy <=it->height&& theta-fi<=0){
			laser->active = false;
		}

	}
	return false;
}

void bricksQueueReduce(){
	Sprite top;
	if(!bricks.empty())
		top = bricks.front();
	else
		return;
	while(top.active==false && !bricks.empty()){
		// cout <<"Pushed"<<endl;
		bricks.pop_front();
		if(!bricks.empty())
			top = bricks.front();
		else
			break;
	}
}

void lasersQueueReduce(){
	Sprite top;
	if(!lasers.empty())
		top = lasers.front();
	else
		return;
	while(top.active==false && !lasers.empty()){
		// cout <<"Pushed"<<endl;
		lasers.pop_front();
		if(!lasers.empty())
			top = lasers.front();
		else
			break;
	}
}
int  collosionChecker(){
	for(std::list<Sprite>::iterator it=bricks.begin(); it!=bricks.end(); ++it){
		if(it->active){

			int p1 = colliBuckBrick(buckets[0],it);
			int p2 = colliBuckBrick(buckets[1],it);
			if(p1==-1 || p2==-1){
				it->active = false;
				it->pos_x=1000;
				it->pos_y=1000;
				cout << "Game Over"<<endl;
				return -1;
			}
			if(p1==2 && p2==0){
				it->pos_x=1000;
				it->pos_y=1000;
				it->active = false;
				scoreUpdate(10);
			}
			if(p2==2 && p1==0){
				it->pos_x=1000;
				it->pos_y=1000;
				it->active = false;
				scoreUpdate(10);
			}
			if(p1==2 && p2==1){
				it->pos_x=1000;
				it->pos_y=1000;
				it->active = false;
			}
			if(p2==2 && p1==1){
				it->pos_x=1000;
				it->pos_y=1000;
				it->active = false;
			}
		}
		//cout << p1 << ' ' << p2 << endl;
	}
}
int  shootChecker(){
	for(std::list<Sprite>::iterator it=bricks.begin(); it!=bricks.end(); ++it){
		for(std::list<Sprite>::iterator laser=lasers.begin(); laser!=lasers.end() && !lasers.empty(); ++laser){

			if(it->active ){
				bool p1 = colliLaserBrick(it,laser);
				if(p1){
					it->active = false;
					laser->active = false;
					laser->pos_x=2000;
					laser->pos_y=2000;
					//cout << p1 << endl;
					if(it->color==2)
						scoreUpdate(20);
					if(it->color==0 || it->color == 1)
						scoreUpdate(-2);
				}
			}

		}

	}

}
int  mirrorChecker(){
	for(std::vector<Sprite>::iterator it=mirrors.begin(); it!=mirrors.end(); ++it){
		for(std::list<Sprite>::iterator laser=lasers.begin(); laser!=lasers.end(); ++laser){
			if(laser->active==true){
				bool p1 = colliLaserMirror(it, laser);
				if(p1){
					//cout <<"Yes"<<endl;
					// if(!reflected)
					LaserReflect(it, laser);
					// else
					// cout <<"Did Not Reflect"<<endl;
				}
			}
		}
	}
}

void gameEngine(){
	fall_bricks();
	bricksQueueReduce();
	Laser();
	lasersQueueReduce();
	collosionChecker();
	shootChecker();
	mirrorChecker();
	mouseChecker();
	recoilCannon();
	//asd

	if(button["A"]){
		SpriteRotateUp(&cannon);

	}
	if(button["D"]){
		SpriteRotateDown(&cannon);

	}
	if(button["W"]){
		SpriteMoveUp(&cannon);

	}
	if(button["S"]){
		SpriteMoveDown(&cannon);

	}

	if(button["CTRL"]&&button["LEFT"]){
		SpriteMoveLeft(&buckets[0]);
	}
	if(button["CTRL"]&&button["RIGHT"]){
		SpriteMoveRight(&buckets[0]);
	}
	if(button["ALT"]&&button["LEFT"]){
		SpriteMoveLeft(&buckets[1]);
	}
	if(button["ALT"]&&button["RIGHT"]){
		SpriteMoveRight(&buckets[1]);
	}
}
int main (int argc, char** argv)
{
	cannon.choose=false;

	int width = 600;
	int height = 600;

	GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);

	double last_update_time = glfwGetTime(), current_time;
	double last_update_time2 = glfwGetTime(), current_time2;

	/* Draw in loop */
	while (!glfwWindowShouldClose(window)) {
		gameEngine();
		// OpenGL Draw commands
		draw();

		// Swap Frame Buffer in double buffering
		glfwSwapBuffers(window);

		// Poll for Keyboard and mouse events
		glfwPollEvents();

		// Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
		current_time = glfwGetTime(); // Time in seconds
		if ((current_time - last_update_time) >= 1) { // atleast 0.5s elapsed since last frame
			// do something every 0.5 seconds ..
			createBrick();

			last_update_time = current_time;
		}

		current_time2 = glfwGetTime(); // Time in seconds


	}

	glfwTerminate();
	//    exit(EXIT_SUCCESS);
}
