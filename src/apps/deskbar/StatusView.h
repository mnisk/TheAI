/*
Open Tracker License

Terms and Conditions

Copyright (c) 1991-2000, Be Incorporated. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice applies to all licensees
and shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF TITLE, MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
BE INCORPORATED BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF, OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Be Incorporated shall not be
used in advertising or otherwise to promote the sale, use or other dealings in
this Software without prior written authorization from Be Incorporated.

Tracker(TM), Be(R), BeOS(R), and BeIA(TM) are trademarks or registered
trademarks of Be Incorporated in the United States and other countries. Other
brand product names are registered trademarks or trademarks of their respective
holders.
All rights reserved.
*/
#ifndef __STATUS_VIEW__
#define __STATUS_VIEW__


#include <Control.h>
#include <Node.h>
#include <Region.h>
#include <Query.h>
#include <Shelf.h>
#include <View.h>

#include "BarView.h"
#include "TimeView.h"


//#define FULL_MODE

const int32 kMinimumReplicantCount = 6;
const int32 kGutter = 1;
const int32 kTrayPadding = B_USE_HALF_ITEM_SPACING;

extern float sIconGap;
extern float gDragWidth, gDragRegionWidth;
extern float gMinReplicantHeight, gMinReplicantWidth;
extern float gMinimumTrayWidth, gMinimumWindowWidth, gMaximumWindowWidth;

#ifdef DB_ADDONS
struct DeskbarItemInfo {
	bool isAddOn;		// attribute tagged item
	int32 id;			// id given to replicant
	entry_ref entryRef;	// entry_ref to item tagged
	node_ref nodeRef;	// node_ref to boot vol item
};
#endif

class TReplicantShelf;

class TReplicantTray : public BView {
public:
									TReplicantTray(TBarView* barView);
		virtual						~TReplicantTray();

		virtual	void				AttachedToWindow();
		virtual	void				DetachedFromWindow();
		virtual	void				MouseDown(BPoint point);
		virtual	void				MessageReceived(BMessage*);
		virtual	void				GetPreferredSize(float*, float*);

				void				AdjustPlacement();
				void				ShowReplicantMenu(BPoint);

				TTimeView*			Time() const { return fTime; }
				void				ShowHideTime();

				status_t			ItemInfo(int32 target, const char** name);
				status_t			ItemInfo(const char* name, int32* id);
				status_t			ItemInfo(int32 index, const char** name,
										int32* id);

				bool				IconExists(int32 target, bool byIndex = false);
				bool				IconExists(const char* name);

				int32				ReplicantCount() const;
				float				MaxReplicantWidth() const
										{ return fMaxReplicantWidth; }
				float				MaxReplicantHeight() const
										{ return fMaxReplicantHeight; }

				status_t			AddIcon(BMessage*, int32* id,
										const entry_ref* = NULL);

				void				RemoveIcon(int32 target,
										bool byIndex = false);
				void				RemoveIcon(const char* name);

				BRect				IconFrame(int32 target,
										bool byIndex = false);
				BRect				IconFrame(const char* name);

				bool				AcceptAddon(BRect frame,
										BMessage* message);
				void				RealignReplicants(int32 startIndex = -1);

				void				SaveTimeSettings();

#ifdef DB_ADDONS
				status_t			LoadAddOn(BEntry* entry, int32* id,
										bool addToSettings = true);
#endif

private:
				BView*				ViewAt(int32* index, int32* id,
										int32 target,
										bool byIndex = false);
				BView*				ViewAt(int32* index, int32* id,
										const char* name);

				void				RealReplicantAdjustment(int32 startindex);

#ifdef DB_ADDONS
				void				InitAddOnSupport();
				void				DeleteAddOnSupport();

				DeskbarItemInfo*	DeskbarItemFor(node_ref &nodeRef);
				DeskbarItemInfo*	DeskbarItemFor(int32 id);
				bool				NodeExists(node_ref &nodeRef);

				void				HandleEntryUpdate(BMessage*);
				status_t			AddItem(int32 id, node_ref nodeRef,
										BEntry &entry, bool isAddon);

				void				UnloadAddOn(node_ref*, dev_t*, bool which,
										bool removeAll);
				void				RemoveItem(int32 id);

				void				MoveItem(entry_ref*, ino_t toDirectory);
#endif

				BPoint				LocationForReplicant(int32 index,
										float replicantWidth);
				BShelf*				Shelf() const;

				status_t			_SaveSettings();

	friend class TReplicantShelf;
	friend class TBarView;

				TTimeView*			fTime;
				TBarView*			fBarView;
				TReplicantShelf*	fShelf;
				BRect				fRightBottomReplicant;
				int32				fLastReplicant;
				float				fMaxReplicantWidth;
				float				fMaxReplicantHeight;
				float				fMinTrayHeight;

				float				fMinimumTrayWidth;
				float				fTrayPadding;
				float				fClockMargin;

				bool				fAlignmentSupport;
#ifdef DB_ADDONS
				BList*				fItemList;
				BMessage			fAddOnSettings;
#endif

};

enum {
	kNoDragRegion,
	kDontDrawDragRegion,
	kAutoPlaceDragRegion,
	kDragRegionLeft,
	kDragRegionRight,
	kDragRegionTop,
	kDragRegionBottom
};

class TDragRegion : public BControl {
public:
	TDragRegion(TBarView* barView, BView* replicantTray);

	virtual void AttachedToWindow();
	virtual void GetPreferredSize(float*, float*);
	virtual void Draw(BRect);
	virtual void DrawAfterChildren(BRect);
	virtual void MouseDown(BPoint);
	virtual void MouseUp(BPoint);
	virtual void MouseMoved(BPoint, uint32, const BMessage*);

	BRect DragRegion() const;

	bool SwitchModeForRegion(BPoint where, BRegion region,
		bool newVertical, bool newLeft, bool newTop, int32 newState);
	void CalculateRegions();

	int32 DragRegionLocation() const;
	void SetDragRegionLocation(int32);

	bool IsDragging() { return IsTracking(); };

private:
	void DrawDragger();

private:
	TBarView* fBarView;
	BView* fReplicantTray;
	BPoint fPreviousPosition;
	int32 fDragLocation;

	BRegion fTopLeftVertical;
	BRegion fTopRightVertical;
	BRegion fBottomLeftVertical;
	BRegion fBottomRightVertical;

	BRegion fTopLeftHorizontal;
	BRegion fTopRightHorizontal;
	BRegion fBottomLeftHorizontal;
	BRegion fBottomRightHorizontal;

	BRegion fMiddleLeft;
	BRegion fMiddleRight;
#ifdef FULL_MODE
	BRegion fLeftSide;
	BRegion fRightSide;
#endif
	BRegion fTopHalf;
	BRegion fBottomHalf;
};

class TResizeControl : public BControl {
public:
	TResizeControl(TBarView* barView);
	virtual	~TResizeControl();

	virtual void AttachedToWindow();
	virtual void Draw(BRect);
	virtual void MouseDown(BPoint);
	virtual void MouseUp(BPoint);
	virtual void MouseMoved(BPoint, uint32, const BMessage*);

	bool IsResizing() { return IsTracking(); };

private:
	TBarView* fBarView;
};


#endif	/* __STATUS_VIEW__ */
