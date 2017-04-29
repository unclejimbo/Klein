#pragma once

class Component
{
public:
	virtual ~Component();

	int id() const;

private:
	static int _id;
};
