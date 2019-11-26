template <typename T>
class Stack {
    private:
        int capacity;
        T* mem;
        T* top;
        T* bottom;
    public:
        Stack(int capacity);
        ~Stack();
        void clear();
        int length();
        T pop();
        int push(T value);
};

template <typename T>
Stack<T>::Stack(int capacity) {
    this->capacity = capacity;
    this->mem = new T[capacity];
    this->top = this->mem;
    this->bottom = this->mem;
}

template <typename T>
Stack<T>::~Stack() {
    delete this->mem;
}

template <typename T>
void Stack<T>::clear() {
    while (this->length()) {
        this->pop();
    }
}

template <typename T>
int Stack<T>::length() {
    return this->top - this->bottom;
}

template <typename T>
T Stack<T>::pop() {
    if (this->top == this->bottom) {
        throw "This stack is empty.";
    }
    return *(--this->top);
}

template <typename T>
int Stack<T>::push(T value) {
    if (this->top - this->bottom == this->capacity) {
        throw "This stack is full.";
    }
    *this->top++ = value;
    return this->top - this->bottom;
}
