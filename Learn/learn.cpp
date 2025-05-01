#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cmath>
using namespace std;
void print(const char* str) {
    cout << str << endl;
}
#pragma region 第11章 封装相关
class MyList
{
private:
    int* arr = nullptr;   // 使用裸指针管理动态数组
    unsigned int len = 0; // unsigned保证非负

public:
    /* 核心构造函数
     * explicit: 禁止隐式类型转换（防止意外的MyList list = 5; 这样的构造）
     * 参数默认值: size=0 支持默认构造（0元素数组）
     * 初始化列表: 优于在构造函数体内赋值（更高效）
     */
    explicit MyList(unsigned int size = 0)
        : len(size),
        arr(size ? new int[size] : nullptr) // 处理size=0的情况，避免new int[0]可能的问题
    {
    }

    /* 析构函数
     * 必须存在以确保正确释放内存
     * 虚函数：若类会被继承，应该声明为virtual（本例不需要）
     */
    ~MyList()
    {
        delete[] arr; // 正确释放数组（如果用new[]分配必须用delete[]）
    }

    /* 拷贝构造函数（深拷贝）
     * 当发生以下情况时被调用：
     * 1. MyList list2 = list1;
     * 2. 函数传参时的值传递
     * 3. 函数返回对象时的某些情况
     */
    MyList(const MyList& another)
        : len(another.len),
        arr(another.len ? new int[another.len] : nullptr)
    {
        print("拷贝构造函数");
        copy(another.arr, another.arr + another.len, arr);
    }

    /* 移动构造函数（C++11引入）
     * 参数必须是右值引用（MyList&&）
     * noexcept 声明对于标准库容器优化很重要（例如vector的扩容操作）
     * 注意事项：
     * 1. 会"窃取"原对象的资源
     * 2. 必须使原对象处于有效但空的状态
     */
    MyList(MyList&& another) noexcept
        : len(another.len), // 直接获取长度
        arr(another.arr)  // 直接接管指针
    {
        print("移动构造函数");
        // 使原对象处于安全可析构状态
        another.arr = nullptr; // 重要！避免双重释放
        another.len = 0;
    }

    /* 拷贝赋值运算符（使用copy-and-swap惯用法）
     * 处理自我赋值安全
     * 参数为const引用
     */
    MyList& operator=(const MyList& rhs)
    {
        if (this != &rhs)
        {                      /* 防止自我赋值，因为这样会导致释放arr，之后就访问不了了,
                                假设上面的if删掉，想想会发生什么*/
            MyList temp(rhs);  // 1. 创建副本（利用拷贝构造函数）
            swap(*this, temp); // 2. 交换资源（见下方swap友元函数）
        } // 3. temp离开作用域，自动释放原有资源
        return *this; // 返回*this以支持链式赋值（a = b = c）
    }

    /* 移动赋值运算符（C++11）
     * 参数为右值引用
     * noexcept声明用于优化
     */
    MyList& operator=(MyList&& rhs) noexcept
    {
        if (this != &rhs)
        {                  // 防止自我移动赋值
            delete[] arr;  // 1. 释放当前资源
            arr = rhs.arr; // 2. 接管资源
            len = rhs.len;

            rhs.arr = nullptr; // 3. 置空源对象
            rhs.len = 0;
        }
        return *this;
    }

    /* 下标运算符（非const版本）
     * 允许修改元素：list[0] = 5;
     * 边界检查：防止越界访问
     */
    int& operator[](unsigned int index)
    {
        if (index >= len)
        {
            throw out_of_range("Index out of range");
        }
        return arr[index];
    }

    /* 下标运算符（const版本）
     * 用于const对象：const MyList& list; int x = list[0];
     * 与非常量版本构成重载
     */
    const int& operator[](unsigned int index) const
    {
        if (index >= len)
        {
            throw out_of_range("Index out of range");
        }
        return arr[index];
    }

    /* 获取数组长度
     * noexcept声明：向编译器保证不会抛出异常
     * 适合简单getter方法
     */
    unsigned int size() const noexcept
    {
        return len;
    }

    /* 交换函数（友元）
     * 用于实现copy-and-swap惯用法
     * noexcept保证交换操作不会失败
     * 被std::swap调用时提供优化
     */
    friend void swap(MyList& a, MyList& b) noexcept
    {
        swap(a.arr, b.arr);
        swap(a.len, b.len);
    }
    void sort() {
        std::sort(arr, arr + len);
    }
    MyList sorted() {
        MyList temp(*this);
        std::sort(temp.arr, temp.arr + temp.len);
        return temp;
    }
};
#pragma endregion

#pragma region 第12章 继承
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

#pragma region 第13章 多态
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

int main()
{
#pragma region 第11章 封装相关 主程序部分
    print("============第11章 封装相关============");
    // 基本构造测试
    MyList list1(3); // 调用普通构造函数
    list1[0] = 10;   // 测试非const版本operator[]
    list1[1] = 30;
    list1[2] = 20;

    // 拷贝构造测试
    MyList list2 = list1; // 调用拷贝构造函数
    cout << "list2[1] should be 30: " << list2[1] << endl;

    // 移动构造测试（注意之后list1不再拥有数据）
    MyList list3 = move(list1); // 调用移动构造函数
    cout << "list3 size should be 3: " << list3.size() << endl;

    // 拷贝赋值测试
    MyList list4;
    list4 = list2; // 调用拷贝赋值运算符
    cout << "list4[2] should be 20: " << list4[2] << endl;

    // 移动赋值测试
    MyList list5;
    list5 = move(list3); // 调用移动赋值运算符
    cout << "list5 elements: ";
    for (unsigned i = 0; i < list5.size(); ++i)
    {
        cout << list5[i] << " "; // 测试const版本operator[]
    }
    cout << endl;
    // 异常测试
    // cout << list5[5] << endl; // 触发out_of_range异常

    // sort测试
    list5.sort();
    cout << "list5 sorted elements: ";
    for (unsigned i = 0; i < list5.size(); ++i)
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
    for (unsigned i = 0; i < list7.size(); ++i)
    {
        cout << list7[i] << " ";
    }
    cout << endl;
#pragma endregion

#pragma region 第12章 继承 主程序部分
    print("============第12章 继承============");
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

#pragma region 多态 主程序部分
    print("============第13章 多态============");
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

    return 0;
}