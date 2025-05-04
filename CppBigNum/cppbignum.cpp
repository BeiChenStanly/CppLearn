#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <deque>
#include <string>
#include <stdexcept>
#include <sstream>
/*
д��ǰ�棺
ע��size_t��unsigned���ͣ���Ҫ���з������ͻ��ã�
digits��int8_t�洢��Ϊ�˽�ʡ�ռ䣬ʵ����char�����ʱ�ǵ�����ת��
��ñ�ĳ�using namespace std;�����Լ�using std::cout֮��ģ���ҪC++17��
��ѧ�������Ķ��壺x=a��10^b,1<=a<10
*/
class BigNum//��ѧ������
{
private:
    std::deque<int8_t> digits;//����λ������洢��
    long long exponent;//ָ��
    bool isNegative;//����

    /*aid functions*/
    void normalize()//��׼����������ǰ��0��ʹ1.1451400e2���1.14514e2��0.114514e3���1.14514e2
    {
        while (!digits.empty() && digits.front() == 0)
        {//ȥ��0��ע��洢�ǵ���ģ����������Ǻ�
            digits.pop_front();
        }
        while (!digits.empty() && digits.back() == 0)
        {//ȥ��ǰ��0
            digits.pop_back();
            --exponent;//ȥ��ǰ��0�ȼ��ڰѵ�һλ���ұ��ƣ���Ϊ��һλ���ұ��ƣ�����ָ����Լ�һ
        }
        if (digits.empty())
        {
            digits.push_back(0);
            exponent = 0;
            isNegative = false;
        }
    }
    static void alignexponent(BigNum& a,BigNum& b)//������0�Զ���ָ��
    {
        while (a.exponent < b.exponent) {
            a.digits.push_back(0);
            a.exponent++;
        }
        while (b.exponent < a.exponent) {
            b.digits.push_back(0);
            b.exponent++;
        }
        while (a.GetSize() < b.GetSize())
        {
            a.digits.push_front(0);
        }
        while (b.GetSize() < a.GetSize())
        {
            b.digits.push_front(0);
        }
    }

public:
    BigNum()
    {
        isNegative = 0;
        exponent = 0;
        digits.clear();
        digits.resize(1);
        digits[0] = 0;
    }
    explicit BigNum(const std::string& s)
    {
        //��ʼ������
        size_t start = 0;
        isNegative = bool(s[0] == '-');
        if (isNegative || s[0] == '+')
            ++start;
        size_t ePos = s.find_first_of("Ee");//ע�����find_first_of��ָEe�е�����һ����һ������

        //����e��ָ��
        if (ePos != std::string::npos)//���������1.14514e5
        {//ע�⵽11.4514e4����������Ҳ�п��ܣ�����ָ��֮����Ҫ��
            exponent = std::stoi(s.substr(ePos + 1, s.size() - ePos - 1));
        }
        else//����������11.4514
        {
            exponent = 0;//����ֻ��e���ʼֵ
        }

        //��ʼ����λ��ָ��
        std::string absoluteValue = s.substr(start, ePos == std::string::npos ? s.size() - start : ePos - start);//��npos�ȽϿ���֪���Ƿ��ҵ���ȥ�����ź�e֮���ָ��
        size_t pointPos = absoluteValue.find('.');//С����
        if (pointPos != std::string::npos)
        {
            absoluteValue.erase(pointPos, 1);
        }
        else
        {
            pointPos = absoluteValue.size();
        }
        exponent += pointPos - 1;
        digits.resize(absoluteValue.size());
        for (size_t i = 0;i < absoluteValue.size();++i)
        {
            digits[i] = absoluteValue[absoluteValue.size() - 1 - i]-'0';
        }
        normalize();//��׼��
    }
    explicit BigNum(int num)
    {
        isNegative = num < 0;
        if (isNegative) num = -num;
        exponent = floor(log10(num));
        for (size_t i = 0;i <= exponent;i++)//��ΪҪ��һλ�����Զ�ѭ��һ��
        {
            digits.push_back(num % 10);
            num /= 10;
        }
    }
    explicit BigNum(long long num)
    {
        isNegative = num < 0;
        if (isNegative) num = -num;
        exponent = floor(log10(num));
        for (size_t i = 0;i <= exponent;i++)//��ΪҪ��һλ�����Զ�ѭ��һ��
        {
            digits.push_back(num % 10);
            num /= 10;
        }
    }
    const int& operator[] (const size_t& i)const
    {
        return digits[digits.size() - 1 - i];
    }
    const size_t GetSize()const//Ѫ�Ľ�ѵ��Ϊʲô����������const size_t&����Ϊdigits.size()���ص�����ֵ��������ֵ
    {
        return digits.size();
    }
    const long long& GetExponent()const
    {
        return exponent;
    }
    const bool& GetNegative()const
    {
        return isNegative;
    }
    explicit operator std::string()
    {
        std::stringstream ss;
        ss << (isNegative ? "-" : "");
        ss << (*this)[0] << '.';
        if (GetSize() != 1)
        {
            for (size_t i = 1;i < GetSize();++i)
            {
                ss << (*this)[i];
            }
        }
        else
        {
            ss << '0';
        }
        ss << "��10^" << (exponent < 0 ? "(" : "") << exponent << (exponent < 0 ? ")" : "");
        return ss.str();
    }
    BigNum operator+(const BigNum& rhs)const
    {
        if (isNegative == rhs.isNegative)//ͬ���߼�
        {
            BigNum a(*this), b(rhs);//��������
            alignexponent(a, b);//��������ͷ�����루ͨ��ǰ�油0�ķ�ʽ��
            BigNum result;
            result.isNegative = isNegative;
            result.exponent = a.exponent;

            int8_t carry = 0;//��һλ�Ľ�λ
            size_t max_len = std::max(a.digits.size(), b.digits.size());
            for (size_t i = 0; i < max_len || carry; ++i) {
                int8_t sum = carry;
                if (i < a.digits.size()) sum += a.digits[i];
                if (i < b.digits.size()) sum += b.digits[i];
                result.digits.push_back(sum % 10);
                carry = sum / 10;
                if (i >= max_len)//����λʱ����Ҫ��ָ������
                {
                    ++result.exponent;
                }
            }
            result.normalize();
            return result;
        }
        else//����߼�
        {
            if (rhs.isNegative == true)
            {
                BigNum copy = rhs;
                copy.isNegative = false;
                return (*this) - copy;
            }
            else
            {
                BigNum copy = (*this);
                copy.isNegative = false;
                return rhs - copy;
            }
        }
    }
    BigNum operator-(const BigNum& rhs)const
    {

    }
    std::string ToFloatFormart()
    {
        std::stringstream ss;
        if (exponent < 0)
        {
            ss << "0.";
            for (long long i = 1;i < -exponent;++i)
            {
                ss << '0';
            }
            for (size_t i = 0;i < GetSize();++i)
            {
                ss << (*this)[i];
            }
        }
        else
        {
            for (long long i = 0;i < exponent + 1;++i)
            {
                if (i < digits.size()) ss << (*this)[i];
                else ss << '0';
            }
            ss << '.';
            if (digits.size() > exponent + 1)
            {
                for (long long i = exponent + 1;i < digits.size();++i)
                {
                    ss << (*this)[i];
                }
            }
            else
            {
                ss << '0';
            }
        }
        return ss.str();
    }
};
int main()
{
    using std::string, std::cout, std::cin, std::endl;
    string s;
    //string s2;
    int num;
    while (true)
    {
        cout << "Enter the first number(enter \"exit\" to quit):";
        cin >> s;
        if (s == "exit")
        {
            break;
        }
        cout << "Enter the second number(enter \"exit\" to quit):";
        /*cin >> s2;
        if (s2 == "exit")
        {
            break;
        }*/

        BigNum bn(s);
        //BigNum bn2(s2);

        /*std::stringstream ss;
        ss << s;
        ss >> num;
        BigNum bn(num);*/

        /*cout << "Negative:" << (bn.GetNegative() ? "Yes" : "No") << endl;
        cout << "Digits:" << endl;
        for (size_t i = 0;i < bn.GetSize() - 1;++i)
        {
            cout << bn[i] << ' ';
        }
        cout << bn[bn.GetSize() - 1] << endl;
        cout << "Exponent:" << bn.GetExponent() << endl;*/

        cout << "Stringify1:" << (string)bn << endl;
        /*cout << "Stringify2:" << (string)bn2 << endl;
        cout << "Sum:" << (string)(bn + bn2) << endl;*/
        cout << "FloatFormatted:" << bn.ToFloatFormart() << endl;
    }
    return 0;
};