#include <iostream>
#include <sstream>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Reactor.h"
#include "ace/Timer_Queue.h" 
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"
#include "ace/Connector.h"
#include "ace/Service_Object.h"
#include "ace/Guard_T.h"
#include "ace/Thread_Mutex.h"
#include <string>
#include <vector>
#include <iterator>
#include<sstream>

using namespace std;

class Client;
class RWEventHandler;
class Client_Svc_Handler;

ACE_Time_Value timeOfDay = ACE_OS::gettimeofday();
/*
 class MyService_Object:  public ACE_Service_Object
 {

 };*/

class RWEventHandler: public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> {
private:
	char* buffer;
	int id;
	Client* c;
	string status;

public:
	RWEventHandler();
	~RWEventHandler();
	virtual int handle_timeout(const ACE_Time_Value &, const void*);
	int getId() {
		return id;
	}
	void setId(int id) {
		this->id = id;
	}
	void setBuffer(char* b) {
		this->buffer = b;
	}
	void setClient(Client* cl) {
		this->c = cl;
	}
	Client* getClient() {
		return c;
	}
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> base;
	int run_svc(void);
	int init(int argc, char *argv[]);
	int fini();
	int suspend();
	int resume();
	int info();

};

RWEventHandler::RWEventHandler() {
	id = 0;
	cout << "Event handler constructor called " << this << endl;
	status = "Started";
}

RWEventHandler::~RWEventHandler() {
	ACE_Reactor::instance()->cancel_timer(this);
	cout << "Event Handler destructor called" << this << endl;

	//close();	

}

int RWEventHandler::init(int argc, char *argv[]) {
	ACE_UNUSED_ARG(argc);
	ACE_UNUSED_ARG(argv);

	//cout << "Init called " << endl;
	status = "Started";
	return 0;
}

int RWEventHandler::fini() {
	//cout << "fini called " << endl;
	ACE_Reactor::instance()->cancel_timer(this);
	close();
	return 0;
}

int RWEventHandler::suspend() {
	//cout << "suspend called " << endl;
	ACE_Reactor::instance()->cancel_timer(this);
	status = "Suspended";
	return 0;
}

int RWEventHandler::resume() {
	//cout << "resume called " << endl;
	const ACE_Time_Value timeout(30, 0); //3 secs
	ACE_Reactor::instance()->schedule_timer(this, (const void*) 0, timeout,
				timeout);
	status = "Resumed";
	return 0;
}

int RWEventHandler::info() {
	//cout << "info called " << endl;
	cout << "Info for obj ::" << this << "::" << status << endl;
	return 0;
}

class Client: public ACE_Connector<RWEventHandler, ACE_SOCK_CONNECTOR> {

private:
	int portNumber;
	char* buffer;
	int id;
	ACE_INET_Addr serverAddress;
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream peer; //tmp peer

public:
	Client(char*);
	~Client();
	int handle_signal(int signum, siginfo_t*, ucontext_t*);
	int makeConnection();
	int getPortNumber() {
		return portNumber;
	}
	int getId() {
		return id;
	}
	void setId(int id) {
		this->id = id;
	}
	typedef ACE_Connector<RWEventHandler, ACE_SOCK_CONNECTOR> base;
	//int open(

};

Client::Client(char* buffer) {
	portNumber = 11111;
	id = 0;
	this->buffer = buffer;

	stringstream ss;
	ss << this;
	//cout << "Client constructor called  " << ss.str() << endl;
	cout << "Client constructor called  " << this << endl;
}

Client::~Client() {
	//ACE_Reactor::instance()->remove_handler(this,ACE_Event_Handler::WRITE_MASK);
	cout << "Client destructor called " << this << endl;
	close();
	ACE_Reactor::instance()->end_reactor_event_loop();

}

typedef pair<string, RWEventHandler*> rl_pair;
vector<rl_pair> vec;
//part 2 singleton obj
struct SvcHandler_holder {
private:

	static SvcHandler_holder* instance;

public:
	string label;
	RWEventHandler* sh_ptr;
	Client* c_ptr;
	SvcHandler_holder() {
		label = "";
	}
	~SvcHandler_holder() {
		label = "";
		delete sh_ptr;
		delete c_ptr;
	}

	static SvcHandler_holder* getInstance() {
		if (!instance) {
			instance = new SvcHandler_holder();
		}
		return instance;
	}

};

SvcHandler_holder* SvcHandler_holder::instance = NULL;

int RWEventHandler::handle_timeout(const ACE_Time_Value &, const void*) {
	stringstream out;
	out << getClient()->getId();
	string buf = "id:";
	buf.append(out.str());
	buf.append(":data:");
	buf.append(buffer);
	cout << "Sending data" << endl;
	if (peer().send_n(buf.c_str(), 50) < 0) {
		return -1;

	}
	char newid[50];
	cout << "Receving" << endl;
	if (getClient()->getId() == 0 && peer().recv(newid, sizeof newid) > 0) {
		id = atoi(newid);
		//cout << newid << endl;
		getClient()->setId(id);
	}

	return 0;
}

struct isHandler {
	string s;
	isHandler(string val) :
			s(val) {
	}
	bool operator()(const rl_pair& x) const {
		stringstream temp;
		temp << x.second;
		return s.compare(temp.str()) == 0;
	}
};

struct isLabel {
	string s;
	isLabel(string val) :
			s(val) {
	}
	bool operator()(const rl_pair& x) const {

		return s.compare(x.first) == 0;
	}
};

struct printHandler {
	ostream& os;
	printHandler(ostream& out) :
			os(out) {
	}
	bool operator()(const rl_pair& x) const {
		os << "label: " << x.first << ":: :Address " << x.second << endl;
	}
};

class ActiveObject: public ACE_Task<ACE_MT_SYNCH> {

private:
	static ACE_Thread_Mutex lock;
public:
	ActiveObject() {
	}
	int svc(void);
//static int run_svc(void);
	int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);

};

ACE_Thread_Mutex ActiveObject::lock(0);

int ActiveObject::handle_input(ACE_HANDLE h) {
	//cout << "Component configurator thread started" << endl;
	vector<string> inp;
	stringstream ss;
	char buf[120];
	if (h == ACE_STDIN) {

		ACE_OS::fflush(stdout);
		ACE_OS::memset(buf, 0, sizeof buf);
		int result = ACE_OS::read(h, buf, sizeof buf);
		inp.clear();
		if (result == -1)
			ACE_ERROR((LM_ERROR, "can't read from STDIN"));

		else if (result > 0) {
			ss << buf;
			vector<string> tokens;
			copy(istream_iterator<string>(ss), istream_iterator<string>(),
					back_inserter<vector<string> >(tokens));

			if ((tokens[0].compare("rename") != 0
					&& tokens[0].compare("suspend") != 0
					&& tokens[0].compare("resume") != 0
					&& tokens[0].compare("info") != 0
					&& tokens[0].compare("fini") != 0)
					|| (tokens[0].compare("rename") == 0 && tokens.size() != 3)
					|| (tokens[0].compare("rename") != 0 && tokens.size() != 2)) {
				cout << "handle_input::Bad command" << endl;
				return 0;
			}

			cout << endl;
			inp.clear();
		}

		ACE_Message_Block* block;
		ACE_NEW_RETURN(block, ACE_Message_Block(result + 1), -1);
		block->copy(buf, result + 1);
		if (this->putq(block) == -1) {
			cout << "error putting message to Queue" << endl;
			block->release();
			return -1;
		}
		block->reset();

	}

	return 0;
}

int ActiveObject::svc(void) {
	//cout << "Thread started" << endl;
	while (true) {
		ACE_Message_Block* block;
		stringstream ss(stringstream::in | stringstream::out);
		if (this->getq(block) == -1) {
			ACE_ERROR_RETURN((LM_ERROR, "%p\n", "getq"), -1);
		}
		ACE_Guard < ACE_Thread_Mutex > guard(lock);
		char* s = reinterpret_cast<char*>(block->rd_ptr());int
		i = strlen(block->rd_ptr());
		*(s + i) = '\0';
		ss << s;
		vector<string> tokens;
		copy(istream_iterator<string>(ss), istream_iterator<string>(),
				back_inserter<vector<string> >(tokens));
		//copy(tokens.begin(),tokens.end(),ostream_iterator<string>(cout,"||"));
		if ((tokens[0].compare("rename") != 0
				&& tokens[0].compare("suspend") != 0
				&& tokens[0].compare("resume") != 0
				&& tokens[0].compare("info") != 0
				&& tokens[0].compare("fini") != 0)
				|| (tokens[0].compare("rename") == 0 && tokens.size() != 3)
				|| (tokens[0].compare("rename") != 0 && tokens.size() != 2)) {
			cout << "svc::Bad command" << endl;
			return 0;
		} else {

			if (tokens[0].compare("rename") == 0) {
				vector<rl_pair>::iterator it = find_if(vec.begin(), vec.end(),
						isHandler(tokens[1]));
				if (it == vec.end()) {
					cout << "Passed handler object not found" << endl;
					return 0;
				}

				it->first = tokens[2];
				for_each(vec.begin(), vec.end(), printHandler(cout));

			} else if (tokens[0].compare("suspend") == 0) {
				vector<rl_pair>::iterator it = find_if(vec.begin(), vec.end(),
						isLabel(tokens[1]));
				if (it == vec.end()) {
					cout << "Passed handler object not found" << endl;
					return 0;
				}
				it->second->suspend();
			} else if (tokens[0].compare("resume") == 0) {
				vector<rl_pair>::iterator it = find_if(vec.begin(), vec.end(),
						isLabel(tokens[1]));
				if (it == vec.end()) {
					cout << "Passed handler object not found" << endl;
					return 0;
				}
				it->second->resume();
			} else if (tokens[0].compare("fini") == 0) {
				vector<rl_pair>::iterator it = find_if(vec.begin(), vec.end(),
						isLabel(tokens[1]));
				if (it == vec.end()) {
					cout << "Passed handler object not found" << endl;
					return 0;
				}
				it->second->fini();
			} else if (tokens[0].compare("info") == 0) {
				vector<rl_pair>::iterator it = find_if(vec.begin(), vec.end(),
						isLabel(tokens[1]));
				if (it == vec.end()) {
					cout << "Passed handler object not found" << endl;
					return 0;
				}
				it->second->info();
			}

		}


		block->release();
	}
	return 0;

}

int Client::handle_signal(int signum, siginfo_t*, ucontext_t*) {
	switch (signum) {
	case SIGINT: {
		cout << " You pressed Ctrl C " << endl;
		//ACE_Reactor::instance()->cancel_timer(this);
		ACE_Reactor::instance()->end_reactor_event_loop();
		break;
	}
	}
	return 0;

}


int Client::makeConnection() {
	const ACE_Time_Value timeout(30, 0); //3 secs
	RWEventHandler *sh = new RWEventHandler();
	ACE_INET_Addr addr(11111, "127.0.1.1");

	if (connect(sh, addr) == -1) {
		cout << "Connection failed" << endl;
		return -1;
	}

	sh->setClient(this);
	sh->setBuffer(buffer);
	SvcHandler_holder* sh_hld = SvcHandler_holder::getInstance();
	sh_hld->sh_ptr = sh;
	rl_pair rl;
	rl.first = sh_hld->label;
	rl.second = sh_hld->sh_ptr;
	vec.push_back(rl);
	for_each(vec.begin(),vec.end(),printHandler(cout));
	ACE_Reactor::instance()->schedule_timer(sh, (const void*) 0, timeout,
			timeout);
	//ACE_Reactor::instance()->run_reactor_event_loop();

	return 0;

}


int main(int argc, char** argv) {

	const ACE_Time_Value timeout(3, 0); //3 secs

	if (argc == 1) {
		cout << "No command line arguments found." << endl;
		return -1;
	}

	char* buffer;
	int i = 1;
	string buf;
	while (i < argc) {
		buf += argv[i];
		buf += " ";
		i++;
	}
	buffer = &buf[0];
	cout << buffer << endl;
	Client c(buffer);
	SvcHandler_holder* sh_hld = SvcHandler_holder::getInstance();
	sh_hld->c_ptr = &c;
	//cout << "label: " << sh_hld->label << ":: :Address " << sh_hld->c_ptr
		//	<< endl;

	c.makeConnection();

	ActiveObject* ao = new (nothrow) ActiveObject();
	ACE_Event_Handler::register_stdin_handler(ao, ACE_Reactor::instance(),
			ACE_Thread_Manager::instance());
	ao->activate();

	//ACE_Reactor::instance()->schedule_timer(&c,(const void*)0,timeout,timeout);
	//part 1
	ACE_Reactor::instance()->register_handler(SIGINT, &c);
	ACE_Reactor::instance()->run_reactor_event_loop();

	cout << "Client reached outside if" << endl;
	return 0;

}
