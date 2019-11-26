#include<iostream>
#include<vector>
using namespace std;


template< typename T >
    class ActiveHeap {
        private:
            std::vector < T > ActiveHeapArray;
            int maxSize;
            T minElement;
            int size;
            void chart(int index);
            void tree(int index);
            void trickleDown(int index);
            void trickleUp(int index);
            int (*compare)(T a, T b);
        public:
            ActiveHeap < T > (int (*compare)(T a ,T b));
            ActiveHeap < T > (int volume, int (*compare)(T a ,T b));
            ActiveHeap < T > (int * array, int length, int (*compare)(T a ,T b));
            ActiveHeap < T > (int * array, int startIndex, int endIndex, int (*compare)(T a ,T b));
            void clear();
            void displayArray();
            bool empty();
            int getMaxSize();
            int getSize();
            void insert(T value);
            bool isFull();
            T pop_out();
    };

template< typename T >
    ActiveHeap< T >::ActiveHeap(int (*compare)(T a ,T b)) {
        ActiveHeapArray.resize(20);
        maxSize = 20;
        size = 0;
        this->compare = *compare;
    }

template< typename T >
    ActiveHeap< T >::ActiveHeap(int volume, int (*compare)(T a ,T b)) {
        ActiveHeapArray.resize(volume);
        maxSize = volume;
        size = 0;
        this->compare = *compare;
    }

template< typename T >
    ActiveHeap< T >::ActiveHeap(int * array, int length, int (*compare)(T a ,T b)) {
        ActiveHeapArray.resize(length);
        maxSize = length;
        size = 0;
        for (int i = 0; i < maxSize; i++)
            insert(array[i]);
        this->compare = *compare;
    }

template< typename T >
    ActiveHeap< T >::ActiveHeap(int * array, int startIndex, int endIndex, int (*compare)(T a ,T b)) {
        ActiveHeapArray.resize(endIndex - startIndex);
        maxSize = endIndex - startIndex;
        size = 0;
        for (int i = 0; i < maxSize; i++)
            insert(array[i + startIndex]);
        this->compare = *compare;
    }

template< typename T >
    void ActiveHeap< T >::clear() {
        for (int i = 0; i < size; i++)
            ActiveHeapArray[0] = 0;
        size = 0;
    }

template< typename T >
    void ActiveHeap< T >::displayArray() {
        if (empty()) {
            cout << "[]" << endl;
            return;
        }
        cout << "[";
        for (int i = 0; i < size - 1; i++)
            cout << ActiveHeapArray[i] << ", ";
        cout << ActiveHeapArray[size - 1] << "]" << endl;
    }

template< typename T >
    bool ActiveHeap< T >::empty() {
        return size == 0 ? true : false;
    }

template< typename T >
    int ActiveHeap< T >::getMaxSize() {
        return maxSize;
    }

template< typename T >
    int ActiveHeap< T >::getSize() {
        return size;
    }

template< typename T >
    void ActiveHeap< T >::insert(T value) {
        try {
            if (isFull()) {
                if (this->compare(value, this->minElement) > 0) {
                    int index = -1;
                    for (int i = 0; i < this->size; i++) {
                        if (this->compare(this->ActiveHeapArray[i], this->minElement) == 0) {
                            index = i;
                            break;
                        }
                    }
                    ActiveHeapArray[index] = value;
                    T min = this->ActiveHeapArray[0];
                    for (int i = 1; i < this->size; i++) {
                        if (this->compare(this->ActiveHeapArray[i], min) < 0) {
                            min = this->ActiveHeapArray[i];
                        }
                    }
                    this->minElement = min;
                    trickleUp(index);
                    return;
                }
                else {
                    return;
                }
            }
            ActiveHeapArray[size] = value;
            if (this->size == 0 || this->compare(value, this->minElement) < 0) {
                this->minElement = value;
            }
            if (size > 0)
                trickleUp(size);
            size++;
        } catch (const char * msg) {
            cerr << msg << endl;
        }
    }

template< typename T >
    bool ActiveHeap< T >::isFull() {
        return size == maxSize ? true : false;
    }

template< typename T >
    void ActiveHeap< T >::trickleUp(int index) {
        int parent = (index - 1) / 2;
        T temp = ActiveHeapArray[index];
        while (index > 0 && this->compare(ActiveHeapArray[index], ActiveHeapArray[parent]) > 0) {
            ActiveHeapArray[index] = ActiveHeapArray[parent];
            ActiveHeapArray[parent] = temp;
            index = parent;
            parent = (parent - 1) / 2;
            temp = ActiveHeapArray[index];
        }
    }

template< typename T >
    void ActiveHeap< T >::trickleDown(int index) {
        int child = index * 2 + 2;
        if (child < size)
            if (this->compare(ActiveHeapArray[child], ActiveHeapArray[index]) > 0) {
                T temp = ActiveHeapArray[index];
                ActiveHeapArray[index] = ActiveHeapArray[child];
                ActiveHeapArray[child] = temp;
                trickleDown(child);
            }
        if (--child < size)
            if (this->compare(ActiveHeapArray[child], ActiveHeapArray[index]) > 0) {
                T temp = ActiveHeapArray[index];
                ActiveHeapArray[index] = ActiveHeapArray[child];
                ActiveHeapArray[child] = temp;
                trickleDown(child);
            }
    }

template< typename T >
    T ActiveHeap< T >::pop_out() {
        try {
            if (empty())
                throw "!!!No any elements in the ActiveHeap";
            T top = ActiveHeapArray[0];
            ActiveHeapArray[0] = ActiveHeapArray[size - 1];
            size--;
            trickleDown(0);
            return top;
        } catch (const char * msg) {
            cerr << msg << endl;
            T voidNode;
            return voidNode;
        }
    }
