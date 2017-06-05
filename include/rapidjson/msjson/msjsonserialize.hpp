#ifndef __MSJSONSERIALIZE_H__
#define __MSJSONSERIALIZE_H__

#include "../document.h"
#include "../pointer.h"
#include "../writer.h"
#include <memory>
#include <map>

namespace MSRPC
{
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

	class JsonOArchive_Helper
	{
	public:
		JsonOArchive_Helper(rapidjson::Document::AllocatorType& allocator,
			rapidjson::Value& node, unsigned int version)
			: aAllocator(allocator)
			, vNode(node)
			, uVersion(version) {}


		template <class T>
		JsonOArchive_Helper& io(const char* strName, const T& tValue)
		{
			rapidjson::Value vValue;
			in_serialize(tValue, vValue);
			vNode.AddMember(rapidjson::StringRef(strName), vValue, aAllocator);
			return *this;
		}

		template <class T>
		void set(const T& tValue)
		{
			in_serialize(tValue, vNode);
		}
	protected:
		template <class T>
		void in_serialize(const T& tValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetObject();
			pa_serialize(JsonOArchive_Helper(aAllocator, vCurNode, uVersion), const_cast<T&>(tValue), uVersion);
		}

		template <>
		void in_serialize(const int& tValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetInt(tValue);
		}

		template <>
		void in_serialize(const unsigned int& tValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetUint(tValue);
		}

		template <>
		void in_serialize(const int64_t& tValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetInt64(tValue);
		}

		template <>
		void in_serialize(const uint64_t& tValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetUint64(tValue);
		}

		template <>
		void in_serialize(const double& tValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetDouble(tValue);
		}

		template <>
		void in_serialize(const float& tValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetFloat(tValue);
		}


		template <>
		void in_serialize(const bool& tValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetBool(tValue);
		}

		template <>
		void in_serialize(const std::string& tValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetString(tValue.c_str(), tValue.size(), aAllocator);
		}

		template <template <class> class L, class T>
		void in_serialize(const L<T>& listValue, rapidjson::Value& vCurNode)
		{
			vCurNode.SetArray();

			for (L<T>::const_iterator itor = listValue.begin(); itor != listValue.end(); ++itor)
			{
				rapidjson::Value vValue;
				in_serialize(*itor, vValue);
				vCurNode.PushBack(vValue, aAllocator);
			}
		}

		template <template <class, class> class L, class K, class V>
		void in_serialize(const L<K, V>& mapValue, rapidjson::Value& vCurNode)
		{
 			vCurNode.SetObject();
 
			for (L<K, V>::const_iterator itor = mapValue.begin(); itor != mapValue.end(); ++itor)
 			{
				typedef IterApt<L, K, V> IA;

 				rapidjson::Value vKey;
				ToStrVal(IA::Key(itor), vKey);
 
 				rapidjson::Value vValue;
				in_serialize(IA::Value(itor), vValue);
 
 				vCurNode.AddMember(vKey, vValue, aAllocator);
 			}
		}

	protected:
		template<class T>
		void ToStrVal(const T& val, rapidjson::Value& vCurNode)
		{
			std::ostringstream ss;
			ss << val;
			in_serialize(ss.str(), vCurNode);
		}

		template<>
		void ToStrVal(const std::string& val, rapidjson::Value& vCurNode)
		{
			in_serialize(val, vCurNode);
		}

	private:
		rapidjson::Document::AllocatorType& aAllocator;
		rapidjson::Value& vNode;
		unsigned int uVersion;
	};

	class JsonIArchive_Helper
	{
	public:
		JsonIArchive_Helper(const rapidjson::Value& node, unsigned int version)
			: vNode(node)
			, uVersion(version) {}

		template <class T>
		JsonIArchive_Helper& io(const char* strName, T& tValue)
		{
			rapidjson::Value::ConstMemberIterator itrFind = vNode.FindMember(strName);
			if (itrFind != vNode.MemberEnd())
			{
				in_serialize(itrFind->value, tValue);
			}
			return *this;
		}

		template <class T>
		void get(T& tValue)
		{
			in_serialize(vNode, tValue);
		}

	protected:
		template <class T>
		void in_serialize(const rapidjson::Value& vCurNode, T& tValue)
		{
			pa_serialize(JsonIArchive_Helper(vCurNode, uVersion), tValue, uVersion);
		}

		template <>
		void in_serialize(const rapidjson::Value& vCurNode, int& tValue)
		{
			tValue = vCurNode.GetInt();
		}

		template <>
		void in_serialize(const rapidjson::Value& vCurNode, unsigned int& tValue)
		{
			tValue = vCurNode.GetUint();
		}

		template <>
		void in_serialize(const rapidjson::Value& vCurNode, double& tValue)
		{
			tValue = vCurNode.GetDouble();
		}

		template <>
		void in_serialize(const rapidjson::Value& vCurNode, float& tValue)
		{
			tValue = vCurNode.GetFloat();
		}

		template <>
		void in_serialize(const rapidjson::Value& vCurNode, int64_t& tValue)
		{
			tValue = vCurNode.GetInt64();
		}

		template <>
		void in_serialize(const rapidjson::Value& vCurNode, uint64_t& tValue)
		{
			tValue = vCurNode.GetUint64();
		}

		template <>
		void in_serialize(const rapidjson::Value& vCurNode, bool& tValue)
		{
			tValue = vCurNode.GetBool();
		}

		template <>
		void in_serialize(const rapidjson::Value& vCurNode, std::string& tValue)
		{
			tValue = vCurNode.GetString();
		}

		template <template <class> class L, class T>
		void in_serialize(const rapidjson::Value& vCurNode, L<T>& listValue)
		{
			for (rapidjson::Value::ConstValueIterator itr = vCurNode.Begin();
				itr != vCurNode.End(); ++itr)
			{
				T tValue;
				in_serialize(*itr, tValue);
				listValue.push_back(tValue);
			}
		}

		template <template <class, class> class L, class K, class V>
		void in_serialize(const rapidjson::Value& vCurNode, L<K, V>& mapValue)
		{
			for (rapidjson::Value::ConstMemberIterator itor = vCurNode.MemberBegin();
				itor != vCurNode.MemberEnd(); ++itor)
			{
				K key;
				FromStrVal(itor->name, key);
				
				V value;
				in_serialize(itor->value, value);

				mapValue[key] = value;
			}

		}
	protected:
		template<class T>
		void FromStrVal(const rapidjson::Value& vCurNode, T& val)
		{
			std::istringstream ss(vCurNode.GetString());
			ss >> val;
		}

		template<>
		void FromStrVal(const rapidjson::Value& vCurNode, std::string& val)
		{
			in_serialize(vCurNode, val);
		}

	private:
		const rapidjson::Value& vNode;
		unsigned int uVersion;
	};

	typedef std::tr1::shared_ptr<rapidjson::Document> SPtrDocument;
	typedef rapidjson::Value* PtrValue;
	
	class JsonOArchive
	{
	public:
		JsonOArchive(unsigned int version = 0)
			: spRoot(new rapidjson::Document)
			, pNode(spRoot.get())
			, uVersion(version)
		{

		}

		JsonOArchive(const SPtrDocument& spDocument, PtrValue pValue, unsigned int version = 0)
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

		template <class T>
		JsonOArchive& operator << (const T& tValue)
		{
			JsonOArchive_Helper helper(spRoot->GetAllocator(), *pNode, uVersion);
			helper.set(tValue);
			return *this;
		}

		template<class T>
		void Print(T& data)
		{
			rapidjson::Writer<T> writer(data);
			pNode->Accept(writer);
		}

	private:
		SPtrDocument spRoot;
		PtrValue pNode;
		unsigned int uVersion;
	};

	class JsonIArchive
	{
	public:
		JsonIArchive(char* szJson, unsigned int version = 0)
			: spRoot(new rapidjson::Document)
			, uVersion(version)
		{
			spRoot->Parse<0>(szJson);
			pNode = spRoot.get();
		}

		JsonIArchive(const SPtrDocument& spDocument, PtrValue pValue, unsigned int version = 0)
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

		template <class T>
		const JsonIArchive& operator >> (T& tValue) const
		{
			JsonIArchive_Helper helper(*pNode, uVersion);
			helper.get(tValue);
			return *this;
		}

	private:
		SPtrDocument spRoot;
		PtrValue pNode;
		unsigned int uVersion;
	};
}


#endif // __MSJSONSERIALIZE_H__
