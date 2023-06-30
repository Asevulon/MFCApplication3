#pragma once
template<typename T>
class queue
{
private:
	struct Node
	{
		Node* prev = nullptr;
		Node* next = nullptr;
		T val;
	};
	Node* first;
	Node* last;
	Node* iter;
	int num;
public:
	queue()
	{
		first = nullptr;
		last = nullptr;
		num = 0;
	}
	void push(T value)
	{
		if (first == nullptr)
		{
			first = new Node;
			first->val = value;
			last = first;
			num++;
			ToBegin();
			return;
		}
		if (num==1)
		{
			last->next = new Node;
			last->next->prev = last;
			last = last->next;
			first->next = last;
			last->val = value;
			num++;
			ToBegin();
			return;
		}
			last->next = new Node;
			last->next->prev = last;
			last = last->next;
			last->val = value;
			num++;
	}
	T pop()
	{
		if (first == nullptr)return NULL;
		T buf = first->val;
		first = first->next;
		delete first->prev;
		num--;
		return buf;
	}
	~queue()
	{
		do
		{
			last = last->prev;
			delete last->next;
		} while (last != first);
		delete first;
	}
	void ToBegin()
	{
		iter = first;
	}
	void ToEnd()
	{
		iter = last;
	}
	void ToNext()
	{
	//	if (iter->next == nullptr)return;
		iter = iter->next;
	}
	void ToPrev()
	{
		if (iter->prev == nullptr)return;
		iter = iter.prev;
	}
	T GetIterValue()
	{
		T buf = iter->val;
		return buf;
	}
	int GetNum()
	{
		int buf = num;
		return buf;
	}
	bool IsEnd()
	{
		if (iter == last)return true;
		return false;
	}
 };