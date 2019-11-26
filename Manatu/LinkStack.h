#include<iostream>
#include<string>

template <typename T>
class LinkStack {
    private:
        int capacity;
        T* mem;
        T* top;
        T* bottom;
        LinkStack<T>* next;
    public:
        LinkStack();
        LinkStack(int capacity);
        ~LinkStack();
        void clear();
        T getTop();
        int length();
        T pop();
        int push(T value);
        T operator[](int index); 
};

template <typename T>
LinkStack<T>::LinkStack() {
    int capacity = 20;
    this->capacity = capacity;
    this->mem = new T[capacity];
    this->top = this->mem;
    this->bottom = this->mem;
    this->next = 0x0;
}

template <typename T>
LinkStack<T>::LinkStack(int capacity) {
    this->capacity = capacity;
    this->mem = new T[capacity];
    this->top = this->mem;
    this->bottom = this->mem;
    this->next = 0x0;
}

template <typename T>
LinkStack<T>::~LinkStack() {
    delete this->mem;
}

template <typename T>
void LinkStack<T>::clear() {
    while (this->length()) {
        this->pop();
    }
}

template <typename T>
T LinkStack<T>::getTop() {
    if (this->top == this->bottom) {
        throw "This LinkStack is empty";
    }
    if (this->next) {
        T temp = this->next->pop();
        if (this->next->length() == 0) {
            delete this->next;
            this->next = 0x0;
        }
        return temp;
    }
    return *(this->top - 1);
}

template <typename T>
T LinkStack<T>::operator[](int index) {
    if (this->top == this->bottom) {
        throw "This LinkStack is empty";
    }
    LinkStack<T>* temp = new LinkStack<T>();
    int pos = 0;
    T flag = this->pop();
    temp->push(flag);
    while (pos++ < this->length() - index) {
    	flag = this->pop();
    	temp->push(flag);
	}
    while (temp->length()) {
    	this->push(temp->pop());
	}
	
    return flag;
}

template <typename T>
int LinkStack<T>::length() {
    return this->next ? this->next->length() + this->top - this->bottom : this->top - this->bottom;
}

template <typename T>
T LinkStack<T>::pop() {
    if (this->top == this->bottom) {
        throw "This LinkStack is empty";
    }
    if (this->next) {
        T temp = this->next->pop();
        if (this->next->length() == 0) {
            delete this->next;
            this->next = 0x0;
        }
        return temp;
    }
    return *--this->top;
}

template <typename T>
int LinkStack<T>::push(T value) {
    if (this->top - this->bottom == this->capacity) {
        if (!this->next) {
            this->next = new LinkStack<T>(int(this->capacity * 1.2));
        }
        LinkStack<T>* that = this->next;
        return this->capacity + that->push(value);
    }
    *this->top++ = value;
    return this->top - this->bottom;
}


