#include<iostream>
#include<string>
#include<ostream>
#include<cstdlib>
#include "ace/Thread_Manager.h"
#include "ace/Guard_T.h"
#include "ace/Thread_Mutex.h"
#include "ace/Thread.h" 
#include "ace/Null_Mutex.h"
#include "ace/Condition_Thread_Mutex.h"
using namespace std;
int myInt;
string buf;



template <class LOCK>
class Sync
{
	private:
		ostream &out;
		int i;
		const string& str;
		static LOCK lock;
		static int j;
		ACE_Condition_Thread_Mutex condMutex;
				
	public:
		Sync(ostream &o = cout , int ii = myInt , const string& s = buf): out(o),i(ii),str(s),condMutex(lock) {}
		//Sync() {}
		~Sync() {} 
		static void* run_svc(void* o);
		static void* run_svc_i(void* o);
		typedef ACE_Guard<LOCK> Guard;
		
		
		
		


};

typedef Sync<ACE_Thread_Mutex> Sync_mutex;
//typedef Sync<ACE_Null_Mutex> Sync_mutex;


template<> ACE_Thread_Mutex Sync_mutex::lock(0) ;
//template<> ACE_Null_Mutex Sync_mutex::lock=0;

typedef ACE_Singleton<Sync_mutex,ACE_Thread_Mutex> Singleton; 

template<> void* Sync_mutex::run_svc_i(void* o)
{
	
	if(o == 0)
	   return 0;
	Sync_mutex *s = (Sync_mutex*) o;
	Guard guard(lock);
	s->out << "Thread Id started: [" << ACE_Thread_Manager::instance()->thr_self() << "]"<<endl;

	//part1
	//ACE_OS::sleep(10);
	//part2
	ACE_Time_Value tenSecs(10);
	ACE_Time_Value oneTenth(0.1);
	ACE_Time_Value enterTime = ACE_OS::gettimeofday();
	//cout << "Enter Time " << enterTime << endl;
	ACE_Time_Value newTime = ACE_OS::gettimeofday();
	//cout << "New Time " << newTime << endl;
	while((newTime-enterTime) <= tenSecs)
	{
		//cout << newTime-enterTime << endl;
		//if(s->condMutex.wait(&oneTenth) == -1)
		//	cout<< "Error on calling wait" << endl;
		s->condMutex.wait(&oneTenth);
		newTime = ACE_OS::gettimeofday();
	}

	//s->condMutex.broadcast();

	//part 3
	//s->condMutex.signal();

	//cout << s << endl;
	//Guard guard(lock);
	s->out << "Thread Id resumed: [" << ACE_Thread_Manager::instance()->thr_self() << "]"<<endl;
	s->out << s->str;
	s->out <<endl;	
	s->out << "Thread Id ended: [" << ACE_Thread_Manager::instance()->thr_self() << "]"<<endl;			
	
  	
		
return 0;

}


template<> void* Sync_mutex::run_svc(void* o)
{
	
	//if(o == 0)
	  // return 0;

	//Sync_mutex *s = (Sync_mutex*) o;
	Sync_mutex *s = Singleton::instance();

	for(int j = 0 ; j < s->i ; j++)
	{
		Sync::run_svc_i(s);	
	}		

return 0;

}

void print (void* args)
{
  int id = ACE_Thread_Manager::instance()->thr_self();
  std::cout << "Thread Id: " << id << std::endl;
}


int main( int argc,char** argv )
{

 
	// store interger argument
	
	myInt = atoi(argv[1]);

	// store string arguments
	int i = 2;
//  	string buf;
 	while(i < argc )
	{
	     buf+=argv[i];
	     buf+=" ";
	     i++;
	}
	//const string& str = buf;
	Sync_mutex s(cout,myInt,buf);
	if(ACE_Thread_Manager::instance()->spawn_n(4,(ACE_THR_FUNC)Sync_mutex::run_svc,/*(void*) &s*/0,THR_JOINABLE | THR_NEW_LWP) == -1)
		return -1;

	//ACE_Thread_Manager::instance()->spawn_n(4, (ACE_THR_FUNC) print, 0, THR_JOINABLE | THR_NEW_LWP);
	ACE_Thread_Manager::instance()->wait();


	
	return 0;
}
