template <typename T>
struct LinkListNode {
    T value;
    LinkListNode<T>* next;
};

template <typename T>
class LinkList {
    private:
        LinkListNode<T>* root;
        int size;
    public:
        LinkList();
        T at(int index);
        void clear();
        int insert(T value, int pos);
        int length();
        T pop_out(int index);
        int push_back(T value);
        T operator[](int index);
};

template <typename T>
LinkList<T>::LinkList() {
    this->root = 0x0;
    this->size = 0;
}

template <typename T>
T LinkList<T>::at(int index) {
    LinkListNode<T>* flag = this->root;
    int pos = 0;
    while (pos < index) {
        flag = flag->next;
        pos++;
    }
    return flag->value;
}

template <typename T>
void LinkList<T>::clear() {
	while (this->size) {
    	this->pop_out(0);
	}
}

template <typename T>
int LinkList<T>::insert(T value, int pos) {
	LinkListNode<T>* temp = new LinkListNode<T>;
	temp->value = value;
	LinkListNode<T>* flag = this->root;
	LinkListNode<T>* left = this->root;
    int index = 0;
    while (index < pos) {
        flag = flag->next;
        if (index != 0) {
        	left = left->next;
		}
        index++;
    }
    temp->next = flag;
    if (pos == 0) {
    	this->root = temp;
	}
	else {
		left->next = temp;
	}
	return ++this->size;
}

template <typename T>
int LinkList<T>::length() {
	return this->size;
}

template <typename T>
T LinkList<T>::pop_out(int index) {
	LinkListNode<T>* flag = this->root;
	T t;
	if (index == 0) {
		this->root = this->size == 1 ? 0x0 : this->root->next;
		this->size--;
		t = flag->value;
	}
	else {
	    int pos = 1;
	    while (pos < index) {
	        flag = flag->next;
	        pos++;
	    }
	    t = flag->next->value;
	    flag->next = flag->next->next;
		this->size--;
	}
	return t;
}

template <typename T>
int LinkList<T>::push_back(T value) {
	LinkListNode<T>* temp = new LinkListNode<T>;
	temp->value = value;
    temp->next = 0x0;
	LinkListNode<T>* left = this->root;
    int index = 1;
    while (index < this->size) {
    	left = left->next;
        index++;
    }
    if (this->size == 0) {
    	this->root = temp;
	}
	else {
		left->next = temp;
	}
	return ++this->size;
}

template <typename T>
T LinkList<T>::operator[](int index) {
	return this->at(index);
}
