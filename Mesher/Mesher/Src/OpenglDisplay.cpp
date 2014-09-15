/*
  Copyright 2010 Centre for Computational Technologies Pvt. Ltd.(CCTech) All rights reserved.
  This software and related documentation are proprietary to 
  Centre for Computational Technologies Pvt. Ltd.(CCTech)
*/
#include "stdafx.h"
#include "OpenglDisplay.h"
#include "Mesher.h"


GLdouble eqn1[4] = { -1.0, 0.0, 0.0, 0.0 };
GLdouble eqn2[4] = { 0.0, -1.0, 0.0, 0.0 };
GLdouble eqn3[4] = { 0.0, 0.0, -1.0, 0.0 };

typedef map <string, pair<GLuint,bool> > MapObjectDisplayList;
MapObjectDisplayList ObjectDisplayList;
MapObjectDisplayList ObjectDisplayWireList;


GLuint OpenglDisplay::backgroundList=0;
GLuint sectionList=0;

bool OpenglDisplay::XYclip=false;
bool OpenglDisplay::YZclip=false;
bool OpenglDisplay::ZXclip=false;
bool OpenglDisplay::vertex=false;
bool OpenglDisplay::wire=false;
bool OpenglDisplay::hidden=true;
bool OpenglDisplay::wireBool=false;
bool OpenglDisplay::render=false;
bool OpenglDisplay::normal=true;
bool OpenglDisplay::testPoints=false;
bool OpenglDisplay::highlightTriangle=false;
bool OpenglDisplay::boundary=false;

bool OpenglDisplay::insidePoint=false;
bool OpenglDisplay::outsidePoint=false;
bool OpenglDisplay::boundaryPoint=false;
bool OpenglDisplay::unknownPoint=false;
bool OpenglDisplay::ccw=true;
bool OpenglDisplay::cullFace=true;

Vector3D OpenglDisplay::bbox[2];
double OpenglDisplay::OrthoSize;
double OpenglDisplay::alpha=1.0;
double OpenglDisplay::normalScale=1.0;
int OpenglDisplay::spin=0;
int OpenglDisplay::lastx=0;
int OpenglDisplay::lasty=0;
int OpenglDisplay::oldx=0;
int OpenglDisplay::oldy=0;

double OpenglDisplay::theta=0;
double OpenglDisplay::zoom=1.0;
double OpenglDisplay::rotx=0;
double OpenglDisplay::roty=0;
double OpenglDisplay::rotz=0;
double OpenglDisplay::tx=0;
double OpenglDisplay::ty=0;
double OpenglDisplay::tz=0;
unsigned char OpenglDisplay::Buttons[3]={0};
Mesher* OpenglDisplay::mMesher=nullptr;
bool OpenglDisplay::mDisplayBoundary = false;
bool OpenglDisplay::mDisplayHoles = true;
bool OpenglDisplay::mDisplayConstraints = true;
bool OpenglDisplay::mDisplayWireframe = true;
bool OpenglDisplay::mDisplayFaceted = true;
GLuint OpenglDisplay::mDisplaylistBoundary = 0;
GLuint OpenglDisplay::mDisplaylistHoles = 0;
GLuint OpenglDisplay::mDisplaylistConstraints = 0;
GLuint OpenglDisplay::mDisplaylistWireframe = 0;
GLuint OpenglDisplay::mDisplaylistFaceted = 0;


GLdouble color[][4]= {{1,0,0,OpenglDisplay::alpha},
					{0,0,1,OpenglDisplay::alpha},
					{0,0,1,OpenglDisplay::alpha},
					{0,1,0,OpenglDisplay::alpha},
					{1,0,1,OpenglDisplay::alpha},
					};
typedef void * LPVOID;
// Here are the fonts:
LPVOID glutFonts[7] = {
    GLUT_BITMAP_9_BY_15,
    GLUT_BITMAP_8_BY_13,
    GLUT_BITMAP_TIMES_ROMAN_10,
    GLUT_BITMAP_TIMES_ROMAN_24,
    GLUT_BITMAP_HELVETICA_10,
    GLUT_BITMAP_HELVETICA_12,
    GLUT_BITMAP_HELVETICA_18
};

// Here is the function
void glutPrint(float x, float y, LPVOID font, char* text, float r, float g, float b, float a)
{
    if(!text || !strlen(text)) return;
    bool blending = false;
    if(glIsEnabled(GL_BLEND)) blending = true;
    glEnable(GL_BLEND);
    glColor4f(r,g,b,a);
    glRasterPos2f(x,y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
    if(!blending) glDisable(GL_BLEND);
}

void displaySphere(Vector3D pos,double redius)
{
	//glDisable(GL_BLEND);
		glPushMatrix();
			glEnable (GL_DEPTH_TEST);
			glTranslated(pos.x(),pos.y(),pos.z());
			glutSolidSphere(redius,20,10);
			glDisable (GL_DEPTH_TEST);
		glPopMatrix();
}

void OpenglDisplay::displayTriangulator()
{
	if(mDisplayBoundary)
		glCallList(mDisplaylistBoundary);
	if(mDisplayHoles)
		glCallList(mDisplaylistHoles);
	if(mDisplayConstraints)
		glCallList(mDisplaylistConstraints);
	if(mDisplayWireframe)
		glCallList(mDisplaylistWireframe);
	if(mDisplayFaceted)
		glCallList(mDisplaylistFaceted);
}
GLuint displayText;
double minAngle;
void OpenglDisplay::createDisplayLists()
{
	mDisplaylistBoundary = glGenLists(1);
	glNewList(mDisplaylistBoundary, GL_COMPILE);
		displayBoundary();
	glEndList();
	mDisplaylistHoles = glGenLists(1);
	glNewList(mDisplaylistHoles, GL_COMPILE);
		displayHoles();
		
		Vector3D circleCenter(mMesher->domain().boundingBox().centroid());
		double radius=mMesher->domain().boundingBox().width()/4.0;
		glColor3d(0,0,1.0);
		glBegin(GL_LINE_LOOP);
		for (double i=0;i<2*M_PI;i+=(2*M_PI)/18.0){
			glVertex3d(circleCenter.x()+radius*cos(i),circleCenter.y()+radius*sin(i),0);		
		}
		glEnd();
	glEndList();
	mDisplaylistConstraints = glGenLists(1);
	glNewList(mDisplaylistConstraints, GL_COMPILE);
		displayConstraints();
	glEndList();
	mDisplaylistFaceted = glGenLists(1);
	glNewList(mDisplaylistBoundary, GL_COMPILE);
		displayFaceted();
	glEndList();
	mDisplaylistWireframe = glGenLists(1);
	glNewList(mDisplaylistWireframe, GL_COMPILE);
		displayWireframe();
	glEndList();
	minAngle=mMesher->findMinimumAngle();
	/*
	char buffer[80];
	displayText = glGenLists(1);
	glNewList(displayText, GL_COMPILE);
		sprintf_s(buffer,"Triangle Count = %d ",mMesher->mesh()->triangles()->size());
		glutPrint(-180,40,GLUT_BITMAP_HELVETICA_12,buffer,0,0,0,1);

		sprintf_s(buffer,"Trianglulation Min Angle = %lf ",mMesher->findMinimumAngle());
		glutPrint(-180,60,GLUT_BITMAP_HELVETICA_12,buffer,0,0,0,1);
	glEndList();*/

}
void OpenglDisplay::clearDisplayLists()
{
	glDeleteLists(mDisplaylistBoundary,1);
	glDeleteLists(mDisplaylistHoles,1);
	glDeleteLists(mDisplaylistConstraints,1);
	glDeleteLists(mDisplaylistFaceted,1);
	glDeleteLists(mDisplaylistWireframe,1);
}

void drawXYFace(Vector3D first, Vector3D last)
{
	Vector3D normal(0,0,1);
	glBegin(GL_QUADS);
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(first.x(),first.y(),first.z());
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(last.x(),first.y(),first.z());
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(last.x(),last.y(),first.z());
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(first.x(),last.y(),first.z());
	glEnd();
}
void drawYZFace(Vector3D first, Vector3D last)
{
	Vector3D normal(1,0,0);
	glBegin(GL_QUADS);
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(first.x(),first.y(),first.z());
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(first.x(),last.y(),first.z());
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(first.x(),last.y(),last.z());
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(first.x(),first.y(),last.z());
	glEnd();
}
void drawXZFace(Vector3D first, Vector3D last)
{
	Vector3D normal(0,1,0);
	glBegin(GL_QUADS);
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(first.x(),first.y(),first.z());
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(last.x(),first.y(),first.z());
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(last.x(),first.y(),last.z());
		glNormal3d(normal.x(),normal.y(),normal.z());
		glVertex3d(first.x(),first.y(),last.z());
	glEnd();
}
OpenglDisplay::OpenglDisplay(Mesher* inTriangulator)
{
   	glutInitDisplayMode (GLUT_DOUBLE| GLUT_RGB|GLUT_DEPTH| GLUT_ALPHA|GLUT_STENCIL);
   	glutInitWindowSize (1000, 700);
   	glutInitWindowPosition (0, 0);
   	glutCreateWindow ("CCTech Geometry Test");
   	glClearColor (0.0, 0.0, 0.0, 0.0);
   	glShadeModel (GL_FLAT); 
   	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glFrontFace(GL_CCW);
   	glutSetWindowTitle("CCTech Geometry Test");
   	glutSetIconTitle("PointLocation 2D");
	glEnable (GL_BLEND); 
	glEnable (GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);
	
	mMesher = inTriangulator;
}

void OpenglDisplay::message(char *message,double x,double y,double z)
{
	int i,n;
	glColor3d(1,0,1);
	glRasterPos3d (x,y,z);
	n=strlen(message);
	for(i=0;i<n;i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10,message[i]);	
}

void OpenglDisplay::message(char *message,int x,int y)
{
	int i,n;
	GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    GLint realy; 
    GLdouble wx1, wy1, wz1; 
	glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
    
    realy = viewport[3] - (GLint) y - 1;
    gluUnProject ((GLdouble) x, (GLdouble) realy, 0.5,
		mvmatrix, projmatrix, viewport, &wx1, &wy1, &wz1);
	glRasterPos3d(wx1,wy1,wz1);

	glColor3d(1,0,1);
	n=strlen(message);
	for(i=0;i<n;i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,message[i]);	
}
/*
void OpenglDisplay::createNormalDisplayList(Triangulation &target)
{
	glDeleteLists(normalListTarget,1);
	normalListTarget= glGenLists(1);
	glNewList(normalListTarget,GL_COMPILE);
		target.normal_display(normalScale);
    glEndList();
}
*/

STATUS OpenglDisplay::add_display_list(vector<Vector3D>& mPointList,string name)
{


	pair<GLuint,bool > displaylist(glGenLists(1),true);
	ObjectDisplayList[name]= displaylist;
	glNewList(displaylist.first,GL_COMPILE);
		for(size_t i = 0; i < mPointList.size() ;i++){
	//		glVertex3d(mPointList[i].x(),mPointList[i].y(),mPointList[i].z());
			displaySphere(mPointList[i],3);
		}
    glEndList();
	return OK;
}

STATUS OpenglDisplay::add_display_list(list<pair<vector<Vector3D> ,Vector3D > > mHighlightPolygon,string name)
{
	pair<GLuint,bool > displaylist(glGenLists(1),true);
	ObjectDisplayList[name]= displaylist;
	glNewList(displaylist.first,GL_COMPILE);
	glLineWidth(5.0);
	for(auto it = mHighlightPolygon.begin() ;it != mHighlightPolygon.end() ;it++){
		glBegin(GL_LINE_STRIP);
		glColor3d(it->second.x(),it->second.y(),it->second.z());
		for(auto itPoint = it->first.begin() ;itPoint != it->first.end();itPoint++)
		{
			glVertex3d(itPoint->x(),itPoint->y(),itPoint->z());
			//displaySphere(*itPoint,2);
		}
		glEnd();
	}

	for(auto it = mHighlightPolygon.begin() ;it != mHighlightPolygon.end() ;it++){
		glColor3d(it->second.x(),it->second.y(),it->second.z());
		for(auto itPoint = it->first.begin() ;itPoint != it->first.end();itPoint++)
		{
			displaySphere(*itPoint,2);
		}
		glEnd();
	}
    glEndList();
	glLineWidth(1.0);

	return OK;
}


STATUS OpenglDisplay::add_display_list(Mesher *mMesher,string name)
{
	Vector3D v1= mMesher->domain().boundingBox().lower();
	Vector3D v2= mMesher->domain().boundingBox().upper();
	OrthoSize=max(max(v2.x()-v1.x(),v2.y()-v1.y()),v2.z()-v1.z());
	
	createDisplayLists();

	//pair<GLuint,bool > displaylist(glGenLists(1),true);
	//ObjectDisplayList[name]= displaylist;
	//glNewList(displaylist.first,GL_COMPILE);
	//	mMesher->displayFaceted();
 //   glEndList();

	//pair<GLuint,bool > displaylistWire(glGenLists(1),true);
	//ObjectDisplayWireList[name]= displaylistWire;
	//glNewList(displaylistWire.first,GL_COMPILE);
	//	mMesher->wireframe();
 //   glEndList();

	//pair<GLuint,bool > displaylistBoundary(glGenLists(1),true);
	//ObjectDisplayWireList[name + "_boundary"]= displaylistBoundary;
	//glNewList(displaylistBoundary.first,GL_COMPILE);
	//	mMesher->displayBoundary();
 //   glEndList();

	//pair<GLuint,bool > displaylistHoles(glGenLists(1),true);
	//ObjectDisplayWireList[name + "_hole"]= displaylistHoles;
	//glNewList(displaylistHoles.first,GL_COMPILE);
	//	mMesher->displayHoles();
 //   glEndList();

	//pair<GLuint,bool > displaylistConstraints(glGenLists(1),true);
	//ObjectDisplayWireList[name + "_constraints"]= displaylistConstraints;
	//glNewList(displaylistConstraints.first,GL_COMPILE);
	//	mMesher->displayConstraints();
 //   glEndList();
//	createNormalDisplayList(target);*/
	return OK;
}

STATUS OpenglDisplay::init()
{
	bbox[1].x()=bbox[1].y()=bbox[1].z()=1.8;
	bbox[0].x()=bbox[0].y()=bbox[0].z()=-1.8;
	
	OrthoSize=max(max(bbox[1].x()-bbox[0].x(),bbox[1].y()-bbox[0].y()),bbox[1].z()-bbox[0].z());

	backgroundList=glGenLists(1);
	glNewList(backgroundList,GL_COMPILE);
	glDisable(GL_LIGHTING);
		glShadeModel(GL_SMOOTH); 
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_QUADS);
			glColor4d(1,1,1,1);
			glVertex3d(-OrthoSize*1,-OrthoSize*1,  -3000.0+1);
			glColor4d(1,1,1,1);
			glVertex3d(OrthoSize*1 ,-OrthoSize*1, -3000.0+1);
			glColor4d(1,1,1,1);
			glVertex3d(OrthoSize*1 ,OrthoSize*1, -3000.0+1);
			glColor4d(0,0,0,1);
			glVertex3d(-OrthoSize*1 ,OrthoSize*1,-3000.0+1);
		glEnd();

	/*float ortho=10;
	glBegin(GL_QUADS);
		glColor3f(1.0, 1.0, 1.0);
		glVertex3f( ortho, ortho, -30.0f); 
		glColor3f(1.0, 1.0, 1.0);
		glVertex3f( -ortho, ortho, -30.0f); 
		glColor3f(0.0, 0.0, 0.0);
		glVertex3f(-ortho,-ortho, -30.0f); 
		glColor3f(0.0, 0.0, 0.0);
		glVertex3f( ortho,-ortho, -30.0f); 
		glEnd();
		*/
	glEndList();
	return OK;
}

STATUS OpenglDisplay::exec(){
	//if(gvoxel3d){
	//	eqn1[3]=gvoxel3d->start.x();
	//	eqn2[3]=gvoxel3d->start.y();
	//	eqn3[3]=gvoxel3d->start.z();
	//}
	glutDisplayFunc(display);
   	glutReshapeFunc(reshape);
   	glutKeyboardFunc(keyboard);
   	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(keypressed);

	int pointMenu=glutCreateMenu(pointmenu);
	glutAddMenuEntry("INSIDE",1);
	glutAddMenuEntry("OUTSIDE",2);
	glutAddMenuEntry("BOUNDARY",3);
	glutAddMenuEntry("UNKNOWN",4);

	int trspMenu=glutCreateMenu(trspmenu);
	glutAddMenuEntry("Vertex",1);
	glutAddMenuEntry("Wire",2);
	glutAddMenuEntry("Render",3);
	glutAddMenuEntry("Normal",4);

	int clipMenu=glutCreateMenu(clipmenu);
	glutAddMenuEntry("XY",1);
	glutAddMenuEntry("YZ",2);
	glutAddMenuEntry("ZX",3);

   	glutCreateMenu(menu);
	int index=1;
	glutAddMenuEntry("Front",index++);
	glutAddMenuEntry("IsoView",index++);
	glutAddMenuEntry("Simulate",index++);
	for(MapObjectDisplayList::iterator it=ObjectDisplayList.begin() ;it!=ObjectDisplayList.end();it++){
		glutAddMenuEntry(it->first.c_str(),index++);
	}
	//glutAddSubMenu("Triangultion", trspMenu);
	//glutAddSubMenu("PointSort", pointMenu);
	glutAddSubMenu("Clip", clipMenu);

   	glutAttachMenu(GLUT_RIGHT_BUTTON);
   	glutMainLoop();
	for(MapObjectDisplayList::iterator it=ObjectDisplayList.begin() ;it!=ObjectDisplayList.end();it++){
		glDeleteLists(it->second.first, 1);
	}
	for(MapObjectDisplayList::iterator it=ObjectDisplayWireList.begin() ;it!=ObjectDisplayWireList.end();it++){
		glDeleteLists(it->second.first, 1);
	}
   	return OK;
}
void tripod()
{
	glBegin(GL_LINES);
		glColor3d(1,0,0);
		glVertex3d(0,0,0);
		glVertex3d(10,0,0);
	glEnd();
	glBegin(GL_LINES);
		glColor3d(0,1,0);
		glVertex3d(0,0,0);
		glVertex3d(0,10,0);
	glEnd();
	glBegin(GL_LINES);
		glColor3d(0,0,1);
		glVertex3d(0,0,0);
		glVertex3d(0,0,10);
	glEnd();
}

void OpenglDisplay::initLightMaterial()
{
	double inColor[]={50.0/255.0, 125.0/255.0, 160.0/255.0,alpha};

	//GLfloat al[] = {1, 0, 0, 1.0};
	//GLfloat light_ambient[4]={0.0, 0.0, 0, alpha};
	//GLfloat light_diffuse[4]={0.8, 0.8, .8, alpha};
	//GLfloat light_specular[4]={0.5, 0.5, 0.5, alpha};
	GLfloat light_ambient[4]={inColor[0], inColor[1],inColor[2],inColor[3]};
	GLfloat light_diffuse[4]={inColor[0], inColor[1],inColor[2],inColor[3]};
	GLfloat light_specular[4]={inColor[0], inColor[1],inColor[2],inColor[3]};;
	
	GLfloat light_position0[] = {-OrthoSize,-OrthoSize,-OrthoSize, 1.0 };
	GLfloat light_position1[] = {2*OrthoSize,2*OrthoSize,2*OrthoSize, 1.0 };
	GLfloat light_position2[] = {-OrthoSize,2*OrthoSize,2*OrthoSize, 1.0 };
	GLfloat light_position3[] = {2*OrthoSize,-OrthoSize,-OrthoSize, 1.0 };
	GLfloat light_position4[] = {-OrthoSize,-OrthoSize,2*OrthoSize, 1.0 };
	GLfloat light_position5[] = {2*OrthoSize,2*OrthoSize,-OrthoSize, 1.0 };
	GLfloat light_position6[] = {2*OrthoSize,-OrthoSize,2*OrthoSize, 1.0 };
	GLfloat light_position7[] = {-OrthoSize,2*OrthoSize,-OrthoSize, 1.0 };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT4, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT4, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT5, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT5, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT6, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT6, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT6, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT7, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT7, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT7, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT3, GL_POSITION, light_position3);
	glLightfv(GL_LIGHT4, GL_POSITION, light_position4);
	glLightfv(GL_LIGHT5, GL_POSITION, light_position5);
	glLightfv(GL_LIGHT6, GL_POSITION, light_position6);
	glLightfv(GL_LIGHT7, GL_POSITION, light_position7);
	
	//double inColor[]={0.0, 0.0, 1.0,.7};
	GLfloat mat_shininess[] = { 200.0 };
	GLfloat mat_solid[4]={inColor[0], inColor[1],inColor[2],inColor[3]};
	GLfloat mat_zero[] = { 0.0f, 0.0f, 0.0f, alpha };

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_solid);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);

	glMaterialfv(GL_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_BACK, GL_DIFFUSE, mat_solid);
	glMaterialfv(GL_BACK, GL_EMISSION, mat_zero);

	glMateriali(GL_FRONT, GL_SHININESS, 200);
	//float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	//lMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);

	//GLfloat  ambientLight[] = {0.4f, 0.4f, 0.4f, 1.0f };
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientLight);


	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable (GL_AUTO_NORMAL);
	glEnable (GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

vector<Vector3D> path;

int currentList=0;

void OpenglDisplay::display(void)
{
   	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glClearColor(1,1,1,1);


	glPushAttrib(GL_LIGHTING_BIT);
        glDisable(GL_LIGHTING);
		//glCallList(backgroundList);
		char buffer[80];
		sprintf_s(buffer,"Triangle Count = %d ",mMesher->mesh()->triangles()->size());
		glutPrint(-180,40,GLUT_BITMAP_HELVETICA_12,buffer,0,0,0,1);

		sprintf_s(buffer,"Trianglulation Min Angle = %lf ",minAngle);
		glutPrint(0,0,GLUT_BITMAP_HELVETICA_12,buffer,0,0,0,1);
	glPopAttrib(); 


	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glScaled(zoom,zoom,zoom);
			//cout ("zoom %lf\n",zoom);
        	glPushMatrix();
        	glTranslated(tx,ty,0);
        	glPushMatrix();
        	glRotated(rotx,1,0,0);
        	glPushMatrix();
        	glRotated(roty,0,1,0);
        	glPushMatrix();

			glRotated(rotz,0,0,1);
        	glPushMatrix();
        	glRotated(spin,0,0,1.0);
   	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

	int i=0;
	for(MapObjectDisplayList::iterator it=ObjectDisplayList.begin();it!=ObjectDisplayList.end();it++,i++){
		if(it->second.second){
			glColor4dv(color[i]);
			glCallList(it->second.first);
		}
	}
	//for(MapObjectDisplayList::iterator it=ObjectDisplayWireList.begin();it!=ObjectDisplayWireList.end();it++,i++){
	//	if(it->second.second){
	//		glColor4dv(color[i]);
	//		glCallList(it->second.first);
	//	}
	//}
	displayTriangulator();
	glPopMatrix();
	//tripod();
	glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
	glutSwapBuffers();
}

void OpenglDisplay::reshape (int w, int h)
{ 
   	if(w>h)
		h=w;
   	else 
		w=h;
   	glViewport (0,0,w,h);
	
   	glMatrixMode (GL_PROJECTION);
   	glLoadIdentity ();
   	glOrtho (-OrthoSize,OrthoSize,-OrthoSize,OrthoSize, -3000.0, 3000.0);
   	
   	glMatrixMode (GL_MODELVIEW);
	theta=0;
	zoom=1.0;
	//rotx=35.26+90;
	rotx=0;
	roty=0.0;
	rotz=0;
	tx=0;
	ty=0;
	tz=0;
}

void OpenglDisplay::keypressed(int key, int x, int y)
{	
	glMatrixMode(GL_PROJECTION);
    if (key ==GLUT_KEY_LEFT)
    {
          spin-= 1;
    }
    if (key ==GLUT_KEY_RIGHT)
    {
          spin+= 1;
    }
    if (key ==GLUT_KEY_UP )
    {
		zoom*=1.1;
    }
    if (key ==GLUT_KEY_DOWN)
    {
		zoom*=.9;
    }
	if (key ==GLUT_KEY_PAGE_UP)
    {
	/*	eqn1[3]+=gvoxel3d->delta.x();
		eqn2[3]+=gvoxel3d->delta.x();
		eqn3[3]+=gvoxel3d->delta.x();
		if(YZclip){
			sectionList=glGenLists(1);
			glNewList(sectionList,GL_COMPILE);
			double pos=eqn1[3]-gvoxel3d->start.x();
			int index=(pos/gvoxel3d->delta.x());
			//gvoxel3d->displaySection(index+1,0);
			glEndList();
		}
		if(ZXclip){
			sectionList=glGenLists(1);
			glNewList(sectionList,GL_COMPILE);
			double pos=eqn2[3]-gvoxel3d->start.y();
			int index=(pos/gvoxel3d->delta.y());
			//gvoxel3d->displaySection(index+1,1);
			glEndList();
		}
		*/

		eqn1[3]+=.1;
		eqn2[3]+=.1;
		eqn3[3]+=.1;
    }

	if (key ==GLUT_KEY_PAGE_DOWN)
    {
		eqn1[3]-=.1;
		eqn2[3]-=.1;
		eqn3[3]-=.1;
	/*	eqn1[3]-=gvoxel3d->delta.x();
		eqn2[3]-=gvoxel3d->delta.x();
		eqn3[3]-=gvoxel3d->delta.x();
		if(YZclip){
			sectionList=glGenLists(1);
			glNewList(sectionList,GL_COMPILE);
			double pos=eqn1[3]-gvoxel3d->start.x();
			int index=(pos/gvoxel3d->delta.x());
			//gvoxel3d->displaySection(index+1,0);
			glEndList();
		}
		if(ZXclip){
			sectionList=glGenLists(1);
			glNewList(sectionList,GL_COMPILE);
			double pos=eqn2[3]-gvoxel3d->start.y();
			int index=(pos/gvoxel3d->delta.y());
			//gvoxel3d->displaySection(index+1,1);
			glEndList();
		}
		*/
    }
	if (key ==GLUT_KEY_HOME)
    {
		eqn1[0]= eqn1[0]*-1.0;
		eqn2[1]= eqn2[1]*-1.0;
		eqn3[2]= eqn3[2]*-1.0;
    }

    glutPostRedisplay();
}

void OpenglDisplay::keyboard(unsigned char key,int x,int y){
//	GLuint displayList;
	switch(key){
		case 'j':
			rotx-= 1;
			break;
		case 'k':
			rotx+= 1;
			break;
		case 'i':
			roty-= 1;
			break;
		case 'o':
			roty+= 1;
			break;
		case ' ':
		case 'n':
			glutPostRedisplay();
			break;	
	
/*		case 'w':
			wireBool=!wireBool;
			glutPostRedisplay();
			break;*/	
		//case 'h':
		//	hidden=!hidden;
		//	glutPostRedisplay();
			break;	
		//case 'r':
		//	render=!render;
		//	if(render){
		//		initLightMaterial();
		//	}
		//	else {
		//		glDisable(GL_LIGHTING);
		//		glDisable(GL_DEPTH_TEST);
		//	}
		//	glutPostRedisplay();
		//	break;
		//case 'c':
		case 'C':
			cullFace = !cullFace;
			if(cullFace)
				glEnable(GL_CULL_FACE);
			else 
				glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		//case 'f':
		case 'F':
			ccw = !ccw;
			glFrontFace(ccw ? GL_CCW : GL_CW);
			glutPostRedisplay();
			break;
		case 't':
			alpha*=0.9;
			if(alpha<.1)
				alpha=.1;
			glutPostRedisplay();
			break;
		case 'T':
			alpha*=1.1;
			if(alpha>1)
				alpha=1;
			glutPostRedisplay();
			break;
		case 'q':
		case 'Q':
			printf("Quiting\n");	
			exit (1);
			return ;
		case 'r':
			mMesher->laplacianSmoothing();
			//mMesher->angleBasedSmoothing();
			clearDisplayLists();
			createDisplayLists();
			break;
		case 'b':
			mDisplayBoundary = !mDisplayBoundary;
			break;
		case 'h':
			mDisplayHoles = !mDisplayHoles;
			break;
		case 'c':
			mDisplayConstraints = !mDisplayConstraints;
			break;
		case 'f':
			mDisplayFaceted = !mDisplayFaceted;
			break;
		case 'w':
			mDisplayWireframe = !mDisplayWireframe;
			break;
		default:
			break;
	}
	glutPostRedisplay();
}

void OpenglDisplay::motion(int x,int y)
{
	int diffx=x-lastx;
	int diffy=y-lasty;

	if( Buttons[0] && Buttons[1] )
	{
		zoom -= (double) 0.05 * diffx;
		glutPostRedisplay();
	}
	else if( Buttons[0] )
	{
		rotx += (double) 0.5 * diffy;
		roty += (double) 0.5 * diffx;
		glutPostRedisplay();
	}
	else if( Buttons[1] )
	{	
		pan(x,y);
		glutPostRedisplay();
		oldx=x,oldy=y;
	}      
	lastx=x;
	lasty=y;
}

void OpenglDisplay::pan(int x,int y)
{
	GLint viewport[4];
    	GLdouble mvmatrix[16], projmatrix[16];
    	GLint realy; /* OpenGL y coordinate position */
    	GLdouble wx1, wy1, wz1; /* returned world x, y, z coords     */
	GLdouble wx2, wy2, wz2; /* returned world x, y, z coords     */
	glGetIntegerv (GL_VIEWPORT, viewport);
    	glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    	glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
    
	/* note viewport[3] is height of window in pixels */
    	realy = viewport[3] - (GLint) y - 1;
    	gluUnProject ((GLdouble) x, (GLdouble) realy, 0.5,
		mvmatrix, projmatrix, viewport, &wx1, &wy1, &wz1);

	realy = viewport[3] - (GLint) lasty - 1;
    	gluUnProject ((GLdouble) lastx, (GLdouble) realy, 0.5,
		mvmatrix, projmatrix, viewport, &wx2, &wy2, &wz2);
	tx+=(wx1-wx2)/1.0;
	ty+=(wy1-wy2)/1.0;
	tz+=(wz1-wz2)/1.0;
}


void OpenglDisplay::mouse(int b,int s,int x,int y)
{
        lastx=x;
        lasty=y;
        switch(b)
        {
        case GLUT_LEFT_BUTTON:
            Buttons[0] = ((GLUT_DOWN==s)?1:0);
        	glutPostRedisplay();
             break;
        case GLUT_MIDDLE_BUTTON:
            Buttons[1] = ((GLUT_DOWN==s)?1:0);
        	glutPostRedisplay();
            break;
        default:
                break;
        }
}

void OpenglDisplay::clipmenu(int option){
	switch(option){
	case 1:
		XYclip =! XYclip;
		glutPostRedisplay();
		break;
	case 2:
		YZclip =! YZclip;
		if(YZclip){
			if(!sectionList){
				sectionList=glGenLists(1);
				glNewList(sectionList,GL_COMPILE);
				glEndList();
			}
		}
		glutPostRedisplay();
		break;
	case 3:
		ZXclip =! ZXclip;
		if(ZXclip){
			if(!sectionList){
				sectionList=glGenLists(1);
				glNewList(sectionList,GL_COMPILE);
				glEndList();
			}
		}
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void OpenglDisplay::trspmenu(int option){
	switch(option){
	case 1:
		vertex=!vertex;
		glutPostRedisplay();
		break;
	case 2:
		wire=!wire;
		glutPostRedisplay();
		break;
	case 3:

		glutPostRedisplay();
		break;
	case 4:
		normal=!normal;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void OpenglDisplay::pointmenu(int option){
	switch(option){
	case 1:
		insidePoint=!insidePoint;
		glutPostRedisplay();
		break;
	case 2:
		outsidePoint=!outsidePoint;
		glutPostRedisplay();
		break;
	case 3:
		boundaryPoint=!boundaryPoint;
		glutPostRedisplay();
		break;
	case 4:
		unknownPoint=!unknownPoint;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}
void idle();
void idleTimer(int ){
	idle();
	//cout<<"timer called"<<endl;
}
void idle()
{
	glutPostRedisplay();

	glutIdleFunc(nullptr);
	glutTimerFunc(100,idleTimer,0);
}

void OpenglDisplay::menu(int option){
	switch(option){
	case 1:
		theta=0;zoom=1.0;
		rotx=0;	roty=0;
		tx=0;	ty=0;	tz=0;
		glutPostRedisplay();
		break;
	case 2:
		theta=0;
		zoom=1.0;
		rotx=35.26;
		roty=-45.0;
	
		tx=0;
		ty=0;
		tz=0;
		glutPostRedisplay();
		break;

	default:
		int i=0;
		MapObjectDisplayList::iterator it=ObjectDisplayList.begin();
		if(it==ObjectDisplayList.end())
			break;
		for(;it!=ObjectDisplayList.end();it++,i++){
			if(i==option-4){
				it->second.second= !it->second.second;
				break;
			}
		}
		glutPostRedisplay();
		break;
	}
}

//--------------------------------------------------------------------
void OpenglDisplay::displayWireframe						(   )
{
	Triangulation* triangulation = mMesher->mesh();
	glLineWidth(1.0);

	Vector3D color(0.0,0.0,0.0);
	double alpha=0.0;
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	for(auto it = triangulation->triangles()->begin();
		it != triangulation->triangles()->end(); it++)
	{
		const Vector3D & v1 = (*it)->vertex(0)->point();
		const Vector3D & v2 = (*it)->vertex(1)->point();
		const Vector3D & v3 = (*it)->vertex(2)->point();

		glBegin(GL_TRIANGLES);
			glColor3d(color.x(),color.y(),color.z());
			glVertex3d(v1.x(), v1.y(), v1.z());
			glVertex3d(v2.x(), v2.y(), v2.z());
			glVertex3d(v3.x(), v3.y(), v3.z());
		glEnd();
	}
}
void OpenglDisplay::displayFaceted							(   )
{
	Triangulation* triangulation = mMesher->mesh();
	glLineWidth(1.0);

	double bands = 1/4.0;
	double minArea = 0, maxArea = 0;
	aspectRatioRange(minArea, maxArea);
//	areaRange(minArea, maxArea);

	Vector3D red(1.0,0.0,0.0);
	Vector3D blue(0.0,0.0,1.0);
	Vector3D color;
	double alpha=0.0;
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	for(auto it = triangulation->triangles()->begin(); 
		it != triangulation->triangles()->end(); it++)
	{
		const Vector3D & v1 = (*it)->vertex(0)->point();
		const Vector3D & v2 = (*it)->vertex(1)->point();
		const Vector3D & v3 = (*it)->vertex(2)->point();
		//double area = fabs(SIGNED_AREA(v1, v2, v3));
		double area = (*it)->aspectRatio();
		alpha= (minArea - area)/(minArea - maxArea);
		//alpha= ceil(alpha/bands)*bands;

		glBegin(GL_TRIANGLES);
			color=red*alpha + blue*(1-alpha);
			glColor3d(color.x(),color.y(),color.z());
			glVertex3d(v1.x(), v1.y(), v1.z());
			glVertex3d(v2.x(), v2.y(), v2.z());
			glVertex3d(v3.x(), v3.y(), v3.z());
		glEnd();
	}
}
void OpenglDisplay::displayBoundary						(   )
{
	auto & boundary = mMesher->domain().boundary();
	glBegin(GL_LINE_LOOP);
	glLineWidth(2.5);
	glColor3d(0,0,0);
	for(size_t i = 0; i < boundary.size(); i++)
	{
			glVertex3d(boundary[i].x(), boundary[i].y(), boundary[i].z()); 
	}
	glEnd();
	glLineWidth(1.0);
}
void OpenglDisplay::displayHoles							(   )
{
	auto & boundary = mMesher->domain().boundary();
	glBegin(GL_LINE_LOOP);
	glLineWidth(2.5);
	glColor3d(0,0,0);
	for(size_t i = 0; i < boundary.size(); i++)
	{
			glVertex3d(boundary[i].x(), boundary[i].y(), boundary[i].z()); 
	}
	glEnd();
	glLineWidth(1.0);

	auto & holes = mMesher->domain().holes();
	for(size_t i = 0; i < holes.size(); i++)
	{
		glColor3d(0,0,0);
		glLineWidth(2.5);
		auto & hole = holes[i];
		glBegin(GL_LINE_LOOP);
		for(size_t j = 0; j < hole.size(); j++)
			glVertex3d(hole[j].x(), hole[j].y(), hole[j].z()); 
		glEnd();
		glLineWidth(1.0);
	}
}
void OpenglDisplay::displayConstraints						(   )
{
	auto & constraints = mMesher->domain().constraints();
	for(size_t i = 0; i < constraints.size(); i++)
	{
		glColor3d(0,0,0);
		glLineWidth(5.0);
		auto & constraint = constraints[i];
		for(size_t j = 0; j < constraint.size()-1; j++)
		{
			glBegin(GL_LINES);
				glVertex3d(constraint[j].x(), constraint[j].y(), constraint[j].z()); 
				glVertex3d(constraint[j+1].x(), constraint[j+1].y(), constraint[j+1].z()); 
			glEnd();
		}
		glLineWidth(1.0);
	}
}
void OpenglDisplay::areaRange								(   double & min, double & max)
{
	min = 1e10, max = 0;
	auto triangleSet = mMesher->mesh()->triangles();
	for(auto triItr = triangleSet->begin(); triItr != triangleSet->end(); triItr++)
	{
		double area = (*triItr)->area();
		if(area < min)
			min = area;
		if(area > max)
			max = area;
	}
}
void OpenglDisplay::aspectRatioRange						(   double & min, double & max)
{
	min = 1e10, max = 0;
	auto triangleSet = mMesher->mesh()->triangles();
	for(auto triItr = triangleSet->begin(); triItr != triangleSet->end(); triItr++)
	{
		double ratio = (*triItr)->aspectRatio();
		if(ratio < min)
			min = ratio;
		if(ratio > max)
			max = ratio;
	}
}