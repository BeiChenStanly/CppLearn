#include<iostream>
#include <sstream>
#include <cmath>
#include <deque>
#include <cstdint>
#include "cppbignum.h"
/*
写在前面：
注意size_t是unsigned类型，不要跟有符号类型混用！
digits用int8_t存储是为了节省空间，实质是char，输出时记得类型转换
最好别改成using namespace std;可以自己using std::cout之类的（需要C++17）
科学计数法的定义：x=a×10^b,1<=a<10
三相比较运算符至少需要C++20
*/
class BigNum // 科学计数法
{
private:
    std::deque<int8_t> digits; // 各数位（倒序存储）
    long long exponent;        // 指数
    bool isNegative;           // 符号

    /*aid functions*/
    void normalize() // 标准化，即处理前后导0，使1.1451400e2变成1.14514e2，0.114514e3变成1.14514e2
    {
        while (!digits.empty() && digits.front() == 0)
        { // 去后导0，注意存储是倒序的，所以这里是后导
            digits.pop_front();
        }
        while (!digits.empty() && digits.back() == 0)
        { // 去除前导0
            digits.pop_back();
            --exponent; // 去除前导0等价于把第一位往右边移，因为第一位向右边移，所以指数相对减一
        }
        if (digits.empty())
        {
            digits.push_back(0);
            exponent = 0;
            isNegative = false;
        }
    }
    static void alignexponent(BigNum& a, BigNum& b) // 后面添0以对齐指数
    {
        while (a.exponent < b.exponent)
        {
            a.digits.push_back(0);
            a.exponent++;
        }
        while (b.exponent < a.exponent)
        {
            b.digits.push_back(0);
            b.exponent++;
        }
        while (a.GetSize() < b.GetSize()) // 前面添加0
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
        // 初始化正负
        size_t start = 0;
        isNegative = bool(s[0] == '-');
        if (isNegative || s[0] == '+')
            ++start;
        size_t ePos = s.find_first_of("Ee"); // 注意这个find_first_of是指Ee中的任意一个第一个索引

        // 处理e后指数
        if (ePos != std::string::npos) // 如果是形如1.14514e5
        {                              // 注意到11.4514e4这样的输入也有可能，所以指数之后还需要加
            exponent = std::stoi(s.substr(ePos + 1, s.size() - ePos - 1));
        }
        else // 否则是形如11.4514
        {
            exponent = 0; // 这里只是e后初始值
        }

        // 初始化数位、指数
        std::string absoluteValue = s.substr(start, ePos == std::string::npos ? s.size() - start : ePos - start); // 与npos比较可以知道是否找到，去掉符号和e之后的指数
        size_t pointPos = absoluteValue.find('.');                                                                // 小数点
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
        for (size_t i = 0; i < absoluteValue.size(); ++i)
        {
            digits[i] = absoluteValue[absoluteValue.size() - 1 - i] - '0';
        }
        normalize(); // 标准化
    }
    explicit BigNum(int num)
    {
        isNegative = num < 0;
        if (isNegative)
            num = -num;
        exponent = floor(log10(num));
        for (size_t i = 0; i <= exponent; i++) // 因为要第一位，所以多循环一次
        {
            digits.push_back(num % 10);
            num /= 10;
        }
    }
    explicit BigNum(long long num)
    {
        isNegative = num < 0;
        if (isNegative)
            num = -num;
        exponent = floor(log10(num));
        for (size_t i = 0; i <= exponent; i++) // 因为要第一位，所以多循环一次
        {
            digits.push_back(num % 10);
            num /= 10;
        }
    }
    const int operator[](const size_t& i) const
    {
        return digits[digits.size() - 1 - i];
    }
    const size_t GetSize() const // 血的教训：为什么不返回引用const size_t&？因为digits.size()返回的是右值，不是左值
    {
        return digits.size();
    }
    const long long& GetExponent() const
    {
        return exponent;
    }
    const bool& GetNegative() const
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
            for (size_t i = 1; i < GetSize(); ++i)
            {
                ss << (*this)[i];
            }
        }
        else
        {
            ss << '0';
        }
        ss << "×10^" << (exponent < 0 ? "(" : "") << exponent << (exponent < 0 ? ")" : "");
        return ss.str();
    }
    std::strong_ordering operator<=>(const BigNum& rhs) const
    {
        if (isNegative != rhs.isNegative)
        {
            return rhs.isNegative <=> isNegative;
        }
        else
        {
            BigNum a(*this);
            BigNum b(rhs);
            alignexponent(a, b);
            for (size_t i = 0; i < a.GetSize(); ++i)
            {
                if (a[i] != b[i])
                {
                    return a[i] <=> b[i];
                }
            }
            return std::strong_ordering::equal;
        }
    }
    BigNum operator+(const BigNum& rhs) const
    {
        if (isNegative == rhs.isNegative) // 同号逻辑
        {
            BigNum a(*this), b(rhs); // 创建副本
            alignexponent(a, b);     // 让两个数头部对齐（通过前面补0的方式）
            BigNum result;
            result.isNegative = isNegative;
            result.exponent = a.exponent;

            int8_t carry = 0; // 上一位的进位
            size_t max_len = std::max(a.digits.size(), b.digits.size());
            for (size_t i = 0; i < max_len || carry; ++i)
            {
                int8_t sum = carry;
                if (i < a.digits.size())
                    sum += a.digits[i];
                if (i < b.digits.size())
                    sum += b.digits[i];
                result.digits.push_back(sum % 10);
                carry = sum / 10;
                if (i >= max_len) // 当进位时，需要把指数增加
                {
                    ++result.exponent;
                }
            }
            result.normalize();
            return result;
        }
        else // 异号逻辑
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
    BigNum operator-(const BigNum& rhs) const
    {
        if (isNegative != rhs.isNegative)
        {
            if (isNegative)
            {
                BigNum copy = (*this);
                copy.isNegative = false;
                BigNum result = rhs + copy;
                result.isNegative = true;
                return result;
            }
            else
            {
                BigNum copy = rhs;
                copy.isNegative = false;
                BigNum result = copy + (*this);
                return result;
            }
        }
        else
        {
            BigNum maxnum,minnum;
            if ((*this) >= rhs)
            {
                maxnum = (*this);
                minnum = rhs;
                maxnum.isNegative = false;
            }
            else
            {
                minnum = (*this);
                maxnum = rhs;
                maxnum.isNegative = true;
            }
            alignexponent(maxnum, minnum);
            for (size_t i = 0;i < maxnum.digits.size();++i)
            {
                maxnum.digits[i] -= minnum.digits[i];
                if (maxnum.digits[i] < 0)
                {
                    maxnum.digits[i] += 10;
                    --maxnum.digits[i + 1];
                }
            }
            maxnum.normalize();
            return maxnum;
        }
    }
    std::string ToFloatFormart()
    {
        std::stringstream ss;
        if (isNegative)
            ss << '-';
        if (exponent < 0)
        {
            ss << "0.";
            for (long long i = 1; i < -exponent; ++i)
            {
                ss << '0';
            }
            for (size_t i = 0; i < GetSize(); ++i)
            {
                ss << (*this)[i];
            }
        }
        else
        {
            for (long long i = 0; i < exponent + 1; ++i)
            {
                if (i < digits.size())
                    ss << (*this)[i];
                else
                    ss << '0';
            }
            ss << '.';
            if (digits.size() > exponent + 1)
            {
                for (long long i = exponent + 1; i < digits.size(); ++i)
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
    string s2;
    while (true)
    {
        cout << "Enter the first number(enter \"exit\" to quit):";
        cin >> s;
        if (s == "exit")
        {
            break;
        }
        cout << "Enter the second number(enter \"exit\" to quit):";
        cin >> s2;
        if (s2 == "exit")
        {
            break;
        }

        BigNum bn1(s);
        BigNum bn2(s2);

        /*cout << "Negative:" << (bn.GetNegative() ? "Yes" : "No") << endl;
        cout << "Digits:" << endl;
        for (size_t i = 0;i < bn.GetSize() - 1;++i)
        {
            cout << bn[i] << ' ';
        }
        cout << bn[bn.GetSize() - 1] << endl;
        cout << "Exponent:" << bn.GetExponent() << endl;*/

        cout << "Stringify1:" << bn1.ToFloatFormart() << endl;
        cout << "Stringify2:" << bn2.ToFloatFormart() << endl;
        cout << "Sum:" << (bn1 + bn2).ToFloatFormart() << endl;
        // cout << "FloatFormatted:" << bn.ToFloatFormart() << endl;
        cout << "A>B:" << ((bn1 <=> bn2) > 0) << endl;//测试三相比较运算符
        cout << "A==B:" << ((bn1 <=> bn2) == 0) << endl;
        cout << "A<B:" << ((bn1 <=> bn2) < 0) << endl;
        cout << "Difference:" << (bn1 - bn2).ToFloatFormart() << endl;
    }
    return 0;
};