#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

template <class Number> struct prime
{
	bool operator()(Number x) const 
	{
		for (size_t i = 2; i < x; i++)
   		 {
      			if (x % i == 0 && i != x)
			        return false;
    		 }
    		return true;
 
	} 
};

int main()
{
	//part 1	
	vector<size_t> v;
	for(size_t i=2;i<=100;i++)
		v.push_back(i); 

	vector<size_t>::iterator first = v.begin();
	vector<size_t>::iterator last = v.end();
	for(;first!=last;)
	{
		vector<size_t>::iterator p;
		p = find_if(first,last,prime<size_t>());
		if(*p==0)
			++first;
		else	
		{
			cout << *p << endl;	
			first = ++p;			
		}
	}

}

