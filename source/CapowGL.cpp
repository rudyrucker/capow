/*******************************************************************************
	FILE:				capowGL.cpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains functions and data for
						the implementation of OpenGL 3D rendering.

	UPDATE LOG:			
				
*******************************************************************************/
//====================INCLUDES===============

#include "capowgl.hpp"
#include <math.h>
#include <string.h>
#include <mmsystem.h>  //for timeGetTime()
#include <commdlg.h>
#include <stdio.h>
//====================DEBUG FLAGS ===============
//====================DEFINE CONSTANTS ===============



 //====================GLOBAL DATA===============
//====================LOCAL FUNCTIONS ===============
//====================EXTERNAL DATA===============

extern WindowBitmap* WBM;
extern BOOL statusON;         
extern BOOL toolbarON;            
extern HWND masterhwnd;
extern int toolBarHeight;
extern int statusBarHeight;
//================================================================

CapowGL::CapowGL(HWND hwnd)
{
	graphtype =			OPENGL_DEFAULTGRAPHTYPE;
	surfacetype = DEFAULTSURFACETYPE;
	material =		DEFAULTMATERIAL;

	heightfactor =	DEFAULTHEIGHTFACTOR;
	spinflag =		DEFAULTSPIN;
	spindelta =		DEFAULTSPINDELTA;
	spinangle =		DEFAULTSPINANGLE;
	z =				DEFAULTZ;
	tiltangle =		DEFAULTTILT;
	lightsflag =	DEFAULTLIGHTS;
	spacing =		DEFAULTSPACING;
	focusIsActive = DEFAULTFOCUSISACTIVE;
	gotLButtonDown = FALSE;
	panY =			DEFAULTPANY;
	panX =			DEFAULTPANX;
	panstep =		DEFAULTPANSTEP;
	lightyaw =		DEFAULTLIGHTYAW;
	lightpitch =	DEFAULTLIGHTPITCH;
	mousemode =		DEFAULTMOUSEMODE;
	zeroplaneflag=	DEFAULTZEROPLANEFLAG;
	maxplaneflag=	DEFAULTMAXPLANEFLAG;
	pointerx =		DEFAULTPOINTERX;
	pointery =		DEFAULTPOINTERY;
	pointerflag =	DEFAULTPOINTERFLAG;
	showgeneratorsflag = DEFAULTSHOWGENERATORSFLAG;
	Resolution(DEFAULTRESOLUTION);

	tempwidth=tempheight=0.0f;
	statsflag = DEFAULTSTATS;
	time = timeGetTime();
	SetUpTorus();
//	SetUpGrid();
	flygo = true;
	showflypos = DEFAULTSHOWFLYPOS;
	flyEye[0] = 0.0f;
	flyEye[1] = 0.0f;
	flyEye[2] =10.0f;
	theta = 3.14159f * -.25f;  //-pi/4  roughly points the eye toward the ca
			dtheta = 0.0f;
			dflyheight = 0.0f;
			flyDir[0] = (float)cos(theta);
			flyDir[1] = (float)sin(theta);
			flyDir[2] = 0.0f;
	cellx = celly = 0;

	threeDGlasses = FALSE;
	interPupilDistance = 0.3f;
	leftColor[0] = .5f; //red
	leftColor[1] = 0.0f;
	leftColor[2] = 0.0f;


	rightColor[0] = 0.0f;
	rightColor[1] = .5f; //green
	rightColor[2] = 0.0f;
	currentEyeColor = leftColor;
	antiAliased = DEFAULTANTIALIASEDFLAG;
	whichEye = LEFTEYE;
	eyeAngle = 2.5f;
	hRC = SetUpOpenGL(hwnd);  // l.andrews 11/2/01 moved from much above so 
          // lots of things (like lightsflag) will be initialized
}

CapowGL::~CapowGL()
{
	wglDeleteContext(hRC);
}

HGLRC CapowGL::SetUpOpenGL(HWND hWnd)
/*this function prepares the rendering context.  An HRC is somewhat analogous
to an HDC, except that an HRC doesn't need to be released during the life
of the program.
*/
{
    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof (PIXELFORMATDESCRIPTOR), // strcut size 
        1,                              // Version number
        PFD_DRAW_TO_WINDOW |    // Flags, draw to a window,
			PFD_DOUBLEBUFFER|
			PFD_SUPPORT_OPENGL, // use OpenGL
        PFD_TYPE_RGBA,          // RGBA pixel values
        24,                     // 24-bit color
        0, 0, 0,                // RGB bits & shift sizes.
        0, 0, 0,                // Don't care about them
        0, 0,                   // No alpha buffer info
        0, 0, 0, 0, 0,          // No accumulation buffer
        32,                     // 32-bit depth buffer
        0,                      // No stencil buffer
        0,                      // No auxiliary buffers
        PFD_MAIN_PLANE,         // Layer type
        0,                      // Reserved (must be 0)
        0,                      // No layer mask
        0,                      // No visible mask
        0                       // No damage mask
    };

    int nMyPixelFormatID;
    HDC hDC;
    // HGLRC hRC; // l.andrews 11/3/01 this was hiding the member variable

    hDC = GetDC( hWnd );
    nMyPixelFormatID = ChoosePixelFormat( hDC, &pfd );

    // catch errors here.
    // If nMyPixelFormat is zero, then there's
    // something wrong... most likely the window's
    // style bits are incorrect (in CreateWindow() )
    // or OpenGl isn't installed on this machine
	if (nMyPixelFormatID==0)
	{
		MessageBox(hWnd, "ChoosePixelFormat() failed!", "OpenGL error", MB_OK);
	}

	SetPixelFormat( hDC, nMyPixelFormatID, &pfd );

    hRC = wglCreateContext( hDC );

    glEnable( GL_DEPTH_TEST );		//enable the Z buffer
	glClearColor(0.0f,0.0f,0.0f,0.0f);   //define the background color
	glClearDepth(1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	SetUpLights();
	ReleaseDC( hWnd, hDC );

	Size(hWnd);

	return hRC;
}

void CapowGL::Draw(HDC hdc, CA* focus)
{
	float time2;
	static float oldtime;
	if (graphtype == FLATCOLOR)
		return;
	time2 = timeGetTime();

/* Problem, this procedure dies sometimes.  It dies if you do your first
zoom in on something a lot of times and if you resize window it wakes up.
Dies in the sense of no longer showing anything changing on scren.
Resizing the window wakes it up. */

/* mike 10-97: actually it doesn't die. CapowGL just wasn't being told the correct
size of the main window, which is done through Size().  Size() wasn't being
called in the right places within Capow.CPP, especially upon startup, so the
viewport for opengl rendering remained zero size.  After tinking with the location of 
CapowGL->Size() in Capow.cpp, it seems to fix the problem.
*/
	if (spinflag)
		spinangle += spindelta;
	graphfocus = focus;

	wglMakeCurrent(hdc, hRC);  //associate the rendering context with an hdc

	//prep the rendering context
    glEnable( GL_DEPTH_TEST );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	if (threeDGlasses)
	{
//		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);

//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_ONE, GL_ONE);
		glPushMatrix();
		currentEyeColor = leftColor;
//		glTranslatef(-interPupilDistance, 0.0f, 0.0f);
//		glRotatef(5.0f, 0.0f, 1.0f, 0.0f);
		whichEye = LEFTEYE;
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		DrawOpenGLScene();
		glPopMatrix();
		glClear(GL_DEPTH_BUFFER_BIT);
		currentEyeColor = rightColor;
//		glTranslatef(interPupilDistance, 0.0f, 0.0f);
//		glRotatef(-5.0f,0.0f, 1.0f, 0.0f);
		whichEye = RIGHTEYE;
		glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
		DrawOpenGLScene();
		//glDisable(GL_BLEND);
		glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	else
		DrawOpenGLScene();  //draw the CA
	SwapBuffers(hdc);  //swap the back buffer to the screen

	//calculate the period between draws, in ms. Used for timing the flying postion
	time = timeGetTime();
	renderperiod = time - time2;  //is the time it took to render the scene
	period = time - oldtime;  //this is the time between each frame, which includes the ca processing
	oldtime = time; //save it
	if (statsflag)
		DrawStats(hdc);

	wglMakeCurrent(NULL,NULL);
}

void CapowGL::DrawOpenGLScene()  //this is the meat of the code
/* Oct 26, 1997, Mike: This function received a thorough rewrite, in order to make 
the code cleaner,and possibly more efficient.  It can now logically accomodate the 
torus shape as well as new shapes in the future (like sphere or cylinder?) Also, 
previously the factor 'spacing' was used extensively to convert the unit length 
between adjacent cells so that the entire CA would fit into the viewport.  However, 
a call to glScalef() at the beginning of this function now replaces most of that work. 
Because it directly changes the Modelview matrix, it should be more efficient this way, 
since the numerous calls to glVertex3f() won't have multiplication with 'spacing'.  
A side effect is that glNormal() will be scaled too, and so unit vectors aren't unit 
vectors anymore.  I use glEnable(GL_NORMALIZE), which will let OpenGL take care of the 
unit-vectorizing. So some of the code, like in ComputeNormals(), which did some 
unitvectorizing, no longer does so now.
*/
{
	float height;
	float fvalue;
	int i,j, i2,j2;
	GLfloat n[3];
	COLORREF pointcolor;

    // Enable depth testing and clear the color and depth
    //  buffers.
    //

//    glLoadIdentity();   //Initialize the Modelview Matrix
	glScalef(spacing, spacing, spacing);  //see comment for DrawOpenGLScene()

	if (mousemode != 6) //not flying, so do the transformations to correctly position the ca
	{
		//take care of panning, and	
		// move the viewpoint out to where we can see everything, in other words zoom
		glTranslatef( panX,panY, z );

		if (threeDGlasses)
		{
			glRotatef(((whichEye)?-eyeAngle: eyeAngle),0.0f, 1.0f, 0.0f);
		}

		//takecare of light position
		glPushMatrix();  //this function pushs the Modelview matrix onto a stack
		glRotatef(lightyaw, 0.0f, 1.0f, 0.0f);
		glRotatef(lightpitch, 1.0f, 0.0f, 0.0f);
		SetUpLights();

/*10-28 mike: I disabled the user's ability to change the light position,
as it now just remains a light pointed in the forward direction.
Consequently, it seems unnecessary to show the light's direction.
*/
	/*	//show light direction
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			glColor3f(1.0f,1.0f,1.0f);
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(0.0f, 0.0f,1.0f);
		glEnd();
		glEnable(GL_LIGHTING);
	*/	
		glPopMatrix();  //get the Modelview matrix which was previously pushed on the stack
	
		//take care of CA orientation
		glRotatef(tiltangle, 1.0f, 0.0f, 0.0f);  //rotate along graph's x axis
		glRotatef(spinangle, 0.0f, 0.0f, 1.0f);  //rotate along graph's y axis
		if (graphtype == SHEET)  
			glTranslatef(-CX_2D/2, CY_2D/2, 0.0f); //center
	}
	else  // flying
	{
		UpdateFly();
/*gluLookAt positions the viewer by specifying it's position, the location to
look at, and an Up vector.
*/
		gluLookAt(flyEye[0], flyEye[1], flyEye[2],
					flyEye[0]+flyDir[0], flyEye[1]+flyDir[1], flyEye[2]+flyDir[2],
					0.0f, 0.0f, 1.0f);
		SetUpLights();
	}

	glEnable(GL_LIGHTING); //lights are set up, so enable them.

	switch (graphtype)
	{
	case SHEET:
		switch(surfacetype)
		{
		case DOTS:
			//if we aren't showing generators or the fly eye, then 
			//every object in the viewport is white, so we can ignore the zbuffer
			if ((!showgeneratorsflag && !showflypos)||threeDGlasses)
				glDisable(GL_DEPTH_TEST);          
			glDisable(GL_LIGHTING);
//			if (threeDGlasses)
//				glColor3fv(currentEyeColor);
//			else
				glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_POINTS);
			for (j = 0; j<CY_2D; j+=interval)
				for(i = 0; i<CX_2D; i+=interval)
					glVertex3f((float)i,(float)-j,(float)GraphHeight(j*CX_2D+i));
			glEnd();
			break;

		case COLORDOTS:
			glShadeModel(GL_SMOOTH);
			glDisable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPointSize(2.0f);
			glBegin(GL_POINTS);
			for (j = 0; j<CY_2D; j+=interval)
				for(i = 0; i<CX_2D; i+=interval)
				{
					pointcolor = GetPixel(WBM->GetHDC(), i, (toolbarON)?j+toolBarHeight :j);
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
					glVertex3f((float)i,(float)-j,(float)GraphHeight(j*CX_2D+i));
				}
			glEnd();		
				glDisable(GL_POINT_SMOOTH);
				glDisable(GL_BLEND);
			glPointSize(1.0f);
			break;

		case POLYLINES:
			glDisable(GL_LIGHTING);
			if (antiAliased)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_LINE_SMOOTH);
//				glLineWidth(1.2f);
//				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			}
			glColor3f(1.0f, 1.0f, 1.0f);

			for (j = 0; j<CY_2D; j+= interval)
			{
				glBegin(GL_LINE_STRIP);
					for(i = 0; i<CX_2D; i+= interval)
						glVertex3f((float)i,(float)-j,(float)GraphHeight(j*CX_2D+i));
				glEnd();
			}
			if (antiAliased)
			{
				glDisable(GL_BLEND);
				glDisable(GL_LINE_SMOOTH);
				glLineWidth(1.0f);
//				glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
			}
			break;

		case COLORLINES:
			glDisable(GL_LIGHTING);
			if (antiAliased)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_LINE_SMOOTH);
			}

			glColor3f(1.0f, 1.0f, 1.0f);
			for (j = 0; j<CY_2D; j+= interval)
			{
				glBegin(GL_LINE_STRIP);
					for(i = 0; i<CX_2D; i+= interval)
					{
						pointcolor = GetPixel(WBM->GetHDC(), i, (toolbarON)?j+toolBarHeight :j);
						glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
						glVertex3f((float)i,(float)-j,(float)GraphHeight(j*CX_2D+i));
					}
				glEnd();
			}
			if (antiAliased)
			{
				glDisable(GL_BLEND);
				glDisable(GL_LINE_SMOOTH);
			}
			break;

		case FACETS:
			glShadeModel(GL_FLAT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			PickMaterial(material);
			glEnable(GL_LIGHTING);
			glEnable(GL_NORMALIZE);  //will convert normals into unit vectors everytime a glNormal() is called

			if (material>1) //if not multicolor
			{
				for(j=0; j<CY_2D-interval;j+= interval)
				{
					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(0.0f, (float)-j, (float)GraphHeight(0,j));
						glVertex3f(0.0f, (float)-(j+interval), (float)GraphHeight(0,j+interval));
						for(i=interval;i<CX_2D; i+= interval)
						{
							NormalVector(i-interval,j, UPPERLEFT,n, interval);				
							glNormal3fv(n);
							glVertex3f((float)i,(float)-j,(float)GraphHeight(i,j));
		
							NormalVector(i-interval,j, LOWERRIGHT,n, interval);				
							glNormal3fv(n);
							glVertex3f((float)i,(float)-(j+interval),GraphHeight(i,j+interval));
						}
					glEnd();
				}
			}
			else  // multicolor
			{
				if (material==1) //unlighted
					glDisable(GL_LIGHTING);
				glEnable (GL_COLOR_MATERIAL);  //to use glColor() as a material
				glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
				for(j=0; j<CY_2D-interval;j+= interval)
				{
					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(0.0f, (float)-j, (float)GraphHeight(0,j));
						glVertex3f(0.0f, (float)-(j+interval), (float)GraphHeight(0,j+interval));
						for(i=interval;i<CX_2D; i+= interval)
						{
							//find the color for each point
							pointcolor = GetPixel(WBM->GetHDC(), i-interval, (toolbarON)?j+toolBarHeight :j);
							glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
							NormalVector(i-interval,j, UPPERLEFT,n, interval);				
							glNormal3fv(n);
							glVertex3f((float)i,(float)-j,(float)GraphHeight(i,j));
		
							NormalVector(i-interval,j, LOWERRIGHT,n, interval);				
							glNormal3fv(n);
							glVertex3f((float)i,(float)-(j+interval),GraphHeight(i,j+interval));
						}
					glEnd();
				}
				glDisable(GL_COLOR_MATERIAL);
			}
			glDisable(GL_NORMALIZE);
			break;

		case SMOOTH:
			if (focusIsActive && material != UNLIGHTED_MULTICOLOR) //if the CA is changing,
				if (! (ThreeDGlasses() && whichEye == RIGHTEYE))
					ComputeNormals();  //compute the normals for gouraud shading
/*Bug: When the CA is paused, ComputeNormals() sometimes isn't called when 
it should have been. Doing a seed for a paused 2-D CA with a smooth surface should show this.
*/
			glShadeModel(GL_SMOOTH); //use Gouraud shading
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  //render both sides of the polygon
			glEnable(GL_NORMALIZE);	//allow opengl to automatically unitvectorize the glNormal() calls
	
			(material == UNLIGHTED_MULTICOLOR)?glDisable(GL_LIGHTING):glEnable(GL_LIGHTING);

			if (material <=1) //if it's multicolor
			{
				glEnable (GL_COLOR_MATERIAL);  //to use glColor() as a material
				glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

				for(j=0; j<CY_2D-interval;j+=interval)
				{
					glBegin(GL_TRIANGLE_STRIP);
					for(i=0;i<CX_2D; i+= interval)
					{
						pointcolor = GetPixel(WBM->GetHDC(), i, (toolbarON)?j+toolBarHeight :j);
						glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
						glNormal3fv(normals[j*CX_2D+i]);
						glVertex3f((float)i,(float)-j, GraphHeight(i,j));

						pointcolor = GetPixel(WBM->GetHDC(), i, interval+ ((toolbarON)?j+toolBarHeight:j));
						glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));					
						glNormal3fv(normals[(j+interval)*CX_2D+i]);
						glVertex3f((float)i,(float)(-(j+interval)),(float)GraphHeight(i,j+interval));
					}
					glEnd();
				}
				glDisable(GL_COLOR_MATERIAL);
			}
			else //single color material
			{
				PickMaterial(material);
				for(j=0; j<CY_2D-interval;j+=interval)
				{
					glBegin(GL_TRIANGLE_STRIP);
					for(i=0;i<CX_2D; i+= interval)
					{
						glNormal3fv(normals[j*CX_2D+i]);
						glVertex3f((float)i,(float)-j, GraphHeight(i,j));

						glNormal3fv(normals[(j+interval)*CX_2D+i]);
						glVertex3f((float)i,(float)(-(j+interval)),(float)GraphHeight(i,j+interval));
					}
					glEnd();
				}
			}
			glDisable(GL_NORMALIZE);
			break;
		}

		if (antiAliased) //anitalias the following planes, if necessary
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.5f);
		}

		//draw reference planes, if necessary
		if (zeroplaneflag)
		{
			glDisable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_LINE_STRIP);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(0.0f, (float)-(CY_2D-1), 0.0f);
				glVertex3f((float)CX_2D-1, (float)-(CY_2D-1), 0.0f);
				glVertex3f((float)CX_2D-1, 0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 0.0f);
			glEnd();
//			glEnable(GL_LIGHTING);
		}
	
		if (maxplaneflag)
		{
			glDisable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
			glPushMatrix();
			glTranslatef(0.0f,0.0f,(float)graphfocus->_max_intensity.Val()*heightfactor);
	
			glBegin(GL_LINE_STRIP);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(0.0f, (float)-(CY_2D-1), 0.0f);
				glVertex3f((float)(CX_2D-1), (float)-(CY_2D-1), 0.0f);
				glVertex3f((float)(CX_2D-1), 0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 0.0f);
			glEnd();	
				glPopMatrix();
//		glEnable(GL_LIGHTING);
		}
	
		//not really used now, since I couldn't find a practical use for it.
		//I thought I could use it to place generators within the 3-D view, but it requires much work.
		if (pointerflag)
		{	//show a white vertical line indicating the current position, like a dipstick
			glDisable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_LINES);
					glVertex3f((float)pointerx,(float)(-pointery),(float)GraphHeight(pointerx,pointery));
					glVertex3f((float)pointerx,(float)(-pointery),(float)graphfocus->_max_intensity.Val()*heightfactor);
			glEnd();
//			glEnable(GL_LIGHTING);
		}

		if (antiAliased)
		{
			glDisable(GL_BLEND);
			glDisable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
		}

		if (showflypos && mousemode!= 6)  //if we're not flying, and we want to show the fly position
		{
			//draw a green sphere with a green line showing its direction
			glDisable(GL_LIGHTING);
			glColor3f(0.0f, 1.0f, 0.0f);
			glPushMatrix();
			glTranslatef(flyEye[0], flyEye[1], flyEye[2]);  //translate to the fly position
			//auxSolidSphere(1.0f);  //draw a sphere //2017 Too much troubel to include GLAUX.LIB
			glBegin(GL_LINES);
				glVertex3f(0.0f, 0.0f,0.0f);
				glVertex3f(flyDir[0], flyDir[1], flyDir[2]);
			glEnd();
			glPopMatrix();
		}



		//show generators as spheres
		if (showgeneratorsflag)
		{
			glDisable(GL_LIGHTING);
			if (ThreeDGlasses())
				glColor3f(1.0f, 1.0f, 1.0f);
			else
				glColor3f(1.0f, 0.0f, 0.0f);  //red
			if (antiAliased)
			{
				glEnable(GL_BLEND);
				glEnable(GL_LINE_SMOOTH);
//				glLineWidth(1.0f);
			}
			for (int k=0; k<graphfocus->generatorlist.Count(); k++)
			{
				glPushMatrix();
				i= graphfocus->generatorlist.Location(k)%CX_2D; 
				j= graphfocus->generatorlist.Location(k)/CX_2D;
				glTranslatef(  i,-j,GraphHeight(i,j));
				// auxSolidSphere(1.0f); //2017  Too much trouble to include GLAUX.LIB
				glPopMatrix();
				glBegin(GL_LINES);
					glVertex3f(i, -j,(float)graphfocus->_max_intensity.Val()*heightfactor);
					glVertex3f(i, -j,-(float)graphfocus->_max_intensity.Val()*heightfactor);
				glEnd();
			}
			if (antiAliased)
			{
				glDisable(GL_BLEND);
				glDisable(GL_LINE_SMOOTH);
//				glLineWidth(1.0f);
			}

		}
		break;  //SHEET case

	case TORUS:  //currently the torus doesn't deform, so DOTS and LINES is pretty useless
		switch(surfacetype)
		{
		case DOTS:
		case COLORDOTS:
			glDisable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_POINTS);

			for (j = 0; j<CY_2D; j+=interval)
			{
				for(i = 0; i<CX_2D; i+=interval)
				{
//					height = GraphHeight(i,j);
//					glVertex3f(slice[i][0]+slice[i][0]*section[j][0]*height,   slice[i][1] + slice[i][1]*section [j][0]*height , section[j][1]*height);
					glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);

				}
			}
			glEnd();
			break;

		
		case POLYLINES:
		case COLORLINES:
			glDisable (GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
			for(i=0; i<CX_2D; i+=interval)
			{
				glBegin(GL_LINE_STRIP);
				for (j=0; j<CY_2D; j+= interval)
				{
//					height = 1.0f + GraphHeight(i,j);
//					glVertex3f(slice[i][0]+slice[i][0]*section[j][0]*height,   slice[i][1] + slice[i][1]*section [j][0]*height , section[j][1]*height);
					glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);
				}
				glEnd();
			}
			break;

		case FACETS:
			//use unlighted color, because calculating the normals for a deforming torus is expensive!
			glDisable(GL_LIGHTING);
			glShadeModel(GL_FLAT);
			glEnable (GL_COLOR_MATERIAL);  //to use glColor() as a material
			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
			i=0;
			for(j=0; j<CY_2D-interval;j+= interval)
			{
				glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);

					glVertex3f(slice[i][0]+slice[i][0]*section[j+interval][0],   slice[i][1] + slice[i][1]*section [j+interval][0] , section[j+interval][1]);

					for(i=interval;i<CX_2D; i+= interval)
					{
						//find the color
						pointcolor = GetPixel(WBM->GetHDC(), i-interval, (toolbarON)?j+toolBarHeight:j);
						glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));

						glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);
	
						glVertex3f(slice[i][0]+slice[i][0]*section[j+interval][0],   slice[i][1] + slice[i][1]*section [j+interval][0] , section[j+interval][1]);

					}
					pointcolor = GetPixel(WBM->GetHDC(), i-interval, (toolbarON)?j+toolBarHeight:j);
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));

					glVertex3f(slice[0][0]+slice[0][0]*section[j][0],   slice[0][1] + slice[0][1]*section [j][0] , section[j][1]);
					glVertex3f(slice[0][0]+slice[0][0]*section[j+interval][0],   slice[0][1] + slice[0][1]*section [j+interval][0] , section[j+interval][1]);

				glEnd();
			}

			glBegin(GL_TRIANGLE_STRIP);
				glVertex3f(slice[0][0]+slice[0][0]*section[j][0],   slice[0][1] + slice[0][1]*section [j][0] , section[j][1]);
				glVertex3f(slice[0][0]+slice[0][0]*section[0][0],   slice[0][1] + slice[0][1]*section [0][0] , section[0][1]);
				for(i=interval; i<CX_2D; i+= interval)
				{
					pointcolor = GetPixel(WBM->GetHDC(), i-interval, (toolbarON)?j+toolBarHeight:j);
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));

					glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);
					glVertex3f(slice[i][0]+slice[i][0]*section[0][0],   slice[i][1] + slice[i][1]*section [0][0] , section[0][1]);
				}
				pointcolor = GetPixel(WBM->GetHDC(), i-interval, (toolbarON)?j+toolBarHeight:j);
				glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));

				glVertex3f(slice[0][0]+slice[0][0]*section[j][0],   slice[0][1] + slice[0][1]*section [j][0] , section[j][1]);
				glVertex3f(slice[0][0]+slice[0][0]*section[0][0],   slice[0][1] + slice[0][1]*section [0][0] , section[0][1]);

			glEnd();
			glDisable(GL_COLOR_MATERIAL);
			
			
			
/*			
			glDisable(GL_LIGHTING);
			glShadeModel(GL_FLAT);
			for (j=0; j<CY_2D-interval-1; j+=interval)
			{
				glBegin(GL_TRIANGLE_STRIP);
				for (i=0; i<CX_2D; i+=interval)
				{
					pointcolor = GetPixel(WBM->GetHDC(), i, (toolbarON)?j+toolBarHeight:j);
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
					height = 1.0f + GraphHeight(i,j);
					glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);

					assert(j+interval <CY_2D);
					pointcolor = GetPixel(WBM->GetHDC(), i, interval+ ((toolbarON)?j+toolBarHeight:j));
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
					height = 1.0f + GraphHeight(i,j+interval);
					glVertex3f(slice[i][0]+slice[i][0]*section[j+interval][0],   slice[i][1] + slice[i][1]*section [j+interval][0] , section[j+interval][1]);


				}
				glEnd();
			}
*/

			break;
		case SMOOTH:
			//for now make the torus into a static unlit, multicolor one,
			//because it's expensive to do otherwise (e.g., normals)
			glDisable(GL_LIGHTING);
			glShadeModel(GL_SMOOTH);
			glEnable (GL_COLOR_MATERIAL);  //to use glColor() as a material
			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
			for(j=0; j<CY_2D-interval;j+= interval)
			{
				glBegin(GL_TRIANGLE_STRIP);

					for(i=0;i<CX_2D; i+= interval)
					{
						//find the color
						pointcolor = GetPixel(WBM->GetHDC(), i, (toolbarON)?j+toolBarHeight:j);
						glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
						glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);

						pointcolor = GetPixel(WBM->GetHDC(), i, interval+((toolbarON)?j+toolBarHeight:j));
						glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
						glVertex3f(slice[i][0]+slice[i][0]*section[j+interval][0],   slice[i][1] + slice[i][1]*section [j+interval][0] , section[j+interval][1]);
					}
					pointcolor = GetPixel(WBM->GetHDC(), 0, (toolbarON)?j+toolBarHeight:j);
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
					glVertex3f(slice[0][0]+slice[0][0]*section[j][0],   slice[0][1] + slice[0][1]*section [j][0] , section[j][1]);

					pointcolor = GetPixel(WBM->GetHDC(), 0, interval+((toolbarON)?j+toolBarHeight:j));
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
					glVertex3f(slice[0][0]+slice[0][0]*section[j+interval][0],   slice[0][1] + slice[0][1]*section [j+interval][0] , section[j+interval][1]);

				glEnd();
			}
			glBegin(GL_TRIANGLE_STRIP);
			for (i=0; i<CX_2D; i+= interval)
			{
				pointcolor = GetPixel(WBM->GetHDC(), i, (toolbarON)?j+toolBarHeight:j);
				glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
				glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);

				pointcolor = GetPixel(WBM->GetHDC(), i,((toolbarON)?toolBarHeight:0));
				glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
				glVertex3f(slice[i][0]+slice[i][0]*section[0][0],   slice[i][1] + slice[i][1]*section [0][0] , section[0][1]);
			}
			pointcolor = GetPixel(WBM->GetHDC(), 0, (toolbarON)?j+toolBarHeight:j);
			glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
			glVertex3f(slice[0][0]+slice[0][0]*section[j][0],   slice[0][1] + slice[0][1]*section [j][0] , section[j][1]);

			pointcolor = GetPixel(WBM->GetHDC(), 0,((toolbarON)?toolBarHeight:0));
			glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
			glVertex3f(slice[0][0]+slice[0][0]*section[0][0],   slice[0][1] + slice[0][1]*section [0][0] , section[0][1]);

			glEnd();
/*			
			i=0;
			for(j=0; j<CY_2D-interval;j+= interval)
			{
				glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);

					glVertex3f(slice[i][0]+slice[i][0]*section[j+interval][0],   slice[i][1] + slice[i][1]*section [j+interval][0] , section[j+interval][1]);

					for(i=interval;i<CX_2D; i+= interval)
					{
						//find the color
						pointcolor = GetPixel(WBM->GetHDC(), i-interval, (toolbarON)?j+toolBarHeight:j);
						glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));

						glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);
	
						glVertex3f(slice[i][0]+slice[i][0]*section[j+interval][0],   slice[i][1] + slice[i][1]*section [j+interval][0] , section[j+interval][1]);

					}
					pointcolor = GetPixel(WBM->GetHDC(), i-interval, (toolbarON)?j+toolBarHeight:j);
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));

					glVertex3f(slice[0][0]+slice[0][0]*section[j][0],   slice[0][1] + slice[0][1]*section [j][0] , section[j][1]);
					glVertex3f(slice[0][0]+slice[0][0]*section[j+interval][0],   slice[0][1] + slice[0][1]*section [j+interval][0] , section[j+interval][1]);

				glEnd();
			}

			glBegin(GL_TRIANGLE_STRIP);
				glVertex3f(slice[0][0]+slice[0][0]*section[j][0],   slice[0][1] + slice[0][1]*section [j][0] , section[j][1]);
				glVertex3f(slice[0][0]+slice[0][0]*section[0][0],   slice[0][1] + slice[0][1]*section [0][0] , section[0][1]);
				for(i=interval; i<CX_2D; i+= interval)
				{
					pointcolor = GetPixel(WBM->GetHDC(), i-interval, (toolbarON)?j+toolBarHeight:j);
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));

					glVertex3f(slice[i][0]+slice[i][0]*section[j][0],   slice[i][1] + slice[i][1]*section [j][0] , section[j][1]);
					glVertex3f(slice[i][0]+slice[i][0]*section[0][0],   slice[i][1] + slice[i][1]*section [0][0] , section[0][1]);
				}
				pointcolor = GetPixel(WBM->GetHDC(), i-interval, (toolbarON)?j+toolBarHeight:j);
				glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));

				glVertex3f(slice[0][0]+slice[0][0]*section[j][0],   slice[0][1] + slice[0][1]*section [j][0] , section[j][1]);
				glVertex3f(slice[0][0]+slice[0][0]*section[0][0],   slice[0][1] + slice[0][1]*section [0][0] , section[0][1]);

			glEnd();
*/
			glDisable(GL_COLOR_MATERIAL);

		
/*
			glDisable(GL_LIGHTING);
			glShadeModel(GL_SMOOTH);
			for (j=0; j<CY_2D-interval-1; j+=interval)
			{
				glBegin(GL_TRIANGLE_STRIP);
				for (i=0; i<CX_2D; i+=interval)
				{
					pointcolor = GetPixel(WBM->GetHDC(), i, (toolbarON)?j+toolBarHeight:j);
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
					height = 1.0f + GraphHeight(i,j);
					glVertex3f(slice[i][0]+slice[i][0]*section[j][0]*height,   slice[i][1] + slice[i][1]*section [j][0]*height , section[j][1]*height);

					assert(j+interval <CY_2D);
					pointcolor = GetPixel(WBM->GetHDC(), i, interval+ ((toolbarON)?j+toolBarHeight:j));
					glColor3ub(GetRValue(pointcolor), GetGValue(pointcolor), GetBValue(pointcolor));
					height = 1.0f + GraphHeight(i,j+interval);
					glVertex3f(slice[i][0]+slice[i][0]*section[j+interval][0]*height,   slice[i][1] + slice[i][1]*section [j+interval][0]*height , section[j+interval][1]*height);


				}
				glEnd();
*/
		break;
		}
	break;
	}      
}


void CapowGL::Size(HWND hWnd)  
/*should be called upon startup, and whenever the window
changes size
*/
{
	GLdouble gldAspect;
	GLsizei glnWidth, glnHeight;
	RECT clientrect;
    // Redefine the viewing volume and viewport
    // when the window size changes.

    // Make the RC current since we're going to
    // make an OpenGL call here...
    HDC hDC = GetDC (hWnd);
    wglMakeCurrent (hDC, hRC);
        
    // get the new size of the client window
    // note that we size according to the height,
    // not the smaller of the height or width.
	GetClientRect(hWnd, &clientrect);
    glnWidth = (GLsizei) clientrect.right;

    glnHeight = (GLsizei) clientrect.bottom;
	if (toolbarON)
		glnHeight-= toolBarHeight;
	if (statusON)
		glnHeight-= statusBarHeight;

    gldAspect = (GLdouble)glnWidth/(GLdouble)glnHeight;

    // set up a projection matrix to fill the
    //  client window
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    // a perspective-view matrix...
	if (mousemode != 6) //Not flying
		gluPerspective(30.0,   // Field-of-view angle
                gldAspect, // Aspect ratio of view volume
				/* Mike had 0.3 and 7.0 for the near and far clipping planes,
				which was OK with the small world, but which is clipping off the
				view in the big world.*/
                .1,    // Distance to near clipping plane
                70.0 ); // Distance to far clipping plane
	else //In the mouse flying mode
		gluPerspective(30.0, gldAspect, .1, 2.5);
	//origin is the lowerleft corner, and positive y is upward, positive x is rightward
    glViewport( 0, (statusON)?statusBarHeight:0, glnWidth, glnHeight );
	glMatrixMode(GL_MODELVIEW);
    wglMakeCurrent( NULL, NULL );
    ReleaseDC( hWnd, hDC );
	tempwidth = glnWidth; tempheight = glnHeight;  //store info for ShowStats()

}

inline GLfloat CapowGL::GraphHeight(int i,int j)
/*returns the graphheight of a cell in a CA.
The variable heightfactor scales the actual intensity value so that the CA
will have a reasonable scale of height upon rendering.
*/
{
/* Changed this so that the 2D rules can either show intensity or variable[1]. 
Recall that "intensity" is #define in CA.HPP to stand for "variable[0]". RR 11/98*/
	if (!(graphfocus->showvelocity))
		return (GLfloat) (graphfocus->wave_source_plane[graphfocus->index(i,j)].
			intensity*heightfactor);
	else
		return (GLfloat) (graphfocus->wave_source_plane[graphfocus->index(i,j)].
			variable[1]*heightfactor);
}

inline GLfloat CapowGL::GraphHeight(int i)  
/* same as previous function, except it uses a one dimensional index
Is it faster? I don't know.
*/
{
/* Changed this so that the 2D rules can either show intensity or variable[1]. 
Recall that "intensity" is #define in CA.HPP to stand for "variable[0]". RR 11/98*/
	if (!(graphfocus->showvelocity))
		return (GLfloat) (graphfocus->wave_source_plane[i].
			intensity*heightfactor);
	else
		return (GLfloat) (graphfocus->wave_source_plane[i].
			variable[1]*heightfactor);
}

void CapowGL::SetUpLights()
/* The minimal implementation of OPENGL allows for 8 lights; 
I'm only using one for capowgl
*/
{
   GLfloat local_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    ::glLightModelfv(GL_LIGHT_MODEL_AMBIENT, local_ambient);
	::glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0f);   //light both sides of the CA's polygons
    GLfloat ambient0[] =  { .6f, .6f, .6f, 1.0f };
    GLfloat diffuse0[] =  { .8f, .8f, .8f, 1.0f };
    GLfloat specular0[] = { .8f, .8f, .8f, 1.0f };
    GLfloat position0[] = { 0.0f, 0.0f, 1.0f, 0.0f };  //directional

//These are just parameters for other lights, which seems unnecessary													   
/*  GLfloat ambient1[] =  { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat diffuse1[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specular1[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat position1[] = { 2.0f, 1.0f, -1.0f, 0.0f };

    GLfloat ambient2[] =  { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat diffuse2[] =  { 0.0f, 0.0f, 1.0f, 1.0f };
    GLfloat specular2[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    GLfloat position2[] = { -0.5f, -0.5f, -1.0f, 1.0f };

	GLfloat ambient3[] =  { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat diffuse3[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specular3[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat position3[] = { 2.0f, 0.5f, 0.5f, 0.0f };
*/

	if (lightsflag)
		::glEnable( GL_LIGHTING );
	else
		::glDisable(GL_LIGHTING);
    ::glEnable(GL_LIGHT0);  
    ::glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    ::glLightfv(GL_LIGHT0, GL_POSITION, position0);
    ::glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    ::glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
/*
    //::glEnable(GL_LIGHT1);
    ::glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
    ::glLightfv(GL_LIGHT1, GL_POSITION, position1);
    ::glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    ::glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	
    //::glEnable(GL_LIGHT2);  
    ::glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);
    ::glLightfv(GL_LIGHT2, GL_POSITION, position2);
    ::glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);
    ::glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);

    //::glEnable(GL_LIGHT3);  
    ::glLightfv(GL_LIGHT3, GL_AMBIENT, ambient3);
    ::glLightfv(GL_LIGHT3, GL_POSITION, position3);
    ::glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse3);
    ::glLightfv(GL_LIGHT3, GL_SPECULAR, specular3);
 */

}

//called by PickMaterial()
void CapowGL::SetMaterials(
	GLfloat ambr, GLfloat ambg, GLfloat ambb,
	GLfloat difr, GLfloat difg, GLfloat difb,
	GLfloat specr, GLfloat specg, GLfloat specb, GLfloat shine)
{
	float mat[4];
	matAmb[0]= mat[0] = ambr;
	matAmb[1]= mat[1] = ambg;
	matAmb[2]= mat[2] = ambb;
	mat[3] = 1.0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat);
	matDif[0] = mat[0] = difr;
	matDif[1] = mat[1] = difg;
	matDif[2] = mat[2] = difb;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat);
	matSpec[0] = mat[0] = specr;
	matSpec[1] = mat[1] = specg;
	matSpec[2] = mat[2] = specb;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);
	matShine = shine;
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine * 128.0);
}


void CapowGL::PickMaterial(int materialtype)
//borrowed from the teapot.c example in the OPENGL programming guide

{
	switch (materialtype)
	{
	case EMERALD:
		SetMaterials( 0.0215, 0.1745, 0.0215,
			0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633, 0.6);
		break;
	case JADE:
		SetMaterials( 0.135, 0.2225, 0.1575,
		0.54, 0.89, 0.63, 0.316228, 0.316228, 0.316228, 0.1);
		break;
	case OBSIDIAN:
		SetMaterials( 0.05375, 0.05, 0.06625,
		0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 0.3);
		break;
	case PEARL:
		SetMaterials( 0.25, 0.20725, 0.20725,
			1, 0.829, 0.829, 0.296648, 0.296648, 0.296648, 0.088);
		break;
	case RUBY:
		SetMaterials( 0.1745, 0.01175, 0.01175,
			0.61424, 0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 0.6);
		break;
	case TURQUOISE:
		SetMaterials( 0.1, 0.18725, 0.1745,
			0.396, 0.74151, 0.69102, 0.297254, 0.30829, 0.306678, 0.1);
		break; 
	case BRASS:
		SetMaterials( 0.329412, 0.223529, 0.027451,
			0.780392, 0.568627, 0.113725, 0.992157, 0.941176, 0.807843,
			0.21794872);
		break;
	case BRONZE:
		SetMaterials( 0.2125, 0.1275, 0.054,
			0.714, 0.4284, 0.18144, 0.393548, 0.271906, 0.166721, 0.2);
		break;
	case CHROME:
		SetMaterials( 0.25, 0.25, 0.25,
			0.4, 0.4, 0.4, 0.774597, 0.774597, 0.774597, 0.6);
		break;
	case COPPER:
		SetMaterials( 0.19125, 0.0735, 0.0225,
			0.7038, 0.27048, 0.0828, 0.256777, 0.137622, 0.086014, 0.1);
		break;
	case GOLD:
		SetMaterials( 0.24725, 0.1995, 0.0745,
			0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 0.4);
		break;
	case SILVER:
		SetMaterials( 0.19225, 0.19225, 0.19225,
			0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 0.4);
		break;
	case BLACK_PLASTIC:
		SetMaterials( 0.0, 0.0, 0.0, 0.01, 0.01, 0.01,
			0.50, 0.50, 0.50, .25);
		break;
	case CYAN_PLASTIC:
		SetMaterials( 0.0, 0.1, 0.06, 0.0, 0.50980392, 0.50980392,
			0.50196078, 0.50196078, 0.50196078, .25);
		break;
	case GREEN_PLASTIC:
		SetMaterials( 0.0, 0.0, 0.0,
			0.1, 0.35, 0.1, 0.45, 0.55, 0.45, .25);
		break;
	case RED_PLASTIC:
		SetMaterials( 0.0, 0.0, 0.0, 0.5, 0.0, 0.0,
			0.7, 0.6, 0.6, .25);
		break;
	case WHITE_PLASTIC:
		SetMaterials( 0.0, 0.0, 0.0, 0.55, 0.55, 0.55,
			0.70, 0.70, 0.70, .25);
		break;
	case YELLOW_PLASTIC:
		SetMaterials( 0.0, 0.0, 0.0, 0.5, 0.5, 0.0,
			0.60, 0.60, 0.50, .25);
		break;
	case BLACK_RUBBER:
		SetMaterials( 0.02, 0.02, 0.02, 0.01, 0.01, 0.01,
			0.4, 0.4, 0.4, .078125);
		break;
	case CYAN_RUBBER:
		SetMaterials( 0.0, 0.05, 0.05, 0.4, 0.5, 0.5,
			0.04, 0.7, 0.7, .078125);
		break;
	case GREEN_RUBBER:
		SetMaterials( 0.0, 0.05, 0.0, 0.4, 0.5, 0.4,
			0.04, 0.7, 0.04, .078125);
		break;
	case RED_RUBBER:
		SetMaterials( 0.05, 0.0, 0.0, 0.5, 0.4, 0.4,
			0.7, 0.04, 0.04, .078125);
		break;
	case WHITE_RUBBER:
		SetMaterials( 0.05, 0.05, 0.05, 0.5, 0.5, 0.5,
			0.7, 0.7, 0.7, .078125);
		break;
	case YELLOW_RUBBER:
		SetMaterials( 0.05, 0.05, 0.0, 0.5, 0.5, 0.4,
			0.7, 0.7, 0.04, .078125);
		break;
	}
}

void CapowGL::ComputeNormals()
//computes the normals for smooth shading, and stores them in normals[]
{
	//the array "normals[]" stores a normal vector for
	//each vertex in the graph. In order for the SMOOTH surface
	//to work, each normal vector has to be average of the 
	//unit normals of the triangles that touch that vertex.
	//However, this function doesn't unitvectorize the final normals
	//We let OpenGL take care of this by calling glEnable(GL_NORMALIZE);

/* This function looks pretty ugly, but I'm doing it this way for efficiency.
To be specific, in reality the smoothed normal of a cell's vertex is the average of the
the normals of the 6 triangles that share that vertex.  But the normals 
of those 6 triangles are shared by the adjacent cells.  My function needs
to calculate a normal for each triangle ONLY once, and then adds them into the
three associated vectors in the normals[] array.  Also, I don't need a
separate loop to initialize the entire array.
*/


	GLfloat n1[3];  //the unit normal for a triangle
	int location;
	int x,y;
	GLfloat d;
	for (x=0; x<CX_2D; x++)  //clear the first row of normals
	{
		normals[x][0] = 0.0f;
		normals[x][1] = 0.0f;
		normals[x][2] = 0.0f;
	}
	//what is being done here is that we look at each triangle associated
	//graph, and add its normal to the three normals associated with 
	//those vertices in the "normals" array.  After that, we need
	//to make each vector in the array a unit vector.
	for(y = 0; y<CY_2D-1; y++)  //for each row of vertices, except for the last row,
	{
		location = (y+1)*CX_2D;  //set the normal for the left most vertex to zero
		normals[location][0] = 0.0f;
		normals[location][1] = 0.0f;
		normals[location][2] = 0.0f;
		for (x=0; x<CX_2D-1; x++)// go across the row, except for the last vertex in the row
		{
			NormalVector(x,y,UPPERLEFT,n1);  //find the normal for the upper left triangle

			//unitvectorize it:
			d = (n1[0]* n1[0] + n1[1] * n1[1] + n1[2]*n1[2]);
			//try to catch small vectors
			if (d< (GLdouble) 0.00000001)
				//error, near zero length vector
				d = (GLdouble) 100000000.0;
			else
				d= (GLdouble) 1.0/sqrt(d);
			n1[0] *= d;
			n1[1] *= d;
			n1[2] *= d;
			
			location = x + y*CX_2D;
			normals[location][0] += n1[0];
			normals[location][1] += n1[1];
			normals[location][2] += n1[2];
			// no more normals will be added to this normals[location]

			//now add n1 to the other two normals associated with the upperleft triangle 
			location++;						//  x,y --*
			normals[location][0] += n1[0];	//	|    /
			normals[location][1] += n1[1];	//	|  /
			normals[location][2] += n1[2];	//	|/
								
			location+=CX_2D-1;				//  x,y --
			normals[location][0] += n1[0];	//  |    /
			normals[location][1] += n1[1];	//	|  /
			normals[location][2] += n1[2];	//	|/
											//  *
			//now look at the lower left triangle
			NormalVector(x,y,LOWERRIGHT,n1);
			//unitvectorize:
			d = (n1[0]* n1[0] + n1[1] * n1[1] + n1[2]*n1[2]);
			//try to catch small vectors
			if (d< (GLdouble) 0.00000001)
				//error, near zero length vector
				d = (GLdouble) 100000000.0;
			else
				d= (GLdouble) 1.0/sqrt(d);
			n1[0] *= d;
			n1[1] *= d;
			n1[2] *= d;
											//  x,y	   
			normals[location][0]+=n1[0];	//	     /|
			normals[location][1]+=n1[1];	//	   /  |
			normals[location][2]+=n1[2];	//	 /	  |
											//  *-----
			location ++;
			normals[location][0]=n1[0];   //these are not += because they are seeing that
			normals[location][1]=n1[1];	  //location for the first time, which must be initialized.
			normals[location][2]=n1[2];		//  x,y	  
											//       /|
											//	   /  |
											//   /	  |
											//  ------*

			location -= CX_2D;				//  x,y   *
			normals[location][0]+=n1[0];	//       /|
			normals[location][1]+=n1[1];	//     /  |
			normals[location][2]+=n1[2];	//   /	  |
											//  ------
		}
	}

}



void CapowGL::NormalVector(int i,int j,int half, GLfloat n[3])
//10-97 mike: this function is not really used; see the next funcion
//which can handle triangles larger than one cell unit


/*this function calculates the normal of a triangle associated 
with a point in the the mesh. Looking down on the x-y plane 
(right handed system), x increase to the right, and y increases downward.

	i,j------------i+1,j------------i+2,j-----....
	|				/				 /|
	|	upper	   /|			   /  |
	|	half	 /	|			  /	  |
	|			/	|		    /	  |
	|		  /		|		  /		  |
	|		 /		|		/		  |
	|	   /  lower	|	  /			  |
	|	 /	  half	|	/			  |
	|	/			| /				  |
	i,j+1----------i+1,j+1----------i+2,j+2


when i,j, and the half is passed to the function, 
the normal vector for the triangle specified will be calculated
and copied into n[]. The vector is a unit vector, 
and should point "out" of the screen, or CA.
*/

{

	GLdouble d;
	if (i<0 ||i >= CX_2D||j<0||j>=CY_2D)
		return;
	switch(half)
	{
	case UPPERLEFT:
		n[0] = GraphHeight(i,j) - GraphHeight(i+1,j);
		n[1] = GraphHeight(i, j+1)- GraphHeight(i,j);
		n[2] = 1;
		break;
	case LOWERRIGHT:
		n[0] = GraphHeight(i, j+1) - GraphHeight(i+1,j+1);
		n[1] = GraphHeight(i+1,j+1) - GraphHeight(i+1, j);
		n[2] = 1;
		break;
	}
/*
	//unitvectorize

	d = (n[0]* n[0] + n[1] * n[1] + n[2]*n[2]);

	//try to catch small vectors
	if (d< (GLdouble) 0.00000001)
	{
		//error, near zero length vector
		// 
		d = (GLdouble) 100000000.0;
	}
	else
	{
		d= (GLdouble) 1.0/sqrt(d);
	}

	n[0] *= d;
	n[1] *= d;
	n[2] *= d;
*/
}

void CapowGL::NormalVector(int i,int j,int half, GLfloat n[3], int interval)
//same as the preceding function, except it can use triangles which are more 
//than one unit in width
{
	GLdouble d;
	if (interval == 0)
		return;
	GLfloat factor = 1.0f/interval;
	if (i<0 ||i >= CX_2D-interval||j<0||j>=CY_2D-interval)
		return;
	switch(half)
	{
	case UPPERLEFT:
		n[0] = (GraphHeight(i,j) - GraphHeight(i+interval,j))*factor;
		n[1] = (GraphHeight(i, j+interval)- GraphHeight(i,j))*factor;
		n[2] = 1;
		break;
	case LOWERRIGHT:
		n[0] = (GraphHeight(i, j+interval) - GraphHeight(i+interval,j+interval))*factor;
		n[1] = (GraphHeight(i+interval,j+interval) - GraphHeight(i+interval, j))*factor;
		n[2] = 1;
		break;
	}

/*10-97 commented out because I started to use glEnable(GL_NORMALIZE).  
This function is used in two places, really- computing the normals for
FACETS mode, and for computing the normals in ComputeNormals(). 
Consequently, the below code is not needed for FACETS part, 'cause 
OpenGL does it.  However in ComputeNormals, unitvectorization has 
to be done, because I'm looking for the equally weighted sum of several
vectors.
*/

/*
	//normalize
	d = (n[0]* n[0] + n[1] * n[1] + n[2]*n[2]);

	//try to catch small vectors
	if (d< (GLdouble) 0.00000001)
	{
		//error, near zero length vector
		// 
		d = (GLdouble) 100000000.0;
	}
	else
	{
		d= (GLdouble) 1.0/sqrt(d);
	}

	n[0] *= d;
	n[1] *= d;
	n[2] *= d;
*/
}


void CapowGL::MouseMove(int x, int y, UINT flags)
/*interpret mouse drag */
{
	if (gotLButtonDown)
	{	
		switch(mousemode)
		{
		case 0:  //CA orientation
			spinangle = oldspinangle + (float)(x-x1)/2.0;
			tiltangle = oldtiltangle + (float)(y-y1)/3.0f;
			break;
		case 1:  //light position
			lightyaw = oldlightyaw + (float) (x-x1)/2.0;
			if (lightyaw > 180.0f)
				lightyaw = 180.0f;
			else if (lightyaw <-180.0f)
				lightyaw = -180.0f;
			lightpitch = oldlightpitch + (float) (y-y1)/3.0f;
			if (lightpitch > 90.0f)
				lightpitch = 90.0f;
			else if(lightpitch <-90.0f)
				lightpitch = -90.0f;
			break;
		case 2:
			if (ThreeDGlasses())
				break;
			panY = oldpanY + (float)-(y-y1)/10.0f;
			panX = oldpanX + (float) (x-x1)/10.0f;
			break;
		case 3:
			pointerx = oldpointerx + (float)(x-x1)/2;
			pointery = oldpointery + (float)(y-y1)/2;
			if (pointerx<0)
				{ pointerx = 0;}
			else if (pointerx >= CX_2D-1)
				{pointerx = CX_2D-1;}
			if (pointery <0)
				{ pointery = 0;}
			else if (pointery >= CY_2D-1)
				{ pointery = CY_2D-1;}
			break;
		case 4:
//			if (ThreeDGlasses())
//				break;
			z = oldzoom + (float) -(y-y1);
			if (z<MINZ)
				z=MINZ;
			else if (z>MAXZ)
				z=MAXZ;
			panX = oldpanX + (float) (x-x1)/10.0f;			
			break;
		case 5:
			heightfactor = oldheightfactor + (float)-(y-y1)/(4.0*(graphfocus->_max_intensity.Val()+.01));
			if (heightfactor <0.0f)
				heightfactor = 0.0f;
			//to take care of the bug that occurs when the height is being changed
			//for a paused 2D smooth graph, and forgets to recompute normals.
			if (surfacetype == SMOOTH && material != UNLIGHTED_MULTICOLOR && !focusIsActive)  
				ComputeNormals();
			break;
		}
	}
	else  //lbutton is up
	{
		if (mousemode == 6) //fly
		{
			dtheta = (float)(tempwidth/2 - x)/tempwidth;
			dflyheight = (float)(tempheight/2 - y + ((toolbarON)?toolBarHeight:0) ) /tempheight;
		}
	}
}


void CapowGL::LeftButtonDown(BOOL fDoubleClick, int x, int y, UINT flags)
/* save info for ensuing mousedrag */
{
	if (graphtype==FLATCOLOR)
		return;
	gotLButtonDown = TRUE;
	x1 = x;
	y1= y;
	switch (mousemode)
	{
	case 0:   //CA orientation
		oldspinangle = spinangle;
		oldtiltangle = tiltangle;
		break;
	case 1:		//light position
		oldlightyaw = lightyaw;
		oldlightpitch = lightpitch;
		break;
	case 2:
		oldpanY= panY;
		oldpanX= panX;
		break;
	case 3:
		oldpointerx = pointerx;
		oldpointery = pointery;
		break;
	case 4:
		oldzoom = z;
		oldpanX = panX;
		break;
	case 5:
		oldheightfactor = heightfactor;
		break;
	case 6:  //fly mode
		flygo = !flygo;   //clicking will alternate between freezing and moving
		break;
	}		
}

void CapowGL::LeftButtonUp(int x, int y, UINT flags)
{
	gotLButtonDown = FALSE;
}


void CapowGL::ZoomIn()  //not really used
{
	z+= ZOOMSTEP;
	if (z>MAXZ)
		z=MAXZ;
}
void CapowGL::ZoomOut()  //not really used
{
	z-= ZOOMSTEP;
	if (z<MINZ)
		z=MINZ;
}


void CapowGL::Pan(int direction) //not really used
{
	switch(direction)
	{
	case 0:
		panY -= panstep;
		break;
	case 1:
		panY += panstep;
		break;
	case 2:
		panX -= panstep; 
		break;
	case 3:
		panX += panstep;
		break;
	}
}

void CapowGL::Reset()
//reset to default settings
{
	Type(OPENGL_DEFAULTGRAPHTYPE);
	surfacetype = DEFAULTSURFACETYPE;
	material =		DEFAULTMATERIAL;

	
//	heightfactor =	DEFAULTHEIGHTFACTOR;
	spinflag =		DEFAULTSPIN;
	spindelta =		DEFAULTSPINDELTA;
	spinangle =		DEFAULTSPINANGLE;
	z =				DEFAULTZ;
	tiltangle =		DEFAULTTILT;
	lightsflag =	DEFAULTLIGHTS;
	spacing =		DEFAULTSPACING;
	gotLButtonDown = FALSE;
	panY =			DEFAULTPANY;
	panX =			DEFAULTPANX;
	panstep =		DEFAULTPANSTEP;
	lightyaw =		DEFAULTLIGHTYAW;
	lightpitch =	DEFAULTLIGHTPITCH;
	mousemode =		DEFAULTMOUSEMODE;
	zeroplaneflag = DEFAULTZEROPLANEFLAG;
	maxplaneflag=	DEFAULTMAXPLANEFLAG;
	pointerx =		DEFAULTPOINTERX;
	pointery =		DEFAULTPOINTERY;
	pointerflag =	DEFAULTPOINTERFLAG;
	showgeneratorsflag = DEFAULTSHOWGENERATORSFLAG;
	AdjustHeightFactor(graphfocus);
	Resolution (DEFAULTRESOLUTION);

}

void CapowGL::Type(int newtype) 
{
	if (ThreeDGlasses() && newtype!= FLATCOLOR)
		newtype = SHEET;
	graphtype = newtype;
}

void CapowGL::SurfaceType(int newtype)
{
	if (newtype == SMOOTH)
		ComputeNormals();
	surfacetype = newtype;
}

#define HEIGHT_FACTOR 30

void CapowGL::AdjustHeightFactor(CA *focus)
{
/* I think that the z field of the CapowGL object is its vertical height?  So
I'd like to adjust heightfactor so that the max point height is something times z.
Now the max point height seems to be heightfactor*focus->MaxIntensity().  So
z * Z_FACTOR = heightfactor*focus->MaxIntensity(),
so divide through to get the value for heightfactor.  I have found the value
of Z_FACTOR simply by hit and  miss.  I'm unclear what the spacing has to do
with it; it does get mulitplied into the height.  Other weird thing: z
is negative, so we have to correct for that with a fabs.  Rudy, May 22, 1997.*/

/*Mike 10-97: no, actually z is the zoom, or the translation of the viewer from the CA,
z is the distance along the z axis, and is limited by MINZ and MAXZ. In this function, I'm just
trying to make the height of the CA reasonable, using heightfactor.  So I've changed
Z_FACTOR to HEIGHT_FACTOR
*/
	Real divisor = fabs(focus->MaxIntensity()); //This isn't ever negative, but I'm paranoid.
	if (divisor < SMALL_REAL)
		divisor = SMALL_REAL;
	heightfactor = fabs(HEIGHT_FACTOR / divisor); 
	//z is negative, so need the fabs, otherwise the picture's upside down.
}

void CapowGL::DrawStats(HDC hdc)
//for diagnostic purposes only
{
	static float accumulatedTime;
	static float oldperiod, oldrenderperiod;

	static char buffer[64];
	int lineheight = 16;
	int x = 0, y=0;
	if (toolbarON)
		y+=toolBarHeight;

	strcpy(buffer, "test123");
	TextOut(hdc, x, y, buffer, strlen(buffer));

	//show the width and height of viewport
	y+= lineheight;
	_itoa (tempwidth, buffer, 10);
	TextOut(hdc, x,y, buffer, strlen(buffer));
	y+= lineheight;
	_itoa (tempheight, buffer, 10);
	TextOut(hdc, x,y, buffer, strlen(buffer));

	y+= lineheight;
	_itoa(cellx, buffer, 10);
	TextOut(hdc, x,y, buffer, strlen(buffer));

	y+= lineheight;
	_itoa (celly, buffer, 10);
	TextOut(hdc, x,y, buffer, strlen(buffer));

	//Show the framerate
	y+= lineheight;
	if (period > 0)
	{
		if (accumulatedTime < 1000)
		{
			accumulatedTime+=period;

		}
		else
		{
			accumulatedTime = 0;
			oldperiod = period;
			oldrenderperiod = renderperiod;
		}
		_gcvt((1000.0f/oldperiod), 4, buffer);
		TextOut(hdc, x,y,buffer, strlen(buffer));

		y+= lineheight; //show the percent of time spent rendering
		_gcvt(oldrenderperiod/oldperiod, 4, buffer);
		TextOut(hdc, x,y, buffer, strlen(buffer));
	}
}


void CapowGL::UpdateFly()
//move the fly eye; it is dependent on the framerate, or period of the program
{
	float factor = .01f*(period);
	if (flygo)
	{
		theta += dtheta*factor*.2;
		flyDir[0] = cos(theta);
		flyDir[1] = sin(theta);
		flyDir[2] = dflyheight;
	}
	if (flygo)
	{
		flyEye[0] += flyDir[0]*factor;
		flyEye[1] += flyDir[1]*factor;
		flyEye[2] += dflyheight*factor;
	}
	//if eye position goes off the CA, wrap to the other side of the CA
	if (flyEye[0] >= CX_2D)
		flyEye[0] = 0.0f;
	else if (flyEye[0] <=0.0f)
		flyEye[0] = CX_2D;
	if (flyEye[1] <= -CY_2D)
		flyEye[1] = 0.0f;
	else if (flyEye[1] >= 0.0f)
		flyEye[1] = -CY_2D;

	//limit the possible height of the eye
	if (flyEye[2] >(float)graphfocus->_max_intensity.Val()*heightfactor + .3f) 
		flyEye[2] =(float)graphfocus->_max_intensity.Val()*heightfactor;
	else if (flyEye[2] < -(float)graphfocus->_max_intensity.Val()*heightfactor)
		flyEye[2] = - (float)graphfocus->_max_intensity.Val()*heightfactor;
}

void CapowGL::SetUpTorus()
/* Here, I could have allocated a CX_2D*CY_2D sized array of 3D points,
but to conserve memory allocation I use two arrays of size CX_2D and CY_2D, storing
2-D points. Conceptually, if we slice the torus up like pie, and look at one thin slice,
the array slice[] stores the location of the center of that slice on a 2-D plane, with
the planes origin located at the center of the torus. The array section[] describes
the face of that slice, like a cross section.
To compute the location of a point on a torus requires some adding and 
multiplying between the two arrays, which would make it a little slower 
than using the larger array of 3-D points as lookup table.
*/
{
	int x, y;
	float pi = 3.14159265f;
	float radius1 = 40.0f;
	float radius2 = 20.0f;

	for (x = 0; x<CX_2D+1; x++)   //go around the torus
	{
		slice[x][0] = radius1*cos((float)x*2*pi/CX_2D);
		slice[x][1] = radius1*sin((float)x*2*pi/CX_2D);
	}
	for (y = 0; y<CY_2D+1; y++)
	{
		section[y][0] = radius2*cos((float)y*2*pi/CY_2D)/radius1;
		section[y][1] = radius2*sin((float)y*2*pi/CY_2D);
	}
}



void CapowGL::MouseMode(int mode)
{	
	if (mousemode != mode)
	{
		if (ThreeDGlasses() && mode !=0 && mode !=5 && mode != 4)
			mode = 0;
		mousemode = mode;
		if (masterhwnd)
			Size(masterhwnd);
	}

}

//not used
/*
void CapowGL::SetUpGrid()
{
	int i,j;
	for (j=0; j<CY_2D; j++)
	{
		for (i=0; i<CX_2D; i++)
		{
			grid[j*CX_2D+i][0] = (GLfloat) i;
			grid[j*CX_2D+i][1] = (GLfloat) -j;
			grid[j*CX_2D+i][2] = 0.0f;
		}
	}
}
*/

void CapowGL::Resolution(int value)
{
	switch (value)
	{
	case HIGHRES:
		interval = 1;   //look at all the cells
		resolution = value;
		break;
	case MEDIUMRES:
		interval = 2;  //look at 1/4 of the cells
		resolution = value;
		break;
	case LOWRES:
		interval = 4;  //look at 1/16 of the cells
		resolution = value;
		break;
	default:
		break;
	}	
}

//this function is incomplete
bool CapowGL::CaptureToVRML()
{
	char filename[_MAX_PATH];
	char titlename[_MAX_FNAME+_MAX_EXT];
	OPENFILENAME ofn;

	char *szFilter[] = {"VRML files (*.wrl)", "*.wrl",""};

	ofn.lStructSize		= sizeof (OPENFILENAME);
	ofn.hwndOwner		= masterhwnd;
	ofn.hInstance		= NULL;
	ofn.lpstrFilter		= szFilter[0];
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter		=  0;
	ofn.nFilterIndex	= 0;
	ofn.lpstrFile		= filename;
	ofn.nMaxFile		= _MAX_PATH;
	ofn.lpstrFileTitle	= titlename;
	ofn.nMaxFileTitle	= _MAX_FNAME + _MAX_EXT;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle		= "Capture to a VRML file";
	ofn.Flags			= OFN_OVERWRITEPROMPT;
	ofn.nFileOffset		= 0;
	ofn.nFileExtension	= 0;
	ofn.lpstrDefExt		= "wrl";
	ofn.lCustData		= 0L;
	ofn.lpfnHook		= NULL;
	ofn.lpTemplateName	= NULL;
	
	if (GetOpenFileName (&ofn))
	{
		
	}
	return true;
}



bool CapowGL::CaptureVRML(void)
{

	FILE *stream;
	int rows, columns;
	double d;
	bool success = FALSE;
	COLORREF pointcolor;
	GLfloat color[3];
GLenum error;
	if (graphfocus->viewmode==IDC_2D_VIEW)
	{
		rows = 0; columns  = 0;
		float gridspacing = 0.1f;
		int    i,j, i2 = 0;
		float* n;
		double fp = 1.5;
//		char   s[] = "this is a string";
//		char   c = '\n';

		stream = fopen( "capow.wrl", "w" );
		fprintf( stream, "#VRML V2.0 utf8\n\n");
		fprintf( stream, "#This file was created by Capow98 from CAMCOS, San Jose State University!\n\n");
		fprintf( stream, "NavigationInfo{\n");
		fprintf( stream, "  type [\"EXAMINE\",\"ANY\"]\n");
		fprintf( stream, "}\n");
		fprintf( stream, "Shape {\n");
		fprintf( stream, "  geometry ElevationGrid {\n");
		fprintf( stream, "    height [");		
		//now write the data
		for (j=0; j<CY_2D; j+= interval)
		{
			for(i=0; i<CX_2D;i+= interval)
			{
				
				fprintf(stream, " %.3f", GraphHeight(i,j)* gridspacing );
				if (i2>=8)
				{
					fprintf(stream, "\n");
					i2=0;
				}
				else i2++;
			}
		}
		//closing bracket
		fprintf(stream, "]\n");

		if (material <= UNLIGHTED_MULTICOLOR)
		{
			fprintf(stream, "    color Color{\n");
			fprintf(stream, "       color[");
			i2 = 0;
			if (surfacetype == FACETS)
			{
				for (j=0; j<CY_2D-interval; j+=interval)
				{
					for (i=0; i<CX_2D-interval; i+= interval)
					{
						pointcolor = GetPixel(WBM->GetHDC(), i,(toolbarON)?j+toolBarHeight:j);		
						fprintf(stream, " %.2f", (float)GetRValue(pointcolor)/255);
						fprintf(stream, " %.2f", (float)GetGValue(pointcolor)/255);
						fprintf(stream, " %.2f,", (float)GetBValue(pointcolor)/255);
						if (i2 >= 3)
						{
							fprintf(stream, "\n");
							i2=0;
						}
						else i2++;
					}
				}
			}
			else
			{
				for (j=0; j<CY_2D; j+=interval)
				{
					for (i=0; i<CX_2D; i+= interval)
					{
						pointcolor = GetPixel(WBM->GetHDC(), i,(toolbarON)?j+toolBarHeight:j);		
						fprintf(stream, " %.2f", (float)GetRValue(pointcolor)/255);
						fprintf(stream, " %.2f", (float)GetGValue(pointcolor)/255);
						fprintf(stream, " %.2f,", (float)GetBValue(pointcolor)/255);
						if (i2 >= 3)
						{
							fprintf(stream, "\n");
							i2=0;
						}
						else i2++;
					}
				}
			}

			fprintf(stream, "]\n");
			fprintf(stream, "      }\n");


		}

		if (surfacetype == SMOOTH)
			//if creaseAngle defines the minimum angle at which the renderer
			//will allow adjacent polygons to appear creased, rather than smoothed
			//6.0 radians is a very large angle, so every thing will be smoothed
			fprintf(stream, "    creaseAngle 3.0\n");
		else if (surfacetype==FACETS)  
			//color of a quad is the color of it's upper left cell
			fprintf(stream, "    colorPerVertex FALSE\n"); 
		fprintf(stream, "    solid FALSE\n"); //don't cull polygons

		/*the following lines fix the calculation of the 
		proper number of rows and columns, in the case
		where the interval does not divide evenly into CX_2D or CY_2D.
		In such a case, we add one.  For example, if interval is 4
		and CY_2D is 90, CY_2D/interval is 22. But there are actually
		23 rows of cells captured.
		*/
		columns = CX_2D/interval;
		if (columns*interval< CX_2D)
			columns++;
		rows = CY_2D/interval;
		if (rows*interval < CY_2D)
			rows++;

		fprintf(stream, "    xDimension %i\n", columns);
		fprintf(stream, "    zDimension %i\n", rows);
		fprintf(stream, "    xSpacing %f\n", gridspacing*interval);
		fprintf(stream, "    zSpacing %f\n", gridspacing*interval);
		fprintf(stream, "  }\n");

		//Material properties
		fprintf(stream, "  appearance Appearance {\n");
		switch(material)
		{
		case MULTICOLOR:
			fprintf(stream, "    material Material{ diffuseColor 1.0 1.0 1.0\n");
			fprintf(stream, "     ambientIntensity .3\n");
			fprintf(stream, "     specularColor .5 .5 .5\n");
			fprintf(stream, "     shininess .5\n");
			fprintf(stream, "    }\n");
			break;
		case UNLIGHTED_MULTICOLOR:
			break;
		default:  //non multicolor e.g. brass, bronze, pearl...
			PickMaterial(material);
			fprintf(stream, "    material Material{ diffuseColor %.3f %.3f %.3f\n", matDif[0], matDif[1], matDif[2]);
			if (matDif[0]>0.0001)  //watch for div by 0
				fprintf(stream, "     ambientIntensity %.3f\n", matAmb[0]/matDif[0]);
			fprintf(stream, "     specularColor %.3f %.3f %.3f\n", matSpec[0], matSpec[1], matSpec[2]);
			fprintf(stream, "     shininess %3f\n", matShine);
			fprintf(stream, "    }\n");
			break;

		}
		fprintf(stream, "  }\n");
		fprintf(stream, "}\n");

		//define the entry view
		fprintf(stream,"Transform {\n");
		fprintf(stream,"  rotation 0 1 0 .5\n");  
		fprintf(stream,"  translation 6 0 4.5\n");
		fprintf(stream,"  children [\n");
		fprintf(stream,"    Viewpoint{\n");
		fprintf(stream,"      orientation 1 0 0 -.5\n");
		fprintf(stream,"      position 0 7 13\n");
		fprintf(stream,"	description \"View 1\"\n");
		fprintf(stream,"    }\n");
		fprintf(stream,"  ]\n");
		fprintf(stream,"}\n"); 




		fclose( stream );
		success = true;
	}
	return success;
}

void CapowGL::ThreeDGlasses(BOOL flag)
{
	static int oldSurface;
	static int oldType;
	static bool oldAntiAliased;
	static int oldMaterial;

	if (flag == TRUE)
	{
		//remember settings
		oldSurface = SurfaceType();
		oldType = Type();
		oldAntiAliased = AntiAliased();
		oldMaterial = Material();
		switch (surfacetype)
		{
		case DOTS:
			break;
		case POLYLINES:
			break;
		case COLORDOTS:
			SurfaceType(DOTS);
			break;
		case COLORLINES:
			SurfaceType(POLYLINES);
			break;
		default:
			Material(PEARL);
//			SurfaceType(POLYLINES);
			break;
		}

		Type(SHEET);
		z =				DEFAULTZ;
		gotLButtonDown = FALSE;
		panY =			DEFAULTPANY;
		panX =			DEFAULTPANX;
		mousemode =		0;	//orient
		threeDGlasses = flag;
	}
	else
	{
		threeDGlasses = FALSE; //have to do this first or the following functions could fail.
		//set to the settings prior to 3D glasses mode
		SurfaceType (oldSurface);
		Type(oldType);
		Material(oldMaterial);
	}
}

