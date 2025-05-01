#include <iostream>
#include <algorithm>
#include <stdexcept>
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
    /*访问修饰符作用：修饰基类的成员，比如：
    private：使子类外部不能访问基类成员，子类的子类也不行
    protected：子类外部不能访问基类成员，子类的子类却可以
    注：在整个继承链状结构中，总是最严格的修饰符起效，比如父类(以下使用生物学记法P)的public成员变量，
    F1如果用private继承P，F2如果就算用public继承F1也不能访问P的成员*/
private:
	DerivedData derivedData; //派生类数据成员
public:
    Derived1():derivedData() {
		print("派生类构造函数");
    }
    ~Derived1() {
		print("派生类析构函数");
    }
};
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
    if (true) {//块级作用域，演示顺序
        Derived1 derived1;
    }//离开块级作用域，析构
    /*output:
        基类数据成员构造函数
        基类构造函数
        派生类数据成员构造函数
        派生类构造函数
        派生类析构函数
        派生类数据成员析构函数
        基类析构函数
        基类数据成员析构函数*/
#pragma endregion

    return 0;
}