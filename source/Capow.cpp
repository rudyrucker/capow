/************************************************************************
	FILE:				capow.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:	Borland C++ 4.0/MS Windows 3.1


	UPDATE LOG:

	4-18-94 (CBM)	changed about dlg procedure use in WinProc to allow
				a bitmap to appear on the dialog box.

	5-16-94 (RR) Let's start really using this log.  Today I built
	the alpha3 version.  This uses the large model instead of medium
	model so we can have 49 CAs.  This version seems unreliable when
	the NOUNLOCK flag is commented out.

	5-17-94 (RR) Elimated all the lock/unlock GlocablAlloc, etc.
	stuff entirely, went to new and delete alone.  Got rid of the
	FUNCTIONPOINTER switch, revised CAlist::Evolve. Fixed a bad
	bug (oldstates not initialized). Caption Alpha 3.

	5-17-94 (AB) Fixed Wave file loading. Put an exit button on the cycle
	box. Reversed the menu order of Controls. Change Settable to SetCA.

	5-19-94 (RR) Changed caption to Alhpa 4. Changed the alternate
	cursors to be more suggestive of what they do, also changed their
	labels.  Upped the max CA count to 64.

	5-19-94 (RR) Removed Timer and Lookup from Tools.  Tweaked the cursor
	menu names, tweaked the names of CAs, Breeding, World.  Moved
	Randomize() to WM_CREATE so that startups are always different.
	Alpha 5.  Ready to Print out source.  This version is the one we
	document and send EPRI in June?

	5-23-94 (RR) Put in the Reverse button on the World menu to
	reverse a reversible CA.  Fixed it so that when TYPE_CA_REVERSIBLE,
	the AvoidStripes will not call Seed at a stripe failure.
	Changed caption to Alpha 6. Made CAlist::Evolve simply reset scores
	and return if zoomflag is on.

	10-8-94 Add 'Experiment' menu item 'Controls' to display
	horizontal and vertical parameters and ranges.

	11-7-94 RR change the updatewave rule and the generator::step.  Beta4.

	2-26-95 RR put in diverse oscillator, fix avoidstripes to work
	with wave methods.  Beta 11.

	3-10-95 JL put in graph display, changed by RR.  RR changed water wave.
----------------------------------------------------------------------------------	
	  
	2/1/96  New semester.  Rucker put in Alt??? wave types.

	2/2/96  Rucker put in smoothsteps variable.  This is to smooth
	the curve out after resizing, seeding, or toggling wrap.  This
	is necessary for the AltWave type which reacts badly if the
	wave states have discontinuites.  Use the CA::Smooth function for
	smoothing.
	
	Rucker also split ca.cpp into ca.cpp (the core) camore.cpp (accessors,
	tweakers, mutators, load and save) and calist.cpp (the CAlist methods).
	This was done because ca.cpp was compiling to a code seg larger than
	64K, which is not allowed.
	
	2/5/96  Rucker fixed the Alt??? rules to work better.  Added a
	SMOOTH_VARIANCE switch to Set_variance, this smooths the individual
	ca tweak params so that the Alt Diverse Wave Oscillator works.  Note
	that the SMOOTH_VARIANCE does make the old Diverse Wave Oscillator
	less bumpy.  Added a _dx_lock variable with a Stable DX button on
	CA menu.  This toggles whether or not _dx = 2 * _dt.  By the way,
	the time_step, space_step, acceleration_multiplier names were changed
	to _dx and _dt names.  Added a SMOOTH button to the World menu. This
	calls Set_smoothsteps(SMOOTHSTEPS) to smooth a too bumpy Alt wave.
	
	2/14/96 Rucker changed ALT_CA_AUTO_ULAM to act like a cubic Ulam wave.
	Changed _wavespeed to 1.0.  Put in a multiplier step to the smooth
	process to try and avoid damping or amping amplitudes in the case
	where the average value isn't 0.
 
	2/18/96 Rucker got 2D Wave to working. Looks very good.
	
	10/28/96 Integrated the TweakParams.

	10/29/96 Got the DLL stuff working!!!
	
    1/31/97 Iota2 build.  Cleaned up ANALOG.CPP, ELECTRIC.CPP, CYCLE.CPP.  Removed
	the bad compress code from LOADSAVE.CPP.  Removed all the __FLAT__ switches to
	make this purely 32-bit code.

	2/3/97 Another Iota2 build.  Arranged it so that you can load four kinds of
	DLL user rules that take 1, 3, 5, or 9 cell indices.  This will be for
	network, 1D 2-neighbor, 1D 4-nabe or 2D 4-nabe, 2D 8-nabe.  Got rid of
	almost all the unsigned int types since now we do only 32 bit builds and an
	int is always big enough.  Got rid of all the far, FAR, huge, and HUGE pointer
	modifiers, as these don't mean anything in 32 bit.  

	2/6/97 Simplified the DLLMEM.CPP code, now we let Windows track the reference
	count on DLL.  Added a USERCATYPE and USERCASIZE functions to the DLL 
	interface; user will specify which regular type his/her type is most like and
	how many neighbors are used in his/her rule.  These set _usercatype and 
	_usercasize fields in the CA.  Tried to fix ODD and EVEN view.

	2/10/97 Put in _smoothflag.

IOTA group:
	
	4/2/97  preliminary merge for iota8 attempted by mike. Features flexible support for
	user defined DLL rules, and 3D views. BUG- the 3D view obscures the status bar.

	4/6/97  Added a view Menu containing menu items to turn off ( hide ) the status bar and
	toolbar.  A little line appears right above status bar when certain resizings are done.  
	A. Forster

	4/08/97 Replaced '+' '-' push buttons in Color dialog with a spin control
	A. Forster

	4/08/97 Replaced UP and DOWN Arrows Digital dialog with  spin controls
	A. Forster

	4/11/97  Removed the ToolBar Code from Capow.cpp and put it into a separate file
	toolbar.cpp.  This is so that future enhancements to the toolbar will be easier
	to implement and to unclutter the capow.cpp
	A. Forster

	4/11/97  Cleaned up and organized the code  in capow.cpp.  Updated labels for sections
	of code and added several new comments.  Also rearrranged code so it is consistent throughout
	the file.  Rewrote the code to handle opening and saving so that must of the "ofn" variables for 
	comman dialogs are not in 5 different places.  Made sure all code is properly formated.
	A. Forster

	4/13/97  Rewrote the WM_COMMAND processing of Load and save to reduced duplicate code.
	A. Forster

	4/14/97   Reworked the layout of the View Dialog.  Separted view styles into Normal and 3D 
	categories.  Changed #of CAs from UP and DOWN control to Radio Control
	A. Forster

	4/14/97   Reworked the layout of the View Dialog.  Separted view styles into Normal and 3D 
	categories.  Changed #of CAs from UP and DOWN control to Radio Control
	A. Forster

	4/14/97   Replaced UP AND DOWN Arrows in Analog, Electric, and the '+' '-' controls in Fourier
	with spin controls.  

	4/14/97  Reworked the Breeding Dialog and removed all '+' '-' and added spin controls.
	A. Forster

	4/14/97  Removed all references to the bitmaps for UP AND DOWN Arrows and removed the bitmaps
	from the resource file
	A. Forster

	4/15/97  Rewrote the Load and Save code for loading and saving CA's and Experiments.
	Loc Ho

	4/21/97 Enhanced status bar to display the name of the current focused CA style or the file name of
	a userrule loaded.  Status bar also indicates the generation count of the focused CA.
	A. Forster

	4/21/97 Updated the string table to include descriptions of the new menu items added recently.  Also
	found out how to display messages for popup menus, which was implemented.
	A. Forster

	4/21/97  Program pause now correctly implemented.  Before WM_SIZE would cause the program
	to become unpaused, whenever a such a message was sent.  Since WM_SIZE was a popular message
	most everything done to the capow program when it was paused caused a call to WM_SIZE and 
	a resulting unpausing of the program.  This bug is fixed. 
	A. Forster

	4/22/97 Iota10 created.  Graph3D tossed out and replaced with CapowGL, which uses the 
	OpenGL API.  

	4/27/97 merged Andrew's code (with a status bar, frame counter).
	Bugs:  the OpenGL view doesn't show upon choosing a focus (resize gets past this).
	Also, counter resets when the window is resized.  3D view works poorly in 8bit color.

	4/29/97 fixed the minor problem which caused the gap between the status bar 
	and the lower CA's.  The cause was in the Locate function, which is now replaced
	by a new version. The old function tried to make all the CA's the same height, which
	would mean that the total height would have to be multiple of 3, plus the border
	heights. The new function uses floats, then casts to int, to fit the CA's better.

	4/25/97  Reworked the Fourier Dialog box.  Rearrange most controls.  Replaced the 
	four Arrow buttons controling the moving of the Test Point wiht a slider bar that can
	control either all the point views or simple the focus.  When the focus is changed the slider
	is updated to a position matching the position of the Test Point.  When the focus is NOT on
	a point view CA the controls in the Fourier Dialog are disabled.  Added a new button to the 
	dialog.  Previously there was just an apply button that did both the fourier analysis and the 
	the reseting of the point.  Now there are two... When nothing has been analyized the APPLY button
	is disable and than enabled after the Anazlye button is pressed.   Because the slider bar
	can only be drawn so much to either side, there exists a problem when multiple point views exist
	and their test points are not in equal position and the user selects to update all test points.
	The problem is that if the user has the focus set on a CA whose test point is not aligned with the 
	others the slider will only slide and subsequently move the other test points as far as the focus'
	test point can be moved.  To get around this the user should select the CA with the longest slider
	range avaiable.  Slider controls are as follows... The user can drag the slider button and this will
	update the postion by one... The user can click focus on the slider and use the left and right arrow
	keys and this moves it by one.  The user can click in the slider channel on either side of the button
	and this will update the test point by 5 in the appropriate direction.  And the user can use the Home
	and End keys to move the slider to the appropriate extreme.	   Page up moves the slider 5 to the left
	and page down 5 to the right.
	A. Forster

	5-5-97 The "slam" button on teh right of the toolbar should close the 3D view
	dialog.  ( CLOSED )  A.F.

	5-5-97 When I load a DLL the status bar doesn't show the name of the custom rule
	until I click on a CA.  ( CLOSED ) A.F.

	5-5-97  Fixed the pause button in the OpenGL dialog to pause the program properly.
	The Status bar message "Pause" now appears when the button in the dialog is pressed.
	Replaced Mike's pause code in the dialog WM_COMMAND with a SendMessage to IDM_PAUSE.
	A.F.

	5-5-97 The nonlinearity control is gone from Analog dialog.  I 
	think this happened because I temporarily put a 3D Height control over it.
	I didn't mean that the nonlinearity should go away, I menat that the dialog
	should be made bigger to hold both buttons.  ( OPEN )  Do Not understand!!


	5-5-97  Added Keyboard Accelerator Support.

	Ctrl + A,           Opens Analog Dialog.
    Ctrl + C            Captures the client window.
    Ctrl + G            Opens OpenGL Dialog.
    Ctrl + L            Clears the client window.
    Ctrl + O            Opens Comman File Dialog for opening an Individual CA.
    Ctrl + P            Pauses Program.
    Ctrl + R            Randomize CAs.
    Ctrl + S            Opens Comman File Dialog for Saving an Individual CA.
    Ctrl + U            Opens the UserDialog
    Ctrl + V            Opens the View Dialog 
    Ctrl + W			Opens the World Dialog
	F1					Invokes Help
    F2,					Opens Dialog to load a userrule to the focus only.
    Alt + F4			Quits the Program.
	Shift + F1			Change Cursor to Pick and Zoom
	Shift + F2			Change Cursor to Copy Mutate
	Shift + F3			Change Cursor to Touch Mode
	Shift + F4			Change Cursor to Place Generator.

	NOTE: Unresolved bug... Sometimes the cursor doesn't change back to
	the appropriate image when using the Shift accelators. 
	
	A. Forster
  6/97 Cleaned up code, got rid of some bugs, built Version 5.0, posted
this on the capow web page.  R.R.
9/97 New team.  Begin Zeta series of builds. R.R.
9/21/97.  Switched to using a thread to keep the program running all the time.
Found the example code in Petzold Windows 95, p. 744.  Put in ScreenSaverProc. 
This meant I had to change the AppName variable in all the dialog *.CPP files
to MyAppName. Put in a switch for SCREENSAVER.  Moved Andrew's command line
code to a separater file Cmd_line.cpp.  R.R.     
9/24/97  Put in compiler switch to turn THREAD on and off.   
Put the various *.LIB into the Link sheet of Project Settings.  PUt in 
SAVE_QUERY switch. Put in CRITICAL_SECTION code to prevent thread conflicts, see
comments in Cellthread.  R.R.
9/25/97  Couldn't make thread stuff work, gave up, saved thread using file as
Capow Thread.cpp.  Put in WM_TIMER code R.R.
9/29/97  Put the timer-using code off in CAScreen.cpp.  Put the screensaver
code in there.  Added a divider_width variable.  Cleaned up the project files.

10-22-97
	-Moved Info Grid to Controls;  Made Cursor Submenu a main menu item;
	-Moved Change All/ Change Focus menu items to Controls;
	-Repaired broken Status Bar menu messages and added new ones for new controls.
	-Put Change All/ Change Focus Radio Buttons back into appropriate dialogs.
	-Move % to Mutate to the CELL ( CLONE ) dialog. 
	-Added Slam Button to Action Toolbar
	-Cleaned up some duplicated code.
	-Added a swap button to each toolbar to toggle between the two.
	-Moved Clear menu item from File to Edit... Seemed more appropriate
	-Changed Windows Intial size from 380 X 400 to 460 X 480 so all of 
	 Dialog toolbar will appear.


	Bug:  World Dialog doesn't seem to update its Change all/Change Focus
	      Radio control, when it is changed outside of the dialog.
		  All other dialogs will change this radio button when some
		  action in the dialog occurs ( I don't like this; should be automatic )
		  
10-27-97
	Added SaveFile.cpp for a save dialog.  Made necessary changes to resource.h.

11-10-97 Took Michael Ling's Zeta 7 build, which has some new 3D features, and made it into
the Zeta 8 build.  Cleaned up behavior of the focus/all radio buttons in dialogs.  Added
an autorandomize feature which emulates the behavoir of the screen saver.  Moved the
necessary timer code into Autorand.cpp so CAPOW.CPP and CASCREEN.CPP can share the code
(can't put both into the same project).  Moved the Cascreen.dsp project up into the 
same workspace directory as Capow.dsp.  

11-17-97  Zeta 9.  Added Generator dialog code (gendlg.cpp).  Added some code to the generator
and generatorlist classes (camore.cpp, ca.hpp, ca.cpp).  Slight changes made to capow.cpp 
LbuttonDown and RbuttonDown switch items.  There still seems to be a bug with the cursor 
not always wanting to change back and forth between the generator and pick cursor styles.

11-18-97 Mike : 
	-Added the capow icon to the screensaver's resources.
	-Optimized the VRML capture files; the captured file size is smaller
	-Added an absorbing edge option to the available boundary conditions

	Fixed bugs:
	- fixed the 3D view dialog logic for the 3D glasses mode
	- a bug which incorrectly updated the upper left corner of a 2D CA
		in Free edge mode.
	- a bug which causes the 3D view to freeze when attempting to select
		a 1-D view mode, for example Split view.  Warning- don't call
		SendMessage (WM_SIZE,...) with 0 as the size parameter, because it will
		fool the 3D view into thinking the size of the window is zero.
		That was the cause the opengl related bug.


12/2/97  Rudy.  I fixed up Rong Liu's new configure.cpp dialog.  I put in a timer so 
user can control the speed; we don't need to use PeekMessage anymore, though I
left the code structured that way instead of putting GetMessage back.  We have
a MASTERTIMER switch to choose between the "WM_TIMER" or "PeekMessage else" way.
12//3/97 Rudy.  * I made the OpenGL pause when the CA update is paused.  Added
a gl_sleep variable so you CAN unpause rendering when CA Update is paused, control
on opengldlg.  * Got rid of the obsolete SCREENSAVER switch in here.  *Fixed  it
so the CA doesn't update if the foreground window isn't the mainwnd or one of the hDlg.
* Rewrote CONFIGURE.CPP.
12/5/97  Rudy.  Added a Halfmax seeding.  Fixed a bug in seeding, velocities weren't
being set to 0 for 1D CAs, now oscillators work better.  Cleaned up the screensaver
code so it's reusable as a model of what to do.  Pause program if you are in a load
or save.  Set cursor to hourglass while loading or saving.  Added a _blt_line.
At fastest speed we use _blt_line 3, otherwise we use 1, except in screensaver
we compromise and use 2.  Fixed a bug with count change.  Made old toolbar the default.
Updated the about box.

3/2/98 Mike - now allows user to add generators in unzoomed view.  Fixed bugs concerning
pausing (sometimes a paused ca would unpause).

3/4/98 Mike - fixed bug concerning update of 3D dialog box. Ordinarily it is grayed
out when 1-D CAs are viewed, and it becomes enabled when 2D is loaded and zoomed, but
this wasn't happening.

3/5/98 Mike- fixed bug: The alignment of the pop-up menus for the Change CA Type 
and Change View toolbar buttons in the Dialog Toolbar are reversed 
FIXED:  In capow.cpp, switched the #define values for VIEWMENU_BUTTON and CATYPEMENU_BUTTON

3/7/98 Mike-
Fixed another bug: When changing the frequency of a generator, it jerks in its displacement; 
it should rather smoothly speed up or slow down according to frequency changes.
FIXED:  Added SmoothOmega() to the Generator class; unfortunately this changes ca.hpp, which
could cause user rules dll's to become nonfunctional.

3/10/98 Mike- modified help files to include new dialogs- the generator dialog and
randomize dialog.

2/16/99 Rudy.  There is a problem that CAPOW runs very slow in my version
of Windows98.  That is, it's slow if MASTERTIMER is on.  It's fast if I comment
out MASTERTIMER and go to the PeekMessage approach.  But then I lose the ability
to use the File|Speed control.  Evidently there are too many background messages
being processed in Windows 98, and since WM_TIMER has lowest priority, it's
languishing at the back of the Queue.  If I move the mouse around rapidly the
program runs faster for a moment.  Checking a good "Moving your Game to Windows"
article in the MSDN knowledge base, I learn that when you move the mouse the
priority of your process and/or thread briefly goes up which is why your timer
messages would be noticed more.  I tried putting  lines like these into  
my MyWnd_CREATE to speed up the program, but even these extreme settings didn't
help.  Nor if I put this in MyWnd_TIMER, nor if I used a HANDLE hMainThread that
I set with GetCurrentThread in WinMain.
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
The same article suggests an alterante approach, to use a PeekMessage technique but
to keep track of the time in there.  You can't use timeGetTime because that
doesn't actually work.  Instead you have to use the QueryPerformanceCounter function
and use this as a timing dealy-bob inside the PeekMessage.  Most of the code is
in the AUTORAND.CPP. So now I don't use the Windows Timer at all, though I
could put in a switch to use it if QueryPerformanceCounter happens not to be
supported. 

April 20, 1999.  Rudy
I noticed that I need to separate out the two functions
extern void setTimerCycle(HWND hwnd, int &timer_handle, int timer_ID, int millisecs);
extern void setPerformanceTimerCycle(int millisecs);
The former uses the old Windows Timer, the Autorandomize uses this, while the latter
uses the performance counter, the message loop uses this.  Really I should encapsulate
the perfomrance coutner timer into a class, as it now sloppily has a bunch of global
variables.
I fixed a bug that the Analog box wasn't initializing when you opened it a second
time because edit_id wasn't being reset to 0 at each start of this dialog box.
I made some new DLL rules. Improved the seeding functions to seed the second
plane of variables in a reasonble way for the activator-inhibitor rules.  
Changed the View Which dialog selection so we have the choice of viewing the
activator or the inhibitor quantity.  
Zipped this up and posted it as Source Ver 6.3 on the CAPOW website.

December 31, 2007  Rudy
Windows Vista has made HLP files obsolete, so I'm switching to CHM, or HTMLHelp. 
For the conversion, using HTMLHelp with Visual Studio 6.0, see Rudy Rucker, 
SOFTWARE ENGINEERING AND COMPUTER GAMES,(Addison Wesley 2003) Chap 21. 

February 28, 2017 Rudy
I upgraded my build IDE to VCC ver 15, which was free online.
I flagged all my new code changes with a comment // 2017
I made the CX_2D and CY_2D be 500 and 250, so get an aspect like the modern 2 x 1 screens. 
I got rid of the "Action" toolbar, and I removed the CURSOR dialog options for Copy and Generator,
although you can still do these via the dialogs.
The new compiler showed an error in the post reading do loop in Loadsave.cpp and Loadsave7.cpp, and I think
I fixed that.
Fixed some include problems with some header files that have been renamed, like from <stdio.h> to <stdio>.
Rewrote the About dialog.
Updated the capowhelplong file a little.  Updated the capow website. Made some new cA and cAs files to load.
Added INITIAL_XSIZE and INITIAL_YSIZE defines in ca.hpp to specify the startup size of the window.
Removed "SCREENSSAVER.CA" from configure.cpp and CONFIGURE dialog---because it doesn't work now and I 
don't remember what it was supposed to do.
Weeded out the old .CA and .CAS files that don't load properly.
*/

//***********************************************************************/
//====================INCLUDES===============

// These first two headers are needed for Randomize()
#include "ca.hpp"
#include "resource.h"
#include "bitmap.hpp"
#include "random.h"
#include <commdlg.h>
#include <string.h>
#include <commctrl.h>// For the 32 bit Toolbar control
#include "userpara.hpp"
#include <stdio.h>
#include "status.hpp"
#include "GUI.hpp"
#include "comcthlp.h"
#include "capowgl.hpp"
//Note that you need COMCTL32.LIB in link library list for this.
//Need this header for HTMLHelp
#include "htmlhelp.h"

//==================== FLAGS ===============

//#define TOOL_IN_CAPOW
//#define LOAD_ACTIVE_CAS
//#define FIXED_640_480
//#define MASTERTIMER 
/* In Windows95 and WindowsNT, MASTERTIMER works fine.  But in Win98, using it
slows my performance waaaay down! The catch is that if I don't use this, 
I lose my speed control. So what I did was to add a handmade timer element
to the PeekMessage loop, and I don't use MASTERTIMER.*/
//====================DEFINE CONSTANTS ===============

#define MAXFILENAME 256  // maximum length of file with pathname
#define PUT_TO_SLEEP 1 // Put CAs to sleep
#define WAKE_UP      0 // Wake CAs up
#define ALL          0
#define FOCUS        1

//Bugfix- the following two defines were swapped in their values, to
//fix the location of the popup menus on the action toolbar.  mike 1/98
#define VIEWMENU_BUTTON   4  // View menu is the thrid button on the toolbar
#define CATYPEMENU_BUTTON 3  // fourth button

#define SEEDMENU_BUTTON 5  // fourth button


//====================CAPTION===============

LPSTR versioncaption = "(";
LPSTR datecaption = "3/8/17) ";

char caption[256] = "";
#ifdef FORCENARROW
LPSTR typecaption = "BorderMaker CAPOW 2017 ";
#else //not FORCENARROW
	#ifdef LITE
	LPSTR typecaption = "CAPOW 2017 LITE! ";
	#else //not LITE
		#ifdef BIG2D
		LPSTR typecaption = "CAPOW 2017 ";
		#else //not BIG2D
			#ifdef BIGGER2D
			LPSTR typecaption = "Capow 2017 ";
			#else //not BIG2D
				#ifdef BIGGEST2D
				LPSTR typecaption = "CAPOW 2017, 600 by 300 ";
				#else //not BIG2D
					LPSTR typecaption = "CAPOW 2017, Small 2D";
				#endif //BIG2D
			#endif //BIGGER2D
		#endif //BIGEST2D
	#endif //LITE
#endif //FORCENARROW
LPSTR standardcaption = "Continuous-Valued Cellular Automata.";

//====================GLOBAL VARIABLES===============

HINSTANCE hInst;			// Our Executable Instance

HWND masterhwnd    = NULL;  // Handle to Master Window
HWND hwndActionToolbar   = NULL;  // Handle to ToolBar
HWND hwndDialogToolbar   = NULL;  // Handle to ToolBar
HWND hwndStatusBar = NULL;  // Handle to StatusBar
HWND hDlgCycle = 0, hDlgExp = 0, hDlgColor = 0, //Handles to  dialog windows
	hDlgFourier = 0, hDlgAnalog = 0, hDlgCell = 0, hDlgElectric = 0,
	hDlgDigital = 0, hDlgView = 0, hDlgWorld = 0,
	hUserDialog = 0, hDlgGenerators = 0, hDlgOpenGL =0, hDlgConfigure = 0;
											   
HMENU   hMainMenu;	// Handle to our Menu
HMENU   hViewMenu;      // Handle to view sub menu
HMENU   hCATypeMenu;      // Handle to view sub menu
HMENU   hSeedMenu;      // Handle to view sub menu
HBITMAP hBitmap;    // Handle to a bitmap object

BOOL  zoomviewflag		   =	FALSE;
BOOL  first_time_flag      = TRUE;
BOOL  not_seeded_yet_flag  = TRUE;
BOOL  update_flag		   = FALSE; // Updates params, cycle, lookup dialog boxes
BOOL  load_save_cells_flag = FALSE;
BOOL  statusON             = TRUE;	// status bar is on
BOOL  toolbarON            = TRUE;	// toolbar is on  Now this is used to hold
	//ActionToolbar or DialogToolbar, I think, rudy 12/6/97.
//BOOL  pauseflag            = FALSE;
BOOL  windowIsMinimized		= FALSE;
BOOL  inloadsave = FALSE; 
BOOL randomizenow = FALSE;
int divider_width = 1; //Defined in CAPOW.CPP and in CASCREEN.CPP
	//Width of the gray line dividers, used in Bitmap.cpp
short focusflag            = START_FOCUSFLAG;  //Set in ca.hpp to ALL=0, or FOCUS=1
short WhichToolBar         = 0;    // 1 = NEW  0 = OLD
BOOL ActionToolbar		   = 0;   // 0 means off   1 means on
BOOL DialogToolbar        = 1;   // o means off 1 means on
	char CA_STYLE_NAME[256]; //Used in several places to get the current rule name. 
int filterflag = 1; //1 means start with .ca in the open file dialog box.
//BOOL screensaver = FALSE;
 /* We use this because we are sharing a dialog with the
screensaver version and we use screensaver as a switch which we define as TRUE
in CAScreen.cpp */

int update_timer_handle = 0; //This will actually be equal to UPDATE_TIMER_ID.
#define UPDATE_TIMER_OPTIONS_COUNT 5 //These are veryslow, slow, medium, fast, fastest
int update_timer_options[UPDATE_TIMER_OPTIONS_COUNT] = {300, 200, 100, 10, 1};
	// These are the times to wait between updates, in milliseconds
	//The difference between FAST and FASTEST is mainly the _blt_line setting,
	//but I pretend that 100 updates vs 1000 updates is a possibility.
int update_millisecs_per_cycle = 10; //msec, actually 50 is about as fast as Windows95 can do it,
	//But maybe Win98 is faster.   RR 2/26/99
int update_timer_speed_index = ID_FAST-ID_VERYSLOW;
	//Start with FAst, not with FAstest
/* I also need another timer for my autorandomizing (and I'll put the same variable
similar thing in CASCREEN.CPP for screen saver).  The randomize_timer_cycle is
a variable that I will safe and load as a profile string, I'll keep it in
AUTORAND.CPP so that the *.SCR and *.EXE can share it. */
int randomize_timer_handle = 0;
//2/16/99 Rudy added this stuff to use with QueryPerformanceCounter.
extern _int64 _start, _end, _freq, _update_ticks_per_cycle;
extern BOOL _performance_counter_present;

//====================GLOBAL DATA===============

CAlist *calife_list = NULL;

char commandline[1024] = { '\0' }; // Stores Commandline passed program
/* Usually you would call this variable szAppName, but we are planning to
make a screen saver version of this program, usign a lot of the same code
modules (though the main will be CASCREEN.CPP instead of CAPOW.CPP), and 
the screensaver SCRNSAVE.LIB has a static TCHAR szAppName[40]; whose definition
would conflict with my using szAppName in my modules.  So instead I use a
different name */
char *szMyAppName = "CAPOW"; //Don't call szAppName so don't conflict with scrnsave.h
char capowDirectory[256];
extern char userDialogName[];
extern char szScreenSaverFileName[];
extern char szScreenSaverFileShortName[];

LPSTR WinArgv[9]= { NULL };  // Holds all commands from the command line

int  cursormode      = CUR_PICK;		// Current cursor mode
int  oldcursormode   = CUR_PICK;
int  toolBarHeight   = TOOLBARHEIGHT ;  // Variable holding toolbar height
int  statusBarHeight = STATUSBARHEIGHT;  //Holds status bar height
int  nDrawMode	     = R2_COPYPEN;
int  cxParent, cyParent;

CapowGL *capowgl;
WindowBitmap *WBM;  // our memory bitmap


/* Here is a flag I use in CONFIGURE.CPP to decide whether that dialog's code
is for the dialog of the *.EXE or for the initializer of the *.SCR */
int buildtype = BUILD_EXE;

//====================LOCAL FUNCTIONS ===============
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void Cellmain(HWND); //This is the continually running thing.

void ParseCommandLine ( char commandline[], char* WinArgv[] );
void GrabExtension ( LPSTR lpszCmdParam, char Extension[] );
BOOL CheckExtension ( char Extension[], char DesiredExtension[] );


//====================EXTERNAL DATA===============

extern DWORD dwStatusBarStyles;
extern BOOL compressFile;
//The following are from Autorand.cpp, they are shared with this project and with the
//Cascreen project. 
extern UINT fRandFlags; 
extern int randomize_timer_cycle; // defualt is 120000 for 2 minutes.

//====================EXTERNAL FUNCTIONS===============

extern BOOL CALLBACK CycleProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CALLBACK ExpProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CALLBACK ColorProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CALLBACK FourierProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CALLBACK AnalogProc( HWND, UINT, WPARAM, LPARAM );
extern BOOL CALLBACK ElectricProc( HWND, UINT, WPARAM, LPARAM );
extern BOOL CALLBACK CellProc( HWND, UINT, WPARAM, LPARAM );
extern BOOL CALLBACK DigitalProc( HWND, UINT, WPARAM, LPARAM );
extern BOOL CALLBACK WorldProc( HWND, UINT, WPARAM, LPARAM );
extern BOOL CALLBACK ViewProc( HWND, UINT, WPARAM, LPARAM );

extern BOOL CALLBACK GeneratorsProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CALLBACK OpenGLProc(HWND, UINT, WPARAM, LPARAM);   //mike
extern BOOL CALLBACK SaveFileProc(HWND, UINT, WPARAM, LPARAM);  
extern BOOL CALLBACK ConfigureProc(HWND , UINT , WPARAM,  LPARAM );

extern LRESULT CALLBACK userDialogProc( HWND, UINT, WPARAM, LPARAM );

extern LRESULT ToolBarNotify(HWND hwnd, int idForm, NMHDR  * pnmhdr);

extern void createUserDialog();
extern HWND RebuildToolBar (HWND hwndParent, WORD wFlag);
//Keep this in Autorand.cpp so that the capow.scr project can use it too.
extern void setTimerCycle(HWND hwnd, int &timer_handle, int timer_ID, int millisecs);
extern void setPerformanceTimerCycle(int millisecs);
//These are in Configure.cpp
/*extern void GetIniEntries(void);
extern void GetIniSettings(void);
extern void WriteProfileInt(LPSTR key, LPSTR tag, int i);
extern void ConfigInitProc(HWND);
extern void ConfigOKProc(HWND);
extern void LocalHelpProc(HWND, WPARAM);
*/



//====================WIN MAIN===============
// Windows Overhead


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow) //2017 Changed PSTR to LPSTR
{
	HACCEL hAccel;
	MSG msg;
	WNDCLASS wndclass;
	strcpy ( commandline, lpszCmdParam );
	ParseCommandLine ( commandline, WinArgv ); // ~ Copies Command line to a global char array
	if (!hPrevInstance)
	{
		wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = LoadIcon( hInstance, szMyAppName );
		wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
		wndclass.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
		wndclass.lpszMenuName = szMyAppName;
		wndclass.lpszClassName = szMyAppName;

		RegisterClass (&wndclass);

		wndclass.style = CS_HREDRAW | CS_VREDRAW  | CS_DBLCLKS;
		wndclass.lpfnWndProc = userDialogProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = LoadIcon( hInstance, szMyAppName );
		wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
		wndclass.hbrBackground = (HBRUSH)GetStockObject( LTGRAY_BRUSH );
		wndclass.lpszMenuName = userDialogName;
		wndclass.lpszClassName = userDialogName;

		RegisterClass (&wndclass);

	}

	hInst = hInstance;	// Make Copy of program instance

	lstrcat(caption, typecaption);
	lstrcat(caption, versioncaption);
	lstrcat(caption, datecaption);
	lstrcat(caption, standardcaption);
	GetCurrentDirectory(256, capowDirectory);
	WriteProfileString(szMyAppName, "Directory", capowDirectory);
	strcpy(szScreenSaverFileName, capowDirectory);
	strcat(szScreenSaverFileName, "\\Files To Open\\");
	strcat(szScreenSaverFileName, szScreenSaverFileShortName);

	masterhwnd = CreateWindow(szMyAppName,		// window class name
         caption,
			WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,		// window style
									//CLIPCHILDREN and CLIPSIBLINGS are for the sake of opengl
			CW_USEDEFAULT,				// initial x position
			CW_USEDEFAULT,				// initial y position
		INITIAL_XSIZE,				// initial x size
		INITIAL_YSIZE,				// initial y size
			NULL,					// parent window handle
			NULL,					// window menu handle
			hInstance,				// program instance handle
			NULL);					// creation parameters
         //If you get an error message here, it is becasue you are
         //doing a 32 bit compile and you need to comment out
         // the caption swith stuff just above.
	
	ShowWindow(masterhwnd, nCmdShow);
	UpdateWindow(masterhwnd);
	  
	hAccel = LoadAccelerators ( hInstance, "Capow_Accelerators" );


	while (TRUE) //Keep it growing.  See Petzold's RANDRECT example.
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if (!(
					(hDlgColor && IsDialogMessage(hDlgColor, &msg)) ||
					(hDlgCycle && IsDialogMessage(hDlgCycle, &msg)) ||
					(hDlgExp && IsDialogMessage(hDlgExp, &msg)) ||
         			(hDlgAnalog && IsDialogMessage( hDlgAnalog, &msg)) ||
					(hDlgElectric && IsDialogMessage( hDlgElectric, &msg)) ||
					(hDlgDigital && IsDialogMessage( hDlgDigital, &msg)) ||
					(hDlgView && IsDialogMessage( hDlgView, &msg)) ||
					(hDlgWorld && IsDialogMessage( hDlgWorld, &msg)) ||
					(hDlgConfigure && IsDialogMessage( hDlgConfigure, &msg)) ||
					(hDlgCell && IsDialogMessage( hDlgCell, &msg )) ||
					(hDlgFourier && IsDialogMessage(hDlgFourier, &msg)) ||
					(hDlgGenerators && IsDialogMessage(hDlgGenerators, &msg)) ||
					(hDlgOpenGL && IsDialogMessage(hDlgOpenGL, &msg))
				) )
				{
					if (msg.message == WM_QUIT)
						break;

					if ( !TranslateAccelerator ( masterhwnd, hAccel, &msg ) )
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
		}
#ifndef MASTERTIMER
		else 
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&_end);
			if ((_end - _start) >= _update_ticks_per_cycle)
			/* If you set update_ticks_per_cycle unrealistically low, then you are going
			to spend so much time in here that your program will be unresponsive.
			And don't be greedy and try and work a "while" instead of an "if" to
			do multiple updates here. Typical values of start and end are in the
			trillions, or higher; it's counting the total machine cycles
			during the program run. A typical value for end-start running on a
			400 MHz machine is end-start = 31,366,904 ticks.  Note that on this
			machine, we can use QueryPerformanceFrequency to find that the machine
			is running at 400,090,000 ticks per second.  In our setTimerCycle
			function in AUTORAND.CPP we use  QueryPerformanceCounter to set the
			update_ticks_per_cycle on the basis of the update_millisecs_per_cycle,
			 on our 400 MHz test machine, we get values like this:
			update_millisecs_per_cycle	update_ticks_per_cycle
			10 msec						4,009,000 ticks
			100 msec					40,090,000 ticks
			 */
		    {
				Cellmain(masterhwnd);
				_start = _end;
			}
		}
#endif //MASTERTIMER
	}
	return msg.wParam;

}
					
//====================MESSAGE CRACKERS ===============
/* As a left-over of the port from 16 bit to 32 bit (and as prepartion for the port
to MFC!) we process our messages with message handling functions (formerly called
message-cracker functions).  The message handlers we use are, in this order:
MyWnd_CREATE
MyWnd_PAINT
MyWnd_SIZE
MyWnd_MOVE
MyWnd_COMMAND
MyWnd_MENUSELECT
MyWnd_LBUTTONDOWN
MyWnd_RBUTTONDOWN
MyWnd_CLOSE
MyWnd_DESTROY
MyWnd_INITDIALOG
MyWnd_MOUSEMOVE
MyWnd_LBUTTONUP
MyWnd_INITMENUPOPUP
MyWnd_TIMER
/*********************************************************/

BOOL MyWnd_CREATE(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	Randomize();  // Seed the randomizer
	//When debugging, comment this line out so that each run is the same,
	//For release comment it in so that runs are pleasingly surprising.
	
	char Extension[5] = { '\0' };	
	
	WBM         = new WindowBitmap(hwnd);

	capowgl = new CapowGL(hwnd);
	capowgl->Size(hwnd);

	calife_list = new CAlist(hwnd, MAX_CAS); //Calls CA:Allocate for members
	calife_list->SetWindowBitmap(WBM); //Registers			

	hViewMenu   = LoadMenu ( hInst, "ViewPopMenu" );
	hCATypeMenu = LoadMenu ( hInst, "CATYPEPOPMENU" );
	hSeedMenu   = LoadMenu ( hInst, "SEEDPOPMENU" );
	hViewMenu   = GetSubMenu ( hViewMenu, 0 );
	hCATypeMenu = GetSubMenu ( hCATypeMenu, 0 );
	hSeedMenu   = GetSubMenu ( hSeedMenu, 0 );


//=============== Process Command Line =====================
	
	// If Command Line Contains a *.CA  File Load_Individual()
	// If Command Line Contains a *.CAS File Loadall() 
	// If Command Line Contains Nothing Randomize 
#ifndef VCC6
/*When you build the DEBUG version with Visual C++ Version 6,
it crashes in CheckExtension.  So I #defined VCC6 in CA.HPP and am using it
here to #ifdef out the code VCC 6 doesn't like.  RR 2/17/99.*/
	GrabExtension ( WinArgv[0], Extension );
	if ( CheckExtension ( Extension, "CA" ) )
	{
		calife_list->Load_Individual(WinArgv[0], calife_list->FocusCA());
		update_flag = TRUE;
	//mike 11-1-97: commented out because WM_SIZE messages should
	//only be sent when the window is resized.
	//		SendMessage(hwnd, WM_SIZE, 0, 0L);
		calife_list->Locate();
		SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);

	}
	else
		if ( CheckExtension ( Extension, "CAS" ) ) 
			if (calife_list->Loadall(WinArgv[0],TRUE) )
 			{
				if (calife_list->Get_justloadedcells())
					not_seeded_yet_flag = 0; //Don't Seed it in WM_SIZE
			}
#endif //VCC6.  End of the #ifdef-ed out code.
//=============== Loading Previously Saved Experiment =====================	

#ifdef LOAD_ACTIVE_CAS         

	if (calife_list->Loadall("ACTIVE.CAS", TRUE))
	//TRUE means startup, menas don't send a WM_SIZE
	{
		if (calife_list->Get_justloadedcells())
			not_seeded_yet_flag = 0; //Don't Seed it in WM_SIZE
		else
			not_seeded_yet_flag = 1; // seed in   WM_SIZE
		
		MessageBox( hwnd,
					(LPSTR)"(If you ever crash, delete ACTIVE.CAS.)",
					(LPSTR)"Good! ACTIVE.CAS Has Loaded Successfully.",
					MB_OK | MB_ICONEXCLAMATION );
	}
#endif //LOAD_ACTIVE_CAS

//=============== Intialize CommonControls =====================	

	hwndStatusBar = InitStatusBar ( hwnd );  // Loads Status Bar
	hwndActionToolbar   = InitActionToolBar   ( hwnd );	 // Loads Tool Bar
	hwndDialogToolbar   = InitDialogToolBar   ( hwnd );	 // Loads Tool Bar
	if (toolbarON)
		ShowWindow (hwndDialogToolbar, SW_SHOW); 
			//ShowWindow (hwndActionToolbar, SW_SHOW); 

	setPerformanceTimerCycle(update_millisecs_per_cycle);
//	focusflag = ALL;
//	SendMessage(hDlgOpenGL, WM_PAINT, 0,0);
	calife_list->FocusCA()->GetCAStyleName ( CA_STYLE_NAME ); //Andrew
	Status_SetText(hwndStatusBar, 1, 0, CA_STYLE_NAME ); //Andrew
//put hwndActionToolbar here if you'd rather start with that, Rudy 12/6/97
	if ( !hwndStatusBar | !hwndActionToolbar | !hwndDialogToolbar )
		return FALSE;
	return TRUE;
}

/*********************************************************/

static void MyWnd_PAINT(HWND hwnd)
{	//((fn)(hwnd), 0L)

	PAINTSTRUCT ps;
	HPALETTE    old_hpal;
	HDC	        hdc = BeginPaint (hwnd, &ps) ;

	if (calife_list->numcolor() == 256)
	{
		old_hpal = WBM->WBMSelectPalette(hdc, calife_list->hpal());
		WBM->WBMRealizePalette(hdc);
	}
	calife_list->Show(hdc, ps.rcPaint);
	calife_list->DrawDivider(hdc);
	calife_list->Boxfocus(hdc,RGB(255,255,255));
	if (calife_list->numcolor() == 256)
		WBM->WBMSelectPalette(hdc, old_hpal);
	
	EndPaint (hwnd, &ps) ;
}

/*********************************************************/


static void MyWnd_SIZE(HWND hwnd, UINT state, int cx, int cy)
{
	//((fn)((hwnd), (UINT)(wParam), (int)LOWORD(lParam), (int)HIWORD(lParam)), 0L)
	RECT rect;
	RECT rWindow;
	
	windowIsMinimized = (state==SIZE_MINIMIZED);
	if (windowIsMinimized)
		return;

/* mike 11-1-97: commented this code out, and used windowIsMinimized
to fix some of the pausing functionality and window minimization
handling.  Windows sends a WM_SIZE message AFTER a window has been
resized, minimized, maximized or restored.  Practically speaking,
resizing a window shouldn't cause a paused CA to unpause.  However,
this was happening, and it was evident for 2-D CAs, 
whose cell dimensions are independent of the size of the window.  
For 1-D CAs, resizing the window will change their cell dimensions, 
and thus make invalid the WBM image, but still, it doesn't relate 
at all to pausing.  The flag windowIsMinimized is obvious in its
purpose, but Cellmain uses it to determine whether to process or not.
Also, even though a CA is paused, Capow can still be processing, 
for example when manipulating the 3D view of a paused 2D CA.  But
when windowIsMinimized is TRUE, no processing occurs.


// Put CA's to Sleep if Window is Minimized
// Wake Up CA's if Window is Restored 

	if (!pauseflag && calife_list->GetSleep() && state == SIZE_RESTORED )
		calife_list->SetSleep(WAKE_UP);
	
	//if (calife_list->GetSleep() && state == SIZE_MAXIMIZED )
	//	calife_list->SetSleep(WAKE_UP);
    
	if (!pauseflag && state == SIZE_MAXIMIZED )
		calife_list->SetSleep(WAKE_UP);
    
	//if (state==SIZE_MAXIMIZED)
	//	calife_list->SetSleep(WAKE_UP);
*/
	GetClientRect(hwnd, &rect);


#ifdef FIXED_640_480
//rect has left and top fields 0, so width, height are right, bottom

	RECT scr;
	int  framepixels, width, height;
	if (rect.right > WBM->CX() ||	rect.bottom  > WBM->CY())
	{ 	//Correct one or both measurements of the window.
		GetWindowRect(hwnd, &scr);
		width = scr.right - scr.left; //window width
		height = scr.bottom - scr.top;  //window height
		if (rect.right > WBM->CX())
		{
			framepixels = width - rect.right;
			//window width - client width
			width = WBM->CX() + framepixels;
		}
		if (rect.bottom  > WBM->CY())
		{
			framepixels = height - rect.bottom;	//window height - client height
			height = WBM->CY() + framepixels;
		}
		//Then resize window to the correct rect.
	
		SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, width, height,
					SWP_NOMOVE); //Last param means only change size.
		GetClientRect(hwnd, &rect); //Reset the rect.
	}
#endif //FIXED_640_480


	calife_list->Locate();
	
// The IDM_CLEAR erases the bitmap and the screen and draws the
// focus box on both of them.

//if paused, zoomed, and focus is 2D, then don't clear the bitmap
//because we need the colors that are stored on it.
//otherwise, clear it. mike 11-1-97
	if (!(calife_list->GetSleep() && zoomviewflag && calife_list->Focus()->Getdimension()==2))
		SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);

// Open controls button bar
		  	
	if (not_seeded_yet_flag) //need to have set size to FourierSeed
    { //You didn't find "ACTIVE.CAS", or ACTIVE.CAS didn't store cells
		calife_list->FourierSeed();
		not_seeded_yet_flag = FALSE;
  	}


// Adjust status bar size.
	if (hwndStatusBar)
//	if (IsWindowVisible (hwndStatusBar))
    {
		GetWindowRect (hwndStatusBar, &rWindow) ;
        statusBarHeight = rWindow.bottom - rWindow.top ;
	    MoveWindow (hwndStatusBar, 0, cy - statusBarHeight, 
                                              cx, statusBarHeight, TRUE) ;
	}
//commented out because statusbarheight is never really 0, but rather
//the status bar is either visible or not visible.  mike 10/27
//use statusON to find out if its visibility
/*    else
    {
	    statusBarHeight = 0 ;
    }
*/
//InvalidateRect(hwndActionToolbar, NULL, FALSE);
//InvalidateRect(hwndActionToolbar, NULL, FALSE);
	
		SendMessage(hwndActionToolbar, WM_SIZE, state, MAKELONG(cx, cy));
		SendMessage(hwndDialogToolbar, WM_SIZE, state, MAKELONG(cx, cy));

	//adjust viewport for opengl
		capowgl->Size(hwnd);//mike

}
/*********************************************************/

static void MyWnd_MOVE(HWND hwnd, int x, int y)
{
	//((fn)((hwnd), (int)LOWORD(lParam), (int)HIWORD(lParam)), 0L)
}

/*********************************************************/

static void MyWnd_COMMAND(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	//((fn)((hwnd), (int)(wParam), (HWND)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

	HDC  hdc;
	HDC  hdc_clip;   // Device Handle to Clipboard
	RECT rect;
	RECT CaptureRect; // Rect to Capture Screen
	RECT r;
//	short focusflag=1;
	char buffer[20];
	POINT point;
//Begin commdlg stuff=====================
//MessageBox ( masterhwnd, "HELLO", "HELL", MB_OK );

	char szFileName[MAXFILENAME];
	char szFileTitle[MAXFILENAME];
	OPENFILENAME ofn;
	
	//CA and CAS file load save
/*	char szFilterSpecCA [128] =
		"CA Files (*.CA)\0All Files (*.*)\0*.*\0";
	char szFilterSpecCAS [128] =
		"Experiment Files (*.CAS)\0All Files (*.*)\0*.*\0";*/

	char szFilter [128] =
	//	"CA Files (*.CA)\0*.CA\0Experiment File (*.CAs)\0*.CAs\0User Rules (*.dll)\0*.dll\0 All Files (*.*)\0*.*\0";
		"Experiment File (*.CAs)\0*.CAs\0CA Files (*.CA)\0*.CA\0User Rules (*.dll)\0*.dll\0 All Files (*.*)\0*.*\0";
//RR 2007 Changed this trying to make CAS default file type
//End commdlg stuff=======================
	
// Message Processing	

	switch(id)
	{

// START FILE MENU====================================
 	case IDM_VIEW_MENU:
			RECT r1;
			ToolBar_GetItemRect(hwndActionToolbar, VIEWMENU_BUTTON, &r1);
			point.x = r1.left;
			point.y = r1.bottom;
			ClientToScreen ( hwnd, &point );
			TrackPopupMenu ( hViewMenu, 0, point.x, point.y, 0 , hwnd, NULL );
		break;
		
		case IDM_CATYPE_MENU:
			RECT r2;
			ToolBar_GetItemRect(hwndActionToolbar, CATYPEMENU_BUTTON, &r2);
			point.x = r2.left;
			point.y = r2.bottom;
			ClientToScreen ( hwnd, &point );
			TrackPopupMenu ( hCATypeMenu, 0, point.x, point.y, 0 , hwnd, NULL );
		break;
 
		case IDM_SEED_MENU:
			RECT r3;
			ToolBar_GetItemRect(hwndActionToolbar, SEEDMENU_BUTTON, &r3);
			point.x = r3.left;
			point.y = r3.bottom;
			ClientToScreen ( hwnd, &point );
			TrackPopupMenu ( hSeedMenu, 0, point.x, point.y, 0 , hwnd, NULL );
		break;

		
		// For View Drop down Menu... Call ViewProc to handle it
		// Call to ViewProc used to avoid duplicate code
		case RADIO_DOWN_VIEW:
		case RADIO_SCROLL_VIEW:
		case RADIO_WIRE_VIEW:
		case RADIO_GRAPH_VIEW:
		case RADIO_SPLIT_VIEW:
		case RADIO_POINT_VIEW:
			ViewProc( hDlgView, WM_COMMAND, id, 0L );
			break;

		// For CA Drop down Menu... 
		case CA_STANDARD: 
		case CA_REVERSIBLE:
		case CA_HEATWAVE:
		case CA_HEATWAVE2:
		case ALT_CA_WAVE:
		case CA_WAVE2:
		case CA_OSCILLATOR:
		case CA_DIVERSE_OSCILLATOR:
        case ALT_CA_OSCILLATOR_WAVE:
		case ALT_CA_DIVERSE_OSCILLATOR_WAVE:
		case ALT_CA_ULAM_WAVE:
		case CA_CUBIC_ULAM_WAVE:
		case CA_AUTO_ULAM_WAVE:
		case CA_WAVE_2D:
		case CA_HEAT_2D:
			if (focusflag)
				calife_list->SetCAType(calife_list->FocusCA(), id,TRUE);
			else // Second argument says adjust for the rule to be stable.
				calife_list->SetAllType(id, TRUE);
			capowgl->AdjustHeightFactor(calife_list->FocusCA());
			if (hDlgOpenGL)
				InvalidateRect(hDlgOpenGL, NULL, TRUE);
		break;

		// For Seed Drop down Menu... Call WorldProc to handle it
		// Call to WorldProc used to avoid duplicate code
		case IDC_ONESEED:
		case IDC_ZEROSEED:
		case IDC_SMOOTH:
		case IDC_SINESEED:
		case IDC_RANDOMSEED:
		case IDC_FOURIERSEED:
		case IDC_RANDOMTOUCH:
		case IDC_SEED_HALFMAX:	
			WorldProc( 0, WM_COMMAND, id, 0L);
			break;

		case IDM_SWAP:
			if ( ActionToolbar == 1 )
				SendMessage ( masterhwnd, WM_COMMAND, IDM_OLDTOOLBAR, 0L );
			else
				if ( DialogToolbar == 1 )
					SendMessage ( masterhwnd, WM_COMMAND, IDM_NEWTOOLBAR, 0L );
			break;

	// Open and Save Commands
		case IDM_LOADRULEFOCUS:
			{
			SetCursor(LoadCursor(NULL, IDC_WAIT)); // Wait, I'm working!
			short temp = 1;
			if (!calife_list->LoadUserRule(masterhwnd, temp))
				break;
			recreateUserDialog();
			calife_list->SetCAType(calife_list->FocusCA(), CA_USER, TRUE);
			update_flag = 1;
			/* Redraws lookup dialog menu */
			if (hDlgWorld)
				SendMessage(hDlgWorld, WM_COMMAND, SC_UPDATE, 0L);
			SetCursor(LoadCursor(NULL, IDC_ARROW)); //I'm done!
			break;
			}
		case IDM_LOADRULEALL:
		  {
			short temp = 0;
			SetCursor(LoadCursor(NULL, IDC_WAIT)); // Wait, I'm working!
			if (!calife_list->LoadUserRule(masterhwnd, temp))
				break;
			recreateUserDialog();
			calife_list->SetAllType(CA_USER, TRUE);

			update_flag = 1;
			/* Redraws lookup dialog menu */
			if (hDlgWorld)
				SendMessage(hDlgWorld, WM_COMMAND, SC_UPDATE, 0L);
			SetCursor(LoadCursor(NULL, IDC_ARROW)); //I'm done!
            break;
		  }

		case IDM_FILE_SAVE:   //Opens Modal Save Dialog  
			inloadsave = TRUE;   //Stop running while you get ready to save            
			DialogBox(hInst, "SAVE", hwnd, (DLGPROC)SaveFileProc);
			inloadsave = FALSE;  //Go back              
			break;

		case IDM_OPEN:
			// fill in non-variant fields of OPENFILENAME struct.
			ofn.lStructSize       = sizeof(OPENFILENAME);
			ofn.hwndOwner	      = hwnd;
			ofn.lpstrCustomFilter = NULL;
			ofn.nMaxCustFilter	  = 0;
			ofn.nFilterIndex	  = 1;
			ofn.nMaxFile	      = MAXFILENAME;
			ofn.lpstrInitialDir   = NULL;
			ofn.lpstrFileTitle    = szFileTitle;
			ofn.nMaxFileTitle     = MAXFILENAME;
			ofn.lpstrTitle        = NULL;
			ofn.Flags             = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
				
		
			strcpy(szFileName, "");
			ofn.nFilterIndex = filterflag;
			ofn.lpstrFile = szFileName;

			ofn.lpstrFilter	= szFilter;  
			ofn.lpstrDefExt   = "CAS";  //RR 2007.  Was CA.
			inloadsave = TRUE; //Don't do updates while you're in here
			if( GetOpenFileName((LPOPENFILENAME)&ofn) ){
			//	char* str1 = strstr(ofn.lpstrFileTitle, ".");
				char* str2 = strupr(ofn.lpstrFileTitle);
				SetCursor(LoadCursor(NULL, IDC_WAIT)); // Wait, I'm working!
						
			  	if(strstr(str2, ".CAS"))
				{
					filterflag = 1;
					calife_list->Loadall(szFileName);
					
				}
				else if(strstr(str2, ".CA"))
				{
					filterflag = 2;
					if (focusflag)
						calife_list->Load_Individual(szFileName, calife_list->FocusCA());
					else
						calife_list->Loadall_Individual(szFileName);
				}
				else if(strstr(str2, ".DLL"))
				{
					filterflag = 3;
					calife_list->LoadUserRule(masterhwnd, szFileName, focusflag);

					update_flag = 1;
					/* Redraws lookup dialog menu */
					if (hDlgWorld)
						SendMessage(hDlgWorld, WM_COMMAND, SC_UPDATE, 0L);
				}
				else
				{
					MessageBox(hwnd, "Please select again", "Invalid Format", MB_OK);
					SendMessage(hwnd, WM_COMMAND, IDM_OPEN, 0);
				}
			SetCursor(LoadCursor(NULL, IDC_ARROW)); //I'm done!
			}
			inloadsave = FALSE; //Go back to updating.
			update_flag = TRUE;
			break;

		case IDM_RANDOMIZE:							// Randomize the CAs	
			calife_list->Randomize();
/* Rudy 11/10/97, I don't think I need to clear because Randoimze calls SetView for each
CA, which does a wbm->ClearSection, which erases that CA's area and doesnt hurt the
grid or the bars. */
	//		SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);
// mike 11-1-97: don't need this line
//			SendMessage(hwnd, WM_SIZE, 0, 0L);// to give us our status bar back
			break;
		case ID_VERYSLOW:
		case ID_SLOW:
		case ID_MEDIUM:
		case ID_FAST:
		case ID_FASTEST:
			int new_blt_lines;
			if (id == ID_FASTEST)
				new_blt_lines = 3;
			else
				new_blt_lines = 1;
			calife_list->set_blt_lines(new_blt_lines);
			update_timer_speed_index = id - ID_VERYSLOW;
			CLAMP(update_timer_speed_index, 0, UPDATE_TIMER_OPTIONS_COUNT-1);
			update_millisecs_per_cycle = update_timer_options[update_timer_speed_index];
			setPerformanceTimerCycle(update_millisecs_per_cycle);
		/*When MASTERTIMER is on, setTimerCycle changes the purpose of the update_timer_handle
		timer.  When MASTERTIMER is off, setTimerCycle instead changes the
		update_ticks_per_cycle variable used in the PeekMessage loop. */
			break;

		case IDM_PAUSE:								// Puts all CAs to Sleep
// mike 11-1-97: don't need pauseflag, but just use ToggleSleep()
//			pauseflag = (!pauseflag);
			Status_GetText(hwndStatusBar, 0, buffer);
			if ( !strcmp ( buffer , "Ready" ) )
				Status_SetText(hwndStatusBar, 0, 0,"Paused");
			else
				Status_SetText(hwndStatusBar, 0, 0,"Ready");
			calife_list->ToggleSleep();
			capowgl->FocusIsActive(calife_list->GetSleep());
			break;

		case IDM_CLEAR:								// Clears all CAs
			WBM->Clear(hwnd, RGB(0,0,0)); //clears the client area
			RECT rect;
			GetClientRect(masterhwnd, &rect);
			//clear the bitmap
			WBM->ClearSection(rect.left, rect.top, rect.right, rect.bottom);

//mike11-1-97 draw to the bitmap, rather than to the window.
			calife_list->Boxfocus(WBM->GetHDC(),RGB(255,255,255));
			calife_list->DrawDivider(WBM->GetHDC());
/*			hdc = GetDC(hwnd);
				calife_list->Boxfocus(hdc,RGB(255,255,255));
				calife_list->DrawDivider(hdc);
			ReleaseDC(hwnd,hdc);
*/			calife_list->ResetAllGenerationCount();
			InvalidateRect(hwnd, NULL, FALSE);
			break;

		case IDM_EXIT:								// Exit Program
			SendMessage(hwnd,WM_CLOSE,0,0L);
			break;

				
// END FILE MENU====================================
// START EDIT MENU====================================
				
		case IDM_CAPTURE:		// Captures Client Rect to Clipboard
									
			hdc = GetDC(hwnd);
			OpenClipboard ( masterhwnd );
			EmptyClipboard();
			hdc_clip = CreateCompatibleDC (hdc);
			GetClientRect(hwnd, &CaptureRect);
   			hBitmap = CreateCompatibleBitmap ( hdc,CaptureRect.right , CaptureRect.bottom+10 );
			SelectObject(hdc_clip, hBitmap);
			BitBlt ( hdc_clip, 0, 0, CaptureRect.right, CaptureRect.bottom+10, WBM->GetHDC(), 0,0, SRCCOPY );
			SetClipboardData ( CF_BITMAP, hBitmap );
			CloseClipboard();
			DeleteDC(hdc_clip);
			DeleteDC(hdc);
			break;

// END EDIT MENU====================================
// START VIEW MENU====================================
		
		case IDM_SHOW_STATUS:				// Hide or Show Status Bar
			if (hwndStatusBar)
			{
				statusON = !IsWindowVisible(hwndStatusBar);  //flip value
				ShowWindow(hwndStatusBar, (statusON)? SW_SHOW:SW_HIDE);
			}
/* mike 11-1-97: the previous statement does what has been
commented out here. But statusBarHeight doesn't need to
change in value, as it had been doing. Rather we just need
to toggle the window's visibility with ShowWindow.
			if (hwndStatusBar && IsWindowVisible (hwndStatusBar))
            {
				ShowWindow (hwndStatusBar, SW_HIDE) ;
				statusON = FALSE;
	            statusBarHeight = 0;
	
			}
             else
             {
				ShowWindow (hwndStatusBar, SW_SHOW) ;
				statusON= TRUE;
				statusBarHeight = STATUSBARHEIGHT;
			  }
*/
			// Resize other windows.
//			GetClientRect (masterhwnd, &r) ;
//			PostMessage (masterhwnd, WM_SIZE, 0,MAKELPARAM (r.right, r.bottom)) ;
			
			calife_list->Locate();
			InvalidateRect(masterhwnd, NULL, FALSE);
			WBM->Clear(masterhwnd, RGB(0,0,0));
			capowgl->Size(hwnd);
			break;
                
		case IDM_NEWTOOLBAR:
			ActionToolbar = !ActionToolbar;
			DialogToolbar = 0;
			toolbarON=ActionToolbar;
			ShowWindow(hwndActionToolbar, ((ActionToolbar && toolbarON)? SW_SHOW:SW_HIDE));
			ShowWindow(hwndDialogToolbar, ((DialogToolbar && toolbarON)?SW_SHOW:SW_HIDE));
			calife_list->Locate();
			WBM->Clear(masterhwnd, RGB(0,0,0));
			capowgl->Size(hwnd);
			InvalidateRect(masterhwnd, NULL, FALSE);
			break;
		case IDM_OLDTOOLBAR:
			DialogToolbar = !DialogToolbar;
			ActionToolbar = 0;
			toolbarON = DialogToolbar;
			ShowWindow(hwndActionToolbar, ((ActionToolbar && toolbarON)? SW_SHOW:SW_HIDE));
			ShowWindow(hwndDialogToolbar, ((DialogToolbar && toolbarON)?SW_SHOW:SW_HIDE));
			calife_list->Locate();
			WBM->Clear(masterhwnd, RGB(0,0,0));
			capowgl->Size(hwnd);
			InvalidateRect(masterhwnd, NULL, FALSE);
			break;
			// Both Dialogs are off.
/*mike 11-1-97: the preceding lines do what has been commented out
here. But also, toolBarHeight doesn't need to change value, as
it had been doing. I just use show toolBarON and ShowWindow() to
toggle the appropriate toolbar's visiblility.  
			if ( DialogToolbar == 0 && ActionToolbar == 0 )
			{
				if ( id == IDM_OLDTOOLBAR )
				{
					ActionToolbar = 0;
					DialogToolbar = 1;
					WhichToolBar = 0;
					toolbarON = TRUE;
//					toolBarHeight = TOOLBARHEIGHT;     
					ShowWindow (hwndDialogToolbar, SW_SHOW) ;
						else  // IDM_NEWTOOLBAR
				{
					ActionToolbar = 1;
					DialogToolbar = 0;
					WhichToolBar = 1;
					toolbarON = TRUE;
//					toolBarHeight = TOOLBARHEIGHT;     
					ShowWindow (hwndActionToolbar, SW_SHOW) ;
				}
		//	break;
			}
			else	
				// Action Toolbar is on Dialog is not				
				if ( DialogToolbar == 0 && ActionToolbar == 1 )
				{
					if ( id == IDM_NEWTOOLBAR )
					{
						ActionToolbar = DialogToolbar = 0;
						ShowWindow (hwndActionToolbar, SW_HIDE) ;
						toolbarON = FALSE;
//						toolBarHeight = 0; 
					}
					else  // old toolbar clicked.
					{
						WhichToolBar = 0;
						DialogToolbar = 1;
						ActionToolbar = 0;
//						toolBarHeight = TOOLBARHEIGHT;     
						toolbarON = TRUE;
						ShowWindow (hwndActionToolbar, SW_HIDE) ;
						ShowWindow (hwndDialogToolbar, SW_SHOW) ;
					}
					//break;
				}  // End else if Dialog == 0 action == 1
			// old toolbar on, new one off
				else
					if ( DialogToolbar == 1 && ActionToolbar == 0 )
					{
						if ( id == IDM_OLDTOOLBAR ) // turn all off
						{
							ActionToolbar = DialogToolbar = 0;
							ShowWindow (hwndDialogToolbar, SW_HIDE) ;
							toolbarON = FALSE;
//							toolBarHeight = 0; 
						}
						else // New toolbar clicked
						{
							WhichToolBar = 1;
							DialogToolbar = 0;
							ActionToolbar = 1;
							toolbarON = TRUE;
//							toolBarHeight = TOOLBARHEIGHT;     
							ShowWindow (hwndDialogToolbar, SW_HIDE) ;
							ShowWindow (hwndActionToolbar, SW_SHOW) ;
						}
					//reak;
					}  // end of if dialog == 1 action == 0
					
				// Resize other windows.
//	             GetClientRect (masterhwnd, &r) ;
//		         PostMessage (masterhwnd, WM_SIZE, 0,MAKELPARAM (r.right, r.bottom)) ;
*/
  
InvalidateRect(masterhwnd, NULL, FALSE);
					capowgl->Size(hwnd);
				 break;
			
// END VIEW MENU====================================			
// START CONTROLS MENU and DIALOG CONTROLS =========
			
		case IDM_WORLD:			// Open World Dialog
			if( !hDlgWorld )
			{
				hDlgWorld = CreateDialog( hInst, (LPCTSTR)"WORLD", hwnd,
                							 (DLGPROC)WorldProc);
				GetWindowRect( hDlgWorld, &rect );
				rect.bottom -= rect.top;
				rect.right  -= rect.left;
				rect.left = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"WORLDX", 100 );
				rect.top  = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"WORLDY", 100 );
				if( GetSystemMetrics(SM_CXSCREEN) < rect.left-10 ) rect.left = 25;
				if( GetSystemMetrics(SM_CYSCREEN) < rect.top -10 ) rect.top = 25;
				MoveWindow( hDlgWorld, rect.left, rect.top, rect.right, rect.bottom,
								FALSE);
				ShowWindow( hDlgWorld, TRUE );
			} 
			else
				DestroyWindow( hDlgWorld );
			
			break;

		case IDM_CONFIGURE:			// ScreenSaver settings Dialog
			if( !hDlgConfigure )
			{
				hDlgConfigure = CreateDialog( hInst, (LPCTSTR)"CONFIGURE", hwnd,
                							 (DLGPROC)ConfigureProc);
				GetWindowRect( hDlgConfigure, &rect );
				rect.bottom -= rect.top;
				rect.right  -= rect.left;
				rect.left = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"CONFIGUREX", 100 );
				rect.top  = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"CONFIGUREY", 100 );
				if( GetSystemMetrics(SM_CXSCREEN) < rect.left-10 ) rect.left = 25;
				if( GetSystemMetrics(SM_CYSCREEN) < rect.top -10 ) rect.top = 25;
				MoveWindow( hDlgConfigure, rect.left, rect.top, rect.right, rect.bottom,
								FALSE);
				ShowWindow( hDlgConfigure, TRUE );
			} 
			else
				DestroyWindow( hDlgConfigure );
			
			break;


		case IDM_COLOR:			// Open Color Dialog
			if (!hDlgColor)
			{
				hDlgColor = CreateDialog (hInst, (LPSTR)"COLOR", hwnd, 
					                     (DLGPROC)ColorProc);
				GetWindowRect(hDlgColor, &rect);
				rect.bottom-=rect.top;
				rect.right-=rect.left;
				rect.left = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"COLORX",100);
				rect.top = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"COLORY",100);
				if (GetSystemMetrics(SM_CXSCREEN)<rect.left-10)
					rect.left=25;
				if (GetSystemMetrics(SM_CYSCREEN)<rect.top-10)
					rect.top=25;
				MoveWindow(hDlgColor, rect.left,rect.top,rect.right,rect.bottom,FALSE);
				ShowWindow(hDlgColor, TRUE);
			}
			else
				DestroyWindow(hDlgColor);
				
			break;
			
			

		case IDM_VIEW:			// Open View Dialog
			if(!hDlgView)
			{	hDlgView = CreateDialog( hInst, (LPSTR)"VIEW", hwnd,
             				   (DLGPROC)ViewProc );
				GetWindowRect( hDlgView, &rect );
				rect.bottom -= rect.top;
				rect.right  -= rect.left;
				rect.left = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"VIEWX",100);
				rect.top  = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"VIEWY",100);
				if (GetSystemMetrics(SM_CXSCREEN) < rect.left-10) rect.left = 25;
				if (GetSystemMetrics(SM_CYSCREEN) < rect.top -10) rect.top = 25;
				MoveWindow(hDlgView, rect.left, rect.top, rect.right, rect.bottom, FALSE);
				ShowWindow(hDlgView, TRUE);
			} // if
			else
				DestroyWindow(hDlgView);
			break;

		case IDM_DIGITAL:			// Open Digital Dialog
			if( !hDlgDigital )
			{
				hDlgDigital = CreateDialog( hInst, (LPSTR)"DIGITAL", hwnd,
                 							  (DLGPROC)DigitalProc );
				GetWindowRect( hDlgDigital, &rect );
				rect.bottom -= rect.top;
				rect.right  -= rect.left;
				rect.left = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"DIGITALX", 100 );
				rect.top  = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"DIGITALY", 100 );
				if( GetSystemMetrics(SM_CXSCREEN) < rect.left-10 ) rect.left = 25;
				if( GetSystemMetrics(SM_CYSCREEN) < rect.top -10 ) rect.top = 25;
				MoveWindow( hDlgDigital, rect.left, rect.top, rect.right, rect.bottom,
							FALSE);
				ShowWindow( hDlgDigital, TRUE );
			} // if
			else
				DestroyWindow( hDlgDigital );
			break;


		case IDM_ANALOG:			// Open Analog Dialog
			if( !hDlgAnalog )
			{
				hDlgAnalog = CreateDialog( hInst, (LPSTR)"ANALOG", hwnd,
						                 (DLGPROC)AnalogProc );
				GetWindowRect( hDlgAnalog, &rect );
				rect.bottom -= rect.top;
				rect.right  -= rect.left;
				rect.left = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"ANALOGX", 100 );
				rect.top  = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"ANALOGY", 100 );
				if( GetSystemMetrics(SM_CXSCREEN) < rect.left-10 ) rect.left = 25;
				if( GetSystemMetrics(SM_CYSCREEN) < rect.top -10 ) rect.top = 25;
				MoveWindow( hDlgAnalog, rect.left, rect.top, rect.right, rect.bottom,
							FALSE);
				ShowWindow( hDlgAnalog, TRUE );
			} // if
			else
				DestroyWindow( hDlgAnalog );
			break;

		case IDM_ELECTRIC:			// Opens Electric Dialog
			if( !hDlgElectric )
			{
				hDlgElectric = CreateDialog( hInst, (LPSTR)"ELECTRIC", hwnd,
							               (DLGPROC)ElectricProc );
				GetWindowRect( hDlgElectric, &rect );
				rect.bottom -= rect.top;
				rect.right  -= rect.left;
				rect.left = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"ELECTRICX", 100 );
				rect.top  = (int)GetProfileInt( (LPSTR)szMyAppName, (LPSTR)"ELECTRICY", 100 );
				if( GetSystemMetrics(SM_CXSCREEN) < rect.left-10 ) rect.left = 25;
				if( GetSystemMetrics(SM_CYSCREEN) < rect.top -10 ) rect.top = 25;
				MoveWindow( hDlgElectric, rect.left, rect.top, rect.right, rect.bottom,
							FALSE);
				ShowWindow( hDlgElectric, TRUE );
			} // if
			else
				DestroyWindow( hDlgElectric );
			break;
			
		case IDM_FOURIER:			// Opens the Fourier Dialog
			if (!hDlgFourier)
			{
				hDlgFourier = CreateDialog (hInst, (LPSTR)"FOURIER", hwnd, 
										   (DLGPROC)FourierProc);
				GetWindowRect(hDlgFourier, &rect);
				rect.bottom -=rect.top;
				rect.right	-=rect.left;
				rect.left 	 = (int)GetProfileInt((LPSTR)szMyAppName,
									 (LPSTR)"FOURIERX",100);
				rect.top 	 = (int)GetProfileInt((LPSTR)szMyAppName,
										 (LPSTR)"FOURIERY",100);
				if (GetSystemMetrics(SM_CXSCREEN)<rect.left-10)
					rect.left=25;
				if (GetSystemMetrics(SM_CYSCREEN)<rect.top-10)
					rect.top=25;
				MoveWindow(hDlgFourier, rect.left,rect.top,rect.right,
					rect.bottom,FALSE);
				ShowWindow(hDlgFourier, TRUE);
			}
			else
				DestroyWindow(hDlgFourier);

			break;

		case IDM_USERDIALOG:	// Opens the User Dialog
			createUserDialog();
			break;

			
		case IDM_CYCLE:
			if (!hDlgCycle)
			{
				hDlgCycle = CreateDialog (hInst, (LPSTR)"CYCLE", hwnd, 
										 (DLGPROC)CycleProc);
				GetWindowRect(hDlgCycle, &rect);
				rect.bottom-=rect.top;
				rect.right-=rect.left;
				rect.left = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"CYCLEX",100);
				rect.top = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"CYCLEY",100);
				if (GetSystemMetrics(SM_CXSCREEN)<rect.left-10)
					rect.left=25;
				if (GetSystemMetrics(SM_CYSCREEN)<rect.top-10)
					rect.top=25;
				MoveWindow(hDlgCycle, rect.left,rect.top,rect.right,rect.bottom,FALSE);
				ShowWindow(hDlgCycle, TRUE);
			}
			else
				DestroyWindow(hDlgCycle);
			break;

		case IDM_EXP:
			if (!hDlgExp)
			{
				hDlgExp = CreateDialog (
				hInst, (LPSTR)"EXPERIMENT", hwnd, (DLGPROC)ExpProc);
				GetWindowRect(hDlgExp, &rect);
				rect.bottom-=rect.top;
				rect.right-=rect.left;
				rect.left = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"EXPERIMENTX",100);
				rect.top = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"EXPERIMENTY",100);
				if (GetSystemMetrics(SM_CXSCREEN)<rect.left-10)
					rect.left=25;
				if (GetSystemMetrics(SM_CYSCREEN)<rect.top-10)
					rect.top=25;
				MoveWindow(hDlgExp, rect.left,rect.top,rect.right,rect.bottom,FALSE);
				ShowWindow(hDlgExp, TRUE);
			}
			else
			{
				DestroyWindow(hDlgExp);
			}
		
			break;
			
			
		case IDM_CELL:
			if(!hDlgCell)
			{	
				hDlgCell = CreateDialog( hInst, (LPSTR)"CELL", hwnd,
             							   (DLGPROC)CellProc );
				GetWindowRect( hDlgCell, &rect );
				rect.bottom -= rect.top;
				rect.right  -= rect.left;
				rect.left = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"CELLX",100);
				rect.top  = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"CELLY",100);
				if (GetSystemMetrics(SM_CXSCREEN) < rect.left-10) rect.left = 25;
				if (GetSystemMetrics(SM_CYSCREEN) < rect.top -10) rect.top = 25;
				MoveWindow(hDlgCell, rect.left, rect.top, rect.right, rect.bottom, FALSE);
				ShowWindow(hDlgCell, TRUE);
			} // if
			else
				DestroyWindow(hDlgCell);
			break;
		
		case IDM_LOAD_USER_RULE:
			
			if (!calife_list->LoadUserRule(masterhwnd, focusflag))
					break;

			recreateUserDialog();
			if (focusflag)
				calife_list->SetCAType(calife_list->FocusCA(), CA_USER,TRUE);
			else // Second argument says adjust for the rule to be stable.
				calife_list->SetAllType(CA_USER, TRUE);
		
			break;		
	
		case IDM_CLOSE:
			// if any dialog box is open
			if (hDlgCycle || hDlgExp || hDlgColor || hDlgFourier ||
	            hDlgAnalog ||	hDlgCell || hDlgElectric ||
				hDlgDigital || hDlgView || hDlgWorld || hDlgConfigure ||
				hUserDialog || hDlgGenerators || hDlgOpenGL )
			{  
				if (hDlgGenerators)
					DestroyWindow(hDlgGenerators );
				if (hDlgOpenGL)
					DestroyWindow(hDlgOpenGL );
				if (hDlgCycle)
					DestroyWindow(hDlgCycle);
				if (hDlgExp)
					DestroyWindow(hDlgExp);
				if (hDlgColor)
					DestroyWindow(hDlgColor);
				if (hDlgFourier)
					DestroyWindow(hDlgFourier);
				if (hDlgAnalog)
					DestroyWindow(hDlgAnalog);
				if (hDlgCell)
					DestroyWindow(hDlgCell);
				if (hDlgElectric)
					DestroyWindow(hDlgElectric);
				if (hDlgDigital)
					DestroyWindow(hDlgDigital);
				if (hDlgView)
					DestroyWindow(hDlgView);
				if (hDlgWorld)
					DestroyWindow(hDlgWorld);
				if (hDlgConfigure)
					DestroyWindow(hDlgConfigure);
				if (hUserDialog)
						DestroyWindow(hUserDialog);
				if (hDlgGenerators)
					DestroyWindow(hDlgGenerators);
			}
           	break;

					
		
// END CONTROLS MENU and DIALOG CONTROLS =============			
// START TOOLS MENU====================================
			
		case CUR_PICK:
		case CUR_ZAP:
		case CUR_TOUCH:
		case CUR_COPY:
		case CUR_GENERATOR:
			if ( id == CUR_PICK )
			{
			    	ToolBar_CheckButton(hwndActionToolbar, CUR_PICK, TRUE );
					ToolBar_CheckButton(hwndActionToolbar, CUR_TOUCH, FALSE );
					ToolBar_CheckButton(hwndActionToolbar, CUR_GENERATOR, FALSE );
					ToolBar_CheckButton(hwndActionToolbar, CUR_ZAP, FALSE );
			}
			else
			if ( id == CUR_TOUCH )
			{
					ToolBar_CheckButton(hwndActionToolbar, CUR_TOUCH, TRUE );				
					ToolBar_CheckButton(hwndActionToolbar, CUR_PICK, FALSE );
					ToolBar_CheckButton(hwndActionToolbar, CUR_GENERATOR, FALSE );
					ToolBar_CheckButton(hwndActionToolbar, CUR_ZAP, FALSE );
			}
			else
			if ( id == CUR_ZAP )
			{
					ToolBar_CheckButton(hwndActionToolbar, CUR_ZAP, TRUE );	
					ToolBar_CheckButton(hwndActionToolbar, CUR_TOUCH, FALSE );				
					ToolBar_CheckButton(hwndActionToolbar, CUR_PICK, FALSE );
					ToolBar_CheckButton(hwndActionToolbar, CUR_GENERATOR, FALSE );
			}
			else
			if ( id == CUR_GENERATOR )
			{
					ToolBar_CheckButton(hwndActionToolbar, CUR_GENERATOR, TRUE );	
					ToolBar_CheckButton(hwndActionToolbar, CUR_TOUCH, FALSE );				
					ToolBar_CheckButton(hwndActionToolbar, CUR_PICK, FALSE );
					ToolBar_CheckButton(hwndActionToolbar, CUR_ZAP, FALSE );	

			}
			SetClassLong(hwnd, GCL_HCURSOR, (int)LoadCursor(hInst,
					     MAKEINTRESOURCE(id)));
			oldcursormode = cursormode;
			cursormode = id;
			break;

		case IDM_CHANGEALL:
			focusflag = 1;
//			ToolBar_CheckButton(hwndToolbar, IDM_CHANGEALL, TRUE    );
//			ToolBar_CheckButton(hwndToolbar, IDM_CHANGEFOCUS, FALSE );
    		ToolBar_ChangeBitmap(hwndActionToolbar, IDM_CHANGEALL, BUT_CHANGEFOCUSLARGE);
			ToolBar_SetCmdID(hwndActionToolbar, CHANGEALLFOCUS_BUTTON, IDM_CHANGEFOCUS);
			update_flag = 1; //Have to update the focus/all radio button in the dialogs.
			break;

		case IDM_CHANGEFOCUS:
			focusflag = 0;
			//ToolBar_CheckButton(hwndToolbar, IDM_CHANGEALL, FALSE    );
			//ToolBar_CheckButton(hwndToolbar, IDM_CHANGEFOCUS, TRUE );
			ToolBar_ChangeBitmap(hwndActionToolbar, IDM_CHANGEFOCUS, BUT_CHANGEALLLARGE);
			ToolBar_SetCmdID(hwndActionToolbar, CHANGEALLFOCUS_BUTTON, IDM_CHANGEALL);
			update_flag = 1; //Have to update the focus/all radio button in the dialogs.
			break;

		case IDM_CHANGEFOCUSMENU:
			focusflag = 1;
			ToolBar_ChangeBitmap(hwndActionToolbar, IDM_CHANGEALL, BUT_CHANGEFOCUSLARGE);
			ToolBar_SetCmdID(hwndActionToolbar, CHANGEALLFOCUS_BUTTON, IDM_CHANGEFOCUS);
			update_flag = 1; //Have to update the focus/all radio button in the dialogs.
			break;
		case IDM_CHANGEALLMENU:
			focusflag = 0;
			ToolBar_ChangeBitmap(hwndActionToolbar, IDM_CHANGEFOCUS, BUT_CHANGEALLLARGE);
			ToolBar_SetCmdID(hwndActionToolbar, CHANGEALLFOCUS_BUTTON, IDM_CHANGEALL);
			update_flag = 1; //Have to update the focus/all radio button in the dialogs.
			break;

// END TOOLS MENU====================================
// START HELP MENU====================================				
			
		case IDM_HELP:								// Calls Help
			//WinHelp(hwnd, "capow.hlp", HELP_FINDER, 0); //Old Way
		   /* As of 12/31/2007, Windows Vista has made HLP files obsolete, so I switched to CHM,
			or HTMLHelp.  For the converstion for using HTMLHelp with Visual Studio 6.0, see Rudy
			Rucker, SOFTWARE ENGINEERING AND COMPUTER GAMES,(Addison Wesley 2003) Chap 21. */

			//HtmlHelp(hwnd, "Capow.chm", HH_DISPLAY_TOPIC, 0);  //2017 got a newer htmlhelp.lib. And it's listed in the Link list.
				//As of 2017 the htmlhelp.lib throws a inker exception relating to SAFESEH so I turned off that link flag there.

				ShellExecute(0, 0, "http://www.rudyrucker.com/capow/capowhelp.htm", 0, 0, SW_SHOW); //BEST solution, found in 2017.  Keep the help file online
					//and let the users go read it online.  Easy to update this way.  ShellExecute does the job!
				
				break;

		case IDM_ABOUT:			// Opens About Dialog
				DialogBox(hInst, "ABOUT", hwnd, (DLGPROC)AboutProc);
				break;
		case IDM_GENERATORS:
			if(!hDlgGenerators)
			{	
				hDlgGenerators = CreateDialog( hInst, (LPSTR)"GENERATORS", hwnd,
             							   (DLGPROC)GeneratorsProc );
				GetWindowRect( hDlgGenerators, &rect );
				rect.bottom -= rect.top;
				rect.right  -= rect.left;
				rect.left = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"GENERATORSX",100);
				rect.top  = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"GENERATORSY",100);
				if (GetSystemMetrics(SM_CXSCREEN) < rect.left-10) rect.left = 25;
				if (GetSystemMetrics(SM_CYSCREEN) < rect.top -10) rect.top = 25;
				MoveWindow(hDlgGenerators, rect.left, rect.top, rect.right, rect.bottom, FALSE);
				ShowWindow(hDlgGenerators, TRUE);
			} // if
			else
				DestroyWindow(hDlgGenerators);
			break;
		case IDM_OPENGL:
			if(!hDlgOpenGL)
			{	
				hDlgOpenGL = CreateDialog( hInst, (LPSTR)"OPENGL", hwnd,
             							   (DLGPROC)OpenGLProc);
				GetWindowRect( hDlgOpenGL, &rect );
				rect.bottom -= rect.top;
				rect.right  -= rect.left;
				rect.left = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"OPENGLX",100);
				rect.top  = (int)GetProfileInt((LPSTR)szMyAppName,(LPSTR)"OPENGLY",100);
				if (GetSystemMetrics(SM_CXSCREEN) < rect.left-10) rect.left = 25;
				if (GetSystemMetrics(SM_CYSCREEN) < rect.top -10) rect.top = 25;
				MoveWindow(hDlgOpenGL, rect.left, rect.top, rect.right, rect.bottom, FALSE);
				ShowWindow(hDlgOpenGL, TRUE);
			} // if
			else
				DestroyWindow(hDlgOpenGL);
			break;
	}  // End of main message processing Switch
}
/*********************************************************/


LRESULT MyWnd_MENUSELECT(HWND hwnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags)
{        
	 return Statusbar_MenuSelect ( hwnd, MAKEWPARAM(item, flags), LPARAM( hmenu ) );
} 
  
/*********************************************************/
 
static void MyWnd_LBUTTONDOWN(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	//((fn)((hwnd), FALSE, (int)LOWORD(lParam), (int)HIWORD(lParam), (UINT)(wParam)), 0L)
	RECT rect;
	
    HDC	hdc = GetDC(hwnd);
	SetCapture(hwnd);  //mike
	CA*  oldfocus = calife_list->FocusCA();
	
	switch(cursormode)
	{
/* If you are unzoomed and click on the focus, this zooms.
	If you are zoomed and click on the focus, this unzooms.
	If you are unzoomed and click on another, it changes the
		focus to the one you clicked on */
		case CUR_PICK:
			if (!zoomviewflag)
			{
				
				if (calife_list->Setfocus(hdc, calife_list->
					Getfocus(x, y)) == 1)
					// If == 1 then the user is clicking on the focused ca, so zoom
				{
					calife_list->Zoom(1);
					zoomviewflag = TRUE;
					calife_list->Locate();
					SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);
/*The next two lines fix a bug relating to the OpenGL view of 2D Cas.  Often
when you zoom in on a 2D CA this view was coming up dead with no action and
would only wake up when the user resized the window.  So we fake a resize,
and this gets rid of the bug! Rudy 5/21/97. By the way, just doing the
obviously relevant line from the _SIZE code isn't enough, the obvious
line being:				capowgl->Size(hwnd);//mike
*/

/*mike 10/97: I believe the problem with capowgl->Size(hwnd) was that upon startup,
it was called too early, before the window was created. So after moving that
function, the problem seems fixed.  As a test, I've commented out the old fix*/

/*					GetClientRect(hwnd, &rect);
					SendMessage(hwnd, WM_SIZE, SIZE_RESTORED,
						MAKELONG(rect.right, rect.bottom));// to wake up OpenGL
*/
					capowgl->AdjustHeightFactor(calife_list->FocusCA());
				if( hDlgOpenGL )
//					SendMessage( hDlgOpenGL, WM_INITDIALOG, 0, 0L );
					InvalidateRect(hDlgOpenGL, NULL, TRUE);

				//ted
				if( hDlgGenerators )  // Initialize list box
					SendMessage( hDlgGenerators, WM_INITDIALOG, 0, 0L );

				}
				else //Unzoomed, and User not clicking on focus CA
				{

					update_flag = TRUE; //Maybe changing focus

					zoomviewflag = FALSE;
				//	calife_list->Locate();
				//	recreateUserDialog();
/* If I have about six user parameters then when I shift focus to somethign with
one user paramter and then come back to the six guy not all six are showing
if I only do recreateUserDialog(), but the following works: */
					if (hUserDialog)
					{
						DestroyWindow(hUserDialog);
						hUserDialog = 0;
						SendMessage(hwnd, WM_COMMAND, IDM_USERDIALOG, 0L);
					} 
//This next line erases the 1D CAs when shifting focus, which is ugly, so
//we took this line out.
//					SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);

				}
			}
			else 
			{	//ifzoomed on a 2d CA
				if(calife_list->FocusCA()->Getviewmode() ==IDC_2D_VIEW)
					capowgl->LeftButtonDown(fDoubleClick, x, y, keyFlags);  //mike 4/97	

			}
			if( hDlgFourier )  // Adjust Slider bar to position of focus
				SendMessage( hDlgFourier, WM_INITDIALOG, 0, 0L );
			break;

/* This just copy mutates the focus to all other CA's.  If you click on
	a CA other that the focus, it changes focus to the one you clicked on
	and copy mutates that CA to all others. */
		case CUR_ZAP:
			//break;  // this was here I didn't understand it so I left it
			if (calife_list->Setfocus(hdc, calife_list->Getfocus(x, y)) != 1)
			{	// focus changed
				recreateUserDialog();
			}
			calife_list->Copymutate();
			SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);
			break;


/* This copies the focused CA into the CA that you left click on.
	It makes an exact duplicate copy regardless of CA type.
	If you right click, it changes the focus */
		case CUR_COPY:
			calife_list->CopyCA(calife_list->FocusCA(),
			calife_list->Getfocus(x, y));
			GetClientRect(hwnd, &rect);
			calife_list->Locate(rect.right,rect.bottom); // Make sure they're at the top
			SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);
			break;

		case CUR_TOUCH:
	// should also do an if focused check
			calife_list->Touch_CA(x,y,WM_LBUTTONDOWN);
			break;
		
		case CUR_GENERATOR:
			calife_list->LocateNewGenerator(x,y,WM_LBUTTONDOWN);
			if (hDlgGenerators)
				SendMessage(hDlgGenerators, WM_COMMAND, SETCURSEL, 0L);
			break;

	}  // End Switch ( cursor Mode )

	ReleaseDC(hwnd, hdc);
//	SendMessage(hDlgOpenGL, WM_PAINT, 0,0);

}

/*********************************************************/

static void MyWnd_RBUTTONDOWN(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	//((fn)((hwnd), FALSE, (int)LOWORD(lParam), (int)HIWORD(lParam), (UINT)(wParam)), 0L)
	HDC	hdc = GetDC(hwnd);
	
	switch(cursormode)
	{
		case CUR_PICK:
		case CUR_GENERATOR:
			if (calife_list->Zoom(0))
			{
//				SendMessage(hwnd, WM_SIZE, 0, 0L);
				
				zoomviewflag = FALSE;
				calife_list->Locate();
				SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L);
//				WBM->Clear(masterhwnd, RGB(0,0,0));

				if( hDlgOpenGL )
//					SendMessage( hDlgOpenGL, WM_INITDIALOG, 0, 0L );
					InvalidateRect(hDlgOpenGL, NULL, TRUE);
			}
			break;

/* This just changes the focus.  I thought it would be neat to copy with
	the left button and change focus with the right */
		case CUR_COPY:
			calife_list->Setfocus(hdc, calife_list->Getfocus(x, y));
			update_flag = TRUE;
			break;
						
		case CUR_TOUCH: // should also do an if focused check
			calife_list->Touch_CA(x,y,WM_RBUTTONDOWN);
			break;

	} // End Switch ( cursormode )

	ReleaseDC(hwnd, hdc);

}

/*********************************************************/

static void MyWnd_CLOSE(HWND hwnd) 	 //((fn)(hwnd), 0L)
{
#ifdef QUERY_ON_CLOSE
	switch ( MessageBox( hwnd, (LPSTR)"Save Current Experiment?",
					   (LPSTR)"Ready To Exit CAPOW!", MB_YESNOCANCEL ) )
	{
		case IDCANCEL:
				return;

		case IDYES:
			load_save_cells_flag = TRUE;
			calife_list->Saveall("ACTIVE.CAS", TRUE);
		
			//The TRUE argument means do an automatic overwrite of
			//any existing ACTIVE.CAS.
			// Now drop down to IDNO case.

		case IDNO:
#endif QUERY_ON_CLOSE
			//Same old exit code
			// Close any boxes that may be open
		if (randomize_timer_handle)
			KillTimer(hwnd, randomize_timer_handle);
		if (update_timer_handle)
			KillTimer(hwnd, update_timer_handle);
		if (hDlgCycle)
		{
			DestroyWindow( hDlgCycle);
			hDlgCycle = 0;
		}

		if (hDlgExp)
		{
			DestroyWindow( hDlgExp);
			hDlgExp = 0;
		}

		//About dialog is modal, so there is no hDlgAbout

		if (hDlgColor)
		{
			DestroyWindow( hDlgColor);
			hDlgColor = 0;
		}

		if (hDlgFourier)
		{
			DestroyWindow( hDlgFourier);
			hDlgFourier = 0;
		}

		if (hDlgAnalog)
		{
			DestroyWindow (hDlgAnalog);
			hDlgAnalog = 0;
		} 

		if ( hDlgElectric )
		{
			DestroyWindow( hDlgElectric );
			hDlgElectric = 0;
		} 

		if( hDlgDigital )
		{
			DestroyWindow( hDlgDigital );
			hDlgDigital = 0;
		} 

		if (hDlgView)
		{
			DestroyWindow( hDlgView);
			hDlgView = 0;
		}

		if( hDlgWorld )
		{	
			DestroyWindow( hDlgWorld );
			hDlgWorld = 0;
		} 

		if( hDlgConfigure )
		{	
			DestroyWindow( hDlgConfigure );
			hDlgConfigure = 0;
		} 

		if( hDlgCell )
		{	
			DestroyWindow( hDlgCell );
			hDlgCell = 0;
		} 

		if (hUserDialog)
		{	
			DestroyWindow(hUserDialog);
			hUserDialog = 0;
		} // if

		if (hDlgGenerators)
		{	
			DestroyWindow(hDlgGenerators);
			hDlgGenerators = 0;
		}

		if (hDlgOpenGL)
		{	
			DestroyWindow(hDlgOpenGL);
			hDlgOpenGL = 0;
		}

		// Free bitmap
		delete WBM;
		//delete graph;
		delete capowgl;
		delete calife_list;
		calife_list = NULL; //This way you can avoid update after it's gone.
	  	//Calls dll_list destructor.  Important to call FreeLibrary on DLLS.
	
		PostQuitMessage (0);
#ifdef QUERY_ON_CLOSE
	}  // End Switch ( MessageBox YES NO )
#endif //QUREY_ON_CLOSE

}

/*********************************************************/

static void MyWnd_DESTROY(HWND hwnd)     //((fn)(hwnd), 0L)
{
    MyWnd_CLOSE(hwnd);
}

/*********************************************************/

BOOL MyWnd_INITDIALOG(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    //(LRESULT)(DWORD)(UINT)(BOOL)(fn)((hwnd), (HWND)(wParam), lParam)
	return 0;
}


static void MyWnd_MOUSEMOVE(HWND hwnd, int x, int y, UINT flags) 
{
  //  ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
	switch(cursormode)
	{
	case CUR_PICK:
		if(zoomviewflag && calife_list->FocusCA()->Getviewmode() ==IDC_2D_VIEW)
			capowgl->MouseMove(x, y, flags);
		break;
	}
}

static void MyWnd_LBUTTONUP(HWND hwnd, int x, int y, UINT flags)
{
//    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
	switch(cursormode)
	{
	case CUR_PICK:
		if(zoomviewflag && calife_list->FocusCA()->Getviewmode() ==IDC_2D_VIEW)
			capowgl->LeftButtonUp(x, y, flags);
		break;
	}
	ReleaseCapture();

}

static void MyWnd_INITMENUPOPUP(HWND hwnd,  HMENU menu, UINT menuindex, BOOL x )		
{
	switch ( menuindex )   
	{
		case 0:			// File Menu
			CheckMenuItem(menu, IDM_COMPRESS, MF_BYCOMMAND |
			(compressFile?MF_CHECKED:MF_UNCHECKED));

			CheckMenuItem(menu,IDM_PAUSE,MF_BYCOMMAND|
			((calife_list->GetSleep())?MF_CHECKED:MF_UNCHECKED));

			CheckMenuItem(menu,IDM_CONFIGURE,MF_BYCOMMAND|
			((randomize_timer_handle)?MF_CHECKED:MF_UNCHECKED));

			int i;
			for (i=ID_VERYSLOW; i<= ID_FASTEST; i++)
				CheckMenuItem(menu, i, MF_BYCOMMAND |
				(i==ID_VERYSLOW+update_timer_speed_index?MF_CHECKED:MF_UNCHECKED));
			break;
		

		case 2:  // Control Menu 

			if ( focusflag ) // Change all change focus
			{
				CheckMenuItem(menu, IDM_CHANGEALLMENU,   MF_BYCOMMAND | MF_UNCHECKED   );
				CheckMenuItem(menu, IDM_CHANGEFOCUSMENU, MF_BYCOMMAND | MF_CHECKED );
			}
			else
			{
				CheckMenuItem(menu, IDM_CHANGEALLMENU, MF_BYCOMMAND   | MF_CHECKED );
				CheckMenuItem(menu, IDM_CHANGEFOCUSMENU, MF_BYCOMMAND | MF_UNCHECKED   );
			}
			
			CheckMenuItem(menu, IDM_EXP, MF_BYCOMMAND |
			(hDlgExp?MF_CHECKED:MF_UNCHECKED));

			CheckMenuItem(menu, IDM_CYCLE, MF_BYCOMMAND |
			(hDlgCycle?MF_CHECKED:MF_UNCHECKED));

			CheckMenuItem(menu, IDM_COLOR, MF_BYCOMMAND |
			(hDlgColor?MF_CHECKED:MF_UNCHECKED));
					
			CheckMenuItem(menu, IDM_FOURIER, MF_BYCOMMAND |
			(hDlgFourier ?MF_CHECKED:MF_UNCHECKED));
					
			CheckMenuItem(menu, IDM_ANALOG, MF_BYCOMMAND |
			(hDlgAnalog?MF_CHECKED:MF_UNCHECKED));
						
			CheckMenuItem(menu, IDM_CELL, MF_BYCOMMAND |
			(hDlgCell?MF_CHECKED:MF_UNCHECKED));
					 
			CheckMenuItem(menu, IDM_ELECTRIC, MF_BYCOMMAND |
			(hDlgElectric?MF_CHECKED:MF_UNCHECKED));
									
			CheckMenuItem(menu, IDM_DIGITAL, MF_BYCOMMAND |
			(hDlgDigital ?MF_CHECKED:MF_UNCHECKED));
			
			CheckMenuItem(menu, IDM_VIEW, MF_BYCOMMAND |
			(hDlgView ?MF_CHECKED:MF_UNCHECKED));
					
			CheckMenuItem(menu, IDM_WORLD, MF_BYCOMMAND |
			(hDlgWorld?MF_CHECKED:MF_UNCHECKED));
					
			CheckMenuItem(menu, IDM_CONFIGURE, MF_BYCOMMAND |
			(hDlgConfigure?MF_CHECKED:MF_UNCHECKED));
					
			CheckMenuItem(menu, IDM_USERDIALOG, MF_BYCOMMAND |
			(hUserDialog ?MF_CHECKED:MF_UNCHECKED));
					 
			CheckMenuItem(menu, IDM_GENERATORS, MF_BYCOMMAND |
			(hDlgGenerators?MF_CHECKED:MF_UNCHECKED));
					
			CheckMenuItem(menu, IDM_OPENGL, MF_BYCOMMAND |
			(hDlgOpenGL?MF_CHECKED:MF_UNCHECKED));
			break;
		case 3:  // view Menu 
			CheckMenuItem(menu, IDM_SHOW_STATUS, MF_BYCOMMAND |
			(statusON?MF_CHECKED:MF_UNCHECKED));
			CheckMenuItem(menu, IDM_SHOW_TOOLBAR, MF_BYCOMMAND |
			(toolbarON?MF_CHECKED:MF_UNCHECKED));
					
			if ( ActionToolbar )  // New or Old Toolbar
			{
				CheckMenuItem(menu, IDM_NEWTOOLBAR, MF_BYCOMMAND | MF_CHECKED   );
				CheckMenuItem(menu, IDM_OLDTOOLBAR, MF_BYCOMMAND | MF_UNCHECKED );
			}
			if ( DialogToolbar )
			{
				CheckMenuItem(menu, IDM_NEWTOOLBAR, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem(menu, IDM_OLDTOOLBAR, MF_BYCOMMAND | MF_CHECKED   );
			}
			if ( DialogToolbar == 0 && ActionToolbar == 0 )
			{
				CheckMenuItem(menu, IDM_NEWTOOLBAR, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem(menu, IDM_OLDTOOLBAR, MF_BYCOMMAND | MF_UNCHECKED   );
			}
			break;
		case 4:  // cursor Menu 
				

	
			CheckMenuItem(menu, CUR_PICK,   MF_BYCOMMAND | MF_UNCHECKED   );
			CheckMenuItem(menu, CUR_ZAP,    MF_BYCOMMAND | MF_UNCHECKED   );
			CheckMenuItem(menu, CUR_GENERATOR, MF_BYCOMMAND | MF_UNCHECKED   );
			CheckMenuItem(menu, CUR_TOUCH,  MF_BYCOMMAND | MF_UNCHECKED   );

			switch ( cursormode )
			{
				case CUR_PICK:
					CheckMenuItem(menu, CUR_PICK,   MF_BYCOMMAND | MF_CHECKED   );
					break;
	
				case CUR_ZAP:
					CheckMenuItem(menu, CUR_ZAP,    MF_BYCOMMAND | MF_CHECKED   );
					break;
						
				case CUR_TOUCH:
					CheckMenuItem(menu, CUR_TOUCH,  MF_BYCOMMAND | MF_CHECKED   );
					break;
				case CUR_GENERATOR:
					CheckMenuItem(menu, CUR_GENERATOR, MF_BYCOMMAND | MF_CHECKED   );
					break;
			}		// end switch ( cusormode )
			break;
		} // end switch ( menuindex )
	
}


static void MyWnd_TIMER(HWND hwnd, UINT timerid)
{

#ifdef MASTERTIMER
		if (timerid == UPDATE_TIMER_ID)
		{
			Cellmain(hwnd);	
			return;
		}
#endif //MASTERTIMER
	 if (timerid == RANDOMIZE_TIMER_ID)
/* Rudy 12/5/97.  I used to just do this line here:
				calife_list->Randomize(fRandFlags);
But I don't want to keep randomizing even when the CA is paused for one reason
or another.  The code at the start of Cellmain tracks if the CA is paused.
So we'll use this check and just set a flag here */
		randomizenow = TRUE; 
/* Rudy 11/10, I don't think I need to clear because Randoimze calls SetView for each
CA, which does a wbm->ClearSection, which erases that CA's area and doesnt hurt the
grid or the bars. */
//			SendMessage(hwnd, WM_COMMAND, IDM_CLEAR, 0L); //Don't need

}


/*===================================================================*/
//						WINPROC


LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT uiStringBase = 100;

	
	switch (message)
	{
//Modified code by Chi Pan Lao at 9/9/96 by using the windowsx.h Marco
	
		HANDLE_MSG(hwnd,WM_CREATE, MyWnd_CREATE);
		HANDLE_MSG(hwnd,WM_PAINT, MyWnd_PAINT);
		HANDLE_MSG(hwnd,WM_SIZE,MyWnd_SIZE);
		HANDLE_MSG(hwnd,WM_MOVE,MyWnd_MOVE);
		HANDLE_MSG(hwnd,WM_COMMAND,MyWnd_COMMAND);
		HANDLE_MSG(hwnd,WM_LBUTTONDOWN,MyWnd_LBUTTONDOWN);
		HANDLE_MSG(hwnd,WM_RBUTTONDOWN,MyWnd_RBUTTONDOWN);
		HANDLE_MSG(hwnd,WM_CLOSE,MyWnd_CLOSE);
		HANDLE_MSG(hwnd,WM_DESTROY,MyWnd_DESTROY);
		HANDLE_MSG(hwnd,WM_INITDIALOG,MyWnd_INITDIALOG);
		HANDLE_MSG(hwnd,WM_NOTIFY,ToolBarNotify);  // Handles Toolbar
		HANDLE_MSG(hwnd, WM_MENUSELECT, MyWnd_MENUSELECT);
		HANDLE_MSG(hwnd, WM_MOUSEMOVE, MyWnd_MOUSEMOVE);  //Mike 4/97
		HANDLE_MSG(hwnd,WM_LBUTTONUP,MyWnd_LBUTTONUP);	//Mike 4/97
		HANDLE_MSG(hwnd, WM_INITMENUPOPUP,MyWnd_INITMENUPOPUP); //Andrew 9/97	
		HANDLE_MSG(hwnd, WM_TIMER, MyWnd_TIMER);
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
//=======================================================================
//=======================================================================
//=======================================================================
// Local Function Definitions



//-------------------Continually running ca update procedure

void Cellmain(HWND hwnd)
{
	HDC hdc;
	HPALETTE old_hpal;
	static long GenCount;
	static char GenCountChar[10];	
	
	MSG msg;
/*  There are sometimes you don't want to run.  First you don't want to run
if this call happens before WM_CREATE or WM_DESTROY, as it possibly might.
Second you don't want to run while you're saving a CA, you want it to stay the
same till you finish saving it.  Third you want to be extra sure not to run
when a window is minimized, though the next check would take care of this as
well. */
	if (!calife_list || inloadsave || windowIsMinimized) //2017 put || instead of |
		return; 
/* If GetForegroundWindow (or GetFocus would work as well) is not my main
window or one of its modeless dialogs, then I won't do an update.  This check
needn't take very long because a boolean combo gets shortcircuted 
in evaluation, and if the focus is on the main hwnd, the eval bails after 1st
term. Rudy 12/2/97.  The reason for this is to be a better "citizen" and let
other programs run.  We could add a _backgroundrun flag to override this good
behavior if we wanted to.*/
	HWND activewnd = GetForegroundWindow();
	if ( hwnd != activewnd &&
	hDlgCycle != activewnd && hDlgExp != activewnd && hDlgColor != activewnd && 
	hDlgFourier != activewnd && hDlgAnalog != activewnd && hDlgCell != activewnd &&
	hDlgElectric != activewnd &&	hDlgDigital != activewnd &&
	hDlgView != activewnd && hDlgWorld != activewnd && hUserDialog != activewnd &&
	hDlgGenerators != activewnd && hDlgOpenGL != activewnd &&
	hDlgConfigure != activewnd)
		return;
	if (randomizenow)
	{
		calife_list->Randomize(fRandFlags);
		randomizenow = FALSE;
	}

	hdc = GetDC(hwnd);
	
	if (calife_list->numcolor() == 256)
	{
		old_hpal = WBM->WBMSelectPalette(hdc, calife_list->hpal());
		WBM->WBMRealizePalette(hdc);
	}

	calife_list->Update_and_Show(hdc); //Does the 3D stuff.

	if ( !calife_list->GetSleep() )
	{
		calife_list->UpdateGenerationCount();
		GenCount = calife_list->FocusCA()->GetGenerationCount();
		ltoa ( GenCount, GenCountChar, 10 );
		Status_SetText(hwndStatusBar, 2, 0, GenCountChar);
	}

/* if update_flag is TRUE, then some procedure changed some vital information
	of the focus CA, and we need to update the information in these
	boxes */
	if (update_flag)
	{
		if (hDlgCycle)
			SendMessage(hDlgCycle, WM_COMMAND, SC_UPDATE, 0L);
		if (hDlgExp)
			SendMessage(hDlgExp, WM_COMMAND, SC_UPDATE, 0L);
		if (hDlgColor)
			SendMessage(hDlgColor, WM_COMMAND, SC_UPDATE, 0L);
		if (hDlgAnalog)
			SendMessage (hDlgAnalog, WM_COMMAND, SC_UPDATE, 0L );
		if( hDlgCell )
			SendMessage( hDlgCell, WM_COMMAND, SC_UPDATE, 0L );
		if( hDlgElectric )
			SendMessage( hDlgElectric, WM_COMMAND, SC_UPDATE, 0L );
		if( hDlgView )
			SendMessage( hDlgView, WM_COMMAND, SC_UPDATE, 0L );
		if( hDlgDigital )
			SendMessage( hDlgDigital, WM_COMMAND, SC_UPDATE, 0L );
		if( hDlgWorld )
			SendMessage( hDlgWorld, WM_COMMAND, SC_UPDATE, 0L );
		if( hDlgFourier )
			SendMessage( hDlgFourier, WM_COMMAND, SC_UPDATE, 0L );
		if( hDlgGenerators )
			SendMessage( hDlgGenerators, WM_COMMAND, SC_UPDATE, 0L );
		if( hDlgOpenGL )
			SendMessage( hDlgOpenGL, WM_COMMAND, SC_UPDATE, 0L );
			
		update_flag = FALSE;
	}

	if (calife_list->numcolor() == 256)
		WBM->WBMSelectPalette(hdc, old_hpal);
	ReleaseDC(hwnd, hdc);
}

//////////////////////
/// Added by Andrew Forster.
void ParseCommandLine ( char commandline[], char* WinArgv[] )
{
	int i=0;
	char *token;
	//WinArgv holds up to 3 file names

	 token = strtok ( commandline, " " );
	if ( !token )
		return;
	WinArgv[i] = new char [ strlen ( token ) +1 ];
	strcpy ( WinArgv[i], token ) ;
	i++;
	while ( token = strtok ( NULL, " " ) )
	{
		WinArgv[i] = new char [ strlen ( token ) +1 ];
		strcpy ( WinArgv[i++], token );
	}

}
 
BOOL CheckExtension ( char Extension[], char DesiredExtension[] )
{
	int i=0;

	while ( Extension[i] != '\0' )
	  {
			Extension[i] = toupper ( Extension[i] );
			
			i++;
	  }
	i = 0;
	while ( DesiredExtension[i] != '\0' )
	  {
			DesiredExtension[i] = toupper ( DesiredExtension[i] );
			i++;
	  }

		if ( strcmp ( DesiredExtension, Extension ) == 0 )
			return TRUE;
		else
			return FALSE;
}

void GrabExtension ( LPSTR lpszCmdParam, char Extension[] )
{
	char *endp, temp[256];

	if ( lpszCmdParam == NULL )
		return;
	strcpy ( temp, lpszCmdParam );

	if ( !( strchr ( temp, '.' ) ) )
		Extension[0] = '\0';
	else
	{
		while ( endp = strchr ( temp, '.' )  )
			strcpy ( temp, ++endp );
		strncpy ( Extension, temp, 3 );
		Extension[3] = '\0';
	}
	return;
}

