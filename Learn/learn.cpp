#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <string>
#include <sstream>
#include <compare>
using namespace std;
void print(const char* str) {
    cout << str << endl;
}
#pragma region 第9章 封装相关
class MyList
{
private:
    int* arr = nullptr;
    unsigned int size = 0;

public:
    explicit MyList(unsigned int size = 0)//explicit: 禁止隐式类型转换（防止MyList list = 5;）
        : size(size),//初始化列表: 优于在构造函数体内赋值
        arr(size ? new int[size] : nullptr)
    {}

    ~MyList()//析构函数确保正确释放内存
    {
        delete[] arr;
    }
    MyList(const MyList& another)//拷贝构造函数（深拷贝）
        : size(another.size),
        arr(another.size ? new int[another.size] : nullptr)
    {
        /*当发生以下情况时被调用：
        1. MyList list2 = list1;
        2. 函数传参时的值传递*/
        print("拷贝构造函数");
        copy(another.arr, another.arr + another.size, arr);
    }
    MyList(MyList&& another) noexcept//移动构造函数，MyList&&右值引用，noexcept优化
        : size(another.size), // 直接获取长度
        arr(another.arr)  // 直接接管指针
    {
        print("移动构造函数");
        // 使原对象处于安全可析构状态
        another.arr = nullptr; // 重要！避免双重释放
        another.size = 0;
    }
    MyList& operator=(const MyList& rhs)//拷贝赋值运算符（使用copy-and-swap惯用法）
    {
        if (this != &rhs)
        {                      /* 防止自我赋值，因为这样会导致释放arr，之后就访问不了了,
                                假设上面的if删掉，想想会发生什么*/
            MyList temp(rhs);  // 利用拷贝构造函数
            swap(*this, temp); //交换资源（见下方swap友元函数）
        } //temp离开作用域，自动释放原有资源
        return *this; //返回*this以支持链式赋值（a = b = c）
    }
    MyList& operator=(MyList&& rhs) noexcept//移动赋值运算符
    {
        if (this != &rhs)
        {                  // 防止自我移动赋值
            delete[] arr;  // 1. 释放当前资源
            arr = rhs.arr; // 2. 接管资源
            size = rhs.size;

            rhs.arr = nullptr; // 3. 置空源对象
            rhs.size = 0;
        }
        return *this;
    }
    int& operator[](unsigned int index)//下标运算符（非const版本）
    {
        if (index >= size)
        {
            throw out_of_range("Index out of range");
        }
        return arr[index];
    }
    const int& operator[](unsigned int index) const//下标运算符（const版本），与非常量版本构成重载
    {
        if (index >= size)
        {
            throw out_of_range("Index out of range");
        }
        return arr[index];
    }
    unsigned int Size() const noexcept
    {
        return size;
    }
    friend void swap(MyList& a, MyList& b) noexcept//友元函数就是可以访问类私有成员
    {
        std::swap(a.arr, b.arr);
        std::swap(a.size, b.size);
    }
    void sort() {//忘记介绍了，最重要的，成员函数
        std::sort(arr, arr + size);
    }
    MyList sorted() {
        MyList temp(*this);
        std::sort(temp.arr, temp.arr + temp.size);
        return temp;
    }
};
#pragma endregion

#pragma region 第10章 继承
class BaseData {
public:
    BaseData() {
		print("基类数据成员构造函数");//为了演示构造和析构顺序
    }
    ~BaseData() {
		print("基类数据成员析构函数");
    }
};
class Base {
private:
    BaseData baseData;
public:
    Base() :baseData(){
        print("基类构造函数");
    }
    void func() {
        print("基类函数");
    }
    void func(int a) {
		print("基类函数重载");
    }
	~Base() {
		print("基类析构函数");
	}
};
class DerivedData {
public:
	DerivedData() {
		print("派生类数据成员构造函数");
	}
	~DerivedData() {
		print("派生类数据成员析构函数");
	}
};
class Derived1 : public Base {//继承语法 : 访问修饰符 基类1，基类2...
    //c++支持多继承，但可能存在同一个父类被初始化多次的问题，可以参阅下一章
    /*访问修饰符作用：修饰基类的成员，比如：
    private：使子类外部不能访问基类成员，子类的子类也不行
    protected：子类外部不能访问基类成员，子类的子类却可以
    注：在整个继承链状结构中，总是最严格的修饰符起效，比如父类(以下使用生物学记法P)的public成员变量，
    F1如果用private继承P，F2如果就算用public继承F1也不能访问P的成员*/
private:
	DerivedData derivedData; //派生类数据成员
public:
    //using Base::func;//可以通过这个方法来引入父类所有func的重载
    Derived1():derivedData() {
		print("派生类构造函数");
    }
    ~Derived1() {
		print("派生类析构函数");
    }
    void func() {
		print("派生类函数");
    }
};
//void func(Base obj){}//会导致切片问题
void func(Base& obj){}//加上引用就可以避免
#pragma endregion

#pragma region 第11章 多态
class Shape {//包含纯虚函数(至少有一个)的类称为抽象类，不能被实例化，可以有其他成员函数
public:
    virtual double Area() = 0;//纯虚函数,用=0来声明,纯虚函数强制需要子类实现
    virtual ~Shape() {
        print("Shape 析构");
    }//虚析构函数，作用是让析构函数有多态，也就是能正确地调用子类的析构函数
};
class Circle : public Shape{
private:
    double r;
public:
    Circle(double r) :r(r){}
    double Area()
    {
        return 3.1416 * r * r;
    }
    ~Circle()
    {
        print("Circle 析构");
    }
};
class Triango : public Shape{
private:
    double a, b, c, p;
public:
    Triango(double a, double b, double c):a(a),b(b),c(c)
    {
        p = (a + b + c) / 2;
    }
    double Area() override//override关键字用于显式覆盖，编译器会查找父类有没有相应需要重写的方法，没有会报错
    {
        return sqrt(p * (p - a) * (p - b) * (p - c));
    }
    ~Triango() {
        print("Triango 析构");
    }
};
void deleteShape(Shape* shape) {//演示虚析构函数的多态
    delete shape;
}
class P{
public:
    int a;
    P():a(0)
    {
        print("P构造函数");
    }
};
class F11:public P{};
class F12:public P{};
class F21 : public F11, public F12 {};//演示虚继承 
class F13 :public virtual P {};//虚继承语法
class F14 :public virtual P {};
class F22:public F13,public F14{};//演示虚继承 
#pragma endregion

#pragma region 第12章 运算符重载
class Day {//本意是星期一，二，……日
private:
    int day;
public:
    Day(int day=0):day(day){}
    Day operator+(const int rhs)//重载+
    {
        Day newDay((day + rhs) % 7 == 0 ? 7 : (day + rhs) % 7);
        return newDay;
    }
    Day& operator+=(const int rhs)
    {
        day=(day + rhs) % 7 == 0 ? 7 : (day + rhs) % 7;
        return (*this);
    }
    Day& operator++() //这个是前缀版本，++day
    {
        (*this) += 1;
        return *this;
    }
    Day operator++(int)//这两个是前缀版本，day++
    {
        Day newDay = *this;
        ++(*this);
        return newDay;
    }
    strong_ordering operator <=>(const Day& another)//三相比较运算符重载 c++20
    {
        return day <=> another.day;
    }
    operator string ()
    {
        stringstream ss;
        ss << day;
        return ss.str();
    }
};
template<typename T>//模板语法
class ArrInHeap {
private:
    T* arr;
    size_t _size;
public:
    explicit ArrInHeap(size_t size)//构造函数，防止隐式类型转换
        :_size(size),
        arr(size ? new T[size]{} /*值初始化为全0*/ : nullptr) {
    }
    ~ArrInHeap()//析构函数
    {
        delete[] arr;
    }
    ArrInHeap(const ArrInHeap& another)//复制构造函数
        :_size(another._size),
        arr(another._size ? new T[another._size] : nullptr)
    {
        if (arr)//检查指针有效性，防止空数组拷贝
            std::copy(another.arr, another.arr + _size, arr);//拷贝
    }
    ArrInHeap(ArrInHeap&& another) noexcept//移动构造函数
        :_size(another._size),
        arr(another.arr)
    {
        another.arr = nullptr;//避免another调用析构函数时释放arr
        another._size = 0;//虽然没必要，但不管了，大家都这么写呢
    }
    friend void swap(ArrInHeap& a, ArrInHeap& b) noexcept {// 交换函数
        using std::swap;
        swap(a.arr, b.arr);
        swap(a._size, b._size);
    }
    ArrInHeap& operator=(const ArrInHeap& another) {// 拷贝赋值运算符
        ArrInHeap temp(another);
        swap(*this, temp);//copy-and-swap
        return *this;
    }
    ArrInHeap& operator=(ArrInHeap&& another) noexcept {// 移动赋值运算符
        if (this != &another) {
            delete[] arr;
            arr = another.arr;
            _size = another._size;
            another.arr = nullptr;
            another._size = 0;
        }
        return *this;
    }
    size_t size() const noexcept
    {
        return _size;
    }
    T& operator[](size_t index)//引用版本，目的是改变
    {
        return arr[index];
    }
    const T& operator[](size_t index) const//const 版本，目的是读取
    {
        return arr[index];
    }
    operator std::string()//类型转换运算符重载
    {//好吧我承认重载这个运算符有点反人类，但不管了，只是示例
        std::stringstream ss;
        for (size_t i = 0;i < _size - 1;i++)
        {
            ss << arr[i] << ' ';
        }
        ss << arr[_size - 1];
        return ss.str();
    }
};
class Display {//注意，函数运算符重载时也可以定义其他东西，和普通对象一样
public:
    void operator() (string input) const//函数运算符 或者叫谓词
    {
        cout << input << endl;
    }
};
class Temperature
{
private:
    double kelvin;//开尔文，热力学温度
public:
    Temperature(double kelvin):kelvin(kelvin){}
    operator string() 
    {
        stringstream ss;
        ss << kelvin << "K";
        return ss.str();
    }
};
static Temperature operator "" _C(long double celsius)//自定义字面量（或者说后缀，如37.5℃）
{//只能在类外定义
    return Temperature(celsius + 273);
}
//static Temperature operator "" ℃(long double celsius)
//{
//    return Temperature(celsius + 273);
//}//UTF-8的含金量还在上升！！！不过我还是把它注释了
#pragma endregion

#pragma region 第14章 模板

#pragma endregion

int main()
{
#pragma region 第9章 封装相关 主程序部分
    print("============第9章 封装相关============");
    MyList list1(3);
    list1[0] = 10;   // 测试非const版本operator[]
    list1[1] = 30;
    list1[2] = 20;
    MyList list2 = list1; // 调用拷贝构造函数
    cout << "list2[1] should be 30: " << list2[1] << endl;
    // 移动构造测试（注意之后list1不再拥有数据）
    MyList list3 = move(list1);
    cout << "list3 size should be 3: " << list3.Size() << endl;
    MyList list4;
    list4 = list2; // 调用拷贝赋值运算符
    cout << "list4[2] should be 20: " << list4[2] << endl;
    MyList list5;
    list5 = move(list3); // 调用移动赋值运算符，同样list3也不再有数据
    cout << "list5 elements: ";
    for (unsigned i = 0; i < list5.Size(); ++i)
    {
        cout << list5[i] << " "; // 测试const版本operator[]
    }
    cout << endl;
    // 异常测试
    // cout << list5[5] << endl; // 触发out_of_range异常

    // sort测试
    list5.sort();
    cout << "list5 sorted elements: ";
    for (unsigned i = 0; i < list5.Size(); ++i)
    {
        cout << list5[i] << " ";
    }
    cout << endl;

    // sorted测试
    MyList list6(3);
    list6[0] = 3;
    list6[1] = 1;
    list6[2] = 2;
    MyList list7 = list6.sorted();// 同时测试拷贝构造函数
    cout << "list6 sorted elements: ";
    for (unsigned i = 0; i < list7.Size(); ++i)
    {
        cout << list7[i] << " ";
    }
    cout << endl;
#pragma endregion

#pragma region 第10章 继承 主程序部分
    print("============第10章 继承============");
    {//块级作用域，演示顺序
        Derived1 derived1;
    }/*离开块级作用域，析构
    output:
        基类数据成员构造函数
        基类构造函数
        派生类数据成员构造函数
        派生类构造函数
        派生类析构函数
        派生类数据成员析构函数
        基类析构函数
        基类数据成员析构函数*/
    print("=====");
    {
        Derived1 derived2;
        //derived2.func(1);//报错，因为子类覆盖了父类的func重载版本
        derived2.Base::func(1);//这行不报错，因为显式指定了要调用的版本
        func(derived2);//演示切片问题
    }
#pragma endregion

#pragma region 第11章 多态 主程序部分
    print("============第11章 多态============");
    {
        Circle circle(2);
        Triango triango(3, 4, 5);
        Shape& shape1 = circle;//这其实是里氏替换原则，父类引用指向子类对象
        Shape& shape2 = triango;//但是注意需要用引用或指针，以防切片问题，详见上一章
        cout << "Circle Area:" << shape1.Area() << endl
            << "Triango Area:" << shape2.Area() << endl;//这里就是多态了，父类引用表现出了两种不同的子类行为
    }
    print("=====");
    Circle* pcircle = new Circle(1);
    deleteShape(pcircle);
    print("=====");
    /*析构output:
     Circle 析构
     Shape 析构
     如果不写虚析构函数，将不会显示Circle 析构
     */
    F21 f21;//将会看到P构造函数两次
    //f21.a = 1;//取消注释将会看到F2::a不明确
    print("=====");
    F22 f22;//使用虚继承后正常
    f22.a = 1;
#pragma endregion

#pragma region 第12章 运算符重载 主程序部分
    print("============第12章 运算符重载============");
    Day day(7);
    cout << (string)day++ << endl << (string)++day << endl;
    day += 10;
    cout << (string)day << endl;

    ArrInHeap<int> myarr(2);
    myarr[0] = 1;
    myarr[1] = 2;
    cout << static_cast<string>(myarr) << endl;//类型转换，和(string)myarr功能一样

    Display displayobj;
    displayobj("Hello World");//这就是函数运算符重载，可以当作函数调用

    //Temperature tem = 20.0℃;//UTF-8的含金量还在上升！！！不过我还是把它注释了
    Temperature tem = 20.0_C;//下划线加后缀的形式更符合标准
    cout << (string)tem << endl;
#pragma endregion

#pragma region 第14章 模板 主程序部分
    print("============第14章 模板============");

#pragma endregion
    return 0;
}