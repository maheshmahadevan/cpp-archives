#include<iostream>
#include<sstream>
#include "ace/Guard_T.h"
#include "ace/Thread_Mutex.h"
#include "ace/Task.h"
#include "ace/TSS_T.h"
#include<iterator>
#include<vector>
using namespace std;

class Counter{
public:
   	Counter(): count(0) {}
	void increment() { count++;}	
	int value()	{ return count; }

private:
	int count;
};

static ACE_TSS<Counter> message_count;

class ActiveObject : public ACE_Task<ACE_MT_SYNCH>
{

	private:
		static ACE_Thread_Mutex lock;
	public :
		ActiveObject() {}
		int svc(void);

};

ACE_Thread_Mutex ActiveObject::lock(0);

int ActiveObject::svc(void)
{
	cout << "Thread started" << endl;
	//wait();

		
	while(true)
	{
		ACE_Message_Block* block;
		if (this->getq (block) == -1)
    		{
     			 ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "getq"), -1);
    		}
		ACE_Guard<ACE_Thread_Mutex> guard(lock);
		message_count->increment();
		cout << "[" << ACE_Thread_Manager::instance()->thr_self() << "] ";
 		char* s = reinterpret_cast<char*>(block->rd_ptr());
		cout << "counter " << message_count->value() << " " << s ;
		cout << " [" << ACE_Thread_Manager::instance()->thr_self() << "]";
		block->release();
		cout<< endl;
	}

	return 0;
}

int main(int argc, char** argv)
{
	
	int numThreads = 1;
	if(argc > 1)
	{
		numThreads = atoi(argv[1]);
	}
	ActiveObject* ao = new (nothrow) ActiveObject();
	ao->activate(THR_NEW_LWP,numThreads);
	//ao->activate();
	
	
	while(true)
	{
		string str;
		cin >> str;
		//cout << str << endl;
		//cout << sizeof(string)*str.length() << endl;
		/*stringstream ss(stringstream::in | stringstream::out);
		ss<< str;
		vector<string> tokens;
		copy(istream_iterator<string>(ss),istream_iterator<string>(),back_inserter<vector<string> >(tokens));
		
		
		for(unsigned int i=0;i<tokens.size();i++)
		{*/
			
			ACE_Message_Block* block;
			ACE_NEW_RETURN(block,ACE_Message_Block(sizeof(char)*str.length()+1),-1);
			//ACE_NEW_RETURN(block,ACE_Message_Block(sizeof(str.c_str())),-1);
			//block->wr_ptr (tokens[i].length());	
			//block->wr_ptr(1);
			block->copy ((const char *) str.c_str(),sizeof(char)*str.length()+1);
			//block->copy ((const char *) str.c_str(),sizeof(str.c_str()));			
			//cout<< "Before put q" << endl;
			if(ao->putq(block) == -1)
			{	
	     		  cout << "error putting message to Queue" << endl;
	     		  block->release();
			  return -1;			
			}
			block->reset();
		//}
		//cout<<"Number of messages" <<ao->msg_queue()->message_count()<< endl;
		//ACE_OS::sleep(1);
		
	}
	
	
	return 0;

}
