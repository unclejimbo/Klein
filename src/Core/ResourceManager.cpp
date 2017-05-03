#include "Core/ResourceManager.h"
#include "Core/Logger.h"

ResourceManager::~ResourceManager()
{
	for (auto&& buf : _bufferMap) {
		buf.second.destroy();
	}
}

ResourceManager& ResourceManager::instance()
{
	static ResourceManager* resourceManager = new ResourceManager();
	return *resourceManager;
}

void ResourceManager::initialize(QOpenGLWidget* context)
{
	_context = context;
}

void ResourceManager::addMesh(const std::string& name, const std::vector<QVector3D>& vertices,
	const std::vector<QVector3D>& normals, const std::vector<unsigned>& indices,
	const std::string& vertexBufferID, const std::string& normalBufferID)
{
	auto mesh = std::make_unique<Mesh>(vertices, normals, indices, vertexBufferID, normalBufferID);

	if (!_meshMap.insert_or_assign(name, std::move(mesh)).second) {
		KLEIN_LOG_WARNING(QString("Mesh %1 already exists and is replaced").arg(name.c_str()));
	}
}

Mesh* ResourceManager::mesh(const std::string& name)
{
	if (_meshMap.find(name) != _meshMap.end()) {
		return _meshMap[name].get();
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't find Mesh %1").arg(name.c_str()));
		return nullptr;
	}
}

bool ResourceManager::removeMesh(const std::string& name)
{
	if (_meshMap.erase(name) == 0) {
		KLEIN_LOG_WARNING(QString("Can't find Mesh %1, nothing removed").arg(name.c_str()));
		return false;
	}
	else {
		return true;
	}
}

void ResourceManager::addGLBuffer(const std::string& name, const std::vector<float>& data,
	QOpenGLBuffer::Type type, QOpenGLBuffer::UsagePattern usage)
{
	if (_context != nullptr) {
		_context->makeCurrent();
		QOpenGLBuffer buffer(type);
		buffer.create();
		buffer.setUsagePattern(usage);
		buffer.bind();
		buffer.allocate(data.data(), static_cast<int>(data.size()));
		buffer.release();
		_context->doneCurrent();

		if (_bufferMap.find(name) == _bufferMap.end()) {
			_bufferMap.insert({ name, buffer });
		}
		else {
			_bufferMap[name].destroy();
			_bufferMap[name] = buffer;
			KLEIN_LOG_WARNING(QString("Buffer %1 already exists and is replaced").arg(name.c_str()));
		}
	}
	else {
		KLEIN_LOG_CRITICAL("Invalid OpenGL context");
	}
}

void ResourceManager::addGLBuffer(const std::string& name, const std::vector<QVector3D>& data,
	QOpenGLBuffer::Type type, QOpenGLBuffer::UsagePattern usage)
{
	if (_context != nullptr) {
		_context->makeCurrent();
		QOpenGLBuffer buffer(type);
		buffer.create();
		buffer.setUsagePattern(usage);
		buffer.bind();
		buffer.allocate(data.data(), static_cast<int>(data.size() * sizeof(QVector3D)));
		buffer.release();
		_context->doneCurrent();

		if (_bufferMap.find(name) == _bufferMap.end()) {
			_bufferMap.insert({ name, buffer });
		}
		else {
			_bufferMap[name].destroy();
			_bufferMap[name] = buffer;
			KLEIN_LOG_WARNING(QString("Buffer %1 already exists and is replaced").arg(name.c_str()));
		}
	}
	else {
		KLEIN_LOG_CRITICAL("Invalid OpenGL context");
	}
}

void ResourceManager::addGLBuffer(const std::string& name, const std::vector<QVector4D>& data,
	QOpenGLBuffer::Type type, QOpenGLBuffer::UsagePattern usage)
{
	if (_context != nullptr) {
		_context->makeCurrent();
		QOpenGLBuffer buffer(type);
		buffer.create();
		buffer.setUsagePattern(usage);
		buffer.bind();
		buffer.allocate(data.data(), static_cast<int>(data.size() * sizeof(QVector4D)));
		buffer.release();
		_context->doneCurrent();

		if (_bufferMap.find(name) == _bufferMap.end()) {
			_bufferMap.insert({ name, buffer });
		}
		else {
			_bufferMap[name].destroy();
			_bufferMap[name] = buffer;
			KLEIN_LOG_WARNING(QString("Buffer %1 already exists and is replaced").arg(name.c_str()));
		}
	}
	else {
		KLEIN_LOG_CRITICAL("Invalid OpenGL context");
	}
}

void ResourceManager::addGLBuffer(const std::string& name, const std::vector<typename Kernel::Point_3>& data,
	QOpenGLBuffer::Type type, QOpenGLBuffer::UsagePattern usage)
{
	if (_context != nullptr) {
		_context->makeCurrent();
		QOpenGLBuffer buffer(type);
		buffer.create();
		buffer.setUsagePattern(usage);
		buffer.bind();
		buffer.allocate(data.data(), static_cast<int>(data.size() * sizeof(typename Kernel::Point_3)));
		buffer.release();
		_context->doneCurrent();

		if (_bufferMap.find(name) == _bufferMap.end()) {
			_bufferMap.insert({ name, buffer });
		}
		else {
			_bufferMap[name].destroy();
			_bufferMap[name] = buffer;
			KLEIN_LOG_WARNING(QString("Buffer %1 already exists and is replaced").arg(name.c_str()));
		}
	}
	else {
		KLEIN_LOG_CRITICAL("Invalid OpenGL context");
	}
}

QOpenGLBuffer* ResourceManager::glBuffer(const std::string& name)
{
	if (_bufferMap.find(name) != _bufferMap.end()) {
		return &_bufferMap[name];
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't find OpenGL buffer %1").arg(name.c_str()));
		return nullptr;
	}
}

bool ResourceManager::removeGLBuffer(const std::string& name)
{
	if (_bufferMap.erase(name) == 0) {
		KLEIN_LOG_WARNING(QString("Can't find OpenGL buffer %1, nothing removed").arg(name.c_str()));
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
