#ifndef __SINGLETEMPLATE_H__
#define __SINGLETEMPLATE_H__


template<class T>
class SingleTemplate
{
public:
	static T& Instance()
	{
		static T theSingleton;
		return theSingleton;
	}

	typedef T CLASS;
};


#endif // __SINGLETEMPLATE_H__