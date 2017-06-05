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
	unsigned id() const;

private:
	SceneNode* _node = nullptr;
	static unsigned _id;
};
