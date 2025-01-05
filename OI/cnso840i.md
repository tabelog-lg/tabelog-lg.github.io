节点类 `node.h`
```cpp
#ifndef NODE_H
#define NODE_H

template <class T>
class Node
{
public:
    Node<T> *next;
    Node<T> *prev;
    T data;
};

#endif /*NODE_H*/
```

链表类 `link.h`
```cpp
#ifndef LINK_H
#define LINK_H

template <class T>
class List
{
public:
    List();               //默认构造函数
    List(const List& ln); //拷贝构造函数
    ~List();              //析构函数
    void add(T e);        //向链表添加数据
    void ascSort();       //升序排序
    void remove(T index); //移除某个结点
    T find(int index);    //查找结点
    bool isEmpty();       //判断是否为空
    int size();           //链表长度
    void show();          //显示链表
    void resShow();       //链表反向显示
    void removeAll();     //删除全部结点
private:
    Node<T> *head;
    Node<T> *tail;
    int length;
};
 
//默认构造函数
template <typename T>
List<T>::List()
{
    head = new Node<T>;
    tail = new Node<T>;
    head->next = tail;
    head->prev = nullptr;
    tail->next = nullptr;
    tail->prev = head;
    length = 0;
}

//拷贝构造函数
template <typename T>
List<T>::List(const List &ln)
{
    head = new Node<T>;
    head->prev = nullptr;
    tail = new Node<T>;
    head->next = tail;
    tail->prev = head;
    length = 0;
    Node<T>* temp = ln.head;
    while(temp->next != ln.tail)
    {
        temp = temp->next;
        tail->data = temp->data;
        Node<T> *p = new Node<T>;
        p->prev = tail;
        tail->next = p;
        tail = p;
        length++;
    }
    tail->next = nullptr;
}

//向链表添加数据
template <typename T>
void List<T>::add(T e)
{
    Node<T>* temp = this->tail;
    tail->data = e;
    tail->next = new Node<T>;
    Node<T> *p = tail;
    tail = tail->next;
    tail->prev = p;
    tail->next = nullptr;
    length++;
}

//查找结点
template <typename T>
T List<T>::find(int index)
{
    if(length == 0)
    {
        std::cout << "List is empty";
        return NULL;
    }
    if(index >= length)
    {
        std::cout << "Out of bounds";
        return NULL;
    }
    int x = 0;
    T data;
    Node<T> *p;
    if(index < length/2)
    {
        p = head->next;
        while (p->next != nullptr && x++ != index)
        {
            p=p->next;
        }
    } else{
        p = tail->prev;
        while(p->prev != nullptr && x++ != index)
        {
            p = p->prev;
        }
    }
    return p->data;
}

//删除结点
template <typename T>
void List<T>::remove(T index)
{
    if(length == 0)
    {
        std::cout << "List is empty";
        return ;
    }
    Node<T> *p = head;
    while(p->next!=nullptr)
    {
        p = p->next;
        if(p->data == index)
        {
            Node<T> *temp = p->prev;
            temp->next = p->next;
            p->next->prev = temp;
            delete p;
            length--;
            return ;
        }
    }
}

//删除所有结点
template <typename T>
void List<T>::removeAll()
{
    if(length == 0)
    {
        return ;
    }
    Node<T> *p = head->next;
    while(p != tail)
    {
        Node<T>* temp = p;
        p = p->next;
        delete temp;
    }
    head->next = tail;
    tail->prev = head;
    length = 0;
}

//升序排序
template <typename T>
void List<T>::ascSort()
{
    if(length <= 1) return;
    Node<T> *p = head->next;
    for (int i = 0; i < length-1; i++)
    {
        Node<T> *q = p->next;
        for (int j = i+1; j < length; j++)
        {
            if(p->data > q->data)
            {
                T temp = q->data;
                q->data = p->data;
                p->data = temp;
            }
            q=q->next;
        }
        p = p->next;
    }
}

//判断是否为空
template <typename T>
bool List<T>::isEmpty()
{
    if(length == 0)
    {
        return true;
    }
    else {
        return false;
    }
}

//链表长度
template <typename T>
int List<T>::size()
{
    return length;
}

//输出链表
template <typename T>
void List<T>::show()
{
    if(length == 0)
    {
        std::cout << "List is empty" << std::endl;
        return;
    }
    Node<T> *p = head->next;
    while (p != tail)
    {
        std::cout << p->data << " ";
        p = p->next;
    }
    std::cout << std::endl;
}

//反向输出链表
template <typename T>
void List<T>::resShow()
{
    if (length == 0)
    	return;
    Node<T> *p = tail->prev;
    while (p != head)
    {
        std::cout << p->data << " ";
        p = p->prev;
    }
    std::cout << std::endl;
}
//析构函数
template <typename T>
List<T>::~List()
{
    if (length == 0)
    {
        delete head;
        delete tail;
        head = nullptr;
        tail = nullptr;
        return;
    }
    while (head->next != nullptr)
    {
        Node<T> *temp = head;
        head = head->next;
        delete temp;
    }
    delete head;
    head = nullptr;
}

#endif /*LINK_H*/
```

测试程序 `_test_node.cpp`
```cpp
#include <iostream>
#include "link.h"
int main()
{
	List<float> ls1;
	ls1.add(1.1);
	ls1.add(0.1);
	ls1.add(6);
	ls1.add(3.3);
	List<float> ls2(ls1);
	ls2.resShow();
	std::cout << ls2.size() << std::endl;
	std::cout << ls2.find(00) << std::endl;
	ls2.remove(6);
	ls1.resShow();
        ls2.show();
        system("pause");
        return 0;
}
```