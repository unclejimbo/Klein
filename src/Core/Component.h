#pragma once

class SceneNode;

class Component
{
public:
	Component();
	explicit Component(SceneNode* node);
	virtual ~Component();

	void attachTo(SceneNode* node);
	SceneNode* sceneNode() const;
	int id() const;

private:
	SceneNode* _node = nullptr;
	static int _id;
};
