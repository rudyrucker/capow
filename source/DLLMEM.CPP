#include "ca.hpp"
#include <commdlg.h>
#include "userpara.hpp"

static char temp_user_rule_file_name[MAXFILENAME];

static BOOL _set_temp_user_rule_file_name(HWND hwnd)
{
//This is a helper function, not a member of any class, use to get a file name.
//Begin commdlg parameters=========================
	char szFileTitle[MAXFILENAME];
	OPENFILENAME ofn;
//Specific DLL file load save parameters
	char szFilterSpecDLL [128] =
		"DLL Files (*.DLL)\0All Files (*.*)\0*.*\0";
		/* file type filters */
//End commdlg stuff=================================
	lstrcpy(temp_user_rule_file_name,"*.DLL");
	// fill in non-variant fields of OPENFILENAME struct.
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner	  = hwnd;
	ofn.lpstrFilter	  = szFilterSpecDLL;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter	  = 0;
	ofn.nFilterIndex	  = 1;
	ofn.lpstrFile         = temp_user_rule_file_name;
	ofn.nMaxFile	  = MAXFILENAME;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrFileTitle    = szFileTitle;
	ofn.nMaxFileTitle     = MAXFILENAME;
	ofn.lpstrTitle        = NULL;
	ofn.lpstrDefExt       = "DLL";
	ofn.Flags             = 0;
	if( GetOpenFileName((LPOPENFILENAME)&ofn) )
		//A valid filename is now in temp_user_rule_file_name
		return TRUE;
	else
	{ //No such file.  Put a blank in temp_user_rule_file_name.
		lstrcpy(temp_user_rule_file_name,"");
		return FALSE;
	}
}

BOOL CA::LoadUserRule(HWND hwnd)
{
	if (_set_temp_user_rule_file_name(hwnd))
		return LoadUserRule(hwnd, temp_user_rule_file_name);
	//else couldn't find the file name.
	return FALSE; 
}

BOOL CA::LoadUserRule(HWND hwnd, char *DLLname)
{
/* First we save all of the old user-rule related fields in case the
loading of the user rule fails at some point.  Another reason to save
these fields is that if the load is successful, you will want to do a 
FreeLibrary on the old _DLLhandle, if that handle was non-NULL. */
	HMODULE old_DLLhandle = _DLLhandle;
	int old_usernabesize = _usernabesize; 
	int old_usercastyle = _usercastyle;  
	FPINT old_lpfnUSERNABESIZE = _lpfnUSERNABESIZE;
	FPINT old_lpfnUSERCASTYLE = _lpfnUSERCASTYLE;
	FPCELLUPDATE_1 old_lpfnUSERRULE_1 = _lpfnUSERRULE_1; 
	FPCELLUPDATE_3 old_lpfnUSERRULE_3 = _lpfnUSERRULE_3; 
	FPCELLUPDATE_5 old_lpfnUSERRULE_5 = _lpfnUSERRULE_5; 
	FPCELLUPDATE_9 old_lpfnUSERRULE_9 = _lpfnUSERRULE_9; 
	//Now try and load the USERRULE_? function pointer.
	if (GetUserRulePtr(hwnd, DLLname))
	{
		/* You loaded everything successfully, so call
		FreeLibrary on the old _DLLhandle*/
		if (old_DLLhandle)
			FreeLibrary(old_DLLhandle);
		return TRUE;
	}
	else //something went wrong, so now restore everything to the old values.
	{
		_DLLhandle = old_DLLhandle;
		_usernabesize = old_usernabesize; 
		_usercastyle = old_usercastyle;  
		_lpfnUSERNABESIZE = old_lpfnUSERNABESIZE;
		_lpfnUSERCASTYLE = old_lpfnUSERCASTYLE;
		_lpfnUSERRULE_1 = old_lpfnUSERRULE_1; 
		_lpfnUSERRULE_3 = old_lpfnUSERRULE_3; 
		_lpfnUSERRULE_5 = old_lpfnUSERRULE_5; 
		_lpfnUSERRULE_9 = old_lpfnUSERRULE_9; 
		return FALSE;
	}
}

BOOL CA::GetUserRulePtr(HWND hwnd, char *DLLname)
{ /*Returns TRUE if it works, FALSE otherwise.  If FALSE you need to
	undo the changes it made to _DLLhandle, and the six _lpfnUSER* function
	pointers. If TRUE you need to do a FreeLibrary on the old _DLLhandle,
	if it was non-NULL.*/
	_DLLhandle = LoadLibrary(DLLname);
/* If you have already loaded this library before, then this increases
the library's "reference count".  You eventually need to call FreeLibrary
once for each time that you call LoadLibary. */
	if ((UINT)_DLLhandle < (UINT)HINSTANCE_ERROR) //LoadLibrary failed
	{
		MessageBox( hwnd,
			(LPSTR)"Unable to Load DLL Library!",
			DLLname,
			MB_OK | MB_ICONEXCLAMATION );
		return FALSE;
	}
//Set all the function pointers to NULL
	_lpfnUSERNABESIZE = NULL;
	_lpfnUSERCASTYLE = NULL;
	_lpfnUSERRULE_1 = NULL;
	_lpfnUSERRULE_3 = NULL;
	_lpfnUSERRULE_5 = NULL;
	_lpfnUSERRULE_9 = NULL;

	FPUSERINITIALIZE _lpfnUSERINITIALIZE = (FPUSERINITIALIZE)GetProcAddress(_DLLhandle, "USERINITIALIZE");
	if (_lpfnUSERINITIALIZE == NULL) //it failed
	{
		MessageBox(hwnd, (LPSTR)"Unable to Get USERINITIALIZE Procedure!",
			DLLname, MB_OK | MB_ICONEXCLAMATION );
		FreeLibrary(_DLLhandle);
		return FALSE;
	}
	
   // Clear all current user parameter
   removeUserParam(this, FALSE);
   (*_lpfnUSERINITIALIZE)(this);

   //Try and find code for the appropriate _lpfnUSERRULE_? in the DLL.
   switch(_usernabesize)
	{
		case 1:
			_lpfnUSERRULE_1	 = (FPCELLUPDATE_1)GetProcAddress(_DLLhandle,
				"USERRULE_1");
			if (_lpfnUSERRULE_1 == NULL) //it failed
			{
				MessageBox(hwnd, (LPSTR)"Unable to Get USERRULE_1 Procedure!",
					DLLname, MB_OK | MB_ICONEXCLAMATION );
				FreeLibrary(_DLLhandle);
				return FALSE;
			}
			break;
		case 3:
			_lpfnUSERRULE_3	 = (FPCELLUPDATE_3)GetProcAddress(_DLLhandle,
				"USERRULE_3");
			if (_lpfnUSERRULE_3 == NULL) //it failed
			{
				MessageBox(hwnd, (LPSTR)"Unable to Get USERRULE_3 Procedure!",
					DLLname, MB_OK | MB_ICONEXCLAMATION );
				FreeLibrary(_DLLhandle);
				return FALSE;
			}
			break;
		case 5:
			_lpfnUSERRULE_5	 = (FPCELLUPDATE_5)GetProcAddress(_DLLhandle,
				"USERRULE_5");
			if (_lpfnUSERRULE_5 == NULL) //it failed
			{
				MessageBox(hwnd, (LPSTR)"Unable to Get USERRULE_5 Procedure!",
					DLLname, MB_OK | MB_ICONEXCLAMATION );
				FreeLibrary(_DLLhandle);
				return FALSE;
			}
			break;
		case 9:
			_lpfnUSERRULE_9	 = (FPCELLUPDATE_9)GetProcAddress(_DLLhandle,
				"USERRULE_9");
			if (_lpfnUSERRULE_9 == NULL) //it failed
			{
				MessageBox(hwnd, (LPSTR)"Unable to Get USERRULE_9 Procedure!",
					DLLname, MB_OK | MB_ICONEXCLAMATION );
				FreeLibrary(_DLLhandle);
				return FALSE;
			}
			break;
	}
//If you get to here, then everything worked!
	lstrcpy(_userrulename, DLLname); //So install the file name,
	type_ca = CA_USER;
	return TRUE; //And tell the world you're happy!
}

void CA::DLLRule_1(int c)
{
	(*_lpfnUSERRULE_1)(this, c);
}

void CA::DLLRule_3(int l, int c, int r)
{
	(*_lpfnUSERRULE_3)(this, l, c, r);
}

void CA::DLLRule_5(int c, int e, int n, int w, int s)
{
	(*_lpfnUSERRULE_5)(this, c, e, n, w, s);
}


void CA::DLLRule_9(int c, int e, int ne, int n, int nw, int w,
				   int sw, int s, int se)
{
	(*_lpfnUSERRULE_9)(this, c, e, ne, n, nw, w, sw, s, se);
} 

//=================CAlist DLL methods==========

BOOL CAlist::LoadUserRule(HWND hwnd, short& focusflag)
{
	/* First get the file name and then load the DLL info individually into
	each of the CAs.  The LoadUserRule should either work for all the CAs
	or fail for all the CAs.  So  we bail as soon as one fails. */
	if (_set_temp_user_rule_file_name(hwnd))
	{
		if (focusflag)
			return (focus->LoadUserRule(hwnd, temp_user_rule_file_name));
		else for (int i=0; i<count; i++)
			if (!(list[i]->LoadUserRule(hwnd, temp_user_rule_file_name)))
				return FALSE; 
		return TRUE; //If you make it through the loop.
	}
	//else couldn't find the file name.
	return FALSE;
}

BOOL CAlist::LoadUserRule(HWND masterhwnd, char* szFileName, short& focusflag)
{
	if(focusflag)
	{
		if (!focus->LoadUserRule(masterhwnd, szFileName))
			return FALSE;
		recreateUserDialog();
		SetCAType(FocusCA(), CA_USER, TRUE);
	}
	else
	{
		for (int i=0; i< count; i++)
				if (!(list[i]->LoadUserRule(hwnd, szFileName)))
					return FALSE;
				recreateUserDialog();
				SetAllType(CA_USER, TRUE);
	}
	return TRUE;
}

