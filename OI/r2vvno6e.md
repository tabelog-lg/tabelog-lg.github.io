# c++汉诺塔循环解法（非递归）
```cpp
#include <iostream>
#include <cmath>
const int StackSize = 100;

template <class DataType>
class SeqStack {
	public:
		SeqStack();
		~SeqStack();
		void Push(DataType x);
		DataType Pop();
		DataType GetTop();
		int Empty();
	private:
		DataType data[StackSize];
		int top;
};

template<class DataType>
SeqStack<DataType>::SeqStack() {
	top = -1;
}

template<class DataType>
SeqStack<DataType>::~SeqStack() {
}

template<class DataType>
void SeqStack<DataType>::Push(DataType x) {
	if (top == StackSize - 1) throw "入栈失败,栈已满";
	top++;
	data[top] = x;
}

template<class DataType>
DataType SeqStack<DataType>::Pop() {
	DataType x;
	if (top == -1) throw "出栈失败,栈已空";
	x = data[top--];
	return x;
}

template<class DataType>
DataType SeqStack<DataType>::GetTop() {
	if (top != -1)return data[top];
	else return 999;
}

template<class DataType>
int
SeqStack<DataType>::Empty() {
	if (top == -1)return 1;
	else return 0;
}
using namespace std;

void putplate(int n, SeqStack<int> &A);                 
int operate1inA(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n);
int operate1inB(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n);
int operate1inC(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n);
int operate1inOddA(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n);
int operate1inOddB(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n);
int operate1inOddC(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n);
void output(char a,char b,int n);
int main() {
	int n;
	cin >> n;
	SeqStack<int>A;
	SeqStack<int>B;
	SeqStack<int>C;
	putplate(n,A);
	try{
		if (n % 2 == 0){
			for (int i = 0;;)
			{
				if (A.GetTop() == 1){
					if (operate1inA(A, B, C, i, n)){
						break;
					}
				}
				else if (B.GetTop() == 1){
					if (operate1inB(A, B, C, i, n)){
						break;
					}
				}
				else if (C.GetTop() == 1){
					if (operate1inC(A, B, C, i, n)){
						break;
					}
				}
			}
		}
		else {
			for (int i = 0;;)			
			{
				if (A.GetTop() == 1){
					if (operate1inOddA(A, B, C, i, n)){
						break;
					}
				}
				else if (B.GetTop() == 1){
					if (operate1inOddB(A, B, C, i, n)){
						break;
					}
				}
				else if (C.GetTop() == 1){
					if (operate1inOddC(A, B, C, i, n)){
						break;
					}
				}
			}
		}
	}
	catch (char *r){
		cout << r << endl;
	}
	return 0;
}
 
void putplate(int n, SeqStack<int> &A){
	for (int i = n; i >= 1; i--){
		A.Push(i);
	}
	return;
}
 
int operate1inA(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n){
	B.Push(1);
	A.Pop();
	i++;
	output('A','B',1);
	if (i >= (pow(2, n) - 1)){
		return 1;
	}
	if (C.GetTop() < A.GetTop()){
        output('C','A',C.GetTop());
		A.Push(C.GetTop());
		C.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;									
		}
	}
	else if (A.GetTop() < C.GetTop()){
		output('A','C',A.GetTop());
		C.Push(A.GetTop());
		A.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;
		}
	}
	return 0;
}
int operate1inB(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n){
	B.Pop();
	C.Push(1);
	i++;
	output('B','C',1);
	if (i >= (pow(2, n) - 1)){						
		return 1;
	}
	if (B.GetTop() < A.GetTop()){				
		output('B','A',B.GetTop());
		A.Push(B.GetTop());					
		B.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;
		}
	}
	else if (A.GetTop() < B.GetTop()){
		output('A','B',A.GetTop());
		B.Push(A.GetTop());							
		A.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;
		}
	}
	return 0;
}
int operate1inC(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n){
	C.Pop();
	A.Push(1);
	i++;
	output('C','A',1);
	if (i >= (pow(2, n) - 1)){						
		return 1;
	}
	if (B.GetTop() < C.GetTop()){				
		output('B','C',B.GetTop());
		C.Push(B.GetTop());							
		B.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;
		}
	}
	else if (C.GetTop() < B.GetTop()){
		output('C','B',C.GetTop());
		B.Push(C.GetTop());							
		C.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;
		}
	}
	return 0;
}
int operate1inOddA(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n){
	C.Push(1);
	A.Pop();
	i++;
	output('A','C',1);
	if (i >= (pow(2, n) - 1)){
		return 1;
	}
	if (B.GetTop() < A.GetTop()){					
		output('B','A',B.GetTop());
		A.Push(B.GetTop());
		B.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){
			return 1;										
		}
	}
	else if (A.GetTop() < B.GetTop()){
		output('A','B',A.GetTop());
		B.Push(A.GetTop());								
		A.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;
		}
	}
	return 0;											
}
int operate1inOddC(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n){
	C.Pop();
	B.Push(1);
	i++;
	output('C','B',1);
	if (i >= (pow(2, n) - 1)){						
		return 1;
	}
	if (C.GetTop() < A.GetTop()){				
		output('C','A',C.GetTop());
		A.Push(C.GetTop());							
		C.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;
		}
	}
	else if (A.GetTop() < C.GetTop()){
		output('A','C',A.GetTop());
		C.Push(A.GetTop());							
		A.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){					
			return 1;
		}
	}
	return 0;
}
int operate1inOddB(SeqStack<int>&A, SeqStack<int>&B, SeqStack<int>&C, int &i, int n){
	B.Pop();
	A.Push(1);
	i++;
	output('B','A',1);
	if (i >= (pow(2, n) - 1)){						
		return 1;
	}
	if (B.GetTop() < C.GetTop()){			
		output('B','C',B.GetTop());
		C.Push(B.GetTop());							
		B.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;
		}
	}
	else if (C.GetTop() < B.GetTop()){
		output('C','B',C.GetTop());
		B.Push(C.GetTop());							
		C.Pop();
		i++;
		if (i >= (pow(2, n) - 1)){						
			return 1;
		}
	}
	return 0;
}
void output(char a,char b,int n) {
	cout<<a<<"-"<<n<<"->"<<b<<endl;
	return;
}
```