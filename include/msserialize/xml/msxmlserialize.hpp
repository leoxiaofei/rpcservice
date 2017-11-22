#ifndef MSXMLSERIALIZE_H__
#define MSXMLSERIALIZE_H__

#include "..\..\rapidxml\rapidxml.hpp"
#include "..\..\rapidxml\rapidxml_print.hpp"
#include "..\msinttypes\stdint.h"

#include <memory>
#include <map>
#include <sstream>
#include <locale>


namespace MSRPC
{
	using rapidxml::xml_attribute;
	using rapidxml::xml_node;
	using rapidxml::xml_document;

	template<template <class, class> class L, class K, class V>
	class IterApt
	{
	public:
		static K& Key(typename L<K,V>::iterator& itor)
		{
			return itor->first;
		}

		static V& Value(typename L<K,V>::iterator& itor)
		{
			return itor->second;
		}

		static const K& Key(typename L<K,V>::const_iterator& itor)
		{
			return itor->first;
		}

		static const V& Value(typename L<K,V>::const_iterator& itor)
		{
			return itor->second;
		}
	};

	class XmlOArchive_Helper
	{
	public:
		XmlOArchive_Helper(xml_document<>& allocator,
			xml_node<>& node, unsigned int version)
			: aAllocator(allocator)
			, vNode(node)
			, uVersion(version) {}


		template <class T>
		XmlOArchive_Helper& io(const char* strName, const T& tValue)
		{
			in_serialize(strName, tValue);
			return *this;
		}

		template <class T>
		void set(const T& tValue)
		{
 			pa_serialize(*this, const_cast<T&>(tValue), uVersion);
		}

	protected:
		template <class T>
		void in_serialize(const char* strName, const T& tValue)
		{
			xml_node<char>* node = aAllocator.allocate_node(rapidxml::node_element, strName);
 			pa_serialize(XmlOArchive_Helper(aAllocator, *node, uVersion), const_cast<T&>(tValue), uVersion);
			vNode.append_node(node);
		}

		template <>
		void in_serialize(const char* strName, const int& tValue)
		{
			ToStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, const unsigned int& tValue)
		{
			ToStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, const int64_t& tValue)
		{
			ToStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, const uint64_t& tValue)
		{
			ToStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, const double& tValue)
		{
			ToStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, const float& tValue)
		{
			ToStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, const bool& tValue)
		{
			ToStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, const std::string& tValue)
		{
			ToStrVal(strName, tValue);
		}

		void in_serialize(const char* strName, const char* tValue)
		{
			ToStrVal(strName, tValue);
		}

		template <template <class, class> class L, class T, class A>
		void in_serialize(const char* strName, const L<T, A>& listValue)
		{
			for (L<T, A>::const_iterator itor = listValue.begin(); itor != listValue.end(); ++itor)
			{
				xml_node<char>* node = aAllocator.allocate_node(rapidxml::node_element, strName);
				pa_serialize(XmlOArchive_Helper(aAllocator, *node, uVersion), const_cast<T&>(*itor), uVersion);
				vNode.append_node(node);
			}
		}

// 		template <template <class, class> class L, class K, class V>
// 		void in_serialize(const L<K, V>& mapValue, xml_attribute<>& vCurNode)
// 		{
//  			vCurNode.SetObject();
//  
// 			for (L<K, V>::const_iterator itor = mapValue.begin(); itor != mapValue.end(); ++itor)
//  			{
// 				typedef IterApt<L, K, V> IA;
// 
//  				xml_node<> vKey;
// 				ToStrVal(IA::Key(itor), vKey);
//  
//  				xml_node<> vValue;
// 				in_serialize(IA::Value(itor), vValue);
//  
//  				vCurNode.AddMember(vKey, vValue, aAllocator);
//  			}
// 		}

	protected:
		template<class T>
		inline void ToStrVal(const char* strName, const T& val)
		{
			std::ostringstream ss;
			ss << val;
			ToStrVal(strName, ss.str());
		}

		template<>
		inline void ToStrVal(const char* strName, const bool& val)
		{
			ToStrVal(strName, val ? "true" : "false");
		}

		template<>
		inline void ToStrVal(const char* strName, const std::string& val)
		{
			vNode.append_attribute(aAllocator.allocate_attribute(strName, 
				aAllocator.allocate_string(val.c_str(), val.size() + 1)));
		}
		
		inline void ToStrVal(const char* strName, const char* tValue)
		{
			vNode.append_attribute(aAllocator.allocate_attribute(strName, tValue));
		}

	private:
		xml_document<>& aAllocator;
		xml_node<>& vNode;
		unsigned int uVersion;
	};

	class XmlIArchive_Helper
	{
	public:
		XmlIArchive_Helper(const xml_node<>& node, unsigned int version)
			: vNode(node)
			, uVersion(version) {}

		template <class T>
		XmlIArchive_Helper& io(const char* strName, T& tValue)
		{
			in_serialize(strName, tValue);
			return *this;
		}

		template <class T>
		void get(T& tValue)
		{
			pa_serialize(*this, tValue, uVersion);
		}

	protected:
		template <class T>
		void in_serialize(const char* strName, T& tValue)
		{
			if (const xml_node<>* vCurNode = vNode.first_node(strName, 0, false))
			{
 				pa_serialize(XmlIArchive_Helper(*vCurNode, uVersion), tValue, uVersion);
			}
		}

		///提供基础数据类型的序列化功能。
		template <>
		void in_serialize(const char* strName, int& tValue)
		{
			FromStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, unsigned int& tValue)
		{
			FromStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, double& tValue)
		{
			FromStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, float& tValue)
		{
			FromStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, int64_t& tValue)
		{
			FromStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, uint64_t& tValue)
		{
			FromStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, bool& tValue)
		{
			FromStrVal(strName, tValue);
		}

		template <>
		void in_serialize(const char* strName, std::string& tValue)
		{
			FromStrVal(strName, tValue);
		}

		template <template <class,class> class L, class T, class A>
		void in_serialize(const char* strName, L<T, A>& listValue)
		{
			int len = strlen(strName);

			for (const xml_node<>* vCurNode = vNode.first_node(strName, len, false);
			vCurNode; vCurNode = vCurNode->next_sibling(strName, len, false))
			{
				T tValue;
				pa_serialize(XmlIArchive_Helper(*vCurNode, uVersion), tValue, uVersion);
				//in_serialize(*itr, tValue);
				listValue.push_back(tValue);
			}
		}

// 		template <template <class, class> class L, class K, class V>
// 		void in_serialize(const char* strName, L<K, V>& mapValue)
// 		{
// 			for (xml_node<>::ConstMemberIterator itor = vCurNode.MemberBegin();
// 				itor != vCurNode.MemberEnd(); ++itor)
// 			{
// 				K key;
// 				FromStrVal(itor->name, key);
// 				
// 				V value;
// 				in_serialize(itor->value, value);
// 
// 				mapValue[key] = value;
// 			}
// 
// 		}

	protected:
		template<class T>
		inline void FromStrVal(const char* strName, T& val)
		{
			if(xml_attribute<char>* attr = vNode.first_attribute(strName))
			{
				std::istringstream ss(attr->value());
				ss >> val;
			}
		}

		template<>
		inline void FromStrVal(const char* strName, bool& val)
		{
			if(xml_attribute<char>* attr = vNode.first_attribute(strName))
			{
				val = (strcmp(attr->value(), "true") == 0);
			}
		}

		template<>
		inline void FromStrVal(const char* strName, std::string& val)
		{
			if(xml_attribute<char>* attr = vNode.first_attribute(strName))
			{
				val = attr->value();
			}
		}

	private:
		const xml_node<>& vNode;
		unsigned int uVersion;
	};

	typedef std::tr1::shared_ptr<xml_document<>> SPtrDocument;
	typedef xml_node<>* PtrValue;
	
	class XmlOArchive
	{
	public:
		XmlOArchive(unsigned int version = 0)
			: spRoot(new xml_document<>)
			, pNode(spRoot.get())
			, uVersion(version)
		{

		}

		XmlOArchive(const SPtrDocument& spDocument, PtrValue pValue, unsigned int version = 0)
			: spRoot(spDocument)
			, pNode(pValue)
			, uVersion(version)
		{
			
		}

		SPtrDocument& GetRoot()
		{
			return spRoot;
		}

		unsigned int GetVersion() const
		{
			return uVersion;
		}

		void SetCurNode(PtrValue pValue)
		{
			pNode = pValue;
		}

		PtrValue GetCurNode() const
		{
			return pNode;
		}

		void SetRootElement(const char* szName)
		{
			pNode = spRoot->allocate_node(rapidxml::node_element, szName);
			spRoot->append_node(pNode);
		}

		template <class T>
		XmlOArchive& operator << (const T& tValue)
		{
			XmlOArchive_Helper helper(*spRoot.get(), *pNode, uVersion);
			helper.set(tValue);
			return *this;
		}

		template<class T>
		void Print(T& data)
		{
			rapidxml::print(std::back_inserter(data), *pNode, 0);
		}

	private:
		SPtrDocument spRoot;
		PtrValue pNode;
		unsigned int uVersion;
	};

	class XmlIArchive
	{
	public:
		XmlIArchive(char* szXml, unsigned int version = 0)
			: spRoot(new xml_document<>)
			, uVersion(version)
		{
			spRoot->parse<0>(szXml);
			pNode = spRoot.get();
		}

		XmlIArchive(const SPtrDocument& spDocument, PtrValue pValue, unsigned int version = 0)
			: spRoot(spDocument)
			, pNode(pValue)
			, uVersion(version)
		{
		}

		const SPtrDocument& GetRoot() const
		{
			return spRoot;
		}

		void SetCurNode(PtrValue pValue)
		{
			pNode = pValue;
		}
		
		PtrValue GetCurNode() const
		{
			return pNode;
		}

		void GetRootElement(const char* szName = 0)
		{
			pNode = spRoot->first_node(szName);
		}

		template <class T>
		const XmlIArchive& operator >> (T& tValue) const
		{
			XmlIArchive_Helper helper(*pNode, uVersion);
			helper.get(tValue);
			return *this;
		}

	private:
		SPtrDocument spRoot;
		PtrValue pNode;
		unsigned int uVersion;
	};
}


#endif // MSXMLSERIALIZE_H__
