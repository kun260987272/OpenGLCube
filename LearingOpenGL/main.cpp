#include<GL\glew.h>
#include<GL\freeglut.h>
#include"matrix.h"


//把shader源程序保存到字符数组
static char* readShaderSource(const char* shaderFile)
{
	FILE* fp;
	errno_t err = fopen_s(&fp, shaderFile, "r");
	if (err) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);

	buf[size] = NULL;
	fclose(fp);

	return buf;
}
//初始化shader
GLuint initShader(const char* vShaderFile, const char* fShaderFile)
{
	struct Shader
	{
		const char* filename;
		GLenum type;
		GLcharARB* source;
	}
	shaders[2] = {
		{ vShaderFile, GL_VERTEX_SHADER, NULL },
		{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
	};
	//创建程序
	GLuint program = glCreateProgram();

	for (int i = 0; i < 2; ++i)
	{
		Shader& s = shaders[i];
		s.source = readShaderSource(s.filename);

		if (shaders[i].source == NULL)
		{
			cerr << "failed to read " << s.filename << endl;
			//exit(EXIT_FAILURE);
		}
		//创建shader对象、绑定shader源程序、编译shader
		GLuint shader = glCreateShader(s.type);
		glShaderSource(shader, 1, (const GLcharARB**)&s.source, NULL);
		glCompileShader(shader);
		//shader编译的错误检测
		GLint compile;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compile);
		if (!compile)
		{
			cerr << "failed to compile " << s.filename << endl;
			GLint logSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader, logSize, NULL, logMsg);
			cerr << logMsg << endl;
			delete[] logMsg;
			//exit(EXIT_FAILURE);
		}
		delete[] s.source;
		//将shader绑定到程序上
		glAttachShader(program, shader);
	}
	//链接程序，将着色器对象链接成一个可执行文件
	glLinkProgram(program);
	//链接程序的错误检测
	GLint link;
	glGetProgramiv(program, GL_COMPILE_STATUS, &link);
	if (!link)
	{
		cerr << "failed to link " << endl;
		GLint logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		cerr << logMsg << endl;
		delete[] logMsg;
		//exit(EXIT_FAILURE);
	}
	return program;
}

typedef vec4 color4;
typedef vec4 point4;

const int NumVertices = 36;//六个面 每个面两个三角形

vec3 normals[NumVertices];
point4 points[NumVertices];

//中心位于坐标原点，边与坐标轴平行的单位立方体的顶点
point4 vertices[8] = {
	point4(-0.5,-0.5,0.5,1.0),
	point4(-0.5,0.5,0.5,1.0),
	point4(0.5,0.5,0.5,1.0),
	point4(0.5,-0.5,0.5,1.0),
	point4(-0.5,-0.5,-0.5,1.0),
	point4(-0.5,0.5,-0.5,1.0),
	point4(0.5,0.5,-0.5,1.0),
	point4(0.5,-0.5,-0.5,1.0)
};


GLuint model_view;
GLuint projection;

//每个面生成两个三角形，并赋值颜色
int index = 0;
void quad(int a, int b, int c, int d)
{
//计算某个面上的法向量
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u,v));

	normals[index] = normal; points[index] = vertices[a]; index++;
	normals[index] = normal; points[index] = vertices[b]; index++;
	normals[index] = normal; points[index] = vertices[c]; index++;
	normals[index] = normal; points[index] = vertices[a]; index++;
	normals[index] = normal; points[index] = vertices[c]; index++;
	normals[index] = normal; points[index] = vertices[d]; index++;
}

//12个三角形
void colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//初始化 
void init()
{
	colorcube();
	
	//使用程序，将程序变成激活状态
	GLuint program = initShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	//顶点数组对象
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//缓冲区对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points)+sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals);
	
	//初始化顶点shader中顶点属性
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(points));

	//初始化关照参数
	//右面绿色光源
	point4 light_position1(1.0, 0.0, 0.0, 0.0);
	color4 light_diffuse1(0.0, 1.0, 0.0, 1.0);
	color4 light_specular1(0.0, 1.0, 0.0, 1.0);
	//上面红色光源
	point4 light_position2(0.0, 1.0, 0.0, 0.0);
	color4 light_diffuse2(1.0, 0.0, 0.0, 1.0);
	color4 light_specular2(1.0, 0.0, 0.0, 1.0);
	//前面蓝色光源
	point4 light_position3(0.0, 0.0, 1.0, 0.0);
	color4 light_diffuse3(0.0, 0.0, 1.0, 1.0);
	color4 light_specular3(0.0, 0.0, 1.0, 1.0);
	//环境光
	color4 light_ambient(0.3, 0.3, 0.3, 1.0);
	//材料全白色
	color4 material_ambient(1.0, 1.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 material_specular(1.0, 1.0, 1.0, 1.0);
	float material_shiness = 100.0;

	color4 diffuse_product1 = light_diffuse1*material_diffuse;
	color4 specular_product1 = light_specular1*material_specular;

	color4 diffuse_product2 = light_diffuse2*material_diffuse;
	color4 specular_product2 = light_specular2*material_specular;

	color4 diffuse_product3 = light_diffuse3*material_diffuse;
	color4 specular_product3 = light_specular3*material_specular;

	color4 ambient_product = light_ambient*material_ambient;
	//初始化uniform变量
	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);

	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct1"), 1, diffuse_product1);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct1"), 1, specular_product1);
	glUniform4fv(glGetUniformLocation(program, "LightPosition1"), 1, light_position1);

	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct2"), 1, diffuse_product2);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct2"), 1, specular_product2);
	glUniform4fv(glGetUniformLocation(program, "LightPosition2"), 1, light_position2);

	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct3"), 1, diffuse_product3);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct3"), 1, specular_product3);
	glUniform4fv(glGetUniformLocation(program, "LightPosition3"), 1, light_position3);

	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shiness);

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	point4 eye(1.0, 1.0, 1.0,1.0);
	point4 at(0.0, 0.0, 0.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 0.0);
	//模视变换矩阵
	mat4 mv = LookAt(eye, at, up);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	//投影矩阵
	mat4 p = Perspective(60, 1, 0.5, 3.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glutSwapBuffers();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("OpenGL");

	glewInit();
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}