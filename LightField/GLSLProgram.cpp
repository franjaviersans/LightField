#include "GLSLProgram.h"


#include <fstream>
#include <iostream>
using std::ifstream;
using std::ios;
using std::cout;
using std::endl;

#include <sstream>
#include <sys/stat.h>
#include <memory>

namespace GLSLShaderInfo {
	struct shader_file_extension {
		const char *ext;
		GLSLShader::GLSLShaderType type;
	};

	struct shader_file_extension extensions[] =
	{
		{ ".vs", GLSLShader::VERTEX },
		{ ".vert", GLSLShader::VERTEX },
		{ ".gs", GLSLShader::GEOMETRY },
		{ ".geom", GLSLShader::GEOMETRY },
		{ ".tcs", GLSLShader::TESS_CONTROL },
		{ ".tes", GLSLShader::TESS_EVALUATION },
		{ ".fs", GLSLShader::FRAGMENT },
		{ ".frag", GLSLShader::FRAGMENT },
		{ ".cs", GLSLShader::COMPUTE }
	};
}

GLSLProgram::GLSLProgram() : handle(0), linked(false) { }

GLSLProgram::~GLSLProgram() {
	destroyProgram();
}

void   GLSLProgram::destroyProgram(){

	if (handle == 0) return;

	// Query the number of attached shaders
	GLint numShaders = 0;
	glGetProgramiv(handle, GL_ATTACHED_SHADERS, &numShaders);


	// Get the shader names
	std::unique_ptr<GLuint[]> shaderNames = std::make_unique<GLuint[]>(numShaders);
	glGetAttachedShaders(handle, numShaders, NULL, shaderNames.get());

	// Delete the shaders
	for (int i = 0; i < numShaders; i++)
		glDeleteShader(shaderNames[i]);

	// Delete the program
	glDeleteProgram(handle);


	handle = 0;
	linked = false;
}

void GLSLProgram::compileShader(const std::string fileName)
throw(...) {
	int numExts = sizeof(GLSLShaderInfo::extensions) / sizeof(GLSLShaderInfo::shader_file_extension);

	// Check the file name's extension to determine the shader type
	std::string ext = getExtension(fileName.c_str());
	GLSLShader::GLSLShaderType type = GLSLShader::VERTEX;
	bool matchFound = false;
	for (int i = 0; i < numExts; i++) {
		if (ext == GLSLShaderInfo::extensions[i].ext) {
			matchFound = true;
			type = GLSLShaderInfo::extensions[i].type;
			break;
		}
	}

	// If we didn't find a match, throw an exception
	if (!matchFound) {
		std::string msg = "Unrecognized extension: " + ext;
		throw GLSLProgramException(msg);
	}

	// Pass the discovered shader type along
	compileShader(fileName, type);
}

std::string GLSLProgram::getExtension(const char * name) {
	std::string nameStr(name);

	size_t loc = nameStr.find_last_of('.');
	if (loc != std::string::npos) {
		return nameStr.substr(loc, std::string::npos);
	}
	return "";
}

void GLSLProgram::compileShader(const std::string fileName,
	GLSLShader::GLSLShaderType type, const std::string header)
	throw(...)
{
	if (!fileExists(fileName))
	{
		std::string message = std::string("Shader: ") + fileName + " not found.";
		throw GLSLProgramException(message);
	}

	if (handle <= 0) {
		handle = glCreateProgram();
		if (handle == 0) {
			throw GLSLProgramException("Unable to create shader program.");
		}
	}

	ifstream inFile(fileName, ios::in);
	if (!inFile) {
		std::string message = std::string("Unable to open: ") + fileName;
		throw GLSLProgramException(message);
	}

	// Get file contents
	std::stringstream code;
	code << inFile.rdbuf();
	inFile.close();

	compileShader(header, code.str(), type, fileName);
}

void GLSLProgram::compileShader(const std::string header, const std::string source,
	GLSLShader::GLSLShaderType type,
	const std::string fileName)
	throw(...)
{
	if (handle <= 0) {
		handle = glCreateProgram();
		if (handle == 0) {
			throw GLSLProgramException("Unable to create shader program.");
		}
	}

	GLuint shaderHandle = glCreateShader(type);
	std::string final_code = header + source;
	const char * c_code = final_code.c_str();
	glShaderSource(shaderHandle, 1, &c_code, NULL);

	// Compile the shader
	glCompileShader(shaderHandle);

	// Check for errors
	int result;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		// Compile failed, get log
		int length = 0;
		std::string logString;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
		if (length > 0) {
			std::unique_ptr<char[]> c_log = std::make_unique<char[]>(length);
			int written = 0;
			glGetShaderInfoLog(shaderHandle, length, &written, c_log.get());
			logString = c_log.get();
		}
		std::string msg = std::string(fileName) + ": shader compliation failed\n";
		msg += logString;

		throw GLSLProgramException(msg);

	}
	else {
		// Compile succeeded, attach shader
		glAttachShader(handle, shaderHandle);
	}
}

void GLSLProgram::link() throw(...)
{
	if (linked) return;
	if (handle <= 0)
		throw GLSLProgramException("Program has not been compiled.");

	glLinkProgram(handle);

	int status = 0;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		// Store log and return false
		int length = 0;
		std::string logString;

		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

		if (length > 0) {
			std::unique_ptr<char[]> c_log = std::make_unique<char[]>(length);
			int written = 0;
			glGetProgramInfoLog(handle, length, &written, c_log.get());
			logString = c_log.get();
		}

		throw GLSLProgramException(std::string("Program link failed:\n") + logString);
	}
	else {
		uniformLocations.clear();
		linked = true;
	}
}

void GLSLProgram::use() throw(...)
{
	if (handle <= 0 || (!linked))
		throw GLSLProgramException("Shader has not been linked");
	glUseProgram(handle);
}

int GLSLProgram::getHandle()
{
	return handle;
}

bool GLSLProgram::isLinked()
{
	return linked;
}

void GLSLProgram::bindAttribLocation(GLuint location, const char * name)
{
	glBindAttribLocation(handle, location, name);
}

void GLSLProgram::bindFragDataLocation(GLuint location, const char * name)
{
	glBindFragDataLocation(handle, location, name);
}

void GLSLProgram::setUniform(const char *name, float x, float y, float z)
{
	GLint loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with " << name << endl;
	glUniform3f(loc, x, y, z);
}

void GLSLProgram::setUniform(const char *name, const vec3 & v)
{
	this->setUniform(name, v.x, v.y, v.z);
}

void GLSLProgram::setUniform(const char *name, const uvec3 & v)
{
	GLint loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with " << name << endl;
	glUniform3ui(loc, v.x, v.y, v.z);
}

void GLSLProgram::setUniform(const char *name, const vec4 & v)
{
	GLint loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with " << name << endl;
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void GLSLProgram::setUniform(const char *name, const vec2 & v)
{
	GLint loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with " << name << endl;
	glUniform2f(loc, v.x, v.y);
}

void GLSLProgram::setUniform(const char *name, const mat4 & m)
{
	GLint loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with "<<name<<endl;
	glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::setUniform(const char *name, const mat3 & m)
{

	GLint loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with " << name << endl;
	glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::setUniform(const char *name, float val)
{
	GLint loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with " << name << endl;
	glUniform1f(loc, val);
}

void GLSLProgram::setUniform(const char *name, int val)
{
	GLint loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with " << name << endl;
	glUniform1i(loc, val);
}

void GLSLProgram::setUniform(const char *name, GLuint val)
{
	GLint loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with " << name << endl;
	glUniform1ui(loc, val);
}

void GLSLProgram::setUniform(const char *name, bool val)
{
	int loc = getUniformLocation(name);
	if (loc < 0) cout << "Problem with " << name << endl;
	glUniform1i(loc, val);
}

void GLSLProgram::printActiveUniforms() {
	GLint numUniforms = 0;
	glGetProgramInterfaceiv(handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	printf("Active uniforms:\n");
	for (int i = 0; i < numUniforms; ++i) {
		GLint results[4];
		glGetProgramResourceiv(handle, GL_UNIFORM, i, 4, properties, 4, NULL, results);

		if (results[3] != -1) continue;  // Skip uniforms in blocks 
		GLint nameBufSize = results[0] + 1;
		std::unique_ptr<char[]> name = std::make_unique<char[]>(nameBufSize);
		glGetProgramResourceName(handle, GL_UNIFORM, i, nameBufSize, NULL, name.get());
		printf("%-5d %s (%s)\n", results[2], name.get(), getTypeString(results[1]));
	}
}

void GLSLProgram::printActiveUniformBlocks() {
	GLint numBlocks = 0;

	glGetProgramInterfaceiv(handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
	GLenum blockProps[] = { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH };
	GLenum blockIndex[] = { GL_ACTIVE_VARIABLES };
	GLenum props[] = { GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX };

	for (int block = 0; block < numBlocks; ++block) {
		GLint blockInfo[2];
		glGetProgramResourceiv(handle, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, NULL, blockInfo);
		GLint numUnis = blockInfo[0];

		std::unique_ptr<char[]> blockName = std::make_unique<char[]>(blockInfo[1] + 1);
		glGetProgramResourceName(handle, GL_UNIFORM_BLOCK, block, blockInfo[1] + 1, NULL, blockName.get());
		printf("Uniform block \"%s\":\n", blockName.get());

		std::unique_ptr<GLint[]> unifIndexes = std::make_unique<GLint[]>(numUnis);
		glGetProgramResourceiv(handle, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnis, NULL, unifIndexes.get());

		for (int unif = 0; unif < numUnis; ++unif) {
			GLint uniIndex = unifIndexes[unif];
			GLint results[3];
			glGetProgramResourceiv(handle, GL_UNIFORM, uniIndex, 3, props, 3, NULL, results);

			GLint nameBufSize = results[0] + 1;
			std::unique_ptr<char[]> name = std::make_unique<char[]>(nameBufSize);
			glGetProgramResourceName(handle, GL_UNIFORM, uniIndex, nameBufSize, NULL, name.get());
			printf("    %s (%s)\n", name.get(), getTypeString(results[1]));
		}

	}
}

void GLSLProgram::printActiveAttribs() {
	GLint numAttribs;
	glGetProgramInterfaceiv(handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

	printf("Active attributes:\n");
	for (int i = 0; i < numAttribs; ++i) {
		GLint results[3];
		glGetProgramResourceiv(handle, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

		GLint nameBufSize = results[0] + 1;

		std::unique_ptr<char[]> name = std::make_unique<char[]>(nameBufSize);
		glGetProgramResourceName(handle, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name.get());
		printf("%-5d %s (%s)\n", results[2], name.get(), getTypeString(results[1]));
	}
}

const char * GLSLProgram::getTypeString(GLenum type) {
	// There are many more types than are covered here, but
	// these are the most common in these examples.
	switch (type) {
	case GL_FLOAT:
		return "float";
	case GL_FLOAT_VEC2:
		return "vec2";
	case GL_FLOAT_VEC3:
		return "vec3";
	case GL_FLOAT_VEC4:
		return "vec4";
	case GL_DOUBLE:
		return "double";
	case GL_INT:
		return "int";
	case GL_UNSIGNED_INT:
		return "unsigned int";
	case GL_BOOL:
		return "bool";
	case GL_FLOAT_MAT2:
		return "mat2";
	case GL_FLOAT_MAT3:
		return "mat3";
	case GL_FLOAT_MAT4:
		return "mat4";
	default:
		return "?";
	}
}

void GLSLProgram::validate() throw(...)
{
	if (!isLinked())
		throw GLSLProgramException("Program is not linked");

	GLint status;
	glValidateProgram(handle);
	glGetProgramiv(handle, GL_VALIDATE_STATUS, &status);

	if (GL_FALSE == status) {
		// Store log and return false
		int length = 0;
		std::string logString;

		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

		if (length > 0) {
			std::unique_ptr<char[]> c_log = std::make_unique<char[]>(length);
			int written = 0;
			glGetProgramInfoLog(handle, length, &written, c_log.get());
			logString = c_log.get();
		}

		throw GLSLProgramException(std::string("Program failed to validate\n") + logString);

	}
}

int GLSLProgram::getUniformLocation(const char * name)
{
	std::map<std::string, int>::iterator pos;
	pos = uniformLocations.find(name);

	if (pos == uniformLocations.end()) {
		uniformLocations[name] = glGetUniformLocation(handle, name);
	}

	return uniformLocations[name];
}

bool GLSLProgram::fileExists(const std::string & fileName)
{
	struct stat info;
	int ret = -1;

	ret = stat(fileName.c_str(), &info);
	return 0 == ret;
}