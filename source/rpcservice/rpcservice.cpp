#include "rpcservice.h"
#include <iostream>
#include <vector>
#include "msserialize\xml\msxmlserialize.hpp"


class Note
{
public:
	std::string strName;
};

class Item
{
public:
	double dDeci;
};

class Test
{
public:
	int a;
	bool b;
	Note note;
	std::vector<Item> vecItem;
};

namespace MSRPC
{
	template<class Archive>
	void pa_serialize(Archive& ar, Note& value, int uVersion)
	{
		ar.io("name", value.strName);
	}

	template<class Archive>
	void pa_serialize(Archive& ar, Item& value, int uVersion)
	{
		ar.io("Deci", value.dDeci);
	}

	template<class Archive>
	void pa_serialize(Archive& ar, Test& value, int uVersion)
	{
		ar.io("a", value.a);
		ar.io("b", value.b);
		ar.io("note", value.note);
		ar.io("Item", value.vecItem);
	}
}

RpcService::RpcService()
{
	Test t;
	t.a = 100;
	t.b = true;

	t.note.strName = "AAABBB";

	Item item;
	for (int ix = 0; ix != 5; ++ix)
	{
		item.dDeci = ix * 1.25;
		t.vecItem.push_back(item);
	}

	MSRPC::XmlOArchive a;

	a.SetRootElement("Test");
	
	a << t;

	std::string strXml;
	a.Print(strXml);

	std::cout << strXml << std::endl;

	MSRPC::XmlIArchive i((char*)strXml.c_str());

	i.GetRootElement();

	Test t2;

	i >> t2;

	std::cout << t2.note.strName << std::endl;

}
