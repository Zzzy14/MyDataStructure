#pragma once

#include<cstddef>
#include<stdexcept>

//  iterator 类别
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

//  iterator 属性定义
/*
template<class Category, class T, class Distance = ptrdiff_t,
        class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category iterator_category; //1
    typedef T value_type;               //2
    typedef Distance difference_type;   //3
    typedef Pointer pointer;            //4
    typedef Reference reference;        //5
};
*/

//  版本1：针对普通iterator对象的版本
template<class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category; //iterator属性1：类别
    typedef typename Iterator::value_type value_type;               //iterator属性2：指向的对象型别
    typedef typename Iterator::difference_type difference_type;     //iterator属性3：迭代器距离
    typedef typename Iterator::pointer pointer;                     //iterator属性4：指针
    typedef typename Iterator::reference reference;                 //iterator属性5：引用
};

//  版本2：针对原生指针的偏特化版本（原生指针不属于struct iterator, 所以不能用版本1提取属性）
template<class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
};

//  版本3：针对const类型的原生指针的偏特化版本（不能用版本2替代，pointer和reference必须保持const）
template<class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;
};

//  获得iterator_category
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category category; //获得对应的stuct
    return category();  //返回stuct对象
}

//  获得迭代器距离所属类型
template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

//  获得指向的对象型别
template<class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator*) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//  distance函数实现：借用_distance
//  输入：首尾迭代器
//  输出：迭代器间距离

//  版本1：针对非RandomAccessIterator的InputIterator，迭代器空间上不连续，不能直接减
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
_distance(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

//  版本2：针对RandomAccessIterator，可以直接减
template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
_distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    return last - first;
}

//  用户接口distance
//  参数1：起始迭代器
//  参数2：终止迭代器
//  _distance参数3：借用iterator_category获得的迭代器类型对象，可以决定使用哪个版本的_distance
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return _distance(first, last, iterator_category(first));
}

//  advance函数实现：借用_advance
//  输入：起始迭代器，移动距离

//  版本1：针对非BidirectionalIterator和RandomAccessIterator的InputIterator
//  特点1：迭代器空间上不连续，需要一个个移
//  特点2：不能回退
template<class InputIterator, class Distance>
inline void _advance(InputIterator &iter, Distance d, input_iterator_tag) {
    if (d < 0)
        throw std::out_of_range("Negative distance");
    while (d--) ++iter;
}

//版本2：针对BidirectionalIterator，迭代器空间上不连续，需要一个个移，可以前进和回退
template<class BidirectionalIterator, class Distance>
inline void _advance(BidirectionalIterator &iter, Distance d, bidirectional_iterator_tag) {
    if (d >= 0)
        while (d--) ++iter;
    else
        while (d++) --iter;
}

//版本3：针对RandomAccessIterator，可以自由移动
template<class RandomAccessIterator, class Distance>
inline void _advance(RandomAccessIterator &iter, Distance d, random_access_iterator_tag) {
    iter += d;
}

//  用户接口advance
//  参数1：起始迭代器
//  参数2：移动距离
//  _advance参数3：借用iterator_category获得的迭代器类型对象，可以决定使用哪个版本的_advance
template<class InputIterator, class Distance>
inline void advance(InputIterator &iter, Distance d) {
    _advance(iter, d, iterator_category(iter));
}