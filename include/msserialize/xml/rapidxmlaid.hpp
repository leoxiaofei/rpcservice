#ifndef __RAPIDXMLAID_H__
#define __RAPIDXMLAID_H__

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include <sstream>
#include <memory>
#include <iostream>

class RapidNode
{
public:
	RapidNode()
	: m_pCurNode(NULL)
	{

	}

	static RapidNode CreateRoot()
	{
		RapidNode node;
		node.InitDoc();
		return node;
	}

	static RapidNode ParseRoot(char* szData)
	{
		RapidNode node;
		node.InitDoc();
		node.Parse(szData);
		return node;
	}

	void Parse(char* szData)
	{
		try
		{
			m_pDoc->parse<0>(szData);
			m_pCurNode = m_pDoc.get();
		}
		catch (const rapidxml::parse_error& e)
		{
			std::cout<< "RapidNode::Parse" <<" " << e.what() <<std::endl;
		}
	}

	template<class T>
	void Print(T& data)
	{
		rapidxml::print(std::back_inserter(data), *m_pCurNode, 0);
	}
	
	operator bool () const
	{
		return m_pDoc && m_pCurNode;
	}
	
	RapidNode CreateElement(const char* szName)
	{
		rapidxml::xml_node<>* pNode = 
			m_pDoc->allocate_node(rapidxml::node_element, szName);

		m_pCurNode->append_node(pNode);

		return RapidNode (m_pDoc, pNode);
	}

	RapidNode CreateElement(const char* szName, const char* szComment)
	{
		m_pCurNode->append_node(m_pDoc->allocate_node(rapidxml::node_comment, NULL, szComment));  

		rapidxml::xml_node<>* pNode = 
			m_pDoc->allocate_node(rapidxml::node_element, szName);

		m_pCurNode->append_node(pNode);

		return RapidNode (m_pDoc, pNode);
	}

	RapidNode FirstSubElement(const char* szName = 0) const
	{
		return RapidNode(m_pDoc, m_pCurNode->first_node(szName));
	}

	RapidNode NextElement(const char* szName = 0) const
	{
		return RapidNode(m_pDoc, m_pCurNode->next_sibling(szName));
	}

	void SetAttribute(const char* szName, const char* szVar)
	{
		m_pCurNode->append_attribute(m_pDoc->allocate_attribute(szName, szVar));
	}

	const char* Attribute(const char* szName) const
	{
		rapidxml::xml_attribute<>* p = m_pCurNode->first_attribute(szName);
		return p ? p->value() : 0;
	}

	const char* Attribute(const char* szName, const char* szVar) const
	{
		rapidxml::xml_attribute<>* p = m_pCurNode->first_attribute(szName);
		return p ? p->value() : szVar;
	}

	bool HasAttribute(const char* szName ) const
	{
		rapidxml::xml_attribute<>* p = m_pCurNode->first_attribute(szName);
		return p != NULL;
	}

	template <class T>
	void SetAttribute(const char* szName, const T& strVal)
	{
		std::string str;
		ToString(str, strVal);
		m_pCurNode->append_attribute(m_pDoc->allocate_attribute(szName,
			m_pDoc->allocate_string(str.c_str(), str.size() + 1)));
	}

#ifdef QT_VERSION
	template <>
	void SetAttribute(const char* szName, const QString& strVal)
	{
		QByteArray ba = strVal.toUtf8();
		m_pCurNode->append_attribute(m_pDoc->allocate_attribute(szName,
			m_pDoc->allocate_string(ba.data(), ba.size() + 1)));
	}
#endif

	template <>
	void SetAttribute(const char* szName, const bool& strVal)
	{
		m_pCurNode->append_attribute(m_pDoc->allocate_attribute(szName,
			strVal ? "true" : "false"));
	}


	template <class T>
	T Attribute(const char* szName, const T& strVal) const
	{
		rapidxml::xml_attribute<>* p = m_pCurNode->first_attribute(szName);
		return p ? ToValue<T>(p->value()) : strVal;
	}

#ifdef QT_VERSION
	template <>
	QString Attribute(const char* szName, const QString& strVal) const
	{
		rapidxml::xml_attribute<>* p = m_pCurNode->first_attribute(szName);
		return p ? QString::fromUtf8(p->value(), p->value_size()) : strVal;
	}
#endif

	template <>
	bool Attribute(const char* szName, const bool& strVal) const
	{
		rapidxml::xml_attribute<>* p = m_pCurNode->first_attribute(szName);
		return p ? (strcmp(p->value(), "true") == 0 ? true : strcmp(p->value(), "1") == 0) : strVal;
	}

	char* Name() const
	{
		return m_pCurNode->name();
	}


	void CreateDeclaration(const char* szVer = "1.0", const char* szEcd = "utf-8")
	{
		rapidxml::xml_node<>* decl = m_pDoc->allocate_node(rapidxml::node_declaration);
		decl->append_attribute(m_pDoc->allocate_attribute("version", szVer));
		decl->append_attribute(m_pDoc->allocate_attribute("encoding", szEcd));
		m_pDoc->append_node(decl);
	}

	std::tr1::shared_ptr<rapidxml::xml_document<> > GetDoc() const
	{
		return m_pDoc;
	}

	rapidxml::xml_node<>* GetCurNode() const
	{
		return m_pCurNode;
	}
	
protected:
	RapidNode(const std::tr1::shared_ptr<rapidxml::xml_document<> >& pDoc,
		rapidxml::xml_node<>* pCurrent)
	: m_pDoc(pDoc) 
	, m_pCurNode(pCurrent)
	{

	}

	void InitDoc()
	{
		m_pDoc = std::tr1::shared_ptr<rapidxml::xml_document<> >(new rapidxml::xml_document<>);
		m_pCurNode = m_pDoc.get();
	}

	template <class T>
	static void ToString(std::string& str, const T& var)
	{
		std::ostringstream ss;
		ss << var;
		str = ss.str();
	}

	template <class T>
	static T ToValue(const char* szValue)
	{
		T val;
		std::istringstream ss(szValue);
		ss >> val;
		return val;
	}

private:
	std::tr1::shared_ptr<rapidxml::xml_document<> > m_pDoc;
	rapidxml::xml_node<>* m_pCurNode;
};



#endif // __RAPIDXMLAID_H__
