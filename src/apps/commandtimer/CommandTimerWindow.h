/*
 * Copyright 2020. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Jason Scaroni, jscaroni@calpoly.edu
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef COMMANDTIMERWINDOW_H
#define COMMANDTIMERWINDOW_H

#include <Application.h>
#include <CheckBox.h>
#include <FilePanel.h>
#include <InterfaceKit.h>
#include <MessageRunner.h>
#include <Spinner.h>
#include <SupportKit.h>
#include <TextControl.h>
#include <Window.h>

enum panelMode {
	COMMAND_MODE,
	PATH_MODE,
};

class CommandTimerWindow : public BWindow
{
public:
	CommandTimerWindow(BRect cTWindowRect, const char* argv);

	virtual void MessageReceived(BMessage* cTMessage);
	virtual bool QuitRequested();

private:
	void 		doPulse();
	void 		updateTime();
	void 		startStopClock();
	void 		toggleRepeat();
	void 		toggleAlarm();
	void 		togglePath();
	void 		setPermissions(bool permission);
	void 		getTime();
	BString 	getPath(BMessage* message);
	BString 	getFolder(BMessage* message);
	void 		executeCommand();
	void 		executeCompileCommand();

	bool 		isRunning;
	int32 		seconds;
	int32 		secondsBackup;
	bool 		alarm;
	bool 		repeat;
	bool 		path;

	BButton* 	startStopButton;
	BButton*	nnButton;
	BButton*	chooseCommandButton;
	BButton*	choosePathButton;
	BCheckBox* 	repeatCheckBox;
	BCheckBox* 	alarmCheckBox;
	BCheckBox* 	pathCheckBox;
	BTextControl* commandTextControl;
	BTextControl* pathTextControl;
	BSpinner* 	hoursSpinner;
	BSpinner* 	minsSpinner;
	BSpinner* 	secsSpinner;
	BMessageRunner* runner;
	panelMode	mode;
	BFilePanel*	choosePanel;
	
	bool ExecuteOrNot = true;
};

#endif
