#ifndef CAPOWGL_HPP
#define CAPOWGL_HPP
/*******************************************************************************
	FILE:				CapowGL.hpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains definitions and data for
						the implementation of OpenGL 3D rendering.

	UPDATE LOG:			

*******************************************************************************/


#include "types.h"
#include "ca.hpp"
#include "bitmap.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
// #include <GL/glaux.h> 2017
#define STACKLIMIT 30 //For OpenGl ?

//--------------------CONSTANTS used by CAPOWGL.CPP
//shape
#define FLATCOLOR 0
#define SHEET 1
#define TORUS 2

//surface
#define DOTS 0
#define COLORDOTS 1
#define POLYLINES 2
#define COLORLINES 3
#define FACETS 4
#define SMOOTH 5

//materials
#define MULTICOLOR	0
#define UNLIGHTED_MULTICOLOR 1
#define EMERALD		2
#define JADE		3
#define OBSIDIAN	4
#define PEARL		5
#define RUBY		6
#define TURQUOISE	7
#define BRASS		8
#define BRONZE		9
#define CHROME		10
#define COPPER		11
#define GOLD		12
#define SILVER		13
#define BLACK_PLASTIC	14	
#define CYAN_PLASTIC	15
#define GREEN_PLASTIC	16
#define RED_PLASTIC		17
#define WHITE_PLASTIC	18
#define YELLOW_PLASTIC	19
#define BLACK_RUBBER	20
#define CYAN_RUBBER		21
#define GREEN_RUBBER	22
#define RED_RUBBER		23
#define WHITE_RUBBER	24
#define YELLOW_RUBBER	25

//resolution
#define HIGHRES 0
#define MEDIUMRES 1
#define LOWRES 2

#define ORIGINAL	0
#define FLY			1

//for the NormalVector() function
#define UPPERLEFT	1
#define LOWERRIGHT	2

//for 3-D glasses mode
#define LEFTEYE 0
#define RIGHTEYE 1

#define MAXZ		-100.0f  //As close as you can get to the graphic with the Zoom in the capowgl.cpp dialog code.
#define MINZ		-1000.0f //2011 As far as you can get back.  Was -800  and I made it bigger as my 2D CAs are bigger now.
#define DEFAULTZ	-800.0f  //was -800

#define ZOOMSTEP	20.0f

#define OPENGL_DEFAULTGRAPHTYPE		FLATCOLOR //RR. 2007. We had SHEET here for default 3D graph view.
						//I changed this to FLATCOLOR, as more often when I zoom I don't want 3D
#define DEFAULTSURFACETYPE	SMOOTH
#define DEFAULTHEIGHTFACTOR  .1f
#define DEFAULTSPIN			FALSE
#define DEFAULTSPINDELTA	1.5f  //degrees, counterclockwise
#define DEFAULTTILT			-60.0f  //0.0=directly overhead; positive=tilted forward
#define DEFAULTSPINANGLE	-45.0f
#define DEFAULTLIGHTS		TRUE
#define DEFAULTMATERIAL		MULTICOLOR
#define DEFAULTSPACING		.02
#define DEFAULTFOCUSISACTIVE TRUE
#define DEFAULTPANY	0.0f	
#define DEFAULTPANX 0.0f
#define DEFAULTPANSTEP		2.0f  //degrees
#define DEFAULTLIGHTYAW		0.0f
#define DEFAULTLIGHTPITCH	0.0f //degrees
#define DEFAULTMOUSEMODE	0
#define DEFAULTZEROPLANEFLAG TRUE
#define DEFAULTMAXPLANEFLAG TRUE
#define DEFAULTPOINTERX  CX_2D/2
#define DEFAULTPOINTERY CY_2D/2
#define DEFAULTPOINTERFLAG  FALSE
#define DEFAULTSHOWGENERATORSFLAG	FALSE
#define DEFAULTSTATS  FALSE /* Use TRUE for debugging to show 
	window dimensions, frame rate, percent of time spent in rendering,
	see DrawStats for more info.*/
#define DEFAULTRESOLUTION	MEDIUMRES
#define DEFAULTSHOWFLYPOS FALSE
#define DEFAULTANTIALIASEDFLAG TRUE
#define USETEXTURE
//========================================End of constants

class CapowGL
{
private:
	int graphtype;				//type of graph to display; 0 is the flat 2-D
	int surfacetype;			//type of surface, e.g. dots, lines, smooth
	int material;				//index for material type, like brass, bronze, etc

//	GLfloat grid[CX_2D*CY_2D][3];
	float heightfactor;		//scale factor for the height ca
	HGLRC hRC;				//handle for a rendering context
	CA* graphfocus;			//should point to the current focus ca
	BOOL spinflag;			//spin the CA
	float spindelta;		//rate of spin
	float spinangle;		//the current spin angle
	float tiltangle;		//the current tilt angle of the CA
	float z;				//zoom, the distance from the camera to the ca, increases negatively
	BOOL lightsflag;		// show lights?
	GLfloat normals[CX_2D*CY_2D][3];		//a normal vector associated with each cell
	float spacing;				//factor to scale the ca when displayed
	BOOL focusIsActive;			//focus is not sleeping
	BOOL gotLButtonDown;		//left button currently pressed
	int x1, y1, x2, y2;			//to remember mouse positions before and after dragging
	float oldspinangle, oldtiltangle;		//remember old angles
	float panY, panX;		//x and y position of camera
	float panstep;				//rate of panning in relation to mouse movement
	int mousemode;				//index for current purpose of mouse, e.g. panning, zooming
	float lightyaw, lightpitch, oldlightyaw, oldlightpitch, oldpanY, oldpanX; 
		//remember old positions.
	BOOL zeroplaneflag;		//show zero plane, which is a rectangle
	BOOL maxplaneflag;		//show max plane, which is a rectangle
	int pointerx, pointery, oldpointerx, oldpointery;   //points to a single cell using a line
	float oldzoom;			//remember old zoom position, which is a value of z
	float oldheightfactor;  //remember old heightfactor
	BOOL pointerflag;		//show pointer
	BOOL showgeneratorsflag;  //show generator locations with spheres (red)
	float flyEye[3], flyDir[3], flyUp[3];    //in fly mode, designates the eye position
	BOOL statsflag;				//show diagnostic info

//	GLfloat torus[CX_2D*CY_2D][3];
	GLfloat slice[CX_2D+1][2];		//for the torus
	GLfloat section[CY_2D+1][2];	//for the torus
	float theta, dtheta;		//in fly mode, the turn angle, and turn rate of the eye
	float dflyheight;			//rate of height gain of the eye in fly mode
	DWORD time;					//system time, in milliseconds
	bool flygo;					//in fly mode, true= eye can move;  false= freeze the eye
	bool showflypos;
	float period;		//the time between updates
	float renderperiod; //the time it took to render the scene
	int resolution;
	int interval;  // the space between sampled vertices; it must be >= 1
	int cellx, celly;

	bool threeDGlasses;
	float leftColor[3];
	float rightColor[3];
	float* currentEyeColor;
	float interPupilDistance;
	bool antiAliased;
	int whichEye;
	float eyeAngle;

	//material properties
	float matAmb[3];
	float matDif[3];
	float matSpec[3];
	float matShine;
public:
	CapowGL(HWND hwnd);
	~CapowGL();
	void Draw(HDC hdc, CA* focus);
	void Size(HWND hwnd);
	HGLRC SetUpOpenGL(HWND hWnd);
	void DrawOpenGLScene();
	inline GLfloat CapowGL::GraphHeight(int i,int j);
	inline GLfloat CapowGL::GraphHeight(int i);
	void SetUpLights();
	void SetMaterials(
		GLfloat ambr, GLfloat ambg, GLfloat ambb,
		GLfloat difr, GLfloat difg, GLfloat difb,
		GLfloat specr, GLfloat specg, GLfloat specb, GLfloat shine);
	void PickMaterial(int materialtype);
	void ComputeNormals();
	void NormalVector(int i,int j,int half, GLfloat n[3]);
	void NormalVector(int i,int j,int half, GLfloat n[3], int interval);

	void MouseMove(int x, int y, UINT flags);
	void LeftButtonDown(BOOL fDoubleClick, int x, int y, UINT flags);
	void LeftButtonUp(int x, int y, UINT flags);
	void ZoomIn();
	void ZoomOut();
	void Pan(int direction);
	void Reset();
	

	//accessors and manipulators
	void Z(float value){z = value;}
	float Z(){return z;}

	void Spin(BOOL value){spinflag = value;}
	BOOL Spin(){return spinflag;}

	float Angle(){return spinangle;}
	void Angle(float theta){spinangle = theta;}

	float TiltAngle(){return tiltangle;}
	void TiltAngle(float value){tiltangle = value;}

	void Lights(BOOL value) {spinflag = value;}
	BOOL Lights(){return spinflag;}

	void Type(int newtype);
	int Type() {return graphtype;}
	
	void SurfaceType(int newtype);
	int SurfaceType(){return surfacetype;}

	void Material(int newmaterial) {material = newmaterial;}
	int Material() {return material;}

	void MouseMode(int mode);
	int MouseMode() {return mousemode;}


	void ShowGenerators(BOOL newflag) {showgeneratorsflag = newflag;}
	BOOL ShowGenerators() {return showgeneratorsflag;}

	void ZeroPlane(BOOL newflag) {zeroplaneflag = newflag;}
	BOOL ZeroPlane(){return zeroplaneflag;}

	void MaxPlane(BOOL newflag) {maxplaneflag = newflag;}
	BOOL MaxPlane(){return maxplaneflag;}
	
	void Resolution(int value);
	int Resolution() {return resolution;}

	void ShowFlyPos(BOOL flag) {showflypos = flag;}
	BOOL ShowFlyPos(){return showflypos;}

	void ThreeDGlasses(BOOL flag);
	BOOL ThreeDGlasses(){return threeDGlasses;}

	void AntiAliased(BOOL flag) {antiAliased = flag;}
	bool AntiAliased() {return antiAliased;}

	void AdjustHeightFactor(CA *focus);

	void FocusIsActive(BOOL flag){focusIsActive = flag;}

	float tempwidth, tempheight;

	void DrawStats(HDC hdc);
	void UpdateFly();

	void SetUpTorus();
//	void SetUpGrid();

	bool CaptureToVRML();
	bool CaptureVRML();
	void SetCellXY(int x,int y){cellx = x; celly=y;}

};

#endif //CAPOWGL_HPP