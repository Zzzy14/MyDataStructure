#pragma once

#include<cstddef>
#include<stdexcept>

//  iterator ���
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

//  iterator ���Զ���
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

//  �汾1�������ͨiterator����İ汾
template<class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category; //iterator����1�����
    typedef typename Iterator::value_type value_type;               //iterator����2��ָ��Ķ����ͱ�
    typedef typename Iterator::difference_type difference_type;     //iterator����3������������
    typedef typename Iterator::pointer pointer;                     //iterator����4��ָ��
    typedef typename Iterator::reference reference;                 //iterator����5������
};

//  �汾2�����ԭ��ָ���ƫ�ػ��汾��ԭ��ָ�벻����struct iterator, ���Բ����ð汾1��ȡ���ԣ�
template<class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
};

//  �汾3�����const���͵�ԭ��ָ���ƫ�ػ��汾�������ð汾2�����pointer��reference���뱣��const��
template<class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;
};

//  ���iterator_category
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category category; //��ö�Ӧ��stuct
    return category();  //����stuct����
}

//  ��õ�����������������
template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

//  ���ָ��Ķ����ͱ�
template<class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator*) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//  distance����ʵ�֣�����_distance
//  ���룺��β������
//  ����������������

//  �汾1����Է�RandomAccessIterator��InputIterator���������ռ��ϲ�����������ֱ�Ӽ�
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

//  �汾2�����RandomAccessIterator������ֱ�Ӽ�
template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
_distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    return last - first;
}

//  �û��ӿ�distance
//  ����1����ʼ������
//  ����2����ֹ������
//  _distance����3������iterator_category��õĵ��������Ͷ��󣬿��Ծ���ʹ���ĸ��汾��_distance
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return _distance(first, last, iterator_category(first));
}

//  advance����ʵ�֣�����_advance
//  ���룺��ʼ���������ƶ�����

//  �汾1����Է�BidirectionalIterator��RandomAccessIterator��InputIterator
//  �ص�1���������ռ��ϲ���������Ҫһ������
//  �ص�2�����ܻ���
template<class InputIterator, class Distance>
inline void _advance(InputIterator &iter, Distance d, input_iterator_tag) {
    if (d < 0)
        throw std::out_of_range("Negative distance");
    while (d--) ++iter;
}

//�汾2�����BidirectionalIterator���������ռ��ϲ���������Ҫһ�����ƣ�����ǰ���ͻ���
template<class BidirectionalIterator, class Distance>
inline void _advance(BidirectionalIterator &iter, Distance d, bidirectional_iterator_tag) {
    if (d >= 0)
        while (d--) ++iter;
    else
        while (d++) --iter;
}

//�汾3�����RandomAccessIterator�����������ƶ�
template<class RandomAccessIterator, class Distance>
inline void _advance(RandomAccessIterator &iter, Distance d, random_access_iterator_tag) {
    iter += d;
}

//  �û��ӿ�advance
//  ����1����ʼ������
//  ����2���ƶ�����
//  _advance����3������iterator_category��õĵ��������Ͷ��󣬿��Ծ���ʹ���ĸ��汾��_advance
template<class InputIterator, class Distance>
inline void advance(InputIterator &iter, Distance d) {
    _advance(iter, d, iterator_category(iter));
}