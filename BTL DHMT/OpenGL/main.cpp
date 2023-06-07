/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 892;
point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[361 * 2]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[361 * 2]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat theta[5] = { 0, 0, 0, 0, 0 };

mat4 model;
GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;



const GLfloat dr = 5;
float quayQuat = 1;
GLboolean fanSwitch = false;
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
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
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
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}

void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}
void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(0.0, 0.0, 1.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
mat4 instance, instance1, instance_nha;
mat4 model1,model2,model3,model4,model5,model6,model7;

point4 light_position(0.0, 0.0, 1.0, 0.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(0.0, 1.0, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;
color4 ambient_product;
color4 diffuse_product;
color4 specular_product;
color4 t;
void setMau(float ad, float bd, float cd) {
	material_diffuse = vec4(ad, bd, cd, 1.0);
	ambient_product = light_ambient * material_ambient;
	diffuse_product = light_diffuse * material_diffuse;
	specular_product = light_specular * material_specular;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
}

//helo
void matban(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(1, 0.5, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void chanban(GLfloat w, GLfloat h)
{
	instance1 = Scale(w, h, w);

	material_diffuse = vec4(0, 0, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void matghe1(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(1, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model3 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void changhe1(GLfloat w, GLfloat h)
{
	instance1 = Scale(w, h, w);

	material_diffuse = vec4(1, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model3 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tamdua1(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(1, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model3 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void matghe2(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(1, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void changhe2(GLfloat w, GLfloat h)
{
	instance1 = Scale(w, h, w);

	material_diffuse = vec4(1, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tamdua2(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(1, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void matghe3(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(1, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model2 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void changhe3(GLfloat w, GLfloat h)
{
	instance1 = Scale(w, h, w);

	material_diffuse = vec4(1, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model2 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tamdua3(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(1, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model2 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void bed(GLfloat w, GLfloat h, GLfloat l)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(0.5, 0.5, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void elemental(GLfloat w, GLfloat h, GLfloat l)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(0.5, 1, 0.5, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void wall(GLfloat w, GLfloat h, GLfloat l)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(1.4, 0.99, 1.64, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ngankeotu(GLfloat w, GLfloat h, GLfloat l)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(0, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model4 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void khung(GLfloat w, GLfloat h, GLfloat l)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(0, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void khung1(GLfloat w, GLfloat h, GLfloat l)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(0.5, 0.5, 1, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhtu1(GLfloat w, GLfloat h, GLfloat l)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(0, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model5 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhtu2(GLfloat w, GLfloat h, GLfloat l)
{
	instance1 = Scale(w, h, l);

	material_diffuse = vec4(0, 1, 0, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model6 * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void day()
{
	instance_nha = Translate(0.0, 0.4f, 0.9) * Scale(0.1f, 0.05f, 0.1f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model7 * instance_nha);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void trucQuat()
{
	instance_nha = Translate(0.0, 0.5f, 0.9) * Scale(0.05f, 0.2f, 0.05f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model7 * instance_nha);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhQuat1()
{

	instance_nha = Translate(0.0, 0.4f, 0.9) * RotateY(quayQuat) * Scale(0.7f, 0.01f, 0.1f);

	material_diffuse = vec4(0, 0.5, 0.8, 1);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model7 * instance_nha);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void canhQuat2()
{
	instance_nha = Translate(0.0, 0.4f, 0.9) * RotateY(quayQuat) * Scale(0.1f, 0.01f, 0.7f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model7 * instance_nha);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
//
//VẼ
void banLamViec()
{
	instance_nha = Translate(-1.74f, -0.30f, 0.00f); matban(0.4f, 1.0f, 0.02f);
	instance_nha = Translate(-1.58f, -0.488f, 0.49f); chanban(0.02f, 0.39f);
	instance_nha = Translate(-1.88f, -0.488f, 0.49f); chanban(0.02f, 0.39f);
	instance_nha = Translate(-1.88f, -0.488f, -0.49f); chanban(0.02f, 0.39f);
	instance_nha = Translate(-1.58f, -0.488f, -0.49f); chanban(0.02f, 0.39f);

	// ghe lam viec
	instance_nha = Translate(-1.4f, -0.47f, 0.0f); matghe1(0.2f, 0.2f, 0.02f);
	instance_nha = Translate(-1.29f, -0.47f, 0.09f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(-1.29f, -0.47f, -0.09f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(-1.49f, -0.573f, 0.09f); changhe1(0.02f, 0.15f);
	instance_nha = Translate(-1.49f, -0.573f, -0.09f); changhe1(0.02f, 0.15f);
	instance_nha = Translate(-1.29f, -0.31f, 0.0f); tamdua1(0.01f, 0.2f, 0.1f);
}


void ban()
{

	instance_nha = Translate(0.00f, -0.30f, 0.00f); matban(0.6f, 0.4f, 0.02f);
	instance_nha = Translate(0.29f, -0.45f, 0.19f); chanban(0.02f, 0.3f);
	instance_nha = Translate(-0.29f, -0.45f, 0.19f); chanban(0.02f, 0.3f);
	instance_nha = Translate(-0.29f, -0.45f, -0.19f); chanban(0.02f, 0.3f);
	instance_nha = Translate(0.29f, -0.45f, -0.19f); chanban(0.02f, 0.3f);


}

void ghe1()
{
	instance_nha = Translate(0.18f, -0.47f, 0.35f); matghe3(0.2f, 0.2f, 0.02f);
	instance_nha = Translate(0.275f, -0.47f, 0.44f); changhe3(0.02f, 0.3f);
	instance_nha = Translate(0.091f, -0.47f, 0.44f); changhe3(0.02f, 0.3f);
	instance_nha = Translate(0.275f, -0.537f, 0.27f); changhe3(0.02f, 0.15f);
	instance_nha = Translate(0.091f, -0.537f, 0.27f); changhe3(0.02f, 0.15f);
	instance_nha = Translate(0.18f, -0.31f, 0.43f); tamdua3(0.2f, 0.01f, 0.1f);
	instance_nha = Translate(-0.18f, -0.47f, 0.35f); matghe3(0.2f, 0.2f, 0.02f);
	instance_nha = Translate(-0.275f, -0.47f, 0.44f); changhe3(0.02f, 0.3f);
	instance_nha = Translate(-0.091f, -0.47f, 0.44f); changhe3(0.02f, 0.3f);
	instance_nha = Translate(-0.275f, -0.537f, 0.27f); changhe3(0.02f, 0.15f);
	instance_nha = Translate(-0.091f, -0.537f, 0.27f); changhe3(0.02f, 0.15f);
	instance_nha = Translate(-0.18f, -0.31f, 0.43f); tamdua3(0.2f, 0.01f, 0.1f);
}
void ghe()
{
	instance_nha = Translate(0.18f, -0.47f, -0.35f); matghe2(0.2f, 0.2f, 0.02f);
	instance_nha = Translate(0.275f, -0.47f, -0.44f); changhe2(0.02f, 0.3f);
	instance_nha = Translate(0.091f, -0.47f, -0.44f); changhe2(0.02f, 0.3f);
	instance_nha = Translate(0.275f, -0.537f, -0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(0.091f, -0.537f, -0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(0.18f, -0.31f, -0.43f); tamdua2(0.2f, 0.01f, 0.1f);
	instance_nha = Translate(-0.18f, -0.47f, -0.35f); matghe2(0.2f, 0.2f, 0.02f);
	instance_nha = Translate(-0.275f, -0.47f, -0.44f); changhe2(0.02f, 0.3f);
	instance_nha = Translate(-0.091f, -0.47f, -0.44f); changhe2(0.02f, 0.3f);
	instance_nha = Translate(-0.270f, -0.537f, -0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(-0.091f, -0.537f, -0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(-0.18f, -0.31f, -0.43f); tamdua2(0.2f, 0.01f, 0.1f);
}
void giuong()
{
	instance_nha = Translate(-0.270f, -0.660f, 2.04f); bed(0.86f, 0.1f, 0.61f);
	instance_nha = Translate(-0.730f, -0.600f, 2.04f); bed(0.06f, 0.2f, 0.612f);
	instance_nha = Translate(0.20f, -0.600f, 2.04f); bed(0.06f, 0.2f, 0.612f);
	instance_nha = Translate(-0.5500f, -0.580f, 2.16f); bed(0.2f, 0.05f, 0.18f);
	instance_nha = Translate(-0.5500f, -0.580f, 1.86f); bed(0.2f, 0.05f, 0.18f);
}
void tu()
{
	//tu sach 1
	instance_nha = Translate(-0.85f, -0.00f, 0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.10f, 0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.20f, 0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.30f, 0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.40f, 0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.50f, 0.50f); elemental(0.2f, 0.22f, 0.50f);
	instance_nha = Translate(-0.85f, -0.30f, 0.76f); elemental(0.2f, 0.6f, 0.032f);
	instance_nha = Translate(-0.85f, -0.30f, 0.58f); elemental(0.2f, 0.6f, 0.032f);
	instance_nha = Translate(-0.85f, -0.30f, 0.25f); elemental(0.2f, 0.6f, 0.015f);
	instance_nha = Translate(-0.85f, -0.30f, 0.41f); elemental(0.2f, 0.6f, 0.015f);
	instance_nha = Translate(-0.92f, -0.30f, 0.50f); elemental(0.015f, 0.6f, 0.5f);

	// tu sach 2
	instance_nha = Translate(-0.85f, -0.00f, -0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.10f, -0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.20f, -0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.30f, -0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.40f, -0.50f); elemental(0.2f, 0.02f, 0.50f);
	instance_nha = Translate(-0.85f, -0.50f, -0.50f); elemental(0.2f, 0.22f, 0.50f);
	instance_nha = Translate(-0.85f, -0.30f, -0.76f); elemental(0.2f, 0.6f, 0.032f);
	instance_nha = Translate(-0.85f, -0.30f, -0.58f); elemental(0.2f, 0.6f, 0.032f);
	instance_nha = Translate(-0.85f, -0.30f, -0.25f); elemental(0.2f, 0.6f, 0.015f);
	instance_nha = Translate(-0.85f, -0.30f, -0.41f); elemental(0.2f, 0.6f, 0.015f);
	instance_nha = Translate(-0.92f, -0.30f, -0.50f); elemental(0.015f, 0.6f, 0.5f);


}
void tuong()
{

	instance_nha = Translate(-0.00f, -0.2f, -0.99f); wall(1.99f, 1.2f, 0.02f);//tuong


	instance_nha = Translate(-2.49f, -0.2f, 0.00f); wall(0.02f, 1.2f, 1.99f);//tuong
	instance_nha = Translate(-1.74f, -0.2f, -0.99f); wall(1.49f, 1.2f, 0.02f);//tuong
	instance_nha = Translate(-1.74f, -0.2f, 2.69f); wall(1.49f, 1.2f, 0.02f);//tuong

	instance_nha = Translate(-0.00f, -0.2f, 2.69f); wall(1.99f, 1.2f, 0.02f);//tuong
	instance_nha = Translate(-2.49f, -0.2f, 1.84f); wall(0.02f, 1.2f, 1.69f);

	//san nha
	instance_nha = Translate(-0.00f, -0.800f, 0.00f); wall(2.00f, 0.02f, 2.00f);//sangiua
	instance_nha = Translate(-1.74f, -0.800f, 0.00f); wall(1.50f, 0.02f, 2.00f);//sanphongben1
	instance_nha = Translate(0.00f, -0.800f, 1.84f); wall(2.00f, 0.02f, 1.70f);//sanphongben2
	instance_nha = Translate(-1.74f, -0.800f, 1.84f); wall(1.50f, 0.02f, 1.75f);



}
void ngankeo()
{
	//day ngan
	instance_nha = Translate(-1.50f, -0.69, 1.70f); ngankeotu(0.3f, 0.02f, 0.7f);
	//canh ngan
	instance_nha = Translate(-1.65, -0.6, 1.70f); ngankeotu(0.02f, 0.2f, 0.7f);
	instance_nha = Translate(-1.35, -0.6, 1.70f); ngankeotu(0.02f, 0.2f, 0.7f);
	instance_nha = Translate(-1.50, -0.6, 1.35f); ngankeotu(0.3f, 0.2f, 0.02f);
	instance_nha = Translate(-1.50, -0.6, 2.05f); ngankeotu(0.3f, 0.2f, 0.02f);
	//tay cam
	instance_nha = Translate(-1.35, -0.6, 1.70f); ngankeotu(0.05f, 0.05f, 0.1f);
}
void khungtu()
{
	//ben tu
	instance_nha = Translate(-1.50, -0.2, 1.33f); khung(0.3f, 1.2f, 0.02f);
	instance_nha = Translate(-1.50, -0.2, 2.07f); khung(0.3f, 1.2f, 0.02f);
	//dinh ngan
	instance_nha = Translate(-1.50f, 0.4, 1.70f); khung(0.3f, 0.02f, 0.7f);
	//lung tu
	instance_nha = Translate(-1.52, -0.2, 1.7f); khung1(0.02f, 1.2f, 0.7f);

}
void canh1()
{
	//canh cua tu
	instance_nha = Translate(0, 0.5 * 0.88, 0); canhtu1(0.02f, 0.88f, 0.35f);
	//tay nam tu
	instance_nha = Translate(0, 0.5 * 0.88, 0.1); canhtu1(0.05f, 0.15f, 0.03f);
}
void canh2()
{
	//canh cua tu
	instance_nha = Translate(0, 0.5 * 0.88, 0); canhtu2(0.02f, 0.88f, 0.35f);
	//tay nam tu
	instance_nha = Translate(0, 0.5 * 0.88, -0.1); canhtu2(0.05f, 0.15f, 0.03f);
}
void quat()
{
	day();
	canhQuat1();
	canhQuat2();
	trucQuat();
}
//hien hhi
GLfloat dich1 = 0, dich3 = 0, dich2 = 0, dich4 = 0;
void nha()
{

	model = RotateX(theta[1]) * RotateY(theta[0]) * RotateZ(theta[4]) * Translate(0, 0, -0.9);
	ban();
	model3 = model * Translate(dich4, 0, 0);
	banLamViec();
	model1 = model * Translate(0, 0, dich3);
	ghe();
	model2 = model * Translate(0, 0, dich1);
	ghe1();
	tuong();
	tu();
	model7 = model;
	quat();
	giuong();
	khungtu();
	model5 = model * Translate(-1.35, -0.47, 1.33) * RotateY(theta[2]) * Translate(0, 0, 0.35 * 0.5);
	canh1();
	model6 = model * Translate(-1.35, -0.47, 2.07) * RotateY(theta[3]) * Translate(0, 0, -0.35 * 0.5);
	canh2();
	model4 = model * Translate(dich2, 0, 0);
	ngankeo();
}
//Cac tham so cho projection
GLfloat l = -0.25, r = 0.25;
GLfloat bottom = -0.25, top = 0.25;
GLfloat zNear = 0.3, zFar = 5;


GLfloat tren_xuong = 1.5;

GLfloat tren_xuong_2 = 0.5;
GLfloat ngang = 1, tienlui = 0;
void display(void)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vec4 eye(ngang, tren_xuong, tienlui, 1);
	vec4 at(0, tren_xuong_2, 0, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(-0.25, 0.25, -0.25, 0.25, 0.3, 5);
	
	nha();
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glutSwapBuffers();
}


void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'b':
		dich3 += 0.05;
		dich1 -= 0.05;
		if (dich3 >= 0.3) dich3 -= 0.3;
		if (dich1 <= -0.3) dich1 += 0.3;
		break;
		//dich ghe lam viec
	case 'r':
		dich4 += 0.05;
		if (dich4 >= 0.4) dich4 -= 0.4;
		break;
		//dieu khien quay 
		//quay theo y
	case 'a':
		theta[0] += 5;
		if (theta[0] > 360) theta[0] -= 360;
		break;
	case 'd':
		theta[0] -= 5;
		if (theta[0] > 360) theta[0] -= 360;
		break;
		//quat the x
	case 'w':
		theta[1] += 5;
		if (theta[1] > 360) theta[1] -= 360;
		break;
	case 's':
		theta[1] -= 5;
		if (theta[1] > 360) theta[1] -= 360;
		break;
		//quay theo z
	case 'c':
		theta[4] += 5;
		if (theta[4] > 360) theta[4] -= 360;
		break;
	case 'v':
		theta[4] -= 5;
		if (theta[4] < 360) theta[4] += 360;
		break;
		//mo tu phai
	case 'q':
		theta[2] += 5;
		if (theta[2] > 90) theta[2] -= 90;
		break;
		//mo cua trai
	case 'e':
		theta[3] -= 5;
		if (theta[3] < -90) theta[3] += 90;
		break;
		//dich ngan keo
	case 't':
		dich2 += 0.05;
		if (dich2 >= 0.3) dich2 -= 0.3;
		break;
// điều khiển camera
	case '9'://Nâng camera theo chiều y
		tren_xuong+= 0.1;
		glutPostRedisplay();
		break;

	case '('://hạ camera theo chiều y
		tren_xuong -= 0.1;
		glutPostRedisplay();
		break;
	case '-':
		tren_xuong_2 += 0.1;
		glutPostRedisplay();
		break;

	case '_':
		tren_xuong_2 -= 0.1;
		glutPostRedisplay();
		break;
	case '='://sang phải theo chiều x
		ngang += 0.1;
		glutPostRedisplay();
		break;

	case '+'://Sang trái theo chiều x
		ngang -= 0.1;
		glutPostRedisplay();
		break;
	case '5'://Tiến theo chiều z
		tienlui += 0.1;
		glutPostRedisplay();
		break;

	case '%'://lùi theo chiều z
		tienlui -= 0.1;
		glutPostRedisplay();
		break;
	case 'f': // tắt/ bật quạt
		if (fanSwitch == false) {
			fanSwitch = true; break;
		}
		else {
			fanSwitch = false; break;
		}
	case ' ':
		tren_xuong = 1.5;
		tren_xuong_2 = 0.5;
		ngang = 1;
		tienlui = 0;
		theta[0] = 0;
		theta[1] = 0;
		theta[2] = 0;
		theta[3] = 0;
		theta[4] = 0;
		glutPostRedisplay();
		break;

	}
}

void animate() {

	if (fanSwitch == true) {
		quayQuat += 1;
		if (quayQuat > 360)
			quayQuat -= 360;
	}
	else {
		quayQuat = quayQuat;
	}

	/* refresh screen */
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Nhóm 4");
	glutIdleFunc(animate);

	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	//	glutIdleFunc(spinCube);
	glutMainLoop();
	return 0;
}
