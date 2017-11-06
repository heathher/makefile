#include "template_builder.h"
#include <iostream>

TemplateBuilder::Node::Node(const std::string &name) : m_name(name)
{

}

TemplateBuilder::Node::~Node()
{

}

void TemplateBuilder::Node::addDependency(const std::string &dep)
{
	m_dependences.push_back(dep);
}

void TemplateBuilder::Node::addAction(const std::string &lex)
{
	m_actions.push_back(lex);
}

bool TemplateBuilder::Node::isMatching(const std::string &name) const
{
	if (name.find('%') != std::string::npos)
		return false;
	unsigned start_pos = 0;
	while (m_name[start_pos] == name[start_pos])
		start_pos++;
	unsigned end_pos = 1;
	unsigned m_len = m_name.length(), len = name.length();
	while (m_name[m_len - end_pos] == name[len - end_pos])
		end_pos++;
	if (start_pos == m_len - end_pos)
	{
		if (m_name[start_pos] == '%')
			return true;
	}
	return false;
}

DependencyTree::Node TemplateBuilder::Node::build(const std::string &name) const
{
	if (name.find('%') != std::string::npos)
	{
		std::cerr<<"Error: couldn't build template \""<<m_name
		         <<"\" to match another template (\""<<name<<"\")"<<std::endl;
		throw 1; //TODO: exception
	}
	unsigned start_pos = 0;
	while (m_name[start_pos] == name[start_pos])
		start_pos++;
	unsigned end_pos = 1;
	unsigned m_len = m_name.length(), len = name.length();
	while (m_name[m_len - end_pos] == name[len - end_pos])
		end_pos++;
	if ((start_pos != m_len - end_pos) || (m_name[start_pos] != '%'))
	{
		std::cerr<<"Error: couldn't build template \""<<m_name
		         <<"\" to match name \""<<name<<"\""<<std::endl;
		throw 1; //TODO: exception
	}
	std::string percent_char = name.substr(start_pos, len - end_pos - start_pos + 1);
	DependencyTree::Node result(name);
	for (std::list<std::string>::const_iterator i = m_dependences.begin(); i != m_dependences.end(); i++)
	{
		size_t prct_pos;
		std::string dep_val = *i;
		while ((prct_pos = dep_val.find('%')) != std::string::npos)
			dep_val.replace(prct_pos, 1, percent_char);
		result.addDependency(dep_val);
	}
	for (std::list<std::string>::const_iterator i = m_actions.begin(); i != m_actions.end(); i++)
	{
		size_t prct_pos;
		std::string act_val = *i;
		while ((prct_pos = act_val.find('%')) != std::string::npos)
			act_val.replace(prct_pos, 1, percent_char);
		result.addAction(act_val);
	}
	return result;
}

void TemplateBuilder::addTemplate(const Node &node)
{
	m_templates.push_back(node);
}
bool TemplateBuilder::hasMatching(const std::string &name) const
{
	std::list<TemplateBuilder::Node>::const_iterator it;
	for (it = m_templates.begin(); it != m_templates.end(); ++it)
	{
		if (it->isMatching(name))
			return true;
	}
	return false;
}
DependencyTree::Node TemplateBuilder::build(const std::string &name) const
{
	std::list<TemplateBuilder::Node>::const_iterator it;
	for (it = m_templates.begin(); it != m_templates.end(); ++it)
	{
		if (it->isMatching(name))
			break;
	}
	if (it == m_templates.end())
	{
		std::cerr<<"Error: string \""<<name<<"\" doesn't match any template"<<std::endl;
		throw 1; //TODO: exceptions: no matching templates
	}
	return it->build(name);
}