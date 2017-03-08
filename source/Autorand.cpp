#include "types.h"
#include "ca.hpp"

// My profile parameters to load and save in CAPOW.EXE or CAPOW.SCR
UINT fRandFlags = RF_START;
	//Set to something like RF_ID | RF_BOTHVAL | RF_BOTHVW | RF_DING from CA.HPP
	//Used in CONFIGURE.CPP and CASCREEN.CPP
int randomize_timer_cycle = RANDOMIZE_TIMER_CYCLE_START;

	// Names of my Tweak Parameters (to use in readIniVariables
char szFlagsName[] = "Randomize Flags";
char szCycleName[]= "Milliseconds Between Randomization";

//Stuff for using QueryPerformanceCounter
_int64 _start, _end, _freq, _update_ticks_per_cycle;
BOOL _performance_counter_present = TRUE;

//Name of my App, don't use stupid SCRNSAVE.H szAppName.
extern char *szMyAppName;

//Prototypes of the  Helper functions I export from here
void writeProfileInt(LPSTR szSection, LPSTR szKey, int save_int);
void writeIniSettings(HWND hDlg);
void readIniSettings();
void setTimerCycle(HWND hwnd, int &timer_handle, int timer_ID, int millisecs);
void setPerformanceTimerCycle(int millisecs);

//=================The Timer Function=========
 void setPerformanceTimerCycle(int millisecs)
{ /* If this is succssful, timer_handle is non-zero.  I *think* that this value
is in fact equal to timer_ID.  Never use 0 for a timer_ID or there could be
be confusion. */
		/*When MASTERTIMER is on, setTimerCycle changes the nature of the update_timer_handle
		timer.  When MASTERTIMER is off, setTimerCycle instead changes the
		update_ticks_per_cycle variable used in the PeekMessage loop. */

	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&_freq)) //sets _freq
		_performance_counter_present = FALSE;
	/*freq is the cycle rate of your chip.  On my 400 MHz machine, I'm seeing
	400,900,000 as the freq, which is reasonably close to 400 million. */

	if (_performance_counter_present)
	{
		// Convert milliseconds per move to performance counter units per move.
		_update_ticks_per_cycle = millisecs * _freq / 1000;
		// Initialize the counter.
		QueryPerformanceCounter((LARGE_INTEGER*)&_start); //sets _start
	}
	return;
}

void setTimerCycle(HWND hwnd, int &timer_handle, int timer_ID, int millisecs)
{ /* If this is succssful, timer_handle is non-zero.  I *think* that this value
is in fact equal to timer_ID.  Never use 0 for a timer_ID or there could be
be confusion. */
		/*When MASTERTIMER is on, setTimerCycle changes the nature of the update_timer_handle
		timer.  When MASTERTIMER is off, setTimerCycle instead changes the
		update_ticks_per_cycle variable used in the PeekMessage loop. */
	/*Now go ahead and set the timer anyway.  You actually need it if (a) the
	peformer_counter_present is FALSE or if (b) you are using the setTimerCycle
	for a timer you actually need, such as to drive a screensaver app. */
	if (timer_handle)
	{
		KillTimer(hwnd, timer_handle);
		timer_handle = 0;
	}
	if (!(timer_handle = SetTimer(hwnd, timer_ID, millisecs, NULL)))
	{	
		MessageBox( hwnd,
			(LPSTR)"Unable to Create a Windows Timer.  Close Other Apps & Try Again!",
			(LPSTR)"Resource Problem!",
			MB_OK | MB_ICONEXCLAMATION );
		SendMessage(hwnd, WM_DESTROY, 0, 0L);
	}
	return;
}

//=================THE PROFILE FUNCTIONS==============
//Helper function used by writeIniSettings
void writeProfileInt(LPSTR szSection, LPSTR szKey, int save_int)
{
	char buf[40];
	wsprintf((LPSTR)buf,"%u", (UINT)save_int); //Read and write as UINT
	WriteProfileString(szSection, szKey,(LPSTR)buf);
}

/* ------------------------------------------------------------------- 
 writeIniSettings --Helper function to Write parameter settings to WIN.INI 
 ------------------------------------------------------------------- */
void writeIniSettings(HWND hDlg)
{
	#define MAX_CHARS 32
	char numberstring[MAX_CHARS];

	GetWindowText(GetDlgItem(hDlg, IDC_TIMER),
		numberstring, MAX_CHARS);
	int time = atoi(numberstring);
	CLAMP(time, 5, 300);
	randomize_timer_cycle = time * 1000;
	writeProfileInt(szMyAppName, szCycleName, randomize_timer_cycle);
	writeProfileInt(szMyAppName, szFlagsName, fRandFlags);
}

/* ----------------------------------------------------------------
 readIniSettings -- helper function to Read parameter settings from WIN.INI           
 ------------------------------------------------------------------- */
void readIniSettings()
{
/*The last argument to GetProfileInt is the default value.  GetProfileInt
returns UINT.  I keep the szFlagsName and szCycleName at the top of the program,
and use them in the configOK as well. */
	fRandFlags = GetProfileInt(szMyAppName, szFlagsName, RF_START);
	randomize_timer_cycle = (int)GetProfileInt(szMyAppName, szCycleName,
		RANDOMIZE_TIMER_CYCLE_START);
}

/*
Here is the sample code I found in the MSDN Knowledge Base entry called 
"Moving your Game to Windows".  I'm going to use this code 
to vary the program speed by letting replacing UDPATE_TICKS_MS 
with a user-adjustable update_millisecs_per_cycle.

#define UPDATE_TICKS_MS 100    // milliseconds per world update

_int64 start, end, freq, update_ticks_per_cycle
MSG     msg;

// Get ticks-per-second of the performance counter.
//   Note the necessary typecast to a LARGE_INTEGER structure
if (!QueryPerformanceFrequency((LARGE_INTEGER*)&freq))
  return -1;  // error – hardware doesn't support performance counter

// Convert milliseconds per move to performance counter units per move.
update_ticks_per_cycle = UPDATE_TICKS_MS * freq / 1000;

// Initialize the counter.
QueryPerformanceCounter((LARGE_INTEGER*)&start);

  {

  // the main message loop begins here -- while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    QueryPerformanceCounter((LARGE_INTEGER*)&end);
  // The inner loop ensures that the world gets updated more than 
  //   once if need be.
    while ((end - start) >= update_ticks_per_cycle)
      {
      UpdateWorld();
      start += update_ticks_per_cycle;
      }
    } // End of message loop.
  return msg.wParam;

*/