#ifndef DEPENDENCY_TREE_H
#define DEPENDENCY_TREE_H

#include <string>
#include <map>
#include <list>
#include <set>

class TemplateBuilder;

class DependencyTree
{
public:
	class Node
	{
	public:
		Node(const std::string &name);
		~Node();
		void addDependency(const std::string &name);
		void addAction(const std::string &action);
		const std::string& getName() const;
		const std::list<std::string>& getDependencies() const;
		const std::list<std::string>& getActions() const;
	private:
		void run() const;
		std::list<std::string> m_children;
		std::list<std::string> m_actions;
		std::string m_name;
		friend class DependencyTree;
	};
	DependencyTree();
	~DependencyTree();
	void insertNode(const Node &node, bool replace = true);
	bool hasNode(const std::string &name) const;
	Node &getNode(const std::string &name);
	void run(const std::string &rule = "") const;
	void buildTemplates(const TemplateBuilder &tmp_builder);

	void print() const;
private:
	std::string m_first_rule;
	std::map<std::string, Node> m_map;
	std::set<std::string> m_open_links;
	void updateOpenLinks(const Node &node);
	void runTree(const std::string &name, std::set<std::string> &existing_targets, std::set<std::string> &targets_done, bool abstract = false) const;
	void error(const std::string &msg, int errcode = 0) const;
};

#endif
