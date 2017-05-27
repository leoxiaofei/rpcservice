#ifndef __MSJSONSERIALIZE_QT_H__
#define __MSJSONSERIALIZE_QT_H__


#include "msjsonserialize.hpp"

#include <QString>
#include <QSize>
#include <QPoint>
#include <QMap>
#include <QHash>

namespace MSRPC
{
	template <>
	inline void JsonIArchive_Helper::in_serialize(const rapidjson::Value& vCurNode, QString& tValue)
	{
		tValue = QString::fromUtf8(vCurNode.GetString(), vCurNode.GetStringLength());
	}

	template <>
	inline void JsonOArchive_Helper::in_serialize(const QString& tValue, rapidjson::Value& vCurNode)
	{
		QByteArray ba = tValue.toUtf8();
		vCurNode.SetString(ba.data(), ba.size(), aAllocator);
	}

	namespace
	{
		enum ElemType {
			ET_WIDTH,
			ET_HEIGHT,
			ET_X,
			ET_Y,
		};

		const char* szElemText[] = {
			"width",
			"height",
			"x",
			"y",
		};
	}

	template <>
	inline void JsonIArchive_Helper::in_serialize(const rapidjson::Value& vCurNode, QSize& tValue)
	{
		tValue.setWidth(vCurNode[szElemText[ET_WIDTH]].GetInt());
		tValue.setHeight(vCurNode[szElemText[ET_HEIGHT]].GetInt());
	}

	template <>
	inline void JsonOArchive_Helper::in_serialize(const QSize& tValue, rapidjson::Value& vCurNode)
	{
		vCurNode.SetObject();
		vCurNode.AddMember(rapidjson::StringRef(szElemText[ET_WIDTH]), tValue.width(), aAllocator);
		vCurNode.AddMember(rapidjson::StringRef(szElemText[ET_HEIGHT]), tValue.height(), aAllocator);
	}

	template<>
	inline void JsonOArchive_Helper::ToStrVal(const QString& val, rapidjson::Value& vCurNode)
	{
		in_serialize(val, vCurNode);
	}

	template<>
	inline void JsonIArchive_Helper::FromStrVal(const rapidjson::Value& vCurNode, QString& val)
	{
		in_serialize(vCurNode, val);
	}

	template<class K, class V>
	class IterApt<QMap, K, V>
	{
	public:
		static K& Key(typename QMap<K,V>::iterator& itor)
		{
			return itor.key();
		}

		static V& Value(typename QMap<K,V>::iterator& itor)
		{
			return itor.value();
		}

		static const K& Key(typename QMap<K,V>::const_iterator& itor)
		{
			return itor.key();
		}

		static const V& Value(typename QMap<K,V>::const_iterator& itor)
		{
			return itor.value();
		}
	};

	template<class K, class V>
	class IterApt<QHash, K, V>
	{
	public:
		static K& Key(typename QHash<K,V>::iterator& itor)
		{
			return itor.key();
		}

		static V& Value(typename QHash<K,V>::iterator& itor)
		{
			return itor.value();
		}

		static const K& Key(typename QHash<K,V>::const_iterator& itor)
		{
			return itor.key();
		}

		static const V& Value(typename QHash<K,V>::const_iterator& itor)
		{
			return itor.value();
		}
	};


	template <>
	inline void JsonIArchive_Helper::in_serialize(const rapidjson::Value& vCurNode, QPoint& tValue)
	{
		tValue.setX(vCurNode[szElemText[ET_X]].GetInt());
		tValue.setY(vCurNode[szElemText[ET_Y]].GetInt());
	}

	template <>
	inline void JsonOArchive_Helper::in_serialize(const QPoint& tValue, rapidjson::Value& vCurNode)
	{
		vCurNode.SetObject();
		vCurNode.AddMember(rapidjson::StringRef(szElemText[ET_X]), tValue.x(), aAllocator);
		vCurNode.AddMember(rapidjson::StringRef(szElemText[ET_Y]), tValue.y(), aAllocator);
	}
}

#endif // __MSJSONSERIALIZE_QT_H__