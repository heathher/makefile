#ifndef TEMPLATE_BUILDER_H
#define TEMPLATE_BUILDER_H

#include <string>
#include <list>
#include "dependency_tree.h"

class TemplateBuilder
{
public:
	class Node
	{
	public:
		Node(const std::string &name);
		~Node();
		void addDependency(const std::string &dep);
		void addAction(const std::string &lex);
		bool isMatching(const std::string &name) const;
		DependencyTree::Node build(const std::string &name) const;
	private:
		std::string m_name;
		std::list<std::string> m_dependences;
		std::list<std::string> m_actions;
	};
	void addTemplate(const Node &node);
	bool hasMatching(const std::string &name) const;
	DependencyTree::Node build(const std::string &name) const;
private:
	std::list<Node> m_templates;
};

#endif