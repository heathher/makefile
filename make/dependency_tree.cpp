#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "dependency_tree.h"
#include "template_builder.h"
#include "my_exceptions.h"

unsigned countWords(const std::string &str);

bool buildIsNecessary(const std::string &target, const std::string &dependency);

bool fileExists(const std::string &filename);

DependencyTree::Node::Node(const std::string &name) : m_name (name)
{

}

DependencyTree::Node::~Node()
{

}

void DependencyTree::Node::run() const
{

	for (std::list<std::string>::const_iterator it = m_actions.begin(); it != m_actions.end(); it++)
	{
		std::cout<<">> "<<(*it)<<std::endl;
		int errcode = system(it->c_str());
		if (errcode != 0)
		{
			std::cerr<<"Command \""<<(*it)<<"\" has failed!"<<std::endl;
			throw Exception(Exception::RUNTIME_ERROR, errcode);
		}
	}
}

void DependencyTree::Node::addDependency(const std::string &name)
{
	std::list<std::string>::iterator it;
	std::istringstream all_deps(name);
	std::string c_dep;
	while (all_deps>>c_dep)
	{
		it = std::find(m_children.begin(), m_children.end(), c_dep);
		if (it == m_children.end())
			m_children.push_back(c_dep);
	}
}

void DependencyTree::Node::addAction(const std::string &action)
{
	if (countWords(action) > 0)
		m_actions.push_back(action);
}

const std::string& DependencyTree::Node::getName() const
{
	return m_name;
}

const std::list<std::string>& DependencyTree::Node::getDependencies() const
{
	return m_children;
}
const std::list<std::string>& DependencyTree::Node::getActions() const
{
	return m_actions;
}

DependencyTree::DependencyTree()
{
	m_first_rule = "";
}

DependencyTree::~DependencyTree()
{

}

void DependencyTree::insertNode(const DependencyTree::Node &node, bool replace)
{
	const std::string &name = node.getName();
	if (countWords(name) == 0)
	{
		std::cerr<<"Error: name of a target cannot be empty"<<std::endl;
		throw Exception(Exception::SYNTAX_ANALYZER_ERROR);
	}
	if (m_first_rule == "")
		m_first_rule = name;
	std::map<std::string, DependencyTree::Node>::iterator it = m_map.find(name);
	if (it != m_map.end())
	{
		/*
		std::cerr<<"Warning: redefinition of target \""<<name<<"\", "
		         <<(replace?"new":"old")<<" version is used"<<std::endl; 
		if (replace)
		{
			m_map.erase(it);
			m_map.insert(std::pair<std::string, DependencyTree::Node>(name, node));
			updateOpenLinks(node);
		}
		*/
		std::cerr << "Rule error" << std::endl;
		throw Exception (Exception::SYNTAX_ANALYZER_ERROR);
	}
	else
	{
		m_map.insert(std::pair<std::string, DependencyTree::Node>(name, node));
		updateOpenLinks(node);
	}
}

bool DependencyTree::hasNode(const std::string &name) const
{
	return (m_map.find(name) != m_map.end());
}

DependencyTree::Node &DependencyTree::getNode(const std::string &name)
{
	std::map<std::string, DependencyTree::Node>::iterator it = m_map.find(name);
	if (it == m_map.end())
		throw 1;
	return it->second;
}

void DependencyTree::run(const std::string &rule) const
{
	std::set<std::string> existing_targets;
	std::set<std::string> targets_done;
	try
	{
		if (hasNode(".PHONY"))
			runTree(".PHONY", existing_targets, targets_done, true);
		if (countWords(rule) == 0)
			runTree(m_first_rule, existing_targets, targets_done);
		else
			runTree(rule, existing_targets, targets_done);
	}
	catch ( ... )
	{
		throw;
	}
}

void DependencyTree::buildTemplates(const TemplateBuilder &tmp_builder)
{
	std::set<std::string>::const_iterator it;
	for (it = m_open_links.begin(); it != m_open_links.end(); ++it)
	{
		if (tmp_builder.hasMatching(*it))
			m_map.insert(std::pair<std::string, DependencyTree::Node>(*it, tmp_builder.build(*it)));
	}
}

void DependencyTree::updateOpenLinks(const DependencyTree::Node &node)
{
	std::set<std::string>::iterator c_name = m_open_links.find(node.m_name);
	if (c_name != m_open_links.end())
		m_open_links.erase(c_name);
	std::list<std::string>::const_iterator it;
	for (it = node.m_children.begin(); it != node.m_children.end(); ++it)
	{
		std::map<std::string, DependencyTree::Node>::const_iterator c_dep = m_map.find(*it);
		if (c_dep == m_map.end())
			m_open_links.insert(*it);
	}
}

void DependencyTree::runTree(const std::string &name, std::set<std::string> &existing_targets, std::set<std::string> &targets_done, bool abstract) const
{
	try
	{
		if (targets_done.find(name) != targets_done.end())
			return;
		std::map<std::string, DependencyTree::Node>::const_iterator it = m_map.find(name);
		if (it == m_map.end()) // search for file with such name
		{
			if (fileExists(name))
				return;
			else
				error("Error: \"" + name + "\" is not a target or a file");
		}
		const Node &current = it->second;
		if (existing_targets.find(name) != existing_targets.end())
			error("Error: target \"" + name + "\" depends on itself");
		existing_targets.insert(name);
		bool build_current_node = abstract;
		if (!fileExists(name))
			build_current_node = true;
		std::list<std::string>::const_iterator c_dep;
		for (c_dep = current.m_children.begin(); c_dep != current.m_children.end(); ++c_dep)
		{
			runTree(*c_dep, existing_targets, targets_done, (name == ".PHONY"));
			build_current_node = build_current_node || buildIsNecessary(current.getName(), *c_dep);
		}
		if (build_current_node)
			current.run();
		existing_targets.erase(name);
		targets_done.insert(name);
	}
	catch ( ... )
	{
		std::cerr<<name<<std::endl;
		throw;
	}
}

void DependencyTree::print() const
{
	std::cout<<"Nodes ("<<m_map.size()<<"): "<<std::endl<<std::endl;
	std::map<std::string, DependencyTree::Node>::const_iterator it = m_map.begin();
	while (it != m_map.end())
	{
		const Node &node = it->second;
		std::cout<<"\t"<<node.m_name<<" :";
		for (std::list<std::string>::const_iterator deps = node.m_children.begin(); deps != node.m_children.end(); ++deps)
			std::cout<<""<<(*deps)<<"";
		std::cout<<std::endl;
		for (std::list<std::string>::const_iterator acts = node.m_actions.begin(); acts != node.m_actions.end(); ++acts)
			std::cout<<"\t\t"<<(*acts)<<std::endl;
		++it;
		std::cout<<std::endl;
	}
}

void DependencyTree::error(const std::string &msg, int errcode) const
{
	std::cerr<<msg<<std::endl;
	throw Exception(Exception::RUNTIME_ERROR, errcode);
}
