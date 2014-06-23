#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <xkeycheck.h>
#include <glew.h>
#include <gl.h>
#include <glaux.h>
#include <glext.h>
#include <tokamak.h>
#include <fstream> 
#include "resource.h"
#include "matrix4x4f.h"

#include "main.h"
#include "header3ds.h"
#include "CTGALoader.h" 
#include "particle.h" 
#include "tga.h"
#include"CCubeMap.h"

#include <winsock2.h>
   
#include <ctime>

#include "water/skybox.h"


int wsizex = 900;
int wsizey = 675;
int wbitdepth = 32;

float xAngle = 0;
float yAngle = 0;
#define M_PI 3.14
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "glaux.lib")

                  

	float xin=0.0f;
	float yin=0.0f;
	float zin=0.0f;
	float fin=0.0f;

	float lodkay=1.0f;

	float tr=1.0f;
	float tyr=1.0f;
	int tr1;
	int korabl=0;

	int un=3;

	int goo=0;


	float yinn=0.0f;

	int menum = 1;
	int menusp = 1;

	int bum=0;
	int fggg=0;
	bool gggg=false;
	int bum1=0;

	bool pr=true;
	bool fr=true; //flag -12 -12 -12

	CTGALoader texturerender;


	GLuint g_textureIDa[30];
	char a[10]="a";
	int m;

	GLuint	texture[10];
	GLuint	texturep[10];
	AUX_RGBImageRec *texturep1[10];
	int sF=1;
	int sN=0;

	CCubeMap cubeMap;
	



	int tt=0; //flag zaggruzki texturi sopernikov

	float money;

	char mm[5];





	

	using namespace std;

// This macro converts a Tokamak matrix (neT3) to my custom matrix type (matrix4x4f).
#define NET3_TO_MATRIX4X4f( tmat )                                        \
matrix4x4f( tmat.rot[0][0], tmat.rot[1][0], tmat.rot[2][0], tmat.pos[0],  \
            tmat.rot[0][1], tmat.rot[1][1], tmat.rot[2][1], tmat.pos[1],  \
            tmat.rot[0][2], tmat.rot[1][2], tmat.rot[2][2], tmat.pos[2],  \
            0.0f,          0.0f,          0.0f,          1.0f           );

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
HWND  g_hWnd = NULL;
HDC	  g_hDC  = NULL;
HGLRC g_hRC  = NULL;

// Tokamak related globals ----------------------------------------------------
LARGE_INTEGER g_performanceFrequency; // Used to store the performance frequency of the target box
LARGE_INTEGER g_startTime;            // Used to store the tick count at the beginning of each game loop
LARGE_INTEGER g_endTime;              // Used to store the tick count at the end of each game loop
float		  g_fFrameTime;           // Used to store the resulting time taken from start to end of each game loop (1 frame)
float		  g_fTimeSinceLastUpdate = 0.0f;
const float	  CONST_FRAME_INTERVAL = 0.016666f;

const int NUM_RED_CUBES = 5;
const int NUM_BLUE_PLATFORMS = 1;

neSimulator    *g_simulator = NULL;
neRigidBody    *g_redCubes[NUM_RED_CUBES];
neAnimatedBody *g_bluePlatform;
float g_fTiltOfBluePlatform = 0.0f;
neRigidBody    *g_redCubes6;
neRigidBody    *g_redCubes61;
neRigidBody    *g_redCubes62;
neRigidBody    *g_yellowSphere  = NULL;
neRigidBody    *g_Sphere[10];
neRigidBody    *box;
neRigidBody    *cubes[3];
neRigidBody    *land[5];
neRigidBody    *obj[10];//объекты кораблей
neRigidBody    *obj1[10];//объекты кораблей

neJoint * joint2;
neJoint * joint;
neJoint * joint1;
neJoint * joint4;

neGeometry *cubeGeometry;


neV3 tps;
bool tpok=false;

float lodkax;
float lodkaz;

float lodkaxs;
float lodkazs;

bool play = false;
bool star = false;

char serv_addr[100];
char buff[100];
SOCKET my_sock;
HOSTENT *hst;
sockaddr_in dest_addr;


//-----------------------------------------------------------------------------

POINT  g_ptLastMousePosit;
POINT  g_ptCurrentMousePosit;
bool   g_bMousing = false;
float  g_fMoveSpeed = 25.0f;
float  g_fElpasedTime;
//float  g_fElpasedTimeg;
int   timegg = 0;
double g_dCurTime;
double g_dLastTime;

vector3f g_vEye(-4.3f, 3.9f, 0.0f);  // Eye Position  1 -2.9
vector3f g_vRight(1.0f, 0.0f, 0.0f);  // Right Vector

vector3f g_vLook()
{
	vector3f look(1.f, 0.f, 0.f);

	look.x = cos(xAngle / 180 * M_PI) * cos(yAngle / 180 * M_PI);
	look.z = -sin(xAngle / 180 * M_PI) * cos(yAngle / 180 * M_PI);
	look.y =  sin(yAngle / 180 * M_PI);

	return look;
}

vector3f g_vUp()
{
	if (fabs(g_vLook().y) < 1e-5)
	{
		return vector3f(0.f, 1.f, 0.f);
	}
	vector3f up;
	vector3f look(g_vLook());
	if (g_vLook().y > 0)
	{
		up = vector3f(-look.x, (look.x * look.x + look.z * look.z) / look.y, -look.z);
	}
	else
	{
		up = vector3f(look.x, -(look.x * look.x + look.z * look.z) / look.y, look.z);
	}
	up.normalize();
	return up;
}

//vector3f g_vEye(-2.9f, 3.9f, 2.9f);  // Eye Position  1 -2.9
//vector3f g_vLook(0.6f, -0.2f, -0.6f); // Look Vector 1 0.6
//vector3f g_vUp(0.0f, 1.0f, 0.0f);     // Up Vector
//vector3f g_vRight(1.0f, 0.0f, 0.0f);  // Right Vector


struct Vertex
{
	// GL_N3F_V3F
	float nx, ny, nz;
	float x, y, z;
};

Vertex g_cubeVertices[] =
{
	{ 0.0f, 0.0f, 1.0f,  -0.5f,-0.5f, 0.5f },
	{ 0.0f, 0.0f, 1.0f,   0.5f,-0.5f, 0.5f },
	{ 0.0f, 0.0f, 1.0f,   0.5f, 0.5f, 0.5f },
	{ 0.0f, 0.0f, 1.0f,  -0.5f, 0.5f, 0.5f },

	{ 0.0f, 0.0f,-1.0f,  -0.5f,-0.5f,-0.5f },
	{ 0.0f, 0.0f,-1.0f,  -0.5f, 0.5f,-0.5f },
	{ 0.0f, 0.0f,-1.0f,   0.5f, 0.5f,-0.5f },
	{ 0.0f, 0.0f,-1.0f,   0.5f,-0.5f,-0.5f },

	{ 0.0f, 1.0f, 0.0f,  -0.5f, 0.5f,-0.5f },
	{ 0.0f, 1.0f, 0.0f,  -0.5f, 0.5f, 0.5f },
	{ 0.0f, 1.0f, 0.0f,   0.5f, 0.5f, 0.5f },
	{ 0.0f, 1.0f, 0.0f,   0.5f, 0.5f,-0.5f },

	{ 0.0f,-1.0f, 0.0f,  -0.5f,-0.5f,-0.5f },
	{ 0.0f,-1.0f, 0.0f,   0.5f,-0.5f,-0.5f },
	{ 0.0f,-1.0f, 0.0f,   0.5f,-0.5f, 0.5f },
	{ 0.0f,-1.0f, 0.0f,  -0.5f,-0.5f, 0.5f },

	{ 1.0f, 0.0f, 0.0f,   0.5f,-0.5f,-0.5f },
	{ 1.0f, 0.0f, 0.0f,   0.5f, 0.5f,-0.5f },
	{ 1.0f, 0.0f, 0.0f,   0.5f, 0.5f, 0.5f },
	{ 1.0f, 0.0f, 0.0f,   0.5f,-0.5f, 0.5f },

	{-1.0f, 0.0f, 0.0f,  -0.5f,-0.5f,-0.5f },
	{-1.0f, 0.0f, 0.0f,  -0.5f,-0.5f, 0.5f },
	{-1.0f, 0.0f, 0.0f,  -0.5f, 0.5f, 0.5f },
	{-1.0f, 0.0f, 0.0f,  -0.5f, 0.5f,-0.5f } 
};

bool sp;

//модели
int countpoligons;
CLoad3DS g_Load3ds;				// Наш 3DS класс. 
t3DModel g_3DModel[5];				// модель лодки
t3DModel g_3DPushka[1];				// модель лодки
t3DModel g_3DJadro[1];				// модель лодки
t3DModel g_ModelLand;	        // модель ландшафта
t3DModel g_ModelLand5;	        // модель ландшафта
t3DModel g_3DModel1[5];				// модель лодки части

//parametri na vhod igroka
int x11=0; //level
int y11=0; //ulushenie
int z11=0; //
int f11=0; //textura za kogo igraet

int f111=0; //version in
int vr=13; //tver 

//texturi
CTGALoader texturekorpus[1];
CTGALoader texturebochki[1];

CTGALoader textureparus[10];

CTGALoader texturepushki[1];

CTGALoader texturejadro[1];


//текстуры
CTGALoader texturebox[100];


struct gooo //structura popadani
{
	int x1; //machta
	int x2; //zad
	int x3; //pered

	int n2;
	int n3;
};

gooo bgoo[10]; //popadani



#define SKYFRONT 0						// Give Front ID = 0
#define SKYBACK  1						// Give Back  ID = 1
#define SKYLEFT  2						// Give Left  ID = 2
#define SKYRIGHT 3						// Give Right ID = 3
#define SKYUP    4						// Give Up    ID = 4
#define SKYDOWN  5						// Give Down  ID = 5
CTGALoader SkyboxTexture[6];

//дескрипторы шейдера света
GLhandleARB glContext;
GLhandleARB glVertexShader;
GLhandleARB glPixelShader;

//переменные шейдера
unsigned int glTexture;
unsigned int glLPos;
unsigned int glLColor;
unsigned int glCPos;
unsigned int glF1;

//дескрипторы шейдера билборда
GLhandleARB glC;
GLhandleARB glVS;
GLhandleARB glPS;

//дескрипторы шейдера ландшафта
GLhandleARB SLand;
GLhandleARB VSLand;
GLhandleARB PSLand;

//дескрипторы шейдера воды
//GLhandleARB SWater;
//GLhandleARB VSWater;
//GLhandleARB PSWater;

//дескрипторы шейдера key
GLhandleARB key;
GLhandleARB VdSkey; 
GLhandleARB PSkedy;

//переменные
float lPos[] = { 0.0f, 4.0f, 3.0f, 1.0f };
float lColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

//переменные для шейдера ландшафта
unsigned int TextureLand;
unsigned int TextureLandP;




//переменные для шейдера воды
unsigned int TextureWater; // карта нормалей
unsigned int TextureNV; // карта отражений (сцены над водой)
unsigned int TextureNV1; // карта глубины сцены
unsigned int TextureNV2; // карта глубины сцены с позиции источника света
unsigned int TextureNV3; // края

unsigned int TimeVater;
unsigned int glLiSour; //light_source
unsigned int glViPos; //view_position
unsigned int glWatCol; //water_color
unsigned int glSpecCol; //specular_color
unsigned int glT; //time_density_clipplane
//uint u_timePrm;
float u_time = 0.f;


float time1=0.4f;
bool fa=true;




//скорость лодки
float speed = 0.0f;
float g_fTiltOfRedPlatform = 0.0f;
int flag = 0;

unsigned int listBase;
unsigned int listBase1;
unsigned int listBase5;

GLUquadricObj *pObj = gluNewQuadric();

struct TableData
	{
		neV3 size;
		neV3 pos;
	};

TableData tableData[2] =
{
	{{3.0f, 0.3f, 2.0f}, {0.0f, 0.0f, 0.0f}},
	{{1.0f, 1.7f, 2.0f}, {0.0f, 1.0f, 0.0f}}
};

int boom=0;
bool zarad=true;

CParticle Particle[50];
int countp = 0;
int st = 0;

CParticle ParticleBum[50];
CParticle ParticleBumSh[50];

//chastisi dim
CParticle ParticleF[50];

//chastisi volna
CParticle ParticleV[100];
CParticle ParticleV2[100];
int cv=0;
int cv2=0;





char strp[50];
char strt[50];
int mous = 0;
POINT MouseCurPos;

//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void init(void);
void render(void);
void shutDown(void);
void getRealTimeUserInput(void);
void updateViewMatrix(void);
void initPhysics(void);
void updatePhysics(void);

char *loadShader(char *shaderName);
unsigned int CreateFontz(char *fontName, int Size);
void keyboard();
void buildAxisAlignedBBMatrix(float m[16], float x, float y, float z);
void RotateAroundPoint(vector3f vCenter, float angle, float x, float y, float z);
void  MakeScreenShot();



//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR     lpCmdLine,
					int       nCmdShow )
{
	WNDCLASSEX winClass;
	MSG        uMsg;

    memset(&uMsg,0,sizeof(uMsg));

	winClass.lpszClassName = "MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
    winClass.hIcon	       = LoadIcon(hInstance, (LPCTSTR)IDI_OPENGL_ICON);
    winClass.hIconSm	   = LoadIcon(hInstance, (LPCTSTR)IDI_OPENGL_ICON);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;
	
	if( !RegisterClassEx(&winClass) ) 
		return E_FAIL;

	g_hWnd = CreateWindowEx( NULL, "MY_WINDOWS_CLASS", 
                             "",
						     WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
					         0, 0, wsizex, wsizey, NULL, NULL, hInstance, NULL );
	

	if( g_hWnd == NULL )
		return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );
    UpdateWindow( g_hWnd );

	init();
	g_dLastTime = timeGetTime();

	while( uMsg.message != WM_QUIT )
	{
		if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
		}
        else
		{
			g_dCurTime     = timeGetTime();
			g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);

			u_time += g_fElpasedTime;

			//if(mous==2)
			//{
			if(timegg<50)
			{
				g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);
				timegg+=1;
			}
			//}


			g_dLastTime    = g_dCurTime;


				render();


		}
	}

	shutDown();

    UnregisterClass( "MY_WINDOWS_CLASS", winClass.hInstance );

	return uMsg.wParam;
}

//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND   hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{
    switch( msg )
	{
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;

	//			case VK_F1:
	//				// Pull up on the bottom cube


	//				neV3 vel;
	//				vel.Set( -8.0f, 7.0f, 0.0f );
	//				box->SetVelocity( vel );
	//				break;

	//			case VK_F2:
	//				g_fTiltOfBluePlatform += 0.01f;
	//				break;

	/*			case VK_F4:
					strcpy(name, "1212");
					break;*/

	//			case VK_F5:
	//				neV3 posy;
	//				neV3 poss;
	//				
	//				boom=true;
	//				break;

	//			case VK_F6:
	//			
	//				flag=1;
	//				g_fTiltOfRedPlatform += 0.03f;

	//				speed-=0.5f;
	//				if (speed<0)
	//					speed=0;
	//				joint1->SetMotor(neJoint::NE_MOTOR_SPEED, speed, 10000.f);
	//				joint4->SetMotor(neJoint::NE_MOTOR_SPEED, speed, 10000.f);
	//				
	//				if(speed==0)
	//				{
	//				joint2->SetMotor(neJoint::NE_MOTOR_SPEED, 0.0f, 10000.f);
	//				joint->SetMotor(neJoint::NE_MOTOR_SPEED, 0.0f, 10000.f);
	//				}

	//				break;

 //               case VK_F7:

	//				flag=1;
	//				g_fTiltOfRedPlatform += 0.03f;

	//				speed+=0.5f;
	//				joint1->SetMotor(neJoint::NE_MOTOR_SPEED, speed, 10000.f);
	//				joint4->SetMotor(neJoint::NE_MOTOR_SPEED, speed, 10000.f);

	//				if(speed>0)
	//				{
 //                   joint2->SetMotor(neJoint::NE_MOTOR_SPEED, 3.0f, 10000.f);
	//				joint->SetMotor(neJoint::NE_MOTOR_SPEED, 3.0f, 10000.f);
	//				}

	//				break;

	//			case VK_F8:

	//				//neV3 poss;
	//				//neV3 posy;
	//				neV3 null;

	//				null.Set(0.0f, 0.0f, 0.0f);
	//				g_yellowSphere->SetVelocity( null );

	//				posy.Set( 0.0f, 1.5f, 0.0f);
	//				poss.Set( (g_redCubes[0]->GetPos()) );
	//				g_yellowSphere->SetPos( (posy + poss) );

	//				null.Set( 0.0f, 0.0f, 0.0f);
	//				null.Set((g_redCubes61->GetPos()) );
	//				g_yellowSphere->SetVelocity((null - (posy + poss))*3+ posy*5); 


	////ofstream fff11111;
	////fff11111.open("userc.txt");
	////fff11111<<"g_vLook "<< g_vLook.x <<" "<<g_vLook.y<<" "<<g_vLook.z<<"/n";
	////fff11111.close(); 


 // 
	//				//neV3 poss;
	//				//neV3 vels;
	//				//neV3 posy;
	//				//posy.Set( 0.0f, 1.0f, 0.0f);

	//				//poss.Set( (g_redCubes[0]->GetPos()) );
	//				//g_yellowSphere->SetPos( poss + posy );
	//				//vels.Set( g_vLook.x*5.0f, 3.0f, g_vLook.z*5.0f ); 
	//				//g_yellowSphere->SetVelocity( vels );

	//				break;



	//		case VK_F3:

	//				//neV3 poss;
	//				//neV3 posy;
	//				//neV3 null;

	//				null.Set(0.0f, 0.0f, 0.0f);
	//				g_yellowSphere->SetVelocity( null );

	//				posy.Set( 0.0f, 1.5f, 0.0f);
	//				poss.Set( (g_redCubes[0]->GetPos()) );
	//				g_yellowSphere->SetPos( (posy + poss) );

	//				null.Set( 0.0f, 0.0f, 0.0f);
	//				null.Set((g_redCubes6->GetPos()) );
	//				g_yellowSphere->SetVelocity((null - (posy + poss))*3+ posy*5); 

	//				break;




			}
		}
        break;

        case WM_LBUTTONDOWN:
		{
			g_bMousing = true;
		}
		break;

		case WM_LBUTTONUP:
		{
			g_bMousing = false;
		}
		break;

		case WM_SIZE:
		{
			int nWidth  = LOWORD(lParam); 
			int nHeight = HIWORD(lParam);
			glViewport(0, 0, nWidth, nHeight);

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			gluPerspective( 90.0, (GLdouble)nWidth / (GLdouble)nHeight, 0.1, 100.0);
		}
		break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);	
		}

        case WM_DESTROY:
		{
            PostQuitMessage(0);
		}
        break;
		
		default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: getRealTimeUserInput()
// Desc: 
//-----------------------------------------------------------------------------
void getRealTimeUserInput( void )
{
	//
	// Get mouse input...
	//

	POINT mousePosit;
	GetCursorPos( &mousePosit );
	ScreenToClient( g_hWnd, &mousePosit );

    g_ptCurrentMousePosit.x = mousePosit.x;
    g_ptCurrentMousePosit.y = mousePosit.y;

	matrix4x4f matRotation;

    if( g_bMousing )
    {
		int nXDiff = (g_ptCurrentMousePosit.x - g_ptLastMousePosit.x);
        int nYDiff = (g_ptCurrentMousePosit.y - g_ptLastMousePosit.y);
        xAngle -= nXDiff / 3.f;
		float limit = 65;
		if (yAngle - nYDiff / 3.f > limit)
		{
			yAngle = limit;
		}
		else if (yAngle - nYDiff / 3.f < - limit)
		{
			yAngle = - limit;
		}
		else
		{
			yAngle -= nYDiff / 3.f;
		}




		if ((g_ptLastMousePosit.y>600) & (g_ptLastMousePosit.x <50))
		{
			menum = 1;
	        goto mf;
	    }

	if(y11>1)
	{
	if ((g_ptLastMousePosit.y>600) & (g_ptLastMousePosit.x >50) & (g_ptLastMousePosit.x <100))
		{
			menum = 2;
			goto mf;
     	}
	}
	 
	if ((g_ptLastMousePosit.y<400) & (g_ptLastMousePosit.x <200))
		{
			menusp = 2;
			goto mf;
     	}

mf:
	;
    }

    g_ptLastMousePosit.x = g_ptCurrentMousePosit.x;
    g_ptLastMousePosit.y = g_ptCurrentMousePosit.y;



	//
	// Get keyboard input...
	//

	unsigned char keys[256];
	GetKeyboardState( keys ); 

	vector3f tmpLook  = g_vLook();
	vector3f tmpRight = g_vRight;

	////// Up Arrow Key - View moves forward
	//if( keys[VK_UP] & 0x80 )
	//	g_vEye -= tmpLook*-g_fMoveSpeed*g_fElpasedTime;

	//// Down Arrow Key - View moves backward
	//if( keys[VK_DOWN] & 0x80 )
	//	g_vEye += (tmpLook*-g_fMoveSpeed)*g_fElpasedTime;

	//// Left Arrow Key - View side-steps or strafes to the left
	//if( keys[VK_LEFT] & 0x80 )
	//	g_vEye -= (tmpRight*g_fMoveSpeed)*g_fElpasedTime;

	//// Right Arrow Key - View side-steps or strafes to the right
	//if( keys[VK_RIGHT] & 0x80 )
	//	g_vEye += (tmpRight*g_fMoveSpeed)*g_fElpasedTime;

	//// Home Key - View elevates up
	//if( keys[VK_HOME] & 0x80 )
	//	g_vEye.y += g_fMoveSpeed*g_fElpasedTime; 

	//// End Key - View elevates down
	//if( keys[VK_END] & 0x80 )
	//	g_vEye.y -= g_fMoveSpeed*g_fElpasedTime;
}

//-----------------------------------------------------------------------------
// Name : updateViewMatrix() 
// Desc : Builds a view matrix suitable for OpenGL.
//
// Here's what the final view matrix should look like:
//
//  |  rx   ry   rz  -(r.e) |
//  |  ux   uy   uz  -(u.e) |
//  | -lx  -ly  -lz   (l.e) |
//  |   0    0    0     1   |
//
// Where r = Right vector
//       u = Up vector
//       l = Look vector
//       e = Eye position in world space
//       . = Dot-product operation
//
//-----------------------------------------------------------------------------
void updateViewMatrix( void )
{
    matrix4x4f view;
    view.identity();

   // g_vLook.normalize();

    g_vRight = crossProduct(g_vLook(), g_vUp());
    g_vRight.normalize();

    view.m[0] =  g_vRight.x;
    view.m[1] =  g_vUp().x;
    view.m[2] = -g_vLook().x;
    view.m[3] =  0.0f;

    view.m[4] =  g_vRight.y;
    view.m[5] =  g_vUp().y;
    view.m[6] = -g_vLook().y;
    view.m[7] =  0.0f;

    view.m[8]  =  g_vRight.z;
    view.m[9]  =  g_vUp().z;
    view.m[10] = -g_vLook().z;
    view.m[11] =  0.0f;

    view.m[12] = -dotProduct(g_vRight, g_vEye);
    view.m[13] = -dotProduct(g_vUp(), g_vEye);
    view.m[14] =  dotProduct(g_vLook(), g_vEye);
    view.m[15] =  1.0f;

    glMultMatrixf( view.m );
}


//-----------------------------------------------------------------------------
// Name: init()
// Desc: 
//-----------------------------------------------------------------------------
void init( void )
{
	ofstream f;
	f.open("log.txt", ios::in);

	GLuint PixelFormat;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW |PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 16;
    pfd.cDepthBits = 16;
	
	g_hDC = GetDC( g_hWnd );
	PixelFormat = ChoosePixelFormat( g_hDC, &pfd );
	SetPixelFormat( g_hDC, PixelFormat, &pfd);
	g_hRC = wglCreateContext( g_hDC );
	wglMakeCurrent( g_hDC, g_hRC );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    glEnable( GL_DEPTH_TEST );
	//glEnable( GL_LIGHTING );
	glEnable( GL_NORMALIZE );
	glClearDepth(1.0f);                         
    glDepthFunc(GL_LEQUAL);      
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glShadeModel(GL_SMOOTH);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 90.0f, float(wsizex) / wsizey, 0.1f, 3000.0f);

    initPhysics();
	glewInit();


	float posxedro1(0.f);
	float poszedro1(0.f);
	//ocean = new Ocean(wsizex, wsizey);
	SkyBox::Startup();

	//грузим модели
	g_Load3ds.Import3DS(&g_3DModel[0], "5.3ds");

	


	
	





	//загрузка текстуру для кубика
   //texturebox[0].LoadTGA("data//texture//parusa.tga");
   texturebox[0].LoadTGA("derevo.tga");
   glGenTextures(1, &texturebox[0].ID);
   glBindTexture(GL_TEXTURE_2D, texturebox[0].ID);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   int type = texturebox[0].type == 3 ? GL_RGB : GL_RGBA;
   gluBuild2DMipmaps(GL_TEXTURE_2D, type, texturebox[0].imageWidth, texturebox[0].imageHeight,
                     type, GL_UNSIGNED_BYTE, texturebox[0].image);

   texturebox[1].LoadTGA("korpus.tga");
   glGenTextures(1, &texturebox[1].ID);
   glBindTexture(GL_TEXTURE_2D, texturebox[1].ID);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   type = texturebox[1].type == 3 ? GL_RGB : GL_RGBA;
   gluBuild2DMipmaps(GL_TEXTURE_2D, type, texturebox[1].imageWidth, texturebox[1].imageHeight,
                     type, GL_UNSIGNED_BYTE, texturebox[1].image);

  

   int x = 800;
   int y = 600;

   //texturerender
   unsigned int *pTexture = NULL;// указатель для сохранения данных текстуры
   pTexture = new unsigned int [x * y * 3];//текстурный массив для сохранения текстуры
   memset(pTexture, 0, x * y * 3 * sizeof(unsigned int));//память под массив текстуры и привяжем к pTexture
   glGenTextures(1, &texturerender.ID);
   glBindTexture(GL_TEXTURE_2D, texturerender.ID);
   glTexImage2D(GL_TEXTURE_2D, 0, 3, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, pTexture);//создаем текстуру и сохраняем в памяти
   
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   delete [] pTexture;//удалить временный массив


   for(int i=9; i<80; i++)
   {

	   switch(i)
		{
	
			break;
		case 12: texturebox[i].LoadTGA("front.tga");
			break;
	
			break;
		case 20: texturebox[i].LoadTGA("rr.tga");
			break;
		
			break;
		case 22: texturebox[i].LoadTGA("2.tga");
			break;
		
			break;
		case 24: texturebox[i].LoadTGA("normal1.tga");
			break;
	
			break;
		case 26: texturebox[i].LoadTGA("f3.tga");
			break;
	
	    }
  
   glGenTextures(1, &texturebox[i].ID);
   glBindTexture(GL_TEXTURE_2D, texturebox[i].ID);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   type = texturebox[i].type == 3 ? GL_RGB : GL_RGBA;
   gluBuild2DMipmaps(GL_TEXTURE_2D, type, texturebox[i].imageWidth, texturebox[i].imageHeight,
                     type, GL_UNSIGNED_BYTE, texturebox[i].image);
   }


  



 
 




	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glEnable(GL_TEXTURE_2D);//разрешить текстуры
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);


	 



   //расширения для использования шейдеров
   char *extensions = (char*)glGetString(GL_EXTENSIONS);

   // Check for the necessary extensions.
   if(strstr(extensions, "GL_ARB_shading_language_100") == NULL)
      {
         MessageBox(NULL, "GL_ARB_shading_language_100 not supported!", "Error...", MB_OK);
         //return false;
      }

   if(strstr(extensions, "GL_ARB_shader_objects") == NULL)
      {
         MessageBox(NULL, "GL_ARB_shader_objects not supported!", "Error...", MB_OK);
         //return 0;
      }

  

   if(strstr(extensions, "GL_ARB_texture_cube_map") == 0)
      MessageBox(NULL, "Error ", "Error ", MB_OK);

   cubeMap.LoadTGAFile("posx.tga", 
					   "negx.tga",
	                   "posy.tga", 
					   "negy.tga",
                       "posz.tga", 
					   "negz.tga");


   //получение адресов функций шейдера
   //glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
   //glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
   //glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
   //glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
   //glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
   //glDeleteObjectARB  = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
   //glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
   //glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
   //glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
   //glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
   //glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
   //glUniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
   //glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
   //glUniform1fARB = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
   //glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
   //glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
   //

   PFNGLTEXIMAGE3DPROC glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
   
   
   char error[40960]; 
   int result;



   ////////////////////////////////////////////////////////////////////подготовка шейдера лодки
   glContext = glCreateProgramObjectARB();//создать объект программы

   //вершинный шейдер
   char *ShaderCode = {"#version 110\n varying vec2 texCoord0; varying vec3 normal; varying vec4 pos; void main(){      texCoord0 = gl_MultiTexCoord0.xy;   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;   normal = gl_NormalMatrix * gl_Normal;   pos = gl_Vertex;}"};
	   //loadShader("shader//ShaderVS.glsl");//загрузка текста шейдера
   //if(!ShaderCode) return false;
   glVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);//создать вершинный шейдер
   glShaderSourceARB(glVertexShader, 1, (const char**)&ShaderCode, NULL);//передать шейдеру его исходный код
   
   //compilv
   glCompileShaderARB(glVertexShader);//компилируем шейдер 
   glGetObjectParameterivARB(glVertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &result);//результат компиляции шейдера
   if(!result)
      {
         glGetInfoLogARB(glVertexShader, sizeof(error), NULL, error);
         MessageBox(NULL, error, "1111Error v ...", MB_OK);
      }

   glAttachObjectARB(glContext, glVertexShader);//присоединяем шейдер к программе
   //delete[] ShaderCode;//очистка памяти
   //ShaderCode = NULL;//очистка памяти

   //фрагментный шейдер
   char *ShaderCode11 = {"#version 110\n	   varying vec2 texCoord0;varying vec3 normal;varying vec4 pos;uniform sampler2D texturebox;uniform vec4 post;uniform vec4 cr;uniform vec4 poss;uniform vec4 lp;void main(){    vec4 lpos2 = vec4(0.0, 15.0, -3.0, 1.0);   vec4 lpos1 = vec4(0.0, 15.0, 3.0, 1.0);   vec3 n = normalize(normal);   vec4 fuse = vec4(0.52,0.52,0.52,1.0);    vec4 ec = normalize(lp - pos);   float ht = max(dot(n, ec.xyz), 0.0);   vec4 e = fuse * cr * ht;   vec4 ec1 = normalize(lpos2 - pos);   float ht1 = max(dot(n, ec1.xyz), 0.0);   vec4 e1 = fuse * cr * ht1;   vec4 mbient = vec4(0.32,0.32,0.32,1.0);   vec4 olor = vec4(0.32,0.32,0.32,1.0);   vec4 t = mbient * olor;   float ss = 90.0;   vec4 ve = vec4(0.12,0.12,0.12,1.0);   vec4 vv = normalize(poss - pos);   vec4 halfVec = normalize(ec + vv);      float ght = pow(max(dot(n, halfVec.xyz), 0.0), ss);   vec4 pec = vec4(0.82,0.82,0.82,1.0);   if(ht <= 0.0) ght = 0.0;   vec4 ar = pec * cr * ght;   gl_FragColor  = (ve + t + (e + e1) + ar) * texture2D(texturebox, texCoord0);   gl_FragColor.w = 1.0;}"};
	   //loadShader("shader//ShaderPS.glsl");
   glPixelShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
   glShaderSourceARB(glPixelShader, 1, (const char**)&ShaderCode11, NULL);
   
   //compilp
   glCompileShaderARB(glPixelShader);
   glGetObjectParameterivARB(glPixelShader, GL_OBJECT_COMPILE_STATUS_ARB, &result);
   if(!result)
      {
         glGetInfoLogARB(glPixelShader, sizeof(error), NULL, error);
         MessageBox(NULL, error, "1111Error p ...", MB_OK);
      }

   glAttachObjectARB(glContext, glPixelShader);
   //delete[] ShaderCode;
   //ShaderCode = NULL;

   //link
   glLinkProgramARB(glContext);//линкуем программу
   glGetObjectParameterivARB(glContext, GL_OBJECT_LINK_STATUS_ARB, &result);
 
    
   if(!result)
      {
         glGetInfoLogARB(glContext, sizeof(error), NULL, error);
         MessageBox(NULL, error, "1111Error linking shaders...", MB_OK);
         //return false;
      }
   //
    



   //////////////////////////////////////////////////////////////////////подготовка шейдера билборда
   //glC = glCreateProgramObjectARB();//создать объект программы

   ////вершинный шейдер
   //char *SCode = loadShader("shader//VS.glsl");//загрузка текста шейдера
   ////if(!ShaderCode) return false;
   //glVS = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);//создать вершинный шейдер
   //glShaderSourceARB(glVS, 1, (const char**)&SCode, NULL);//передать шейдеру его исходный код
   //glCompileShaderARB(glVS);//компилируем шейдер 
   //glGetObjectParameterivARB(glVS, GL_OBJECT_COMPILE_STATUS_ARB, &result);//результат компиляции шейдера
   //glAttachObjectARB(glC, glVS);//присоединяем шейдер к программе
   //delete[] SCode;//очистка памяти
   //SCode = NULL;//очистка памяти

   ////фрагментный шейдер
   //SCode = loadShader("shader//PS.glsl");
   //glPS = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
   //glShaderSourceARB(glPS, 1, (const char**)&SCode, NULL);
   //glCompileShaderARB(glPS);
   //glGetObjectParameterivARB(glPS, GL_OBJECT_COMPILE_STATUS_ARB, &result);
   //glAttachObjectARB(glC, glPS);
   //delete[] SCode;
   //SCode = NULL;

   //glLinkProgramARB(glC);//линкуем программу
   //glGetObjectParameterivARB(glC, GL_OBJECT_LINK_STATUS_ARB, &result);

   //if(!result)
   //   {
   //      glGetInfoLogARB(glC, sizeof(error), NULL, error);
   //      MessageBox(NULL, error, "Error linking shaders...", MB_OK);
   //      //return false;
   //   }



   //связать переменные с шейдером
   glTexture = glGetUniformLocationARB(glContext, "texturebox");
   glLPos = glGetUniformLocationARB(glContext, "post");
   glLColor = glGetUniformLocationARB(glContext, "cr");
   glCPos = glGetUniformLocationARB(glContext, "poss");
   glF1 = glGetUniformLocationARB(glContext, "lp");



   /////////////////////////////////////////////////////////////////////шейдер обработки ландшафта
   //подготовка шейдера
   SLand = glCreateProgramObjectARB();//создать объект программы

   //вершинный шейдер
   char *CodesLand = { "#version 110\n varying vec2 texCoord0; varying vec4 pos; void main(){ texCoord0 = gl_MultiTexCoord0.xy; gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; pos = gl_Vertex; } " };
	   //loadShader("shader//LandVS.glsl");//загрузка текста шейдера

   //MessageBox(NULL, CodesLand, "2222Error p ...", MB_OK);

   //if(!CodesLand) return false;
   VSLand = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);//создать вершинный шейдер
   glShaderSourceARB(VSLand, 1, (const char**)&CodesLand, NULL);//передать шейдеру его исходный код
   
   glCompileShaderARB(VSLand);//компилируем шейдер 
   glGetObjectParameterivARB(VSLand, GL_OBJECT_COMPILE_STATUS_ARB, &result);//результат компиляции шейдера
   if(!result)
      {
         glGetInfoLogARB(VSLand, sizeof(error), NULL, error);
         MessageBox(NULL, error, "2222Error v ...", MB_OK);
      }

   glAttachObjectARB(SLand, VSLand);//присоединяем шейдер к программе
   //delete[] CodesLand;//очистка памяти
   //CodesLand = NULL;//очистка памяти

   //фрагментный шейдер
   char *CodesLand1={"#version 110\n varying vec2 texCoord0; varying vec4 pos; uniform sampler2D TextureMesh; uniform sampler2D TextureMesh2; void main() { vec4 cvet; vec4 cv = vec4(0.69, 0.77, 0.78, 1.0); cvet = vec4(texture2D(TextureMesh, texCoord0))+vec4(texture2D(TextureMesh2, texCoord0))*0.6; if(cvet.x>0.69)cvet.x=0.69; if(cvet.y>0.77)cvet.y=0.77; if(cvet.z>0.78)cvet.z=0.78; if(pos.y<-1.0) cvet.xyz=vec3(0.36,0.50,0.68);  if(pos.y>30.0)			cvet.xyz=vec3(0.25,0.37,0.55); gl_FragColor.xyz = cvet.xyz; }  "};
	   //char *CodesLand1=loadShader("shader//LandPS.glsl");
   PSLand = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
   glShaderSourceARB(PSLand, 1, (const char**)&CodesLand1, NULL);
   
   glCompileShaderARB(PSLand); 
   glGetObjectParameterivARB(PSLand, GL_OBJECT_COMPILE_STATUS_ARB, &result);
   if(!result)
      {
         glGetInfoLogARB(PSLand, sizeof(error), NULL, error);
         MessageBox(NULL, error, "2222Error p ...", MB_OK);
      }

   glAttachObjectARB(SLand, PSLand);
   //delete[] CodesLand;
   //CodesLand = NULL;

   glLinkProgramARB(SLand);//линкуем программу
   glGetObjectParameterivARB(SLand, GL_OBJECT_LINK_STATUS_ARB, &result);
   
   if(!result)
      {
         glGetInfoLogARB(SLand, sizeof(error), NULL, error);
         MessageBox(NULL, error, "2222Error linking shaders...", MB_OK);
         //return false;
      } 
 
   // связать переменные с шейдером
   TextureLand = glGetUniformLocationARB(SLand, "TextureMesh");
   TextureLandP = glGetUniformLocationARB(SLand, "TextureMesh2");

    /////////////////////////////////////////////////////////////////////шейдер обработки воды
   //подготовка шейдера
   //SWater = glCreateProgramObjectARB();//создать объект программы

   //вершинный шейдер
   //char *CodesWaterv = {"#version 110\n varying vec4 vertex;varying vec4 shadow_proj_coords;varying vec4 proj_coords;void main(){ 	 mat4 edmat = mat4(1.0, 0.0, 0.0, 0.0,			       0.0, 1.0, 0.0, 0.0,			       0.0, 0.0, 1.0, 0.0,			       0.0, 0.0, 0.0, 1.0); mat4 shadow_matrix = edmat; vertex = gl_Vertex; shadow_proj_coords = shadow_matrix * vertex;	    gl_TexCoord[0] = gl_MultiTexCoord0; gl_Position = gl_ModelViewProjectionMatrix * vertex; proj_coords = gl_Position;}"};
	//char *CodesWaterv=loadShader("shader//WaterVS.glsl");//загрузка текста шейдера
   //if(!CodesWater) return false;
   //VSWater = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);//создать вершинный шейдер
   //glShaderSourceARB(VSWater, 1, (const char**)&CodesWaterv, NULL);//передать шейдеру его исходный код
   //
   //glCompileShaderARB(VSWater);//компилируем шейдер 
   //glGetObjectParameterivARB(VSWater, GL_OBJECT_COMPILE_STATUS_ARB, &result);//результат компиляции шейдера
   //if(!result)
   //   {
   //      glGetInfoLogARB(VSWater, sizeof(error), NULL, error);
   //      MessageBox(NULL, error, "3333Error v ...", MB_OK);
   //   }
   // 
   //glAttachObjectARB(SWater, VSWater);//присоединяем шейдер к программе
   //delete[] CodesWater;//очистка памяти
   //CodesWater = NULL;//очистка памяти

   //фрагментный шейдер
   /*
   char *CodesWaterb = {
	   "#version 110\n uniform sampler2D normal_texture;"\	   
	   "uniform sampler2D refract_texture;"\
	   "uniform sampler2D reflect_texture;"\
	   "uniform sampler2D depth_texture;"\
	   "uniform sampler2DShadow shadow_texture;"\
	   "uniform sampler2D texture_r;"\
	   "uniform vec4 time_density_clipplane;"\
	   "uniform vec3 light_source;"\
	   "uniform vec3 view_position;"\
	   "uniform vec4 water_color;"\
	   "uniform vec3 specular_color;"\
	   "varying vec4 vertex;"\
	   "varying vec4 shadow_proj_coords;"\
	   "varying vec4 proj_coords;"\
	   "const float fNormalScale = 1.0;"\
	   "float SampleShadow(vec3 shadow_tc)"\
	   "{"\
			"return shadow2D(shadow_texture, shadow_tc).x;"\
		"}"\
		"float calculate_linear_depth(float value)"\
	   "{"\
	   "return time_density_clipplane.w * time_density_clipplane.z / ( time_density_clipplane.w - value * (time_density_clipplane.w - time_density_clipplane.z) );"\
		"}"\
		"void main(){"\
		"vec3 normal = 2.0 * texture2D(normal_texture, gl_TexCoord[0].st * fNormalScale   + vec2(time_density_clipplane.x) ).xzy - vec3(1.0);"\
		"normal = normalize(normal + vec3(0.0, 1.0, 0.0));"\
		"float fHDRscale = water_color.w;"\
		"vec3 lpnorm   = normalize(light_source);"\
		"vec3 vpnorm   = normalize(view_position - vertex.xyz);"\
		"vec3 proj_tc  = 0.5 * proj_coords.xyz / proj_coords.w + 0.5;"\
		"float fresnel = 1.0 - dot(vpnorm, normal);"\
		"float fOwnDepth = calculate_linear_depth(proj_tc.z);"\
		"float fSampledDepth = texture2D(depth_texture, proj_tc.xy).x;"\
		"fSampledDepth       = calculate_linear_depth(fSampledDepth);"\
		"float fLinearDepth  = fSampledDepth - fOwnDepth;"\
		"float fExpDepth = 1.0 - exp( -time_density_clipplane.y * fLinearDepth);"\
		"float fExpDepthHIGH = 1.0 - exp( -0.95 * fLinearDepth );"\
		"vec3 shadow_tc = shadow_proj_coords.xyz / shadow_proj_coords.w +     0.06 * vec3(normal.x, normal.z, 0.0);"\
		"float fShadow = SampleShadow(shadow_tc);"\
		"float fSurfaceShadow = 0.25 * fShadow + 0.75;"\
		"vec3 ref_vec = reflect(-lpnorm, normal);"\
		"float VdotR = max( dot(ref_vec, vpnorm), 0.0 );"\
		"VdotR /= 1024.0 - VdotR * 1024.0 + VdotR;"\
		"vec3 specular = specular_color * vec3(VdotR) * fExpDepthHIGH * fShadow;"\
		"float fDistortScale = 0.1 * fExpDepth;"\
		"vec2 vDistort = normal.zx * fDistortScale;"\
		"float fDistortedDepth = texture2D(depth_texture, proj_tc.xy + vDistort).x;"\
		"fDistortedDepth = calculate_linear_depth(fDistortedDepth);"\
		"float fDistortedExpDepth =     1.0 - exp( -time_density_clipplane.y * (fDistortedDepth - fOwnDepth) );"\
		"if (fOwnDepth > fDistortedDepth)   {"\
		"vDistort = vec2(0.0);"\
		"fDistortedExpDepth = fExpDepth;"\
   "}"\
   "vec3 refraction = texture2D(refract_texture, proj_tc.xy + vDistort).xyz;  "\
   "refraction = mix(refraction, water_color.xyz * fHDRscale, fDistortedExpDepth); "\
   "vDistort = normal.xz * 0.025; "\
   "vec3 reflection = texture2D(reflect_texture, proj_tc.st + vDistort).xyz;"\
   "float fMix = fresnel * fExpDepthHIGH;"\
   "vec3 result_color = mix(refraction, reflection, fMix) * fSurfaceShadow;"\
   "result_color += specular;"\
   "result_color.xyz = (result_color.xyz + vec4(texture2D(texture_r, gl_TexCoord[0].xy)).xyz)*1.2;"\
   "if(result_color.x>0.69)		   result_color.x=0.69;		if(result_color.y>0.77)		   result_color.y=0.77;		if(result_color.z>0.78)		   result_color.z=0.78;  gl_FragColor = vec4( result_color, 1.0);   }"};
   */
   //char *CodesWaterb = loadShader("shader//WaterPS.glsl");
   //PSWater = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
   //glShaderSourceARB(PSWater, 1, (const char**)&CodesWaterb, NULL);
   //
   //glCompileShaderARB(PSWater);
   //glGetObjectParameterivARB(PSWater, GL_OBJECT_COMPILE_STATUS_ARB, &result); 
   //if(!result)
   //   {
   //      glGetInfoLogARB(PSWater, sizeof(error), NULL, error);
   //      MessageBox(NULL, error, "3333Error p ...", MB_OK);
   //   }

   //glAttachObjectARB(SWater, PSWater);
   ////delete[] CodesWater;
   ////CodesWater = NULL;

   //glLinkProgramARB(SWater);//линкуем программу
   //glGetObjectParameterivARB(SWater, GL_OBJECT_LINK_STATUS_ARB, &result);

   //if(!result)
   //   {
   //      glGetInfoLogARB(SWater, sizeof(error), NULL, error);
   //      MessageBox(NULL, error, "3333Error linking shaders...", MB_OK);
   //      //return false;
   //   } 

   //// связать переменные с шейдером
   //TextureWater = glGetUniformLocationARB(SWater, "normal_texture");
   //TextureNV = glGetUniformLocationARB(SWater, "reflect_texture");
   //TextureNV1 = glGetUniformLocationARB(SWater, "depth_texture");
   //TextureNV2 = glGetUniformLocationARB(SWater, "shadow_texture");
   //TextureNV3 = glGetUniformLocationARB(SWater, "texture_r");

   //TimeVater = glGetUniformLocationARB(SWater, "time");
   //glLiSour = glGetUniformLocationARB(SWater, "light_source");
   //glViPos = glGetUniformLocationARB(SWater, "view_position");
   //glWatCol = glGetUniformLocationARB(SWater, "water_color");
   //glSpecCol = glGetUniformLocationARB(SWater, "specular_color");
   //glT = glGetUniformLocationARB(SWater, "time_density_clipplane");
   //u_timePrm = glGetUniformLocationARB(SWater, "u_time");
   //u_time = 0.f;



   ////////////////////////////////////////////////шейдер key
   //подготовка шейдера
   key = glCreateProgramObjectARB();//создать объект программы

   //вершинный шейдер
   char *CodesWater1111 = {"#version 110\n varying vec2 vTexCoord; void main(){   vTexCoord = gl_MultiTexCoord0.xy;   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;}    "};
	   //loadShader("shader//keyv.glsl");//загрузка текста шейдера
   VdSkey = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);//создать вершинный шейдер
   glShaderSourceARB(VdSkey, 1, (const char**)&CodesWater1111, NULL);//передать шейдеру его исходный код
   
   glCompileShaderARB(VdSkey);//компилируем шейдер 
   glGetObjectParameterivARB(VdSkey, GL_OBJECT_COMPILE_STATUS_ARB, &result);//результат компиляции шейдера
   if(!result)
      {
         glGetInfoLogARB(VdSkey, sizeof(error), NULL, error);
         MessageBox(NULL, error, "Error VdSkey ...", MB_OK);
      }

   glAttachObjectARB(key, VdSkey);//присоединяем шейдер к программе
   //delete[] CodesWater;//очистка памяти
   //CodesWater = NULL;//очистка памяти
  
   //фрагментный шейдер
   char *CodesWater8 = {"#version 110\n uniform sampler2D myTexture; varying vec2 vTexCoord; void main () {     vec4 color = texture2D(myTexture, vTexCoord);                 if (color.rgb == vec3(0.0,0.0,0.0))      discard;    gl_FragColor = color*color*color;} "};
	//char *CodesWater8 = loadShader("shader//keyp.glsl");
   PSkedy = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
   glShaderSourceARB(PSkedy, 1, (const char**)&CodesWater8, NULL);
   
   glCompileShaderARB(PSkedy);
   glGetObjectParameterivARB(PSkedy, GL_OBJECT_COMPILE_STATUS_ARB, &result); 
   if(!result)
      {
         glGetInfoLogARB(PSkedy, sizeof(error), NULL, error);
         MessageBox(NULL, error, "Error pkey ...", MB_OK);
      }

   glAttachObjectARB(key, PSkedy);
   //delete[] CodesWater;
   //CodesWater = NULL;

   glLinkProgramARB(key);//линкуем программу
   glGetObjectParameterivARB(key, GL_OBJECT_LINK_STATUS_ARB, &result);

   if(!result)
      {
         glGetInfoLogARB(key, sizeof(error), NULL, error);
         MessageBox(NULL, error, "Error linking shaders key...", MB_OK);
         //return false;
      } 


   listBase = CreateFontz("Comic Sans MS", 34);
   listBase1 = CreateFontz("Comic Sans MS", 34);
   listBase5 = CreateFontz("Comic Sans MS", 20);




   //3dtex  
   //texturebox[22].LoadTGA("data//texture//BostonTeapot.tga");
   //glGenTextures(1, &texturebox[22].ID);
   //glBindTexture(GL_TEXTURE_3D, texturebox[22].ID);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   //type = texturebox[22].type == 3 ? GL_RGB : GL_RGBA;
   //gluBuild2DMipmaps(GL_TEXTURE_3D, type, texturebox[22].imageWidth, texturebox[22].imageHeight,
   //                  type, GL_UNSIGNED_BYTE, texturebox[22].image);
  

 /* unsigned char* buffer = new unsigned char[256*256*3*4];
  texturebox[22].LoadTGA("data//texture//BostonTeapot.tga");
  glGenTextures(1, &texturebox[22].ID);
  glBindTexture(GL_TEXTURE_3D, texturebox[22].ID);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, 256, 256, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
*/

  


}

//-----------------------------------------------------------------------------
// Name: initPhysics()
// Desc: 
//-----------------------------------------------------------------------------
void initPhysics( void )
{
    QueryPerformanceFrequency( &g_performanceFrequency );
    QueryPerformanceCounter( &g_startTime );

	//
	// Setup the physics environment...
	//

	neV3 gravity;
	gravity.Set( 0.0f, -10.0f, 0.0f );

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount        = NUM_RED_CUBES+100;
	sizeInfo.animatedBodiesCount     = NUM_BLUE_PLATFORMS+30;
	sizeInfo.rigidParticleCount      = 15;
	sizeInfo.controllersCount        = 15;
	// Use the following formula to compute the number of overlapped-pairs required: 
	//(num_rigid_bodies * num_animated_bodies) + num_rigid_bodies * (num_rigid_bodies - 1) / 2
	sizeInfo.overlappedPairsCount    = ((NUM_RED_CUBES * NUM_BLUE_PLATFORMS) + NUM_RED_CUBES * (NUM_RED_CUBES - 1) / 2)+2000;
	sizeInfo.geometriesCount         = NUM_RED_CUBES + NUM_BLUE_PLATFORMS + 100;
	sizeInfo.constraintsCount        = 10;
	sizeInfo.constraintSetsCount     = 10;
	sizeInfo.constraintBufferSize    = 10;
	sizeInfo.sensorsCount            = 10;
	sizeInfo.terrainNodesStartCount  = 10;
	sizeInfo.terrainNodesGrowByCount = 10;

	g_simulator = neSimulator::CreateSimulator( sizeInfo, NULL, &gravity );

	//
	// Physics for the blue platform...
	//

	g_bluePlatform = g_simulator->CreateAnimatedBody();

	neGeometry *platformGeometry = g_bluePlatform->AddGeometry();

	platformGeometry->SetBoxSize( 500.0f, 1.0f, 500.0f );

	g_bluePlatform->UpdateBoundingInfo();

	neV3 position;
	position.Set( 0.0f, 0.0f, 0.0f );

	g_bluePlatform->SetPos( position );

	
	
	//////////////////////////////////////////////////////////////////////////////korpus
	float xs=50.0f;
	float zs=10.0f;

	g_redCubes[0] = g_simulator->CreateRigidBody();
	neGeometry *cubeGeometry = g_redCubes[0]->AddGeometry();

	neV3 cubeSize;
    cubeSize.Set( 3.0f, 1.0f, 2.0f ); // Unit size

	cubeGeometry->SetBoxSize( cubeSize );

	g_redCubes[0]->UpdateBoundingInfo();

    float fMass = 1.0f;
	g_redCubes[0]->SetInertiaTensor( neBoxInertiaTensor( cubeSize, fMass ) );
	g_redCubes[0]->SetMass( fMass );

	//neV3 position;
    position.Set( 0.0f + xs, 1.5f, 0.0f + xs );
	g_redCubes[0]->SetPos( position );
   







	//61
	{
    g_redCubes6 = g_simulator->CreateRigidBody();
	neGeometry *cubeGeometry = g_redCubes6->AddGeometry();

	//neV3 cubeSize;
    cubeSize.Set( 1.0f, 1.0f, 1.0f ); // Unit size

	cubeGeometry->SetBoxSize( cubeSize );

	g_redCubes6->UpdateBoundingInfo();

    /*fMass = 0.1f;
	g_redCubes6->SetInertiaTensor( neBoxInertiaTensor( cubeSize, fMass ) );
	g_redCubes6->SetMass( fMass );*/

    position.Set( 1.5f + xs, 2.0f, 1.5f + xs );
	g_redCubes6->SetPos( position );
	}



    //62
	{
    g_redCubes61 = g_simulator->CreateRigidBody();
	neGeometry *cubeGeometry = g_redCubes6->AddGeometry(); 

	//neV3 cubeSize;
    cubeSize.Set( 1.0f, 1.0f, 1.0f ); // Unit size

	cubeGeometry->SetBoxSize( cubeSize );

	g_redCubes61->UpdateBoundingInfo();

    /*fMass = 0.1f;
	g_redCubes6->SetInertiaTensor( neBoxInertiaTensor( cubeSize, fMass ) );
	g_redCubes6->SetMass( fMass );*/

    position.Set( 1.5f + xs, 2.0f, -1.5f + xs);
	g_redCubes61->SetPos( position );
	}



    //63
	{
    g_redCubes62 = g_simulator->CreateRigidBody();
	neGeometry *cubeGeometry = g_redCubes6->AddGeometry(); 

	//neV3 cubeSize;
    cubeSize.Set( 1.0f, 1.0f, 1.0f ); // Unit size

	cubeGeometry->SetBoxSize( cubeSize );

	g_redCubes62->UpdateBoundingInfo();

    /*fMass = 0.1f;
	g_redCubes6->SetInertiaTensor( neBoxInertiaTensor( cubeSize, fMass ) );
	g_redCubes6->SetMass( fMass );*/

    position.Set( 2.0f + xs, 2.0f, xs);
	g_redCubes62->SetPos( position );
	}





	//kolesa
	for( int i = 1; i < NUM_RED_CUBES; ++i )
	{
		g_redCubes[i] = g_simulator->CreateRigidBody();
		neGeometry *cylinderGeom = g_redCubes[i]->AddGeometry();

		float cylinderDiameter = 1.0f;
	    float cylinderHeight   = 1.0f;
	    cylinderGeom->SetCylinder( cylinderDiameter, cylinderHeight );

		float massOfCylinder = 0.1f;
	    g_redCubes[i]->SetMass( massOfCylinder );
	    g_redCubes[i]->SetInertiaTensor( neCylinderInertiaTensor( cylinderDiameter, cylinderHeight, massOfCylinder ) );

		neV3 position;

		switch(i)
		{
		case 0: position.Set( 0.0f + xs, 1.5f, 0.0f + xs );
			break;
		case 1: position.Set( 1.5f + xs, 1.0f, -2.0f + xs );
			break;
		case 2: position.Set( -1.5f + xs, 1.0f, -2.0f + xs );
			break;
		case 3: position.Set( -1.5f + xs, 1.0f, 2.0f + xs );
			break;
		case 4: position.Set( 1.5f + xs, 1.0f, 2.0f + xs );
			break;
		}

		g_redCubes[i]->SetPos( position );

	    neQ rotationOfCylinder;
	    neV3 axis;
	    axis.Set(1.0f, 0.0f, 0.0f);
	    rotationOfCylinder.Set(55.0f, axis );
	    g_redCubes[i]->SetRotation( rotationOfCylinder );
        
		g_redCubes[i]->UpdateBoundingInfo();		
	}



	//////////////////////////////////////////objecti sopernikov
	for(int i=0; i<10; i++)
	{	
	obj[i] = g_simulator->CreateRigidBody();
	neGeometry *cubeGeometryobj = obj[i]->AddGeometry();

    cubeSize.Set( 1.0f, 1.0f, 1.0f ); // Unit size

	cubeGeometry->SetBoxSize( cubeSize );

	obj[i]->UpdateBoundingInfo();

    position.Set( 100.0f, 100.0f, 0.0f );
	obj[i]->SetPos( position );
	}



	//////////////////////////////////////////////object igroka
	obj1[0] = g_simulator->CreateRigidBody();
	neGeometry *cubeGeometryobj1 = obj1[0]->AddGeometry();

    cubeSize.Set( 1.0f, 1.0f, 1.0f ); // Unit size

	cubeGeometry->SetBoxSize( cubeSize );

	obj1[0]->UpdateBoundingInfo();

    position.Set( 50.0f, 5.0f, 20.0f );
	obj1[0]->SetPos( position );
	







	//кости
	neT3 jointFrame;
    neV3 positionj;

	neJoint * joint6 = g_simulator->CreateJoint(g_redCubes6, g_redCubes[0]);
    //joint6->SetIteration(1);
		
    jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
    jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
    jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
	jointFrame.pos = positionj.Set( 1.5f + xs, 2.0f, 1.0f + xs );

	joint6->SetJointFrameWorld(jointFrame);
    joint6->SetType(neJoint::NE_JOINT_HINGE);
	joint6->Enable(true);	
 
	neJoint * joint61 = g_simulator->CreateJoint(g_redCubes61, g_redCubes[0]);
	joint61->SetIteration(1);
		
    jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
	jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
	jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
	jointFrame.pos = positionj.Set( 1.5f + xs, 2.0f, -1.0f + xs );

	joint61->SetJointFrameWorld(jointFrame);
	joint61->SetType(neJoint::NE_JOINT_HINGE);
	joint61->Enable(true);	


	neJoint * joint62 = g_simulator->CreateJoint(g_redCubes62, g_redCubes[0]);
	joint62->SetIteration(1);
		
    jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
	jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
	jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
	jointFrame.pos = positionj.Set( 2.0f + xs, 2.0f, xs );

	joint62->SetJointFrameWorld(jointFrame);
	joint62->SetType(neJoint::NE_JOINT_HINGE);
	joint62->Enable(true);	


	joint= g_simulator->CreateJoint(g_redCubes[2], g_redCubes[0]);
	joint->SetIteration(1);
		
    jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
	jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
	jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
	jointFrame.pos = positionj.Set( -1.5f + xs, 1.0f, -1.5f + xs );

    joint->SetJointFrameWorld(jointFrame);
	joint->SetType(neJoint::NE_JOINT_HINGE);
	joint->Enable(true);
	joint->EnableMotor(true);
	joint->SetMotor(neJoint::NE_MOTOR_SPEED, 0.0f, 10000.f);
	   

	//neJoint * joint2;
	joint2= g_simulator->CreateJoint(g_redCubes[3], g_redCubes[0]);
	joint2->SetIteration(1);
		
    jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
	jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
	jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
	jointFrame.pos = positionj.Set( -1.5f + xs, 1.0f, 1.5f + xs );

    joint2->SetJointFrameWorld(jointFrame);
	joint2->SetType(neJoint::NE_JOINT_HINGE);
	joint2->Enable(true);
	joint2->EnableMotor(true);
	joint2->SetMotor(neJoint::NE_MOTOR_SPEED, 0.0f, 10000.f);


	joint1 = g_simulator->CreateJoint(g_redCubes[1], g_redCubes[0]);
	joint1->SetIteration(1);
		
    jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
	jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
	jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
	jointFrame.pos = positionj.Set( 1.5f + xs, 1.0f, -1.5f + xs );

    joint1->SetJointFrameWorld(jointFrame);
	joint1->SetType(neJoint::NE_JOINT_HINGE);
	joint1->Enable(true);
	joint1->EnableMotor(true);
	joint1->SetMotor(neJoint::NE_MOTOR_SPEED, 0.0f, 10000.f);
		 

	joint4 = g_simulator->CreateJoint(g_redCubes[4], g_redCubes[0]);
	joint4->SetIteration(1);
		
    jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
	jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
	jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
	jointFrame.pos = positionj.Set( 1.5f + xs, 1.0f, 1.5f + xs );

	joint4->SetJointFrameWorld(jointFrame);
	joint4->SetType(neJoint::NE_JOINT_HINGE);
	joint4->Enable(true);
	joint4->EnableMotor(true);
	joint4->SetMotor(neJoint::NE_MOTOR_SPEED, 0.0f, 10000.f);



	g_yellowSphere = g_simulator->CreateRigidBody();

	neGeometry *sphereGeom = g_yellowSphere->AddGeometry();

	float sphereDiameter = 0.5f;
	sphereGeom->SetSphereDiameter( sphereDiameter );

	float massOfSphere = 0.1f;
	g_yellowSphere->SetMass( massOfSphere );
	g_yellowSphere->SetInertiaTensor( neSphereInertiaTensor( sphereDiameter, massOfSphere ) );

	neV3 positionOfSphere;
	positionOfSphere.Set( -20.0f, 1.0f, 0.0f );
	g_yellowSphere->SetPos( positionOfSphere );

	g_yellowSphere->UpdateBoundingInfo();



	//{
 //   box = g_simulator->CreateRigidBody();
	//neGeometry *cubeGeometry = box->AddGeometry();

 //   cubeSize.Set( 0.5f, 0.5f, 0.5f ); // Unit size
	//cubeGeometry->SetBoxSize( cubeSize );

	//box->UpdateBoundingInfo();

 //   position.Set( 10.0f, 1.0f, 0.0f );
	//box->SetPos( position );
	//}

	//{
 //   cubes[0] = g_simulator->CreateRigidBody();
	//neGeometry *cubeGeometry = cubes[0]->AddGeometry();

 //   cubeSize.Set( 1.0f, 1.0f, 1.0f ); // Unit size
	//cubeGeometry->SetBoxSize( cubeSize );

	//cubes[0]->UpdateBoundingInfo();

 //   position.Set( 0.0f, 1.0f, 5.0f );
	//cubes[0]->SetPos( position );

	//cubes[0]->CollideConnected(false);
	//}

	//land
	{
	for( int i = 0; i < 5; i++ )
	{
		land[i] = g_simulator->CreateRigidBody();
		neGeometry *cubeGeometry = land[i]->AddGeometry();

		neV3 cubeSize;
		switch(i)
		{
		case 0: cubeSize.Set( 1.0f, 4.0f, 100.0f ); 
			break;
		case 1: cubeSize.Set( 1.0f, 4.0f, 100.0f ); 
			break;
		case 2: cubeSize.Set( 90.0f, 4.0f, 1.0f ); 
			break;
		case 3: cubeSize.Set( 90.0f, 4.0f, 1.0f ); 
			break;
		case 4: cubeSize.Set( 8.0f, 4.0f, 8.0f ); 
			break;
		}
	    cubeGeometry->SetBoxSize( cubeSize );

		fMass = 40.0f;
		land[i]->SetInertiaTensor( neBoxInertiaTensor( cubeSize, fMass ) );
		land[i]->SetMass( fMass );

		neV3 position;
		switch(i)
		{
		case 0: position.Set( 95.0f, 2.5f, 50.0f );
			break;
		case 1: position.Set( 1.0f, 2.5f, 50.0f );
			break;
		case 2: position.Set( 50.0f, 2.5f, 1.0f );
			break;
		case 3: position.Set( 50.0f, 2.5f, 99.0f );
			break;
		case 4: position.Set( 70.0f, 2.0f, 58.0f );
			break;
		}

		land[i]->SetPos( position );  
		land[i]->UpdateBoundingInfo();		
	}
	}



	for(int i=0; i<10; i++)
	{
	g_Sphere[i] = g_simulator->CreateRigidBody();

	neGeometry *sphereGeomt = g_Sphere[i]->AddGeometry();

	sphereDiameter = 0.5f;
	sphereGeomt->SetSphereDiameter( sphereDiameter );

	massOfSphere = 0.1f;
	g_Sphere[i]->SetMass( massOfSphere );
	g_Sphere[i]->SetInertiaTensor( neSphereInertiaTensor( sphereDiameter, massOfSphere ) );

	positionOfSphere;
	positionOfSphere.Set( -15.0f, 5.0f, 5.0f );
	g_Sphere[i]->SetPos( positionOfSphere );

	g_Sphere[i]->UpdateBoundingInfo();
	}
	


	
	

}

//-----------------------------------------------------------------------------
// Name: shutDown()
// Desc: 
//-----------------------------------------------------------------------------
void shutDown( void )	
{
	SkyBox::Shutdown();
    neSimulator::DestroySimulator( g_simulator );

	if( g_hRC != NULL )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( g_hRC );
		g_hRC = NULL;
	}

	if( g_hDC != NULL )
	{
		ReleaseDC( g_hWnd, g_hDC );
		g_hDC = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name: updatePhysics()
// Desc: 
//-----------------------------------------------------------------------------
void updatePhysics( void )
{
	neT3 rotMatrix;
    neV3 rotation;
    rotation.Set( 0.0f, 0.0f, g_fTiltOfBluePlatform );
    rotMatrix.rot.RotateXYZ( rotation );

    g_bluePlatform->SetRotation( rotMatrix.rot );


	rotation.Set( 0.0f, g_fTiltOfRedPlatform, 0.0f );
    rotMatrix.rot.RotateXYZ( rotation );

	if(flag == 1)
    g_redCubes61->SetRotation( rotMatrix.rot );

	if(flag == 2)
	g_redCubes6->SetRotation( rotMatrix.rot );
    
	flag = 0;

    //
    // Before we advance the simulation, lets update the orientation of our  
    // blue platform just in case the user adjusted it.
    //

    //neT3 rotMatrix;
    //neV3 rotation;
    rotation.Set( 0.0f, 0.0f, g_fTiltOfBluePlatform );
    rotMatrix.rot.RotateXYZ( rotation );

    g_bluePlatform->SetRotation( rotMatrix.rot );

    //
    // Now, advance the simulation...
    //

    //
    // A note from the Tokamak developers:
    //
    // This sample advances the simulation by placing an upper limit on 
    // the speed with which the physics can update so that if the frame time 
    // is less than 1/60th, the physics is not updated. If you are running on a 
    // faster machine the physics rate will be the same to the observer.
    //
    // Unfortunately, a problem arises when the frame time is substantially 
    // more than 1/60th of a second. The risk here is that on a slow machine 
    // the physics will appreciably slow down. One approach to solving this is 
    // to have several different render calls of varying "costs". When you 
    // reach a physics spike you use one of the lower cost render calls, until 
    // the spike is passed. This requires a more sophisticated approach which 
    // involves keeping track of the physics cost as well as the render cost 
    // (not shown in this sample).
    //

    QueryPerformanceCounter( &g_endTime );

    // Calculate the elapsed time since the physics was last updated.
    g_fFrameTime = ( (float)g_endTime.QuadPart - (float)g_startTime.QuadPart ) 
                   / g_performanceFrequency.QuadPart;

    g_fTimeSinceLastUpdate += g_fFrameTime;

    // If the time is greater than 1/60th of a second update the physics
    if( g_fTimeSinceLastUpdate > CONST_FRAME_INTERVAL )
    {
        g_simulator->Advance( CONST_FRAME_INTERVAL, 1, NULL );
        g_fTimeSinceLastUpdate -= CONST_FRAME_INTERVAL;

		keyboard();


	float g_fElpasedTimep = 0.014f;
	for(int ii=0; ii<50; ii++)
		{
			ParticleBum[ii].UpdateParticleBum(g_fElpasedTimep);		
			Particle[ii].UpdateParticle(g_fElpasedTimep);
		}

	g_fElpasedTimep = 0.001f;
	for(int i=0; i<100; i++)
		{
			ParticleV[i].UpdateParticle(g_fElpasedTimep);
		}


    }

    QueryPerformanceCounter( &g_startTime );
}


//-----------------------------------------------------------------------------
// Name: loadShader()
// Desc: 
//-----------------------------------------------------------------------------
char* loadShader(char *shaderName)
{
   char tempLine[128] = {0};
   int count = 0;

   ifstream input;
   input.open(shaderName);
   if(!input.is_open())
      {
         return NULL;
      }
   while(input.getline(tempLine, 128, '\n'))
      {
         count++;
      }
   input.close();
   
   if(count == 0) return NULL;  
   ifstream sourceInput;
   char *ShaderSource = new char[128 * count];
   sourceInput.open(shaderName);
   sourceInput.getline(ShaderSource , 128 * count, '\0');
   sourceInput.close(); 

   return ShaderSource;
}



unsigned int CreateFontz(char *fontName, int Size)
{
   // Windows font.
   HFONT hFont;
   unsigned int base;

   // Create space for 96 characters.
   base = glGenLists(96);

   if(stricmp(fontName, "symbol")==0)
      {
         hFont = CreateFont(Size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                            SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                            ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, fontName);
      }
   else
      {
         hFont = CreateFont(Size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                            ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                            ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, fontName);
      }

   if(!hFont) return 0;

   SelectObject(g_hDC, hFont);
   wglUseFontBitmaps(g_hDC, 32, 96, base);

   return base;
}



void PrintText(unsigned int base, char *string)
{
   if((base == 0 || string == NULL)) return;

   glPushAttrib(GL_LIST_BIT);
      
      glListBase(base - 32);
      glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);
   
   glPopAttrib();
}



void keyboard()
{
	
	if(/*(GetKeyState(VK_UP) & 0x80) ||*/ (GetKeyState('W') & 0x80))
	{	
		flag=1;
		g_fTiltOfRedPlatform += 0.0001f;

		speed+=0.1f;
		joint1->SetMotor(neJoint::NE_MOTOR_SPEED, speed, 10000.f);
		joint4->SetMotor(neJoint::NE_MOTOR_SPEED, speed, 10000.f);

		if(speed>0)
		{
        //joint2->SetMotor(neJoint::NE_MOTOR_SPEED, 1.0f, 10000.f);
		//joint->SetMotor(neJoint::NE_MOTOR_SPEED, 1.0f, 10000.f);
		}

		if(speed>0)
		{
        speed=2.8f;
		}
		g_vEye.x += g_vLook().x * 0.05;
		g_vEye.z += g_vLook().z * 0.05;
	}

	if(/*(GetKeyState(VK_DOWN) & 0x80) || */(GetKeyState('S') & 0x80))
	{
		flag=1;
		g_fTiltOfRedPlatform += 0.0001f;

		speed-=0.1f;
		if (speed<0)
		speed=0;
		joint1->SetMotor(neJoint::NE_MOTOR_SPEED, speed, 10000.f);
		joint4->SetMotor(neJoint::NE_MOTOR_SPEED, speed, 10000.f);
					
		if(speed==0)
		{
		//joint2->SetMotor(neJoint::NE_MOTOR_SPEED, 0.0f, 10000.f);
		//joint->SetMotor(neJoint::NE_MOTOR_SPEED, 0.0f, 10000.f);
		}
		g_vEye.x -= g_vLook().x * 0.05;
		g_vEye.z -= g_vLook().z * 0.05;
	}

	if(/*(GetKeyState(VK_LEFT) & 0x80) || */(GetKeyState('A') & 0x80))
	{	
		flag=1;
		g_fTiltOfRedPlatform += 0.005f/* * (g_fElpasedTime*50)*/;

		yinn = 0.005f/* * (g_fElpasedTime*50)*/;
		vector3f v (lodkax, 0.0f, lodkaz);
		RotateAroundPoint(v, yinn, 0, 1, 0);

		/*neV3 p;
		p.Set( (g_redCubes[0]->GetPos()) );
		float p1 = p[0];
		float g1 = g_vEye.x;

		float p11=p[2];
		float g11=g_vEye.z;*/
	}

	if(/*(GetKeyState(VK_RIGHT) & 0x80) || */(GetKeyState('D') & 0x80))
	{
		flag=1;
		g_fTiltOfRedPlatform -= 0.005f/* * (g_fElpasedTime*50)*/;

		yinn = -0.005f/* * (g_fElpasedTime*50)*/;
		vector3f v (lodkax, 0.0f, lodkaz);
		RotateAroundPoint(v, yinn, 0, 1, 0);

		/*neV3 p;
		p.Set( (g_redCubes[0]->GetPos()) );
		float p1 = p[0];
		float g1 = g_vEye.x;

		float p11=p[2];
		float g11=g_vEye.z;*/
		//g_vLook.x=g1-p1;
		//g_vLook[2]
	}




	if(/*(GetKeyState(VK_RIGHT) & 0x80) || */(GetKeyState('Z') & 0x80))
	{
		yinn = -0.005f/* * (g_fElpasedTime*50)*/;
		vector3f v (lodkax, 0.0f, lodkaz);
		RotateAroundPoint(v, yinn, 0, 1, 0);
	}

	if(/*(GetKeyState(VK_RIGHT) & 0x80) || */(GetKeyState('C') & 0x80))
	{
		yinn = 0.005f/* * (g_fElpasedTime*50)*/;
		vector3f v (lodkax, 0.0f, lodkaz);
		RotateAroundPoint(v, yinn, 0, 1, 0);
	}




	if(/*(GetKeyState(VK_LEFT) & 0x80) || */(GetKeyState('Q') & 0x80))
	{	
		//alSourcePlay(Source);

		

		if (zarad==true)
		{

		

		neV3 poss;
		neV3 posy;
		neV3 null;

		null.Set(0.0f, 0.0f, 0.0f);
		g_yellowSphere->SetVelocity( null );

		posy.Set( 0.0f, 1.0f, 0.0f);
		poss.Set( (g_redCubes[0]->GetPos()) );
		g_yellowSphere->SetPos( (posy + poss) );

		null.Set( 0.0f, 0.0f, 0.0f);
		null.Set((g_redCubes61->GetPos()) );


		neV3 tp;
		tp.Set((null - (posy + poss))*3+ posy*5);

	

		neV3 tp1;
		tp1.Set(tp.v[0], tp.v[1], tp.v[2]);
		g_yellowSphere->SetVelocity(tp1); 


		zarad = false;
		st=1;
		}
	}

	if(/*(GetKeyState(VK_RIGHT) & 0x80) || */(GetKeyState('E') & 0x80))
	{
		//alSourcePlay(Source);
		// MakeScreenShot();

		if(zarad==true)
		{
		

		neV3 poss;
		neV3 posy;
		neV3 null;

		null.Set(0.0f, 0.0f, 0.0f);
		g_yellowSphere->SetVelocity( null );

		posy.Set( 0.0f, 1.0f, 0.0f);
		poss.Set( (g_redCubes[0]->GetPos()) );
		g_yellowSphere->SetPos( (posy + poss) );

		null.Set( 0.0f, 0.0f, 0.0f);
		null.Set((g_redCubes6->GetPos()) );
		//g_yellowSphere->SetVelocity((null - (posy + poss))*3+ posy*5); 

		neV3 tp;
		tp.Set((null - (posy + poss))*3+ posy*5);

	

		neV3 tp1;
		tp1.Set(tp.v[0], tp.v[1], tp.v[2]);
		g_yellowSphere->SetVelocity(tp1); 
		
		zarad =false;
		st=2;
		}
	}
	}



void buildAxisAlignedBBMatrix( float m[16], float x, float y, float z )
{
    float pi    = 3.141592654f;
    float theta = -180 * atan2f(m[8], m[10]) / pi;
    float d     = x*x + y*y + z*z;
    float ct    = cosf(DEGTORAD(theta));
    float st    = sinf(DEGTORAD(theta));

    // Normalize
    if( d > 0 )
    {
	    d = 1/d;
	    x *= d;
	    y *= d;
	    z *= d;
    }

    // Clear out the view matrix passed in
    m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
    m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
    m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;

    //---------------------------------------------
    // R = uu' + cos(theta)*(I-uu') + sin(theta)*S
    //
    // S =  0  -z   y    u' = (x, y, z)
    //	    z   0  -x
    //	   -y   x   0
    //---------------------------------------------

     m[0] = x*x + ct*(1-x*x) + st*0;
     m[4] = x*y + ct*(0-x*y) + st*-z;
     m[8] = x*z + ct*(0-x*z) + st*y;

     m[1] = y*x + ct*(0-y*x) + st*z;
     m[5] = y*y + ct*(1-y*y) + st*0;
     m[9] = y*z + ct*(0-y*z) + st*-x;

     m[2] = z*x + ct*(0-z*x) + st*-y;
     m[6] = z*y + ct*(0-z*y) + st*x;
     m[10]= z*z + ct*(1-z*z) + st*0;
}


void RotateAroundPoint(vector3f vCenter, float angle, float x, float y, float z)
{
    vector3f vNewPosition;
 
    // Чтобы вращать нашу позицию вокруг точки, всё, что нам надо - найти вектор
    // от нашей позиции к центральной точке вращения. Один раз получив вектор, 
    // мы вращаемся вокруг точки с заданной скоростью/углом. 
    // Новый вектор vCenter - точка, вокруг которой мы вращаемся.
 
    // Получим центр, вокруг которого нужно вращатся
    vector3f vPos = g_vEye - vCenter;
 
    // Вычислим синус и косинус угла
    float cosTheta = (float)cos(angle);
    float sinTheta = (float)sin(angle);
 
    // Найдем значение X точки вращения
    vNewPosition.x  = (cosTheta + (1 - cosTheta) * x * x)       * vPos.x;
    vNewPosition.x += ((1 - cosTheta) * x * y - z * sinTheta)   * vPos.y;
    vNewPosition.x += ((1 - cosTheta) * x * z + y * sinTheta)   * vPos.z;
 
    // И значение Y
    vNewPosition.y  = ((1 - cosTheta) * x * y + z * sinTheta)   * vPos.x;
    vNewPosition.y += (cosTheta + (1 - cosTheta) * y * y)       * vPos.y;
    vNewPosition.y += ((1 - cosTheta) * y * z - x * sinTheta)   * vPos.z;
 
    // И Z...
    vNewPosition.z  = ((1 - cosTheta) * x * z - y * sinTheta)   * vPos.x;
    vNewPosition.z += ((1 - cosTheta) * y * z + x * sinTheta)   * vPos.y;
    vNewPosition.z += (cosTheta + (1 - cosTheta) * z * z)       * vPos.z;
 
	//g_vLook.x = -vPos.x;
	//g_vLook.y = -1.0f;
	//g_vLook.z = -vPos.z;

	g_vRight.set(1.0f, 0.0f, 0.0f);  // Right Vector

    // Теперь просто прибавим новый вектор к нашей позиции,
    // чтобы установить новую позицию камеры.
    g_vEye = vCenter + vNewPosition;


}


void Draw_Skybox(float x, float y, float z, float width, float height, float length)
{
	// Center the Skybox around the given x,y,z position
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	//glMultMatrixf( matWorld.m );

    glUseProgramObjectARB(SLand);//разрешить шейдер
	glPushMatrix();
   
      //установка переменных шейдера
   glUniform1iARB(TextureLand, 0);
   glUniform1iARB(TextureLandP, 1);

   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texturebox[12].ID);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glActiveTextureARB(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texturebox[20].ID);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	// Draw Front side
	//glBindTexture(GL_TEXTURE_2D, texturebox[20].ID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBegin(GL_QUADS);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height, z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height, z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z+length);
	glEnd();


		
	

	// Draw Back side
	//glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYBACK].ID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
	glEnd();

	// Draw Left side
	//glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYLEFT].ID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z);		
	glEnd();

	// Draw Right side
	//glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYRIGHT].ID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height,	z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height,	z);
	glEnd();

	

	// Draw Down side
	//glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYDOWN].ID);
	glBindTexture(GL_TEXTURE_2D,  SkyboxTexture[0].ID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y,		z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y,		z);
	glEnd();

		// Draw Up side
	glBindTexture(GL_TEXTURE_2D, texturebox[12].ID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y+height, z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y+height, z+length); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
	glEnd();

	glPopMatrix();
	glUseProgramObjectARB(0);//запретить шейдер
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, NULL);





}






void MakeScreenShot()
{
    // Массив данных будущего изображения
    unsigned char *output = 0;

    // Выделяем необходимую память: ширина*высота*3 цветовых бита
    output = new unsigned char[800 * 600 * 3];

    // Для получения данных экрана используем функцию glReadPixels:
    //glReadPixels(0,0, 800, 600, GL_RGB, GL_UNSIGNED_BYTE,
     //               output);
	glReadPixels(0,0, 800, 600, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,  output);

    //
    // Теперь нужно записать полученные данные в файл Screen.tga.
    //

    FILE *sFile = 0;        // Дескриптор файла

    // Обьявляем переменные, которые понадобятся нам в дальнейшем:
    unsigned char tgaHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char header[6];
    unsigned char bits = 0;
    int colorMode = 0;
    unsigned char tempColors = 0;

    // Открываем файл скриншота
    sFile = fopen("Screen.tga", "wb");

    // Проверяем, правильно ли произошло открытие
    if(!sFile){
        MessageBox(NULL, "Can't open ScreenShot File!","Error",MB_OK);
        return;
    }

    // Устанавливаем цветовой режим и глубину цвета:
    colorMode = 3;
    bits = 24;

    // Записываем ширину и высоту:
    header[0] = 800 % 256;
    header[1] = 800 / 256;
    header[2] = 600 % 256;
    header[3] = 600 / 256;
    header[4] = bits;
    header[5] = 0;
    
    // Записываем хидеры в начало файла:
    fwrite(tgaHeader, sizeof(tgaHeader), 1, sFile);
    fwrite(header, sizeof(header), 1, sFile);

    // Поскольку в формате TGA цвета хранятся не в RGB, а в BRG, нам нужно
    // поменять местами наши данные:
    for(int i=0; i < 800 * 600 * colorMode; i+=colorMode)
    {
        tempColors = output[i];
        output[i] = output[i+2];
        output[i+2] = tempColors;
    }

    // Записываем данные изображения:
    fwrite(output, 800 * 600 * colorMode, 1, sFile);

    // Закрываем файл
    fclose(sFile);
    // Удаляем ненужные теперь данные
    delete[] output;
}


//-----------------------------------------------------------------------------
// Name: render()
// Desc: 
//-----------------------------------------------------------------------------
void render( void )
{
    updatePhysics();//обновление физики

	/////////////////////GLdouble equation[4] = {0.0, 1.8, 0.0, 0.1};
	
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/////////////////////////////////////////////////////////////обработка клавиш и мышки
	getRealTimeUserInput();
	//////keyboard();

        

	g_vEye.x+=lodkax-lodkaxs;
	g_vEye.z+=lodkaz-lodkazs;

	lodkaxs=lodkax;
	lodkazs=lodkaz;

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
 
	//g_vEye.y=-0.3;
	//g_vLook.y= 0.5;
	updateViewMatrix();//обновление камеры
	

	matrix4x4f matWorld;
	matrix4x4f matScale;

	matrix4x4f matWorldz;
	matrix4x4f matWorldc;
	matrix4x4f matWorldx;

	matrix4x4f matTranslate;
	matrix4x4f matRotate;
	matrix4x4f matTx;
	matrix4x4f matTz;

	//matrica brizgi
	matrix4x4f matWorldV;

  //  //////////////////////////////////////////////////////////////////////платформа
  //  matWorld = NET3_TO_MATRIX4X4f( g_bluePlatform->GetTransform() );

  //  matScale.scale( vector3f( 10.0f, 1.0f, 10.0f ) ); // Scale it so it matches the Tokamak object
  //  matWorld = matWorld * matScale;

  //  glPushMatrix();
  //  {
  //      glMatrixMode( GL_MODELVIEW );
  //      glMultMatrixf( matWorld.m );
		//glColor3f( 0.0f, 0.0f, 1.0f );
  //      glInterleavedArrays( GL_N3F_V3F, 0, g_cubeVertices );
  //      glDrawArrays( GL_QUADS, 0, 24 );
  //  }
  //  glPopMatrix();

	matWorld = NET3_TO_MATRIX4X4f( g_redCubes[0]->GetTransform() );

		float posxedro1;
	float poszedro1;
	matWorld.posx(posxedro1);
	matWorld.posz(poszedro1);


	//glPushMatrix();
	//Draw_Skybox(posxedro1,0,poszedro1,150,50,150);
	//glPopMatrix();
  

	//poluchenie novih
	matWorld.posx(lodkax);
	matWorld.posz(lodkaz);

	neV3 vr;
	vr.Set(g_redCubes[0]->GetRotationQ());
	lodkay=vr[1];
	
	////////////////////////////glDisable(GL_CLIP_PLANE0);
	glBindTexture(GL_TEXTURE_2D, texturerender.ID);//текстура в которую будем рендерить
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 800, 600, 0);


	///////////////////////////////////////////////////////////////////////////////////////////
	/////
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	

	/////////////////////////////////////////////////////////////обработка клавиш и мышки
	//getRealTimeUserInput();
	//keyboard();

 	g_vEye.x+=lodkax-lodkaxs;
	g_vEye.z+=lodkaz-lodkazs;

	lodkaxs=lodkax;
	lodkazs=lodkaz;

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
 

	glColor3f(0.0f, 0.0f, 0.0f);


	updateViewMatrix();//обновление камеры

	//glPushMatrix();
	//Draw_Skybox(posxedro1,0,poszedro1,150,50,150);
	//glPopMatrix();
	/////////////////////////////////////////////////////////////////////вода
	glPushMatrix();
	//glTranslatef(0.f, -1.8f, 0.f);
	//SkyBox::camera.pos = SkyBox::vector(posxedro1, 0.f, poszedro1);
	SkyBox::camera.pos = SkyBox::vector(g_vEye.x, g_vEye.y + 50.8f, g_vEye.z);
	SkyBox::camera.rot = SkyBox::vector(yAngle, xAngle/*-atan2(g_vUp.z, g_vUp.x) / M_PI * 180*/, 0.f);
	//SkyBox::camera.rot = SkyBox::vector(atan2(g_vLook.y, g_vLook.x) / M_PI * 180, -atan2(g_vLook.z, g_vLook.x) / M_PI * 180/*-atan2(g_vUp.z, g_vUp.x) / M_PI * 180*/, 0.f);
	SkyBox::Render();
    glPopMatrix();
	//SwapBuffers( g_hDC );

	//return;
  


    /////////////////////////////////////////////////////////////////////////лодка
	matWorld = NET3_TO_MATRIX4X4f( g_redCubes[0]->GetTransform() );



	float posxv;
	float poszv;

	matWorld.posx(posxv);
	matWorld.posz(poszv);



	//////////////////brizgi2
	//matWorldV = NET3_TO_MATRIX4X4f( g_redCubes62->GetTransform() );
	//matWorldV.posx(posxv);
	//matWorldV.posz(poszv);

	//cv2++;
	//if (cv2>99)
	//	cv2=0;
	//ParticleV2[cv2].InitParticle(CPos(posxv, 1.8, poszv));


    glPushMatrix();

	//float lPos1[] = { 0.0f, 5.0f, 0.0f, 1.0f };

	//glMatrixMode( GL_MODELVIEW );

	//if(x11!=1)
		//matTranslate.translate_y(tr-1.5f);
	//else
		matTranslate.translate_y(tr);

	//matRotate.rotate_x(0.0f);
	matRotate.rotate_z((30.0f*tr)-2.0f);
	matWorld = matWorld*matRotate*matTranslate;
    glMultMatrixf( matWorld.m );

    glUseProgramObjectARB(glContext);//разрешить шейдер
   
    //установка переменных шейдера
    glUniform1iARB(glTexture, 0);
    glUniform4fARB(glLPos, 0, 4, 3, 1.0f);
	glUniform4fARB(glF1, lPos[0], lPos[1], lPos[2], 1.0f);
    glUniform4fARB(glLColor, lColor[0], lColor[1], lColor[2], 1.0f);
	glUniform4fARB(glCPos, g_vEye.x, g_vEye.y, g_vEye.z, 1.0f);


    for (int i = 0; i<2; i++)
    {

		//if((un==1)&(i==2))
		//	goto mbgo1; //ubrat parus
		//if((un==1)&(i==4))
		//	goto mbgo1; //ubrat parus

		switch(i)
		{
		case 0: glBindTexture(GL_TEXTURE_2D, texturebox[i].ID);
			break;
		case 1: glBindTexture(GL_TEXTURE_2D, texturebox[i].ID);
			break;

		
	    }

    //glBindTexture(GL_TEXTURE_2D, texturebox[i].ID);

    //countpoligons = 0;//количество полигонов
  
    // Так как мы знаем число обьектов в нашей модели, проходим через каждый из них
	//for(int i = 0; i < g_3DModel[0].numOfObjects; i++)
	//{
		// Убедимся, что передан верный обьект
		//if(g_3DModel[0].pObject.size() <= 0) break;
 
		// Получим текущий обьект
		t3DObject *pObject;
		
			pObject = &g_3DModel[0].pObject[i];
	
 
		// Начинаем отрисовку в выбранном режиме
		glBegin(GL_TRIANGLES);	// Рисуем обьекты (треугольники или линии)
 
			// Проходим через все полигоны обьекта и рисуем их
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// Проходим через каждый угол треугольника и рисуем его
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Получаем индекс для каждой точки полигона
					int index = pObject->pFaces[j].vertIndex[whichVertex];
 
					// Передаём OpenGL нормаль этой вершины
					glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
 
					// Если обьект имеет текстуру, передаем текст. координаты
					if(pObject->bHasTexture) {
 
						//// Убедимся, что UVW-мап применена на обьект, иначе
						//// он не будет иметь текстурных координат
						if(pObject->pTexVerts) {
							glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
						}
					} 
					
 
					// Передаём текущую вершину обьекта
					glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
				}
				//countpoligons++;
			}
	
   
			glEnd();


	}



	glUseProgramObjectARB(0);//запретить шейдер
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, NULL);
	
	 /////////////////////////////////////////////////////////////////////////пальма
	matWorld = NET3_TO_MATRIX4X4f( land[0]->GetTransform() );

	matWorld.posx(posxv);
	matWorld.posz(poszv);

	//////////////////brizgi2
	//matWorldV = NET3_TO_MATRIX4X4f( g_redCubes62->GetTransform() );
	//matWorldV.posx(posxv);
	//matWorldV.posz(poszv);

	//cv2++;
	//if (cv2>99)
	//	cv2=0;
	//ParticleV2[cv2].InitParticle(CPos(posxv, 1.8, poszv));


    glPushMatrix();

	//float lPos1[] = { 0.0f, 5.0f, 0.0f, 1.0f };

	//glMatrixMode( GL_MODELVIEW );

	//if(x11!=1)
		//matTranslate.translate_y(tr-1.5f);
	//else
		matTranslate.translate_y(tr);

	//matRotate.rotate_x(0.0f);
	matRotate.rotate_z((30.0f*tr)-2.0f);
	matWorld = matWorld*matRotate*matTranslate;
    glMultMatrixf( matWorld.m );

    glUseProgramObjectARB(glContext);//разрешить шейдер
   
    //установка переменных шейдера
    glUniform1iARB(glTexture, 0);
    glUniform4fARB(glLPos, 0, 4, 3, 1.0f);
	glUniform4fARB(glF1, lPos[0], lPos[1], lPos[2], 1.0f);
    glUniform4fARB(glLColor, lColor[0], lColor[1], lColor[2], 1.0f);
	glUniform4fARB(glCPos, g_vEye.x, g_vEye.y, g_vEye.z, 1.0f);


    for (int i = 0; i<2; i++)
    {

		//if((un==1)&(i==2))
		//	goto mbgo1; //ubrat parus
		//if((un==1)&(i==4))
		//	goto mbgo1; //ubrat parus

		switch(i)
		{
		case 0: glBindTexture(GL_TEXTURE_2D, texturebox[i].ID);
			break;
		case 1: glBindTexture(GL_TEXTURE_2D, texturebox[i].ID);
			break;

		
	    }

    //glBindTexture(GL_TEXTURE_2D, texturebox[i].ID);

    //countpoligons = 0;//количество полигонов
  
    // Так как мы знаем число обьектов в нашей модели, проходим через каждый из них
	//for(int i = 0; i < g_3DModel[0].numOfObjects; i++)
	//{
		// Убедимся, что передан верный обьект
		//if(g_3DModel[0].pObject.size() <= 0) break;
 
		// Получим текущий обьект
		t3DObject *pObject;
		
			pObject = &g_3DModel[0].pObject[i];
	
 
		// Начинаем отрисовку в выбранном режиме
		glBegin(GL_TRIANGLES);	// Рисуем обьекты (треугольники или линии)
 
			// Проходим через все полигоны обьекта и рисуем их
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// Проходим через каждый угол треугольника и рисуем его
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Получаем индекс для каждой точки полигона
					int index = pObject->pFaces[j].vertIndex[whichVertex];
 
					// Передаём OpenGL нормаль этой вершины
					glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
 
					// Если обьект имеет текстуру, передаем текст. координаты
					if(pObject->bHasTexture) {
 
						//// Убедимся, что UVW-мап применена на обьект, иначе
						//// он не будет иметь текстурных координат
						if(pObject->pTexVerts) {
							glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
						}
					} 
					
					// Передаём текущую вершину обьекта
					glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
				}
				//countpoligons++;
			}
	
			glEnd();

	}

	glUseProgramObjectARB(0);//запретить шейдер
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, NULL);
	

	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, NULL);

	SwapBuffers( g_hDC );
}














