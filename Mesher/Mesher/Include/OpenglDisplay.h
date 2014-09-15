/*
  Copyright 2010 Centre for Computational Technologies Pvt. Ltd.(CCTech) All rights reserved.
  This software and related documentation are proprietary to 
  Centre for Computational Technologies Pvt. Ltd.(CCTech)
*/
#ifndef _OPENGLDISPLAY_H
#define _OPENGLDISPLAY_H

class Mesher;
typedef enum _STATUS{
	FAIL,
	OK,
	DELETE_VOXEL,
}STATUS;

class OpenglDisplay{
	/* static data*/
	//static Triangulation *trsp;
	static GLuint backgroundList;
			
	static int spin;
	static int lastx,lasty;
	static double theta,zoom ;
	static double rotx,roty,rotz;
	static int oldx,oldy;
	static double tx,ty,tz;
	static unsigned char Buttons[3];
	
	static Vector3D bbox[2];
	static double OrthoSize;
	static bool vertex,wire,render,normal,hidden,wireBool;
	static bool XYclip,YZclip,ZXclip;
	static bool	testPoints,highlightTriangle,boundary;
	static bool insidePoint,outsidePoint,boundaryPoint,unknownPoint;
	static bool ccw,cullFace;
	static double normalScale;
public:
	static double alpha;
	OpenglDisplay(Mesher* inTriangulator);	
	~OpenglDisplay(){
	}

	STATUS OpenglDisplay::add_display_list(Mesher * ,string );
	STATUS OpenglDisplay::add_display_list(vector<Vector3D>& ,string );
	STATUS OpenglDisplay::add_display_list(list<pair<vector<Vector3D>,Vector3D > >,string );

	STATUS OpenglDisplay::init();
	STATUS exec();
	
	/* static functions */
	static void display(void);
	static void menu(int option);
	static void clipmenu(int option);
	static void trspmenu(int option);
	static void pointmenu(int option);
	static void mouse(int b,int s,int x,int y);
	static void motion(int x,int y);
	static void pan(int x,int y);
	static void keyboard(unsigned char key,int x,int y);
	static void keypressed(int key, int x, int y);
	static void reshape (int w, int h);
	static void OpenglDisplay::initLightMaterial();
	static void message(char *message,double x,double y,double z);
	static void message(char *message,int x,int y);

private:
	static Mesher* mMesher;
	static bool mDisplayBoundary;
	static bool mDisplayHoles;
	static bool mDisplayConstraints;
	static bool mDisplayWireframe;
	static bool mDisplayFaceted;

	static GLuint mDisplaylistBoundary;
	static GLuint mDisplaylistHoles;
	static GLuint mDisplaylistConstraints;
	static GLuint mDisplaylistWireframe;
	static GLuint mDisplaylistFaceted;

	static void displayTriangulator();
	static void createDisplayLists();
	static void clearDisplayLists();

	static void	displayFaceted(   );
	static void	displayWireframe(   );
	static void	displayBoundary(   );
	static void	displayHoles(   );
	static void	displayConstraints(   );

	static void areaRange(   double & min, double & max);
	static void aspectRatioRange(   double & min, double & max);
};

#endif
