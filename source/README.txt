==================================================
Capow Release
March 3, 2017
Code is now Copyright (C) Rudy Rucker 2017.

To build, install a recent version of Microsoft Visual Studio, I used 2015.
You can download it for free at.
https://msdn.microsoft.com/en-us/library/e2h7fzkw.aspx
When installing, not that it's only the Visual C++ part of the studio that you want,
and the project doesn't use MFC.

To build capow.exe, open Visual C++ and open the project file called capow.sl in your Capow source code directory. sl stands for "solution"  That will build Capow 2017 with only a few trivial warnings.  

If you have trouble getting Visual C++ to open the solution file capow.sln, close Visual C++, not saving any of its changes, and navigate to the directory with the source code and double click on capow.sln.

To build new user CA rules, open Carule.sln, and read the comments in the CArule.cpp file for advice.

The main change from 2007 to 2017 is that I turned on to the BIGGER2D switch in ca.hpp to go to a larger (500 x 250) size for the 2D CAs.  The new size has an
aspect that fits better into modern wide monitors.

In upgrading my project and code from the old Visual C++ Ver 6.0 that I used,
I had to fix a few things in the code.  I tagged all or most of my fixes with
a comment starting with //2017 

I improved the Copy Mutate control and the Userparam Dialog Randomize controls.  I rebuilt a copy of the user rule .dll.  For some reason, with the 2015 version of 
Visul Studio, these rule files are now 300 K big instead of about 60 K.

Another change is that I switched from using HtmlHelp and a local help file to 
 keeping my help file as a pure HTML online at 
rudyrucker.com/capow/capowhelp.html
Loading the old CA or CAS files seems to work, although when I load an old file
that includes a 2D pattern the old patterns is trashed as now we're using bigger world. Usually this doesn't matter much, as a good pattern grows up from
the trashed pattern.

I no longer have the option of saving my CA or CAS files in compressed format as the old Windows compress.exe function is no longer available.

==================================================

Capow Release
March 30, 2004.

Mainly I've added a lot of new rules in the carule.cpp file.  

So as to look at bigger 2D CAs, I did two builds Capow 2004.exe and Capow 2004 (Big2D).exe.
You toggle this build by commenting in or out #define BIG2D in ca.hpp. This switches between
a 2D CA size of a new "big" 320 by 200 or the old small sixe of 120 by 90. If you have lots
of RAM and a fast machine you can try putting in larger numbers.

As the Big2D mode uses a lot of RAM and computation, When loading a 2D CA rule with the Big2D
guy it's better to first select Controls|Change Focus, so that you only load a 2D rule 
into one of the nine panes.  Before loading, it's also better to zoom in on that pane.

==================================================
A Capow user named Larry Andrews made some fixes in 2001.  See details in
Larry Andrews Fixes 2001.txt
==================================================

Capow Source Code Version 6.3
May 3, 1999.

The code is Copyright (C) San Jose State University 1999.
As of May 3, 1999, it is being distributed as shareware and can be freely
reused for non-commercial purposes.  For commercial use, contact Rudy
Rucker at rucker@mathcs.sjsu.edu for written permission.

This source code was written by Rudy Rucker and by the following students
of his at SJSU.

The CAPOW project was funded by a contract from EPRI, the Electric
Power Research Institute of Palo Alto, CA.

Spring 94: Alan Borecky, Andrew Chu, Howard Lin, Charles Miller.
Fall 94: Juyoung Lee, Tuyen Ly, Bob Westergaard.
Spring 95: David Kent, Juyoung Lee, Tuyen Ly, Ping-Chak Wong.
Fall 95: Jerry Chang, Ning Tian.
Spring 96: Darrel Cherry, John Briere, Bang Nguyen, Thai Truong,
	Juiliekara Techasaratoole, Rajaneekara Techasaratoole.
Fall 96: Chi Pan Lao, Otto Leung, Darin Levy, James Kroutch, Siu Ming Tong.
Spring 97: Michael Ling, Andrew Forster, Loc Ho, Lorrie Tanabe.
Fall 97: Andrew Forster, Michael Ling, Ted Colbert, Rong Liu.
Spring 98: Michael Ling.
Fall 2003: Put in a few fixes by Larry Andrews.

This release of the source code is designed to be built with Microsoft
Visual C++, Version 6.0.  To build the code, simply open the Capow.dsw
file with Visual C++ and select Build|Build Capow.exe.  You can also
build the screensaver version Capow.scr with the Capow.dsw project by
using the Build|Set Active Configuration... dialog to change which
version of the program you are building.

The code is not as well annotated as it should be, nor is it wholly 
consistent in style, but it's reasonably stable and bug-free. It's
also worth mentioning that the code is not as object-oriented as it
could be. The code was originally developed for the 16-bit Windows
platform under the Borland compiler and in 1996 it was ported to 
32-bit Windows and the Microsoft compiler.  It does not use the MFC.

Note that by opening the Carule.dsw workspace you can build new *.dll
files for loading new CA rules.

More information about Capow is in the Capow.hlp file and in the papers
posted on the Capow home page.

No guarantees are offers of support are implicit in this shareware offering.

But feel free to contact Rucker if you're doing something interesting with
the code.  Email: rudy@rudyrucker.com

Seek ye the gnarl!

==================================================