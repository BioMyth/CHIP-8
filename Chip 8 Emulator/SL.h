
template<typename T>
class SortedList{
public:
	void insert(T newData);
	T* iterate();
	T pop();
	T peek();
	bool empty();

private:
	struct node{
		node(T data=nullptr,node* next=nullptr):data(data),next(next){};
		~node(){
			delete this->next;
		}
		node* next;
		T data;
	};
	node* root;
	node* cursor;
};

template<typename T>
void SortedList<T>::insert(T newData){
	node** cursor=&this->root;
	while((*cursor)!=nullptr && (*cursor)->data<newData)
		cursor=&(*cursor)->next;

	//node* tmp=(*cursor);
	(*cursor)=new node(newData,(*cursor));//tmp);
	//(*cursor)->data=newData;
	//(*cursor)->next=tmp;
}

template<typename T>
T* SortedList<T>::iterate(){
	if(this->cursor==nullptr){
		this->cursor=this->root;
		return nullptr;
	}
	T prev=cursor->data;
	this->cursor=this->cursor->next;
	return &prev;
}

template<typename T>
T SortedList<T>::pop(){
	T tmp=root->data;
	node* oldRoot=this->root;
	this->root=this->root->next;
	delete oldRoot;
	return tmp;
}

template<typename T>
T SortedList<T>::peek(){
	return root->data;
}

template<typename T>
bool SortedList<T>::empty(){
	return (root==nullptr);
}