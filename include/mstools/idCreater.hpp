#ifndef IDCREATER_H__
#define IDCREATER_H__


template <typename T>
class IdCreater
{
public:
    IdCreater(const T& start) : m_digit(start) {}
    ~IdCreater(){}

    T operator()()
    {
        return ++m_digit;
    }
    
    const T& CurID()
    {
        return m_digit;
    }

	void Touch(const T& value)
	{
		m_digit = (m_digit < value) ? value : m_digit;
	}

private:
    T m_digit;
};

#endif // IDCREATER_H__
