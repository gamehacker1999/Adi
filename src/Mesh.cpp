#include "Mesh.h"
#include <GL/freeglut.h>


Mesh::Mesh() 
{
	vert_num = tri_num = 0;
	vao = vbo = nbo = ibo = 0;
	modelMat = mat4(1.0f); 
}

Mesh::~Mesh() 
{
	delete[] vertices;
	delete[] triangles;
	delete[] fnormals;
	delete[] vnormals;
}

void Mesh::computeNormals()
{
	fnormals = new vec3[tri_num];
	vnormals = new vec3[vert_num];

	vec3 a, b, n;

	// Compute face normals
	for (unsigned int i = 0; i < tri_num; i++) {
		
		a = vertices[triangles[i][1]] - vertices[triangles[i][0]];
		b = vertices[triangles[i][2]] - vertices[triangles[i][0]];
		
		n = cross(a, b);
		fnormals[i] = normalize(n);
	}

	// Compute vertex normals
	for (unsigned int i = 0; i < vert_num; i++) {
		vnormals[i] = vec3(0.0f);
	}

	for (unsigned int i = 0; i < tri_num; i++) {
		for (unsigned int j = 0; j < 3; j++) {
			vnormals[triangles[i][j]] += fnormals[i];
		}
	}

	for (unsigned int i = 0; i < vert_num; i++) {
		vnormals[i] = normalize(vnormals[i]);
	}

}

void Mesh::prepareVBOandShaders(const char* v_shader_file, const char* f_shader_file)
{
	vShader.create(v_shader_file, GL_VERTEX_SHADER);
	fShader.create(f_shader_file, GL_FRAGMENT_SHADER);
	shaderProg.create();
	shaderProg.link(vShader);
	shaderProg.link(fShader);

	vShader.destroy();
	fShader.destroy();

	// create vbo 
	// generate a new VBO and get the associated ID
	glGenBuffers(1, &vbo);

	// bind VBO in order to use
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// upload data to VBO - data went to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vert_num, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // clean up
	// delete[] vertices; // commented out, since it's handled by the destructor


	// repeat for normals and indices
	
	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vert_num, vnormals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // clean up
	// delete[] vnormals; // commented out, since it's handled by the destructor

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uvec3) * tri_num, triangles, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // clean up
	// delete[] triangles; // commented out, since it's handled by the destructor
	
	// so far, we transferred the position and index data to GPU. There will be no data transfer calls at drawing time

	// Define VAO
	// There are 2 buffer objects (vertices and normals), which should be input "attribute" variables to the vertex shader. 
	// We set up the layout of both of these with vertex array object. 
	// The actual vertices and normal vectors have already been kept on the GPU memory by VBOs. 
	// The purpose of VAO is to have vertices and normal vectors as separate attributes in the vertex shader. 
	// So VAO's attrobites point to these data on the GPU, rather than referring back to any CPU data. 

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // 0 - the layout id in vertex shader
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL); // 1 - the layout id in fragment shader

	// Ettributes are disabled by default in OpenGL 4. 
	// We have to explicitly enable each one.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Mesh::create(const char* filename, const char* v_shader_file, const char* f_shader_file) {

	vector<vec3> ori_vertices;
	vector<uvec3> ori_triangles;

    ifstream fs(filename);

    char c;
    vec3 pos;
    int index[3];
	int vid = 0;

    std::string line;
    while (std::getline(fs, line)) {
        std::istringstream iss(line);

        iss >> c;

        switch (c) {
        case 'v':
			{
				// read a vertex
				iss >> pos.x;
				iss >> pos.y;
				iss >> pos.z;
				ori_vertices.push_back(pos);
				break;
			}
        case 'f':
			{
				// read a triangle's vertex indices
				iss >> index[0];
				iss >> index[1];
				iss >> index[2];
				// NOTE: index in obj files starts from 1
				ori_triangles.push_back(uvec3(index[0] - 1, index[1] -1, index[2] -1));
				break;
			}
        default:
            // skip the line
            break;
        }
    }
    fs.close();

	vert_num = ori_vertices.size();
	tri_num = ori_triangles.size();
	vertices = new vec3[vert_num];
	triangles = new uvec3[tri_num];

	// Use arrays to store vertices and triangles, instead of using c++ vectors.
	// This is because we have to use arrays when sending data to GPUs. 
	for (uint i = 0; i < vert_num; i++) {
		vertices[i] = ori_vertices[i];
	}
	for (uint i = 0; i < tri_num; i++) {
		triangles[i] = ori_triangles[i];
	}
	
	computeNormals();
	prepareVBOandShaders(v_shader_file, f_shader_file);
}

void Mesh::draw(mat4 viewMat, mat4 projMat, vec3 lightPos, float time) {

	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	
	if (vert_num <= 0 && tri_num <= 0)
		return;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();


	glUseProgram(shaderProg.id);
	mat4 m = translate(mat4(1.0), vec3(0.0f, 2.0f, 0.0f));
	modelMat = scale(m, vec3(0.3f, 0.3f, 0.3f));
	shaderProg.setMatrix4fv("modelMat", 1, value_ptr(modelMat));
	shaderProg.setMatrix4fv("viewMat", 1, value_ptr(viewMat));
	shaderProg.setMatrix4fv("projMat", 1, value_ptr(projMat));
	shaderProg.setFloat3V("lightPos", 1, value_ptr(lightPos));
	shaderProg.setFloat("time", time);
	shaderProg.setFloat("offset", normal_offset);

	//cout << glm::to_string(modelMat) << endl;
	//cout << glm::to_string(viewMat) << endl;
	//cout << glm::to_string(projMat) << endl;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, tri_num * 3, GL_UNSIGNED_INT, NULL);

	glutSolidSphere(20, 50, 50);

	glPopMatrix();
	glDisable(GL_POLYGON_OFFSET_FILL);

    glPopMatrix();
}

void Mesh::draw(mat4 viewMat, mat4 projMat, vec3 lightPositions[], vec3 ambientValues[], vec3 diffuseValues[], vec3 specularValues[], int coeffValues[], float time, vec3 translation, vec3 scaleVal, vec3 eyePos) {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if (vert_num <= 0 && tri_num <= 0)
		return;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();


	glUseProgram(shaderProg.id);
	mat4 m = translate(mat4(1.0), translation);
	modelMat = scale(m, scaleVal);
	shaderProg.setMatrix4fv("modelMat", 1, value_ptr(modelMat));
	shaderProg.setMatrix4fv("viewMat", 1, value_ptr(viewMat));
	shaderProg.setMatrix4fv("projMat", 1, value_ptr(projMat));
	shaderProg.setFloat3V("pointLights[0].position", 1, value_ptr(lightPositions[0]));
	shaderProg.setFloat3V("pointLights[1].position", 1, value_ptr(lightPositions[1]));

	shaderProg.setFloat3V("pointLights[0].ambient", 1, value_ptr(ambientValues[0]));
	shaderProg.setFloat3V("pointLights[1].ambient", 1, value_ptr(ambientValues[1]));

	shaderProg.setFloat3V("pointLights[0].diffuse", 1, value_ptr(diffuseValues[0]));
	shaderProg.setFloat3V("pointLights[1].diffuse", 1, value_ptr(diffuseValues[1]));

	shaderProg.setFloat3V("pointLights[0].specular", 1, value_ptr(specularValues[0]));
	shaderProg.setFloat3V("pointLights[1].specular", 1, value_ptr(specularValues[1]));

	shaderProg.setInt("pointLights[0].coeff", coeffValues[0]);
	shaderProg.setInt("pointLights[1].coeff", coeffValues[1]);

	shaderProg.setFloat3V("eyePos", 1, value_ptr(eyePos));
	shaderProg.setFloat("time", time);
	shaderProg.setFloat("offset", normal_offset);

	//cout << glm::to_string(modelMat) << endl;
	//cout << glm::to_string(viewMat) << endl;
	//cout << glm::to_string(projMat) << endl;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, tri_num * 3, GL_UNSIGNED_INT, NULL);

	glPopMatrix();
	glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();
}

void Mesh::draw(mat4 viewMat, mat4 projMat, PointLight lights[], vec3 translation, vec3 scaleVal, vec3 eyePos)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if (vert_num <= 0 && tri_num <= 0)
		return;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();


	glUseProgram(shaderProg.id);
	mat4 m = translate(mat4(1.0), translation);
	modelMat = scale(m, scaleVal);
	shaderProg.setMatrix4fv("modelMat", 1, value_ptr(modelMat));
	shaderProg.setMatrix4fv("viewMat", 1, value_ptr(viewMat));
	shaderProg.setMatrix4fv("projMat", 1, value_ptr(projMat));
	shaderProg.setFloat3V("pointLights[0].position", 1, value_ptr(lights[0].position));
	shaderProg.setFloat3V("pointLights[1].position", 1, value_ptr(lights[1].position));

	shaderProg.setFloat3V("pointLights[0].ambient", 1, value_ptr(lights[0].ambient));
	shaderProg.setFloat3V("pointLights[1].ambient", 1, value_ptr(lights[1].ambient));

	shaderProg.setFloat3V("pointLights[0].diffuse", 1, value_ptr(lights[0].diffuse));
	shaderProg.setFloat3V("pointLights[1].diffuse", 1, value_ptr(lights[1].diffuse));

	shaderProg.setFloat3V("pointLights[0].specular", 1, value_ptr(lights[0].specular));
	shaderProg.setFloat3V("pointLights[1].specular", 1, value_ptr(lights[1].specular));

	shaderProg.setInt("pointLights[0].coeff", lights[0].coeff);
	shaderProg.setInt("pointLights[1].coeff", lights[1].coeff);

	shaderProg.setFloat3V("eyePos", 1, value_ptr(eyePos));
	shaderProg.setFloat("time", 0);
	shaderProg.setFloat("offset", normal_offset);

	//cout << glm::to_string(modelMat) << endl;
	//cout << glm::to_string(viewMat) << endl;
	//cout << glm::to_string(projMat) << endl;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, tri_num * 3, GL_UNSIGNED_INT, NULL);

	glPopMatrix();
	glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();
}
