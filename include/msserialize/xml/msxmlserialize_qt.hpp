#ifndef __MSJSONSERIALIZE_QT_H__
#define __MSJSONSERIALIZE_QT_H__


#include "msxmlserialize.hpp"

#include <QString>
#include <QSize>
#include <QPoint>
#include <QMap>
#include <QHash>
#include <QStringList>

namespace MSRPC
{
	template <>
	inline void XmlIArchive_Helper::in_serialize(const char* strName, QString& tValue)
	{
		if(xml_attribute<char>* attr = vNode.first_attribute(strName))
		{
			tValue = QString::fromUtf8(attr->value(), attr->value_size());
		}
	}

	template <>
	inline void XmlOArchive_Helper::in_serialize(const char* strName, const QString& tValue)
	{
		QByteArray ba = tValue.toUtf8();

		vNode.append_attribute(aAllocator.allocate_attribute(strName, 
			aAllocator.allocate_string(ba.data(), ba.size() + 1)));
	}

// 	template <>
// 	inline void XmlIArchive_Helper::in_serialize(const rapidjson::Value& vCurNode, QStringList& tValue)
// 	{
// 		in_serialize(vCurNode, (QList<QString>&)tValue);
// 	}
// 
// 	template <>
// 	inline void XmlOArchive_Helper::in_serialize(const QStringList& tValue, rapidjson::Value& vCurNode)
// 	{
// 		in_serialize((const QList<QString>&)tValue, vCurNode);
// 	}

 
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
 	inline void XmlIArchive_Helper::in_serialize(const char* strName, QSize& tValue)
 	{
		QString strValue;
		in_serialize(strName, strValue);
		QStringList list = strValue.split(",");
		if (list.size() > 1)
		{
			tValue.setWidth(list[0].toInt());
			tValue.setHeight(list[1].toInt());
		}
 	}
 
 	template <>
 	inline void XmlOArchive_Helper::in_serialize(const char* strName, const QSize& tValue)
 	{
		in_serialize(strName, QString("%1,%2").arg(tValue.width()).arg(tValue.height()));
 	}
 
	template <>
	inline void XmlIArchive_Helper::in_serialize(const char* strName, QPoint& tValue)
	{
		QString strValue;
		in_serialize(strName, strValue);
		QStringList list = strValue.split(",");
		if (list.size() > 1)
		{
			tValue.setX(list[0].toInt());
			tValue.setY(list[1].toInt());
		}
	}

	template <>
	inline void XmlOArchive_Helper::in_serialize(const char* strName, const QPoint& tValue)
	{
		in_serialize(strName, QString("%1,%2").arg(tValue.x()).arg(tValue.y()));
	}

// 	template<class K, class V>
// 	class IterApt<QMap, K, V>
// 	{
// 	public:
// 		static K& Key(typename QMap<K,V>::iterator& itor)
// 		{
// 			return itor.key();
// 		}
// 
// 		static V& Value(typename QMap<K,V>::iterator& itor)
// 		{
// 			return itor.value();
// 		}
// 
// 		static const K& Key(typename QMap<K,V>::const_iterator& itor)
// 		{
// 			return itor.key();
// 		}
// 
// 		static const V& Value(typename QMap<K,V>::const_iterator& itor)
// 		{
// 			return itor.value();
// 		}
// 	};
// 
// 	template<class K, class V>
// 	class IterApt<QHash, K, V>
// 	{
// 	public:
// 		static K& Key(typename QHash<K,V>::iterator& itor)
// 		{
// 			return itor.key();
// 		}
// 
// 		static V& Value(typename QHash<K,V>::iterator& itor)
// 		{
// 			return itor.value();
// 		}
// 
// 		static const K& Key(typename QHash<K,V>::const_iterator& itor)
// 		{
// 			return itor.key();
// 		}
// 
// 		static const V& Value(typename QHash<K,V>::const_iterator& itor)
// 		{
// 			return itor.value();
// 		}
// 	};



}

#endif // __MSJSONSERIALIZE_QT_H__