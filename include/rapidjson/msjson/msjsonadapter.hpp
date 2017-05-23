#ifndef __MSJSONADAPTER_H__
#define __MSJSONADAPTER_H__

#include <QByteArray>

namespace MSRPC
{
	class QByteArrayAdapter
	{
	public:
		QByteArrayAdapter(QByteArray& ba) 
			: baData(ba) {}

		void Put(char c) { baData.append(c); }
		void Flush() { }

		void Clear() { baData.clear(); }
		void ShrinkToFit() {
			// Push and pop a null terminator. This is safe.
			baData.squeeze();
		}
		char* Push(size_t count) {
			int nCurSize = baData.size();
			baData.resize(nCurSize + count);
			return baData.data() + nCurSize;
		}
		void Pop(size_t count) { baData.chop(count); }

		const char* GetString() const {
			// Push and pop a null terminator. This is safe.
			baData.append('\0');
			baData.chop(1);

			return baData.data();
		}

		size_t GetSize() const { return baData.size(); }
		
	private:
		QByteArray& baData;
	};
}


#endif // __MSJSONADAPTER_H__
