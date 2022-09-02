//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int Axis = Xaxis;
GLfloat Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat The[NumAxes] = { 0.0, 0.0, 0.0 };
mat4 ctm;
GLfloat theta_Matrix[5] = { 0,0,0 };
GLuint loc_modelMatrix;
GLuint projection_loc;
GLuint loc_model;
point4 new_points[NumPoints];
GLuint quay;
mat4 instance;
mat4 instance1;
mat4 instance2;
mat4 instance_ban;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}

vec4 mau_day_tau = vec4(0, 1.0, 1.0, 1.0);
vec4 mau_than_tau = vec4(0, 1.0, 0, 1.0);
vec4 mau_cot_tau = vec4(0, 0.2, 0.2, 1.0);
vec4 mau_mai_che = vec4(0.75, 0, 0.75, 1.0);
vec4 mau_de_co = vec4(1.0, 1.0, 1.0, 1.0);
vec4 mau_cot_co = vec4(1.0, 1.0, 0, 1.0);
vec4 mau_la_co = vec4(1.0, 1.0, 0, 1.0);
void initGPUBuffers( void )
{
	// Tạo một VAO - vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );     
    glBindVertexArray( vao );

    // Tạo và khởi tạo một buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors); 
	
}


void shaderSetup( void )
{
	// Nạp các shader và sử dụng chương trình shader
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );   // hàm InitShader khai báo trong Angel.h
    glUseProgram( program );

    // Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
    GLuint loc_vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc_vPosition );
    glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	//modelMatrix = glGetUniformLocation(program, "model");
	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	loc_modelMatrix = glGetUniformLocation(program, "modelMatrix");
	projection_loc = glGetUniformLocation(program, "projection");
	glEnable(GL_DEPTH_TEST);
    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}
GLfloat l = -1.0, r = 1.0, tp = 1.0, bt = -1.0, zNear = 0.5, zFar = 3.0;
GLfloat radius = 1, thetal = 0, phi = 0;
GLfloat dr = 5 * M_PI / 180;
GLfloat theta[3] = { 0,0,0 };
GLfloat quayCanh = 0;

mat4 ins;
mat4 quayBase;
void MatPhang(GLfloat x, GLfloat y, GLfloat z, mat4 mt) {
	point4 eye(radius * sin(thetal) * cos(phi),
		radius * sin(thetal) * sin(phi),
		radius * cos(thetal),
		1.0);
	point4 at(0, 0, 0, 1.0);
	vec4 up(0, 1, 0, 1.0);
	mat4 v = LookAt(eye, at, up);

	mat4 in = Scale(x, y, z);
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, v * quayBase * mt * in);
	mat4 p = Frustum(l, r, bt, tp, zNear, zFar);

	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
#pragma region Nhà
/*------------------------Tường---------------------------------- -*/
void tuong1_3(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 ins = Translate(x, y, z);

	MatPhang(2.5, 0.02, 2.5, ins);
}
void tuong2_4(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 ins = Translate(x, y, z);

	MatPhang(0.02, 2.28, 2.5, ins);
}
void tuongSau(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 ins = Translate(x, y, z);

	MatPhang(2.54, 2.28, 0.02, ins);
}
#pragma endregion

GLfloat t;
GLfloat t1;
GLfloat t2;
GLfloat t3;
GLfloat t4;
GLfloat t5;
GLfloat t6;
GLfloat t7;
GLfloat t8;

GLfloat a = 0.6;//X
GLfloat b = 0.1;//Y

#pragma region T Anh
void than_thuyen1(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.32, 0.02, 0.02, instance);
}

void than_thuyen2(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.36, 0.02, 0.05, instance);
}

void than_thuyen3(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.31, 0.02, 0.08, instance);
	instance = Translate(w, l, h);
	MatPhang(0.33, 0.02, 0.07, instance);
	instance = Translate(w, l, h);
	MatPhang(0.35, 0.02, 0.06, instance);
	instance = Translate(w, l, h);
	MatPhang(0.37, 0.02, 0.05, instance);
	instance = Translate(w, l, h);
	MatPhang(0.39, 0.02, 0.04, instance);
	instance = Translate(w, l, h);
	MatPhang(0.41, 0.02, 0.03, instance);
	instance = Translate(w, l, h);
	MatPhang(0.43, 0.02, 0.02, instance);
	instance = Translate(w, l, h);
	MatPhang(0.46, 0.02, 0.01, instance);
}

void than_thuyen4(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.34, 0.02, 0.11, instance);
	instance = Translate(w, l, h);
	MatPhang(0.36, 0.02, 0.1, instance);
	instance = Translate(w, l, h);
	MatPhang(0.38, 0.02, 0.09, instance);
	instance = Translate(w, l, h);
	MatPhang(0.4, 0.02, 0.08, instance);
	instance = Translate(w, l, h);
	MatPhang(0.42, 0.02, 0.07, instance);
	instance = Translate(w, l, h);
	MatPhang(0.44, 0.02, 0.06, instance);
	instance = Translate(w, l, h);
	MatPhang(0.46, 0.02, 0.05, instance);
	instance = Translate(w, l, h);
	MatPhang(0.48, 0.02, 0.04, instance);
	instance = Translate(w, l, h);
	MatPhang(0.5, 0.02, 0.03, instance);
	instance = Translate(w, l, h);
	MatPhang(0.52, 0.02, 0.02, instance);
	instance = Translate(w, l, h);
	MatPhang(0.55, 0.02, 0.01, instance);
}

void than_thuyen5(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.37, 0.02, 0.14, instance);
	instance = Translate(w, l, h);
	MatPhang(0.39, 0.02, 0.13, instance);
	instance = Translate(w, l, h);
	MatPhang(0.41, 0.02, 0.12, instance);
	instance = Translate(w, l, h);
	MatPhang(0.42, 0.02, 0.11, instance);
	instance = Translate(w, l, h);
	MatPhang(0.44, 0.02, 0.1, instance);
	instance = Translate(w, l, h);
	MatPhang(0.46, 0.02, 0.09, instance);
	instance = Translate(w, l, h);
	MatPhang(0.48, 0.02, 0.08, instance);
	instance = Translate(w, l, h);
	MatPhang(0.5, 0.02, 0.07, instance);
	instance = Translate(w, l, h);
	MatPhang(0.52, 0.02, 0.06, instance);
	instance = Translate(w, l, h);
	MatPhang(0.54, 0.02, 0.05, instance);
	instance = Translate(w, l, h);
	MatPhang(0.56, 0.02, 0.04, instance);
	instance = Translate(w, l, h);
	MatPhang(0.6, 0.02, 0.03, instance);
	instance = Translate(w, l, h);
	MatPhang(0.62, 0.02, 0.02, instance);
	instance = Translate(w, l, h);
	MatPhang(0.64, 0.02, 0.01, instance);
}


void khoang_lai(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.16, 0.1, 0.07, instance);
	instance = Translate(w, l, h);
	MatPhang(0.12, 0.12, 0.05, instance);
	instance = Translate(w, l, h);
	MatPhang(0.08, 0.14, 0.03, instance);
	instance = Translate(w, l, h);
	MatPhang(0.04, 0.16, 0.01, instance);
}

void cot_buom(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.26, 0.005, instance);
}

void la_buom1(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.14, instance);
}
void la_buom2(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.11, instance);
}
void la_buom3(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.11, instance);
}
void la_buom4(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.115, instance);
}
void la_buom5(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.12, instance);
}
void la_buom6(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.12, instance);
}
void la_buom7(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.125, instance);
}
void la_buom8(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.13, instance);
}
void la_buom9(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.13, instance);
}
void la_buom10(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.13, instance);
}
void la_buom11(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.13, instance);
}
void la_buom12(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.13, instance);
}
void la_buom13(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.125, instance);
}
void la_buom14(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.125, instance);
}
void la_buom15(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.12, instance);
}
void la_buom16(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.12, instance);
}
void la_buom17(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.115, instance);
}
void la_buom18(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.115, instance);
}
void la_buom19(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.11, instance);
}
void la_buom20(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.01, 0.11, instance);
}

void cot_phu(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.05, 0.005, instance);
}

void than_thuyen() {
	than_thuyen1(0.02, -0.51, -0.25);
	than_thuyen2(0.02, -0.49, -0.25);
	than_thuyen3(0.02, -0.47, -0.25);
	than_thuyen4(0.02, -0.45, -0.25);
	than_thuyen5(0.02, -0.43, -0.25);
}

void khoang_lai() {
	khoang_lai(0.16, -0.38, -0.25);
}
void cot_buom() {
	cot_buom(-0.025, -0.3, -0.25);
}
void la_buom() {
	la_buom1(-0.028, -0.20, -0.25);
	la_buom2(-0.031, -0.21, -0.25);
	la_buom3(-0.034, -0.22, -0.25);
	la_buom4(-0.037, -0.23, -0.25);
	la_buom5(-0.039, -0.24, -0.25);
	la_buom6(-0.041, -0.25, -0.25);
	la_buom7(-0.0425, -0.26, -0.25);
	la_buom8(-0.044, -0.27, -0.25);
	la_buom9(-0.0445, -0.28, -0.25);
	la_buom10(-0.045, -0.29, -0.25);
	la_buom11(-0.045, -0.3, -0.25);
	la_buom12(-0.0445, -0.31, -0.25);
	la_buom13(-0.044, -0.32, -0.25);
	la_buom14(-0.0435, -0.33, -0.25);
	la_buom15(-0.042, -0.34, -0.25);
	la_buom16(-0.04, -0.35, -0.25);
	la_buom17(-0.038, -0.36, -0.25);
	la_buom18(-0.036, -0.37, -0.25);
	la_buom19(-0.034, -0.38, -0.25);
	la_buom20(-0.032, -0.39, -0.25);
}
void cot_phu() {
	cot_buom(-0.1, -0.36, -0.25);
}
void buom_phu() {
	la_buom1(-0.102, -0.25, -0.25);
	la_buom2(-0.105, -0.26, -0.25);
	la_buom3(-0.108, -0.27, -0.25);
	la_buom4(-0.111, -0.28, -0.25);
	la_buom5(-0.114, -0.29, -0.25);
	la_buom6(-0.116, -0.3, -0.25);
	la_buom7(-0.117, -0.31, -0.25);
	la_buom13(-0.118, -0.32, -0.25);
	la_buom14(-0.117, -0.33, -0.25);
	la_buom15(-0.118, -0.34, -0.25);
	la_buom16(-0.116, -0.35, -0.25);
	la_buom17(-0.113, -0.36, -0.25);
	la_buom18(-0.111, -0.37, -0.25);
	la_buom19(-0.108, -0.38, -0.25);
	la_buom20(-0.105, -0.39, -0.25);
}
void thuyen_Tung_Anh() {
	than_thuyen();
	khoang_lai();
	cot_buom();
	la_buom();
	cot_phu();
	buom_phu();
}
#pragma endregion

#pragma region H Anh
/*------------------------Thuyen_HongAnh---------------------------------- -*/
void bungthuyen1(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.28f, 0.01f, 0.2, instance);
}
void bungthuyen2(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.29f, 0.01f, 0.2, instance);
}
void bungthuyen3(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.3f, 0.01f, 0.2, instance);
}
void bungthuyen4(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.31f, 0.01f, 0.2, instance);
}
void bungthuyen5(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.32f, 0.01f, 0.2, instance);
}
void bungthuyen6(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.33f, 0.01f, 0.2, instance);
}
void bungthuyen7(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.34f, 0.01f, 0.2, instance);
}
void bungthuyen8(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.35f, 0.01f, 0.2, instance);
}
void bungthuyen9(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.36f, 0.01f, 0.2, instance);
}
void bungthuyen10(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.37f, 0.01f, 0.2, instance);
}
void sauthuyen(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.1f, 0.1f, 0.2, instance);
}
void bacthuyen1(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.1f, 0.03f, 0.2, instance);
}
void bacthuyen2(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.08f, 0.03f, 0.2, instance);
}
void bacthuyen3(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.06f, 0.03f, 0.2, instance);
}
void ongkhoi1(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.02f, 0.07f, 0.04, instance);
}
void thuyen_HongAnh() {
	bungthuyen1(-0.5, -0.07, -0.3);
	bungthuyen2(-0.5, -0.06, -0.3);
	bungthuyen3(-0.5, -0.05, -0.3);
	bungthuyen4(-0.5, -0.04, -0.3);
	bungthuyen5(-0.5, -0.03, -0.3);
	bungthuyen6(-0.5, -0.02, -0.3);
	bungthuyen7(-0.5, -0.01, -0.3);
	bungthuyen8(-0.5, 0, -0.3);
	bungthuyen9(-0.5, 0.01, -0.3);
	bungthuyen10(-0.5, 0.02, -0.3);
	sauthuyen(-0.4, 0.07, -0.3);
	bacthuyen1(-0.5, 0.035, -0.3);
	bacthuyen2(-0.49, 0.065, -0.3);
	bacthuyen3(-0.48, 0.095, -0.3);
	ongkhoi1(-0.42, 0.15, -0.3);
	ongkhoi1(-0.38, 0.15, -0.3);

}
#pragma endregion

GLfloat trans = 0;
#pragma region Chính
void BottomShip()
{
	instance = Translate(0, trans, 0) * Translate(0.57, -0.44, 0.35) * Translate(0.0, -0.01, 0.0) * RotateY(theta_Matrix[3]);
	MatPhang(0.5, 0.024, 0.1, instance);//dayduoi1
	instance = instance * Translate(0, 0, 0) * Translate(-0.258, 0.07, 0.0) * RotateZ(-45) * RotateY(45);
	MatPhang(0.11, 0.11, 0.11, instance);//dayduoi2
	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.0, 0.07, 0.07) * RotateX(110);
	MatPhang(0.5, 0.01, 0.16, instance);//daytruoc
	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.0, 0.07, -0.07) * RotateX(-110);
	MatPhang(0.5, 0.01, 0.16, instance);//daysau
	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.0, 0.14, 0.0);
	MatPhang(0.5, 0.01, 0.17, instance);//daytren

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.25, 0.1, 0.07 / 2) * RotateY(-45);
	MatPhang(0.09, 0.09, 0.09, instance);//mui day truoc 1
	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.25, 0.1, -0.07 / 2) * RotateY(-45);
	MatPhang(0.09, 0.09, 0.09, instance);//mui day truoc 2
	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.265, 0.09, 0) * RotateY(-45);
	MatPhang(0.1, 0.08, 0.1, instance);//mui day truoc 3

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.25, 0.07, 0);
	MatPhang(0.01, 0.14, 0.1, instance);//mui day sau1
	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.25, 0.07, -0.018) * RotateX(-110);
	MatPhang(0.01, 0.09, 0.15, instance);//mui day sau2
	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.25, 0.07, 0.018) * RotateX(110);
	MatPhang(0.01, 0.09, 0.15, instance);//mui day sau3

	//chan vit 
	for (int i = 0;i <= 90;i++)
	{
		instance = RotateZ(-8) * Translate(0, trans, 0) * Translate(0.62, -0.35, 0.35) * Translate(0.29, 0.03, 0) * RotateX(i) * RotateY(theta_Matrix[3]);
		MatPhang(0.05, 0.02, 0.02, instance);
		
		instance =  RotateZ(-8) * Translate(0, trans, 0) * Translate(0.62, -0.35, 0.35) * Translate(0.3, 0.03, 0) * RotateX(theta_Matrix[4]);
		MatPhang(0.006, 0.04, 0.14, instance);

		instance = RotateZ(-8) * Translate(0, trans, 0) * Translate(0.62, -0.35, 0.35) * Translate(0.3, 0.03, 0) * RotateX(90) * RotateX(theta_Matrix[4]);
		MatPhang(0.006, 0.04, 0.14, instance);
	}
}

void MiddleShip()
{
	//2 cạnh bên thân tàu
	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.0, 0.15, 0.09) * RotateX(90);
	MatPhang(0.5, 0.01, 0.04, instance);//viền trước

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.0, 0.15, -0.09) * RotateX(-90);
	MatPhang(0.5, 0.01, 0.06, instance);//viền sau

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.33 + 0.07 / 2, 0.16, 0.07 / 2 + 0.01) * RotateY(-45);
	MatPhang(0.115, 0.08, 0.01, instance);//viền trai' 1

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.33 + 0.07 / 2, 0.16, -0.07 / 2 - 0.01) * RotateY(45);
	MatPhang(0.115, 0.08, 0.01, instance);//viền trai' 2

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.26, 0.14, 0.0);
	MatPhang(0.01, 0.06, 0.16, instance);//viền phải 

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.15, 0.3, 0.0);
	MatPhang(0.15, 0.3, 0.15, instance);

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.15, 0.22, 0.0);
	MatPhang(0.18, 0.05, 0.18, instance);////tang1

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.15, 0.315, 0.0);
	MatPhang(0.17, 0.049, 0.17, instance);//tang 2

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.15, 0.4, 0.0);
	MatPhang(0.16, 0.048, 0.16, instance);//tang 3

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.125, 0.47, 0.0);
	MatPhang(0.05, 0.048, 0.1, instance);//tang dinh
}

void Funnel()
{
	for (int i = 0; i <= 90; i++)
	{
		instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.26, 0.25, 0.0) * RotateY(i);
		MatPhang(0.006, 0.3, 0.006, instance);
		instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.26, 0.4, 0.0) * RotateY(i);
		MatPhang(0.015, 0.015, 0.015, instance);

		instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.26, 0.38, 0.0) * RotateX(90) * RotateY(i);
		MatPhang(0.006, 0.15, 0.006, instance);

		instance = RotateZ(-1) * Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.18, 0.48, 0.02) * RotateY(i);
		MatPhang(0.025, 0.1, 0.025, instance);//ong khoi 1

		instance = RotateZ(-1) * Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(0.18, 0.48, -0.02) * RotateY(i);
		MatPhang(0.025, 0.1, 0.025, instance);//ong khoi 2
	}
}

void Container() {
	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.13, 0.18, -0.038);
	MatPhang(0.12, 0.07, 0.07, instance);

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.13, 0.18, 0.038);
	MatPhang(0.12, 0.07, 0.07, instance);

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.01, 0.18, -0.038);
	MatPhang(0.12, 0.07, 0.07, instance);

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.01, 0.18, 0.038);
	MatPhang(0.12, 0.07, 0.07, instance);

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.13, 0.25, 0.0) * RotateY(90);
	MatPhang(0.12, 0.07, 0.07, instance);

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.05, 0.25, 0.0) * RotateY(90);
	MatPhang(0.12, 0.07, 0.07, instance);

	instance = Translate(0, trans, 0) * Translate(0.57,-0.44,0.35) * Translate(-0.10, 0.32, 0.0);
	MatPhang(0.12, 0.07, 0.07, instance);

}

void DisplayStand()
{
	//	//kệ để thuyền
	//	instance = Translate(-0.15, -0.07, 0) * RotateY(90);
	//	MatPhang(0.06, 0.1, 0.02, instance);//kedotrai
	//
	//	instance = Translate(0.15, -0.07, 0) * RotateY(90);
	//	MatPhang(0.06, 0.1, 0.02, instance);//kedophai
	//
	//	instance = Translate(0, -0.07, 0);
	//	MatPhang(0.29, 0.05, 0.01, instance);//thanhngang
	//
	////bantrungbay
	//	instance = Translate(0, -0.14, 0);
	//	MatPhang(0.899, 0.02, 0.99, instance); //matbantren
	//
	//	instance = Translate(0.45, -0.17, 0) * RotateZ(40);
	//	MatPhang(0.02, 0.08, 1.0, instance);//canhbanphai
	//
	//	instance = Translate(-0.45, -0.17, 0) * RotateZ(320);
	//	MatPhang(0.02, 0.08, 1.0, instance);//canhbantrai
	//
	//	instance = Translate(0, -0.17, -0.5) * RotateX(40);
	//	MatPhang(0.9, 0.08, 0.02, instance);//canhbansau
	//
	//	instance = Translate(0, -0.17, 0.5) * RotateX(320);
	//	MatPhang(0.9, 0.08, 0.02, instance);//canhbantruoc
	//
	//	instance = Translate(0, -0.2, 0);
	//	MatPhang(0.95, 0.01, 1.05, instance);//matbanduoi

	//chân kệ
	for (int i = 0; i <= 90; i++)
	{
		instance = Translate(0.0, 0, 0.0) * Translate(0.57, -0.44, 0.35) * Translate(0,-0.075,0) ;
		MatPhang(0.3, 0.03, 0.15,  instance);

		instance = instance * Translate(0.1,0.025,0);
		MatPhang(0.02, 0.05, 0.08, instance);

		instance = instance * Translate(-0.2, 0, 0);
		MatPhang(0.02, 0.05, 0.08, instance);

		//instance = Translate(0.0, 0.0, 0.0) * Translate(-0.3, -0.353, -0.3) * RotateY(i);
		//MatPhang(0.06, 0.3, 0.06, instance);
	}
}

void thuyen_Chinh()
{
	BottomShip();
	MiddleShip();
	Funnel();
	Container();
	DisplayStand();
}

#pragma endregion


mat4 tran;
#pragma region Hà
/*------------------------Thuyen_Ha---------------------------------- -*/
void day_tau1(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.4, 0.02, 0.2, instance);
}
void day_tau2(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.44, 0.02, 0.2, instance);
}
void day_tau3(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.48, 0.02, 0.2, instance);
}
void day_tau4(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.52, 0.02, 0.2, instance);
}
void day_tau5(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.56, 0.02, 0.2, instance);
}
void cot_tau_1(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.02, 0.04, 0.01, instance);
}
void mai_che(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.38, 0.015, 0.18, instance);
}
void de_co(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.1, 0.04, 0.1, instance);
}
void cot_co(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.01, 0.14, 0.01, instance);
}
void la_co(GLfloat w, GLfloat l, GLfloat h) {
	instance = Translate(w, l, h);
	MatPhang(0.06, 0.05, 0.001, instance);
}
void day_tau() {
	day_tau1(0.1, -0.07, -0.3);
	day_tau2(0.1, -0.05, -0.3);
	day_tau3(0.1, -0.03, -0.3);
	day_tau4(0.1, -0.01, -0.3);
	day_tau5(0.1, 0.01, -0.3);
}

void cot_tau() {
	cot_tau_1(0.2, 0.03, -0.22);
	cot_tau_1(0, 0.03, -0.22);
	cot_tau_1(0.2, 0.03, -0.38);
	cot_tau_1(0, 0.03, -0.38);
}

void mai_che() {
	mai_che(0.1, 0.06, -0.3);
}
void de_co() {
	de_co(0.1, 0.09, -0.3);
}
void cot_co() {
	cot_co(0.1, 0.14, -0.3);
}
void la_co() {
	la_co(0.13, 0.185, -0.3);
}
void thuyen_Ha() {
	day_tau();
	cot_tau();
	mai_che();
	de_co();
	cot_co();
	la_co();
}

#pragma endregion

#pragma region Giá đỡ
/*------------------------Kệ---------------------------------- -*/
void ChanGiaT(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z);
	MatPhang(0.05, 1.4, 0.5, ins);
}
void ThanhNgang1(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z);
	MatPhang(1.4, 0.05, 0.5, ins);
}

void giaDo() {
	ChanGiaT(-0.9, -0.3, -0.3);
	ChanGiaT(0.5, -0.3, -0.3);
	ThanhNgang1(-0.2, 0.375, -0.3);
	ThanhNgang1(-0.2, -0.1, -0.3);
	ThanhNgang1(-0.2, -0.55, -0.3);
	ThanhNgang1(-0.2, -0.975, -0.3);
}
#pragma endregion

#pragma region Quạt
/*------------------------Quạt---------------------------------- -*/
void mumQuat(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z);
	MatPhang(0.1f, 0.03f, 0.1, ins);
}
void trucQuat(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z) * RotateY(theta_Matrix[4]);
	MatPhang(0.05f, 0.1f, 0.05, ins);
}
void trucCanh1_3(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z);
	MatPhang(0.05f, 0.03f, 0.03, ins);
}
void trucCanh2_4(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z);
	MatPhang(0.03f, 0.03f, 0.05, ins);
}
void canhQuat1_3(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z) * RotateY(theta_Matrix[4]);
	MatPhang(0.2f, 0.03f, 0.1, ins);
}
void canhQuat2_4(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z) * RotateY(theta_Matrix[4]);
	MatPhang(0.1f, 0.03f, 0.2, ins);
}
void quatTran() {
	ins = Translate(0, 0.9, 0);
	MatPhang(0.1f, 0.03f, 0.1, ins);	
	ins = Translate(0, 0.835, 0) ;
	MatPhang(0.05f, 0.1f, 0.05, ins);
	ins = Translate(0, 0.77, 0) * RotateY(theta_Matrix[4]);
	MatPhang(0.1f, 0.03f, 0.1, ins);
	ins = ins * Translate(0.075, 0, 0);
	MatPhang(0.05f, 0.03f, 0.03, ins);
	ins = ins * Translate(-0.075, 0, 0) * Translate(-0.075, 0, 0);
	MatPhang(0.05f, 0.03f, 0.03, ins);
	ins = ins * Translate(0, 0, 0.075) * Translate(0.075, 0, 0);
	MatPhang(0.03f, 0.03f, 0.05, ins);
	ins = ins * Translate(0, 0, -0.075) * Translate(0, 0, -0.075);
	MatPhang(0.03f, 0.03f, 0.05, ins);
	ins = ins * Translate(0.19, 0, 0.075) ;
	MatPhang(0.2f, 0.03f, 0.1, ins);
	ins = ins * Translate(-0.38, 0, 0) ;
	MatPhang(0.2f, 0.03f, 0.1, ins);
	ins = ins * Translate(0.19, 0, 0.19) ;
	MatPhang(0.1f, 0.03f, 0.2, ins);
	ins = ins * Translate(0, 0, -0.38) ;
	MatPhang(0.1f, 0.03f, 0.2, ins);
}
#pragma endregion

#pragma region Bàn
/*------------------------Bàn-----------------------------------*/
void chanBan(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z);
	MatPhang(0.03f, 0.43f, 0.03, ins);
}
void matBan(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z);
	MatPhang(0.7f, 0.05f, 0.5f, ins);
}
void chanGhe(GLfloat x, GLfloat y, GLfloat z) {
	ins = Translate(x, y, z);
	MatPhang(0.03f, 0.3f, 0.03, ins);
}
void ban() {
	chanBan(0.35, -0.8, 0.5);
	chanBan(0.8, -0.8, 0.5);
	chanBan(0.8, -0.8, 0.2);
	chanBan(0.35, -0.8, 0.2);
	matBan(0.53, -0.56, 0.35);
}
#pragma endregion

#pragma region Quầy thu ngân
void quaythungan()
{
	//bàn
	ins = Translate(0, 0, 0) * Translate(-0.85, -0.3, 0.5) * Translate(-0.1, -0.4, 0.5);
	MatPhang(0.6, 0.05, 0.4, ins);//

	ins = Translate(0, 0, 0) * Translate(-0.85, -0.3, 0.5) * Translate(0.4, -0.4, 0.3);
	MatPhang(0.4, 0.05, 0.8, ins);//

	//mặt bàn sau
	ins = Translate(0, 0, 0) * Translate(-0.85, -0.3, 0.5) * Translate(0.1, -0.4, 0.7);
	MatPhang(1, 0.6, 0.02, ins);//

	//mặt bàn cạnh bên
	ins = Translate(0, 0, 0) * Translate(-0.85, -0.3, 0.5) * Translate(0.6, -0.4, 0.3);
	MatPhang(0.03, 0.6, 0.8, ins);//

	//chân bàn dưới
	ins = Translate(0, 0, 0) * Translate(-0.85, -0.3, 0.5) * Translate(0.4, -0.55, -0.1);
	MatPhang(0.4, 0.3, 0.02, ins);
	//mặt bàn trên
	ins = Translate(0, 0, 0) * Translate(-0.85, -0.3, 0.5) * Translate(0.1, -0.1, 0.6);
	MatPhang(1, 0.02, 0.2, ins);

	ins = Translate(0, 0, 0) * Translate(-0.85, -0.3, 0.5) * Translate(0.5, -0.1, 0.3);
	MatPhang(0.2, 0.02, 0.8, ins);
	//chân bàn trên
	ins = Translate(0, 0, 0) * Translate(-0.85, -0.3, 0.5) * Translate(0.5, -0.25, -0.1);
	MatPhang(0.2, 0.3, 0.02, ins);
}
#pragma endregion

void nha()
{
	tuong1_3(0, -1.01, 0.5);
	tuong1_3(0, 1.255, 0.5);
	tuong2_4(-1.255, 0.12, 0.5);
	tuong2_4(1.255, 0.12, 0.5);
	tuongSau(0, 0.12,-0.755);
	
	thuyen_Tung_Anh();
	thuyen_HongAnh();
	thuyen_Chinh();
	thuyen_Ha();
	
	giaDo();
	quatTran();
	quaythungan();
	ban();
	glutSwapBuffers();
	
}


void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	quayBase = RotateX(theta_Matrix[0]) * RotateY(theta_Matrix[1]) * RotateZ(theta_Matrix[2]);;	//xoay doi tuong
	//glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, ctm);

	//glDrawArrays( GL_TRIANGLES, 0, NumPoints );    /*Vẽ các tam giác*/
	nha();
}



void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 'x': l *= 1.1; r *= 1.1; break;
	case 'X': l *= 0.9; r *= 0.9; break;
	case 'y': bt *= 1.1; t *= 1.1; break;
	case 'Y': bt *= 0.9; t *= 0.9; break;
	case 'z': zNear *= 1.1; zFar *= 1.1; break;
	case 'Z': zNear *= 0.9; zFar *= 0.9; break;
	case 'r': radius *= 2.0; break;
	case 'R': radius *= 0.5; break;
	case 'j': thetal += dr; break;
	case 'J': thetal -= dr; break;
	case 'q': phi += dr; break;
	case 'Q': phi -= dr; break;
	case ' ': //reset values to their defaults
		l = -1.0;
		r = 1.0;
		bt = -1.0;
		t = 1.0;
		zNear = 0.2;
		zFar = 3.0;
		radius = 1.0;
		thetal = 0.0;
		phi = 0.0;
		theta_Matrix[0] = 0;
		theta_Matrix[1] = 0;
		theta_Matrix[2] = 0;
		break;
	case 'k': {
		quayCanh += 160;
		break;
	}
			//case 'b':
			//	Theta[0] += 5;
			//	if (Theta[0] > 360) Theta[0] -= 360;
			//	glutPostRedisplay();
			//	break;
			//case 'c':
			//	Theta[1] += 5;
			//	if (Theta[1] >= 360) Theta[1] -= 360;
			//	glutPostRedisplay();
			//	break;
			//case 'd':
			//	Theta[2] += 0.01;
			//	if (Theta[2] > 0.95) Theta[2] = 0;
			//	glutPostRedisplay();
			//	break;
	case 'm':
		trans += 0.05;
		if (trans > 0.25 && trans <= 2)
			trans -= 0.05;
		
		break;
	case 'M':
		trans -= 0.05;
		if (trans < 0 ) trans += 0.05;
		
		break;
	case 'a': {
		theta_Matrix[0] += 5;
		break;
	}
	case 'A': {
		theta_Matrix[0] -= 5;
		break;
	}
	case 'b': {
		theta_Matrix[1] += 5;
		break;
	}
	case 'B': {
		theta_Matrix[1] -= 5;
		break;
	}
	case 'c': {
		theta_Matrix[2] += 5;
		break;
	}
	case 'C': {
		theta_Matrix[2] -= 5;
		break;
	}
	case 'i': {
		theta_Matrix[3] += 5;
		if (theta_Matrix[3] > 360) theta_Matrix[3] -= dr;
		break;
	}
	case 't':
		t += 0.01;
		t1 += 0.01;
		t2 += 0.01;
		t3 += 0.01;
		t4 += 0.01;
		t5 += 0.01;
		t6 += 0.01;
		t7 += 0.01;
		t8 += 0.01;
		if (t1 > 1.4)
		{
			t1 = 0;
		}
		if (t2 > 1.2)
		{
			t2 = -0.2;
		}
		if (t3 > 1)
		{
			t3 = -0.4;
		}
		if (t4 > 0.75)
		{
			t4 = -0.65;
		}
		if (t5 > 0.6)
		{
			t5 = -0.8;
		}
		if (t6 > 0.5)
		{
			t6 = -0.9;
		}
		if (t7 > 0.3)
		{
			t7 = -1.1;
		}
		if (t8 >= 0.1)
		{
			t8 = -1.3;
		}
		quay += 20;
		
		break;
	case 'T':
		t -= 0.01;
		
		break;

	case '1': {
		The[0] += 5;
		
		break;
	}
	case '2': {
		The[1] += 5;
		
		break;
	}
	case '3': 
		The[2] += 5;
		break;
	
	}
	glutPostRedisplay();
}
//void mouse(int button, int state, int x, int y)
//{
//	if (state == GLUT_DOWN) {
//		switch (button) {
//		case GLUT_LEFT_BUTTON: Axis = Xaxis; break;
//		case GLUT_MIDDLE_BUTTON: Axis = Yaxis; break;
//		case GLUT_RIGHT_BUTTON: Axis = Zaxis; break;
//		}
//	}
//}

//void spinCube(void)
//{
//	Theta[Axis] += 0.01;
//	if (Theta[Axis] > 360.0) {
//		Theta[Axis] -= 360.0;
//	}
//	glutPostRedisplay();
//}

void SpinPropeller(void)
{
	theta_Matrix[4] += 10;
	if (theta_Matrix[4] > 360.0) {
		theta_Matrix[4] -= 360.0;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	// main function: program starts here

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1280,1020);
	glutInitWindowPosition(100, 0);
	glutCreateWindow("Drawing a Cube");


	glewInit();


	generateGeometry();
	initGPUBuffers();
	shaderSetup();


	glutDisplayFunc(display);

	glutKeyboardFunc(keyboard);
	glutIdleFunc(SpinPropeller);
	//glutMouseFunc(mouse);


	glutMainLoop();
	return 0;
}
