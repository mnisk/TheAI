/*
 * Copyright 2019, Adrien Destugues, pulkomandy@pulkomandy.tk.
 * Copyright 2011-2014, Rene Gollent, rene@gollent.com.
 * Copyright 2005-2009, Ingo Weinhold, bonefish@users.sf.net.
 * Distributed under the terms of the MIT License.
 */

#include "nnWindow.h"
#include "nn.hpp"

#include <map>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <AppMisc.h>
#include <AutoDeleter.h>
#include <Autolock.h>
#include <debug_support.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <Invoker.h>
#include <Path.h>

#include <DriverSettings.h>
#include <MessengerPrivate.h>
#include <RegExp.h>
#include <RegistrarDefs.h>
#include <RosterPrivate.h>
#include <Server.h>
#include <StringList.h>

#include <util/DoublyLinkedList.h>

// #include<wren.hpp>//wren language

#include <termios.h>
#include <File.h>
#include "DPath.h"

// command pipe include
#include <private/shared/CommandPipe.h>

// NN
#include <NN.h>

#include <msg.h>

class DPath;
class BFile;

//static const char* kDebuggerSignature = "application/x-vnd.Haiku-nn";
static const int32 MSG_DEBUG_THIS_TEAM = 'dbtt';


//#define TRACE_DEBUG_SERVER
#ifdef TRACE_DEBUG_SERVER
#	define TRACE(x) debug_printf x
#else
#	define TRACE(x) ;
#endif

#include "queue.h"
#include "file/TextFile.h"

using std::map;
using std::nothrow;


static const char *kSignature = "application/x-vnd.Haiku-nn";



class NN : public BServer {
public:
	NN(status_t &error);

	status_t Init();

	virtual bool QuitRequested();

private:
	static status_t _ListenerEntry(void *data);
	status_t _Listener();
	status_t GotMessage(Msg *message);

//	void _DeleteNNHandler(NNHandler *handler);

private:
//	typedef map<team_id, NNHandler*>	NNHandlerMap;

	port_id				fListenerPort;
	thread_id			fListener;
	bool				fTerminating;
};


// #pragma mark -


// #pragma mark -


NN::NN(status_t &error)
	:
	BServer(kSignature, false, &error),
	fListenerPort(-1),
	fListener(-1),
	fTerminating(false)
{
}


class DebugMessage : public DoublyLinkedListLinkImpl<DebugMessage> {
public:
	DebugMessage()
	{
	}

	void SetCode(debug_debugger_message code)		{ fCode = code; }
	debug_debugger_message Code() const				{ return fCode; }

	debug_debugger_message_data &Data()				{ return fData; }
	const debug_debugger_message_data &Data() const	{ return fData; }

private:
	debug_debugger_message		fCode;
	debug_debugger_message_data	fData;
};

typedef DoublyLinkedList<DebugMessage>	DebugMessageList;

typedef int (*Func)(void);
//testing wren
/*static void writeFn(WrenVM* vm, const char* text)
{
  debug_printf("%s", text);
}
void errorFn(WrenVM* vm, WrenErrorType errorType,
             const char* module, const int line,
             const char* msg)
{
  switch (errorType)
  {
    case WREN_ERROR_COMPILE:
    {
      debug_printf("[%s line %d] [Error] %s\n", module, line, msg);
    } break;
    case WREN_ERROR_STACK_TRACE:
    {
      debug_printf("[%s line %d] in %s\n", module, line, msg);
    } break;
    case WREN_ERROR_RUNTIME:
    {
      debug_printf("[Runtime Error] %s\n", msg);
    } break;
  }
}

*/
//done
extern "C" int max(int ,int);

/*
thread_id
PipeCommand(int argc, const char** argv, int& in, int& out,
	int& err, const char** envp = (const char**)environ)
{
	// This function written by Peter Folk <pfolk@uni.uiuc.edu>
	// and published in the BeDevTalk FAQ
	// http://www.abisoft.com/faq/BeDevTalk_FAQ.html#FAQ-209

	// Save current FDs
	int old_out = dup(1);
	int old_err = dup(2);

	int filedes[2];
	
	// create new pipe FDs as stdout, stderr
	pipe(filedes);  dup2(filedes[1], 1); close(filedes[1]);
	out = filedes[0]; // Read from out, taken from cmd's stdout
	pipe(filedes);  dup2(filedes[1], 2); close(filedes[1]);
	err = filedes[0]; // Read from err, taken from cmd's stderr

	// taken from pty.cpp
	// create a tty for stdin, as utilities don't generally use stdin
	int master = posix_openpt(O_RDWR);
	if (master < 0)
		return -1;

	int slave;
	const char* ttyName;
	if (grantpt(master) != 0 || unlockpt(master) != 0
		|| (ttyName = ptsname(master)) == NULL
		|| (slave = open(ttyName, O_RDWR | O_NOCTTY)) < 0) {
		close(master);
		return -1;
	}

	int pid = fork();
	if (pid < 0) {
		close(master);
		close(slave);
		return -1;
	}

	// child
	if (pid == 0) {
		close(master);

		setsid();
		if (ioctl(slave, TIOCSCTTY, NULL) != 0)
			return -1;

		dup2(slave, 0); 
		close(slave);

		// "load" command.
		execv(argv[0], (char *const *)argv);

		// shouldn't return
		return -1;
	}

	// parent
	close (slave);
	in = master;

	// Restore old FDs
	close(1); dup(old_out); close(old_out);
	close(2); dup(old_err); close(old_err);

	// Theoretically I should do loads of error checking, but
	// the calls aren't very likely to fail, and that would
	// muddy up the example quite a bit. YMMV.

	return pid;
}*/
/*
thread_id
PipeCommand(int argc, const char** argv, int& in, int& out, int& err,
	//const char** envp const char** envp = (const char**)environ)
{
	// This function written by Peter Folk <pfolk@uni.uiuc.edu>
	// and published in the BeDevTalk FAQ
	// http://www.abisoft.com/faq/BeDevTalk_FAQ.html#FAQ-209

	if (!envp)
		envp = (const char**)environ;

	// Save current FDs
	int old_in  =  dup(0);
	int old_out  =  dup(1);
	int old_err  =  dup(2);

	int filedes[2];

	// Create new pipe FDs as stdin, stdout, stderr
	pipe(filedes);  dup2(filedes[0], 0); close(filedes[0]);
	in = filedes[1];  // Write to in, appears on cmd's stdin
	pipe(filedes);  dup2(filedes[1], 1); close(filedes[1]);
	out = filedes[0]; // Read from out, taken from cmd's stdout
	pipe(filedes);  dup2(filedes[1], 2); close(filedes[1]);
	err = filedes[0]; // Read from err, taken from cmd's stderr

	// "load" command.
	thread_id ret  =  load_image(argc, argv, envp);
	if (ret < B_OK)
		goto cleanup;

	// thread ret is now suspended.

	setpgid(ret, ret);

cleanup:
	// Restore old FDs
	close(0); dup(old_in); close(old_in);
	close(1); dup(old_out); close(old_out);
	close(2); dup(old_err); close(old_err);

	//Theoretically I should do loads of error checking, but
	//   the calls aren't very likely to fail, and that would 
	//   muddy up the example quite a bit.  YMMV. 

	return ret;
}*/
// Option flags for SourceType::CreateSourceFile
enum
{
	SOURCEFILE_PAIR = 0x00000001	// Create a source file and a partner file, if appropriate
};

entry_ref
MakeProjectFile (DPath folder, const char *name, const char *data = NULL, const char *type =NULL)
{
	debug_printf("MakeProjectFile started\n");
	entry_ref ref;
	
	DPath path(folder);
	path.Append(name);
	debug_printf("MakeProjectFile %s\n",path.GetFullPath());

	BEntry entry(path.GetFullPath());
	
	if (entry.Exists())
	{
	debug_printf("MakeProjectFile file exists\n");
		//BString errstr = B_TRANSLATE("%filepath% already exists. Do you want to overwrite it?");
		//errstr.ReplaceFirst("%filepath%", path.GetFullPath());
		//int32 result = ShowAlert(errstr.String(),B_TRANSLATE("Overwrite"),B_TRANSLATE("Cancel"));
		//if (result == 1)
		//	return ref;
	}
	
	//BFile file(path.GetFullPath(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	//BFile file("boot/home/Projects/any.cpp", B_READ_WRITE);


	/*BPath path2;
	if (find_directory(JS_MAK_DIRECTORY, &path2) == B_OK) {
		
	}
	
	path2.Append("any.cpp");
debug_printf("MakeProjectFile path2 = %s \n",path2.Path());*/
	debug_printf("MakeProjectFile path.path() = %s\n",path.GetFullPath());
	BFile file(path.GetFullPath(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
		//BFile fil2e(path.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
		
	if (data && strlen(data) > 0)
		file.Write(data,strlen(data));
	
	BString fileType = (type && strlen(type) > 0) ? type : "text/x-source-code";
	file.WriteAttr("BEOS:TYPE",B_STRING_TYPE, 0, fileType.String(),
						fileType.Length() + 1);
	
	file.Unset();
	entry.GetRef(&ref);
	return ref;
}


BString
MakeHeaderGuard(const char *name)
{
	BString define(name);
	define.ReplaceSet(" .-","_");
	
	// Normally, we'd just put something like :
	// define[i] = toupper(define[i]);
	// in a loop, but the BString defines for Zeta are screwed up, so we're going to have to
	// work around them.
	char *buffer = define.LockBuffer(define.CountChars() + 1);
	for (int32 i = 0; i < define.CountChars(); i++)
		buffer[i] = toupper(buffer[i]);
	define.UnlockBuffer();
	
	BString guard;
	guard << "#ifndef " << define << "\n"
		<< "#define " << define << "\n"
			"\n"
			"\n"
			"\n"
			"#endif\n";
	return guard;
}


entry_ref
CreateSourceFile(const char *dir, const char *name, uint32 options, BString data2)
{
	if (!dir || !name)
		return entry_ref();
	
	BString folderstr(dir);
	if (folderstr.ByteAt(folderstr.CountChars() - 1) != '/')
		folderstr << "/";
	
	DPath folder(folderstr.String()), filename(name);
	
	bool is_cpp = false;
	bool is_header = false;
	bool create_pair = ((options & SOURCEFILE_PAIR) != 0);
	
	BString ext = filename.GetExtension();
	if ( (ext.ICompare("cpp") == 0) || (ext.ICompare("c") == 0) ||
		(ext.ICompare("cxx") == 0) || (ext.ICompare("cc") == 0) )
		is_cpp = true;
	else if ((ext.ICompare("h") == 0) || (ext.ICompare("hxx") == 0) ||
			(ext.ICompare("hpp") == 0) || (ext.ICompare("h++") == 0))
		is_header = true;
	
	if (!is_cpp && !is_header)
		return entry_ref();
	
	BString sourceName, headerName;
	if (is_cpp)
	{
		sourceName = name;//"newie.cpp";//filename.GetFileName();
		headerName = filename.GetBaseName();//"new";//filename.GetBaseName();
		headerName << ".h";
	}
	else
	{
		sourceName = "new";//filename.GetBaseName();
		sourceName << ".cpp";
		headerName = "newiee.cpp";//filename.GetFileName();
	}
		
	
	entry_ref sourceRef, headerRef;
	BString data;
	if (is_cpp || create_pair)
	{
		if (create_pair)
			data << "#include \"" << headerName << "\"\n\n";
			
		data.Append(data2);
		
		sourceRef = MakeProjectFile(folder.GetFullPath(),sourceName.String(),data.String());
	}
	
	if (is_header || create_pair)
	{
		data = MakeHeaderGuard(headerName.String());
		headerRef = MakeProjectFile(folder.GetFullPath(),headerName.String(),data.String());
	}
	
	return is_cpp ? sourceRef : headerRef;
}


void pushWordsToQueue(string input, Queue<string> &q){
	long unsigned int i = 0;
	string words;
	
	while(i != input.length()){
	
		if(input[i] == ' ' || input[i] == '!' || input[i] == '.' || input[i] == '?')
		{
			q.enQueue(words);
			words.clear();
		} else
		{
			words = words + input[i];
		}
		
		i++;
	}
}

static const uint32 kMsgCompile = 'DRCT';
status_t
NN::Init()
{
	debug_printf("NN::Init\n");
	
	debug_printf("NN::Init creating listener port\n");
	// create listener port
	fListenerPort = create_port(10, NN_PORT);
	if (fListenerPort < 0)
		return fListenerPort;

	// spawn the listener thread
	fListener = spawn_thread(_ListenerEntry, "neural listener",
		B_NORMAL_PRIORITY, this);
	if (fListener < 0)
		return fListener;

	// resume the listener
	resume_thread(fListener);
	
	
	debug_printf("Queue started \n");
	
	Queue<string> input;
	string ip_sentence;
	ip_sentence = "create a program to output 5.";
	
	pushWordsToQueue(ip_sentence, input);
	input.displayQ();
	
	BPath pathiii;
	pathiii.SetTo("boot/system/MaK/neural/ai.txt");
	//BFile file(path.Path(), B_READ_ONLY);
	TextFile file("/boot/system/MaK/neural/ai.txt", B_READ_ONLY);
	BString linedata = file.ReadLine();
	debug_printf("linedata = %s \n", linedata.String());
	//char buffer[100];
	//ssize_t bytesRead = file.Read( buffer, sizeof(buffer));
	
	
	debug_printf("Queue ended \n");

	debug_printf("Making the cpp file {started}...\n");
/*
entry_ref reff = CreateSourceFile("Haiku/system/","any.cpp", true);
entry_ref reff2 = CreateSourceFile("/","any2.cpp", true);
*/

	BString data;
	data << "#include <cstdio> \n"
	"int main(){ printf(\"output\"); return 0;} ";
	entry_ref reff3 = CreateSourceFile("/boot/home/","any2.cpp", true, data);



///BFile file("/boot/home/new.txt", B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
//BFile file2("/boot/home/new2.txt", B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
//const char* unknown = "mak-jaat";


		
//sourceRef = MakeProjectFile(folder.GetFullPath(),sourceName.String(),data.String());

//char unknown[10]={'m','a','k'};

///file.Write(data.String(), strlen(data));
//file.Write(unknown, sizeof(unknown));

	/*BPath pathii;
	if (find_directory(JS_MAK_DIRECTORY, &pathii) == B_OK) {
		pathii.Append("seeting.cpp");

		BFile file3(pathii.Path(), B_CREATE_FILE | B_READ_WRITE);
		//if (file.InitCheck() == B_OK)
		//	file.Read(&offset, sizeof(BPoint));
	}*/
	debug_printf("before launching the terminal...\n");


	BPath path;
	if(find_directory(B_SYSTEM_APPS_DIRECTORY, &path) != B_OK || path.Append("Terminal") != B_OK)
	{
	debug_printf("not foundd\n");
		path.SetTo("/boot/system/apps/Terminal");
	}
	

	BEntry entry(path.Path());
	entry_ref ref;
	
	///BString cmd;
	
	///cmd = "cd /boot/home; gcc any2.cpp";
	/*const char* arguments[] = {cmd.String(),"any2.cpp"};
	entry.GetRef( &ref);
	be_roster->Launch(&ref,2,arguments);
	*/
	
	//system(cmd);
	
	
	/*entry_ref pwd;
	int					fStdIn;
	int					fStdOut;
	int fStdErr;
	//BString cmd;
	int32 argc = 3;
	const char** argv = new const char * [argc + 1];

	argv[0] = strdup("/bin/sh");
	argv[1] = strdup("-c");
	argv[2] = strdup(cmd.String());
	argv[argc] = NULL;
	thread_id fThreadId;
	fThreadId = PipeCommand(argc, argv, fStdIn, fStdOut, fStdErr);
	set_thread_priority(fThreadId, B_LOW_PRIORITY);

	resume_thread(fThreadId);*/
	
	BPath targetPath;

	if (entry.GetPath(&targetPath) != B_OK)
	{debug_printf("error in terminal launching...");
	}

	// Launch the Terminal.
	const char* kTerminalSignature = "application/x-vnd.Haiku-Terminal";
	const char* argv[] = {"-w", "/boot/home", "-t", "AI", "/bin/sh", "-c", "gcc any2.cpp", NULL};
	be_roster->Launch(kTerminalSignature, 7, argv);
	
	// Launch the command without Terminal
/*	BString cd;
	//BString thePath;
	BString commandStr="gcc any2.cpp";
	cd.SetToFormat("cd '%s' &&","/boot/home");
	commandStr.Prepend(cd);
	
	BPrivate::BCommandPipe pipe;
	pipe.AddArg("/bin/sh");
	pipe.AddArg("-c");
	pipe.AddArg(commandStr);
	debug_printf("NNServer :: %s\n", commandStr.String());
	pipe.RunAsync();
*/	
	// Launch the Command Timer
/*	BMessage commandMsg('DRCT');//(kMsgCompile);
	const char* kCommandTimerSignature = "application/x-vnd.jas.CommandTimer";
	be_roster->Launch(kCommandTimerSignature, &commandMsg);
*/	
	//const char* argv[] = {"-w", "/boot/home", "-t", "AI", NULL};
	//const char* argv[] = {"-w", targetPath.Path(), "gcc", "any2.cpp", NULL};
	
	/*BPath rasta;
	BEntry entr(rasta.Path());
	entry_ref reference;
	BString cmdi;
	cmdi="gcc";
	const char* argvv[] = {cmdi.String()};
	if(entr.GetRef(&reference) != B_OK || be_roster->Launch(&reference,1,argvv) != B_OK)
	{
	debug_printf("unable to launch");
	}
	*/
	//static const uint32 FULLSCREEN							= 'fscr';
	
	///BMessage msg(FULLSCREEN);
	//msg->AddMessenger("fscr");

//	be_roster->Launch(kTerminalSignature, 4, argv);
	
	//BString command = "gcc input.cpp";
	//system(command);
	
	
	
	
	//be_roster->Launch(kTerminalSignature, &msg);
//	be_roster->Launch("application/x-vnd.Haiku-About");
	/*
	cmd="gcc --help";
	const char* argv[]={"gcc","--help"};//{"gcc --help"};//{cmd.String()};
	if(entry.GetRef(&ref) != B_OK || be_roster->Launch(&ref,2,argv) != B_OK)
	{
		debug_printf("unable to launch the terminal...\n");
	}*/

debug_printf("after launching the terminal...\n");

int delay=0;
while(delay<99999999){
delay++;
}
// running the code start
// it is executed at /boot/home
BString	fCommand = "a.out";//"pwd"; //"/bin/bash -c ./a.out";
	debug_printf("fCommand %s\n", fCommand.String());
	FILE* fd = popen(fCommand.String(),"r");
	if (fd != NULL) {
	debug_printf("got something\n");
		BString data;
		char buffer[4096];
		while (fgets(buffer,4096,fd)) {
			if (!ferror(fd)) {
				data += buffer;
			}
		}

		int status = pclose(fd);
		if (0 != status) {
				debug_printf("popen returned non zero (error) code: %i",status);
			}

		debug_printf("output of the command %s",data.String());
	}
// code ended

// trying to pass msg and run command timer
	debug_printf("command timer launch starts \n");
	//BMessage commandMsg('DRCT');//(kMsgCompile);
	BMessage commandMsg(kmsg);
	//commandMsg.what = 'DRCT';
	
	const char* cmndArgs[] = { "-r", "a.out", NULL};
	const char* kCommandTimerSignature = "application/x-vnd.jas.CommandTimer";
	be_roster->Launch(kCommandTimerSignature, 2, cmndArgs);

	//be_roster->Launch(kCommandTimerSignature, &commandMsg);
	
	debug_printf("Command timer launched\n");


	
/*	// create listener port
	fListenerPort = create_port(10, "kernel listener");
	if (fListenerPort < 0)
		return fListenerPort;

	// spawn the listener thread
	fListener = spawn_thread(_ListenerEntry, "kernel listener",
		B_NORMAL_PRIORITY, this);
	if (fListener < 0)
		return fListener;

	// register as default debugger
	// TODO: could set default flags
	status_t error = install_default_debugger(fListenerPort);
	if (error != B_OK)
		return error;

	// resume the listener
	resume_thread(fListener);
*/
	return B_OK;
}


bool
NN::QuitRequested()
{
	// Never give up, never surrender. ;-)
	return false;
}


status_t
NN::_ListenerEntry(void *data)
{
debug_printf("NN:: _ListenerEntry \n");
	return ((NN*)data)->_Listener();
}


status_t
NN::_Listener()
{
debug_printf("NN:: _Listener \n");
	while (!fTerminating) {
	debug_printf("NN:: _Listener fTerminating false \n");
		// receive the next debug message
		DebugMessage *message = new DebugMessage;
		int32 code;
		//char data[100];
		ssize_t bytesRead;
		//char abc[200]; //working good
		Msg *deta = new Msg();
		do {
		debug_printf("reading\n");
			/*bytesRead = read_port(fListenerPort, &code, &message->Data(),
				sizeof(debug_debugger_message_data));*/
			//below is working good
			//bytesRead = read_port(fListenerPort, &code, &abc,
			//	sizeof(abc));
			bytesRead = read_port(fListenerPort, &code, &deta->Data(),
				sizeof(kile));
			//bytesRead = read_port(fListenerPort, &code, data,
			//	sizeof(data));
		} while (bytesRead == B_INTERRUPTED);
		debug_printf("NN _Listener = %ld\n",bytesRead);

		if (bytesRead < 0) {
			debug_printf("debug_server: Failed to read from listener port: "
				"%s. Terminating!\n", strerror(bytesRead));
			exit(1);
		}
		debug_printf("code = %d\n", code);
/*debug_printf("debug_server: Got debug message: team: %" B_PRId32 ", code: %" B_PRId32
	 " %s\n", message->Data().origin.team, code, abc);*/
	 debug_printf("debug_server: Got debug message: team: %" B_PRId32 ", code: %" B_PRId32
	 " %ld area_id = %d \n", message->Data().origin.team, code, deta->Data().size, deta->Data().areaID);

		deta->SetCode((nn_msg)code);

		// dispatch the message
		GotMessage(deta);
	}

	return B_OK;
}


status_t NN::GotMessage(Msg *message){

	debug_printf("NN DispatchMessage \n");
	debug_printf("size = %ld \n", message->Data().size);
	
	switch(message->Code()){
		case NN_INIT:
			debug_printf("NN NN_INIT\n");
			
		break;
		default:
			debug_printf("NN GotMessage default\n");
		break;
	}
	
	
	return B_OK;
	
}


// #pragma mark -


int
main()
{
debug_printf("hello hi , i m nn , born right now...\n");
	status_t error;


debug_printf("hello hi , i m nn , adult...\n");
	BPath path;
	error = find_directory(JS_MAK_DIRECTORY, &path, true);
	if (error != B_OK)
		debug_printf("failed in creation MaK directory.\n");

//testing wren it works man
/*debug_printf("wren testing.....\n");
 WrenConfiguration config;
  wrenInitConfiguration(&config);
    config.writeFn = &writeFn;
    config.errorFn = &errorFn;
  WrenVM* vm = wrenNewVM(&config);

  const char* module = "main";
  const char* script = "System.print(\"I am running in a VM!\")";

  WrenInterpretResult result2 = wrenInterpret(vm, module, script);

  switch (result2) {
    case WREN_RESULT_COMPILE_ERROR:
      { printf("Compile Error!\n"); } break;
    case WREN_RESULT_RUNTIME_ERROR:
      { printf("Runtime Error!\n"); } break;
    case WREN_RESULT_SUCCESS:
      { printf("Success!\n"); } break;
  }

  wrenFreeVM(vm);
  debug_printf("wren testing done.....\n");
*/
//debug_printf("before main2...\n");
//int ret=main2();
//debug_printf("ret =%d\n",ret);
//debug_printf("after main2...\n");


	// for the time being let the debug server print to the syslog
	int console = open("/dev/dprintf", O_RDONLY);
	if (console < 0) {
		debug_printf("debug_server: Failed to open console: %s\n",
			strerror(errno));
	}
	dup2(console, STDOUT_FILENO);
	dup2(console, STDERR_FILENO);
	close(console);

	// create the team debug handler roster
/*	if (!NNHandlerRoster::CreateDefault()) {
		debug_printf("debug_server: Failed to create team debug handler "
			"roster.\n");
		exit(1);
	}
*/
	// create application
	NN server(error);
	if (error != B_OK) {
		debug_printf("debug_server: Failed to create BApplication: %s\n",
			strerror(error));
		exit(1);
	}

	// init application
	error = server.Init();
	if (error != B_OK) {
		debug_printf("debug_server: Failed to init application: %s\n",
			strerror(error));
		exit(1);
	}

	server.Run();

	return 0;
}
