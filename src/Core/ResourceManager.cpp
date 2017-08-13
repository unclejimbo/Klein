#include "Core/ResourceManager.h"

ResourceManager& ResourceManager::instance()
{
	static ResourceManager* resourceManager = new ResourceManager();
	return *resourceManager;
}

void ResourceManager::initialize(QOpenGLWidget* context)
{
	_context = context;
}

unsigned ResourceManager::addMesh(const std::vector<Point_3>& vertices,
	const std::vector<unsigned>& indices,
	unsigned vertexBufferID, unsigned normalBufferID)
{
	auto mesh = std::make_unique<Mesh>(vertices, indices, vertexBufferID, normalBufferID);
	auto id = mesh->id();
	if (!_meshMap.insert_or_assign(id, std::move(mesh)).second) {
		KLEIN_LOG_WARNING(QString("Mesh %1 already exists and is replaced").arg(id));
	}
	return id;
}

Mesh* ResourceManager::mesh(unsigned id)
{
	if (_meshMap.find(id) != _meshMap.end()) {
		KLEIN_LOG_WARNING(QString("Can't find Mesh %1").arg(id));
		return _meshMap[id].get();
	}
	else {
		return nullptr;
	}
}

bool ResourceManager::removeMesh(unsigned id)
{
	if (_meshMap.erase(id) == 0) {
		KLEIN_LOG_WARNING(QString("Can't find Mesh %1, nothing removed").arg(id));
		return false;
	}
	else {
		return true;
	}
}

unsigned ResourceManager::addPointCloud(const std::vector<Point_3>& vertices, unsigned vertexBufferID)
{
	auto pc = std::make_unique<PointCloud>(vertices, vertexBufferID);
	auto id = pc->id();
	if (!_pointCloudMap.insert_or_assign(id, std::move(pc)).second) {
		KLEIN_LOG_WARNING(QString("PointCloud %1 already exists and is replaced").arg(id));
	}
	return id;
}

unsigned ResourceManager::addPointCloud(const std::vector<Point_3>& vertices,
	const std::vector<Vector_3>& normals, unsigned vertexBufferID)
{
	auto pc = std::make_unique<PointCloud>(vertices, normals, vertexBufferID);
	auto id = pc->id();
	if (!_pointCloudMap.insert_or_assign(id, std::move(pc)).second) {
		KLEIN_LOG_WARNING(QString("PointCloud %1 already exists and is replaced").arg(id));
	}
	return id;
}

PointCloud* ResourceManager::pointCloud(unsigned id)
{
	if (_pointCloudMap.find(id) != _pointCloudMap.end()) {
		return _pointCloudMap[id].get();
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't find PointCloud %1").arg(id));
		return nullptr;
	}
}

bool ResourceManager::removePointCloud(unsigned id)
{
	if (_pointCloudMap.erase(id) == 0) {
		KLEIN_LOG_WARNING(QString("Can't find PointCloud %1, nothing removed").arg(id));
		return false;
	}
	else {
		return true;
	}
}

GLBuffer* ResourceManager::glBuffer(unsigned bufferID)
{
	if (_bufferMap.find(bufferID) != _bufferMap.end()) {
		return _bufferMap[bufferID].get();
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't find OpenGL Buffer Object %1").arg(bufferID));
		return nullptr;
	}
}

bool ResourceManager::removeGLBuffer(unsigned bufferID)
{
	if (_bufferMap.erase(bufferID) == 0) {
		KLEIN_LOG_WARNING(QString("Can't find OpenGL Buffer Object %1, nothing removed").arg(bufferID));
		return false;
	}
	else {
		return true;
	}
}

void ResourceManager::addPBRMaterial(const std::string& name, const QVector3D& albedo,
	const float roughness, const float metallic, float ao)
{
	PBRMaterial material{ albedo, roughness, metallic, ao };
	if (!_pbrMaterialMap.insert_or_assign(name, material).second) {
		KLEIN_LOG_WARNING(QString("Material %1 already exists and is replaced").arg(name.c_str()));
	}
}

PBRMaterial* ResourceManager::pbrMaterial(const std::string & name)
{
	if (_pbrMaterialMap.find(name) != _pbrMaterialMap.end()) {
		return &_pbrMaterialMap[name];
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't find Material %1").arg(name.c_str()));
		return nullptr;
	}
}

bool ResourceManager::removePBRMaterial(const std::string& name)
{
	if (_pbrMaterialMap.erase(name) == 0) {
		KLEIN_LOG_WARNING(QString("Can't find Material %1, nothing removed").arg(name.c_str()));
		return false;
	}
	else {
		return true;
	}
}

void ResourceManager::addShaderProgram(const std::string& name,
	const std::string& vertexShader, const std::string& fragmentShader)
{
	if (_context != nullptr) {
		_context->makeCurrent();
		auto program = std::make_unique<QOpenGLShaderProgram>();
		program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShader.c_str());
		program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShader.c_str());
		program->link();
		_context->doneCurrent();

		if (!_shaderMap.insert_or_assign(name, std::move(program)).second) {
			KLEIN_LOG_WARNING(QString("Shader %1 already exists and is replaced").arg(name.c_str()));
		}
	}
	else {
		KLEIN_LOG_CRITICAL("Invalid OpenGL context");
	}
}

QOpenGLShaderProgram* ResourceManager::shaderProgram(const std::string& name)
{
	if (_shaderMap.find(name) != _shaderMap.end()) {
		return _shaderMap[name].get();
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't find Shader %1").arg(name.c_str()));
		return nullptr;
	}
}

bool ResourceManager::removeShaderProgram(const std::string& name)
{
	if (_shaderMap.erase(name) == 0) {
		KLEIN_LOG_WARNING(QString("Can't find Shader %1").arg(name.c_str()));
		return false;
	}
	else {
		return true;
	}
}
