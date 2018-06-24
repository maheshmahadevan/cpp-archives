#include<iostream>
#include<deque>
#include<vector>
#include<list>
#include<iterator>
#include<set>
#include<map>
#include<functional>
#include<algorithm>
using namespace std;
/*
struct greater {
  bool operator() (int a, int b) const { return a > b; }
};*/

int main()
{

	deque<int> d;
	vector<int> v;
	list<int> l;

	d.push_front(5);
	d.push_front(4);
	d.push_front(3);
	d.push_front(2);
	d.push_front(1);
	
	l.push_back(1);
	l.push_back(2);
	l.push_back(3);
	l.push_back(4);
	l.push_back(5);

	
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
/*
	cout << "Deque print" << endl;
	for(deque<int>::iterator i = d.begin() ;i != d.end();i++)
	{
		cout<< *i << " ";
	}
	cout << endl;

	cout << "List print" << endl;
	for(list<int>::iterator i = l.begin() ;i != l.end();i++)
	{
		cout<< *i << " ";
	}
	cout << endl;

	cout << "Vector list" << endl;
	for(vector<int>::iterator i = v.begin() ;i != v.end();i++)
	{
		cout<< *i << " ";
	}
	cout << endl;

	cout << "Deque print" << endl;
	for(unsigned int x = 0 ;x < d.size();x++)
	{
		cout<< d[x] << " ";
	}
	cout << endl;

	cout << "List print" << endl;
	for(unsigned int y = 0 ;y < l.size();y++)
	{
		cout<< l[y] << " ";
	}
	cout << endl;

	cout << "Vector list" << endl;
	for(unsigned int z = 0 ;z < v.size();z++)
	{
		cout<< v[z] << " ";
	}
	cout << endl;*/




	set<int> s;
	multiset<int> m;
/*
	s.insert(1);
	s.insert(2);
	s.insert(1);
	s.insert(2);
	s.insert(6);
	s.insert(4);
*/
	cout << "Set print" << endl;
	for(set<int>::iterator i = s.begin() ;i != s.end();i++)
	{
		cout<< *i << " ";
	}
	cout << endl;

	m.insert(1);
	m.insert(2);
	m.insert(1);
	m.insert(6);
	m.insert(3);
	m.insert(4);

	cout << "Multiset print" << endl;
	for(multiset<int>::iterator i = m.begin() ;i != m.end();i++)
	{
		cout<< *i << " ";
	}
	cout << endl;

	map<int, const char *> mp1;
	map<const char *,int> mp2;

	mp1.insert(make_pair(2,"two"));
	mp1.insert(make_pair(3,"three"));
	mp1.insert(make_pair(1,"one"));
	mp1.insert(make_pair(4,"four"));
	mp1.insert(make_pair(4,"four"));

	cout << "Map <int,char> print" << endl;
	for(map<int, const char *>::iterator i = mp1.begin() ;i != mp1.end();i++)
	{
		cout<< i->first << " ";
	}
	cout<< endl;
	mp2.insert(make_pair("two",2));
	mp2.insert(make_pair("three",3));
	mp2.insert(make_pair("one",1));
	mp2.insert(make_pair("four",4));
	mp2.insert(make_pair("four",4));

	cout << "Map <char,int> print" << endl;
	for(map<const char *,int >::iterator i = mp2.begin() ;i != mp2.end();i++)
	{
		cout<< i->second << " ";
	}

return 0;
}
