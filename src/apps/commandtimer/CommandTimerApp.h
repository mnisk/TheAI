/*
 * Copyright 2020. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Jason Scaroni, jscaroni@calpoly.edu
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef COMMANDTIMERAPP_H
#define COMMANDTIMERAPP_H

#include "CommandTimerWindow.h"
#include <Application.h>

class CommandTimerApp : public BApplication
{
public:
	CommandTimerApp();
	virtual void ArgvReceived(int32 argc, char** argv);


private:
	CommandTimerWindow* cTWindow;
	 const char* fCommand;
};

#endif
