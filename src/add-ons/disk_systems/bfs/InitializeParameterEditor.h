/*
 * Copyright 2013, Axel Dörfler, axeld@pinc-software.de.
 * Copyright 2009-2010, Stephan Aßmus <superstippi@gmx.de>
 * Copyright 2009, Bryce Groff, brycegroff@gmail.com.
 * Distributed under the terms of the MIT License.
 */
#ifndef _INITIALIZE_PARAMETER_EDITOR
#define _INITIALIZE_PARAMETER_EDITOR


#include <PartitionParameterEditor.h>
#include <String.h>


class BCheckBox;
class BMenuField;
class BMenuItem;
class BTextControl;
class BView;


class InitializeBFSEditor : public BPartitionParameterEditor {
public:
								InitializeBFSEditor();
	virtual						~InitializeBFSEditor();

	virtual		void			SetTo(BPartition* partition);

	virtual		bool			ValidateParameters() const;
	virtual		status_t		ParameterChanged(const char* name,
									const BVariant& variant);

	virtual		BView*			View();

	virtual		status_t		GetParameters(BString& parameters);

private:
				void			_CreateViewControls();

private:
				BView*			fView;
				BTextControl*	fNameControl;
				BMenuField*		fBlockSizeMenuField;
				BCheckBox*		fUseIndicesCheckBox;
				BMenuItem*		f1KBlockMenuItem;
				BMenuItem*		f2KBlockMenuItem;
				BMenuItem*		f4KBlockMenuItem;
				BMenuItem*		f8KBlockMenuItem;
};


#endif //_INITIALIZE_PARAMETER_EDITOR
