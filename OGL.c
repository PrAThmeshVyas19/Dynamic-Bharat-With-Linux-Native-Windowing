// Standard Header Files
#include <stdio.h>
#include <stdlib.h> // for exit()
#include <memory.h> // for memset()
#include <math.h>
#include <AL/alut.h>    // internally includes al.h and alc.h

// X11 Header Files
#include <X11/Xlib.h>   // for All XWindow API's 
#include <X11/Xutil.h>   // simillar to glu (for XVisualInfo() and related api's)
#include <X11/XKBlib.h> // 

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <SOIL/SOIL.h>

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global Variable Declarations
Display *display = NULL;
Colormap colormap;
Window window;
XVisualInfo *visualinfo = NULL;

// OpenGL Global Variables
GLXContext glxContext = NULL;

Bool bFullScreen = False;
Bool bActiveWindow = False;

FILE *gpFile = NULL;

// Global Variables
GLfloat YTranslate = 0.0f;
GLfloat XTranslate = 0.0f;
GLfloat ScaleX = 0.0f;
GLfloat ScaleY = 0.0f;
GLfloat ZTranslate = 0.0f;

float i = 178.0f;
float j = 178.0f;

float ALpha_Bharat_B = 0.0f;
float ALpha_Bharat_H = 0.0f;
float ALpha_Bharat_A1 = 0.0f;
float ALpha_Bharat_R = 0.0f;
float ALpha_Bharat_A2 = 0.0f;
float ALpha_Bharat_T = 0.0f;

float ALpha_India_I1 = 1.0f;
float ALpha_India_N = 1.0f;
float ALpha_India_D = 1.0f;
float ALpha_India_I2 = 1.0f;
float ALpha_India_A = 1.0f;

float Transition_I1_X = 3.0f;
float Transition_I1_Y = 2.0f;
float Transition_N_X = 2.0f;
float Transition_N_Y = 2.0f;
float Transition_D_X = 0.0f;
float Transition_D_Y = -2.0f;
float Transition_I2_X = 0.0f;
float Transition_I2_Y = 2.0f;
float Transition_A_X = 3.0f;
float Transition_A_Y = 0.0f;

float Jet_Transition_Y = 1.0f;

float Jet_Angle = 30;

GLfloat Jet_From_Side_Counter = 0.0f;

GLfloat Egnition_Counter = 0.0f;

// OpenAL related GLOBAL variables declarations
ALCenum error;                      // error-checking of ALUT and OpenAL functions
ALuint audioBuffer , audioSource;   // OpenALs typedef of unsigned integer

// Entry Point Function
int main(void)
{
    // Local Function declarations
    void uninitialize(void);
    void initiallize(void);
    void resize(int , int);
    void draw(void);
    void update(void);
    void toggleFullScreen(void);

    // Local Variable Declarations
    int defaultScreen;
    XSetWindowAttributes windowattributes;
    int styleMask;
    Atom windowManagerDelete;
    XEvent event;
    KeySym keySym;
    char keys[26];

    int screenWidth , screenHeight;

    int frameBufferAttributes[] = {GLX_DOUBLEBUFFER , True ,
                                   GLX_RGBA ,
                                   GLX_RED_SIZE , 8 ,
                                   GLX_GREEN_SIZE , 8 ,
                                   GLX_BLUE_SIZE , 8 ,
                                   GLX_ALPHA_SIZE , 8 ,
                                   GLX_DEPTH_SIZE , 24 ,
                                   None};
    Bool bDone = False;

    // code
    gpFile = fopen("Log.txt" , "w");
    if (gpFile == NULL)
    {
        printf("Log File cannot be opened !!\n");
        exit(-1);
    }

    fprintf(gpFile , "Jay Ganeshay Namaha \nProgram Started Successfully \n");
    // Step 1
    display = XOpenDisplay(NULL);  
    if(display == NULL)
    {
        printf("XOpenDisplay() Failed !!\n");
        uninitialize();
        exit(1);
    }

    defaultScreen = XDefaultScreen(display);

    visualinfo = glXChooseVisual(display , defaultScreen , frameBufferAttributes);
    if(visualinfo == NULL)
    {
        printf("glXChooseVisual() Failed !!\n");
        uninitialize();
        exit(1);
    }

    memset((void *)&windowattributes , 0 , sizeof(XSetWindowAttributes));
    windowattributes.border_pixel = 0;
    windowattributes.background_pixel = XBlackPixel(display , visualinfo->screen);
    windowattributes.background_pixmap = 0;
    windowattributes.colormap = XCreateColormap(display ,
                                                XRootWindow(display , visualinfo->screen) ,
                                                visualinfo->visual ,
                                                AllocNone);
    
    colormap = windowattributes.colormap;

    styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask ; // CW = CreateWindow

    window = XCreateWindow(display ,
                           XRootWindow(display , visualinfo->screen) ,
                           0 ,
                           0 ,
                           WIN_WIDTH ,
                           WIN_HEIGHT ,
                           0 ,
                           visualinfo->depth ,
                           InputOutput ,
                           visualinfo->visual ,
                           styleMask ,
                           &windowattributes);

    if (!window)
    {
        printf("XCreateWindow() Failed !!\n");
        uninitialize();
        exit(1);
    }

    XSelectInput(display , 
                 window ,
                 ExposureMask |
                 VisibilityChangeMask |
                 StructureNotifyMask |
                 KeyPressMask |
                 ButtonPressMask |
                 PointerMotionMask | 
                 FocusChangeMask);

    windowManagerDelete = XInternAtom(display , "WM_DELETE_WINDOW" , True); // WM = Window Manager

    XSetWMProtocols(display , window , &windowManagerDelete , 1);

    XStoreName(display , window , "RTR5 : PPV : XWindow");

    XMapWindow(display , window);

    // Center the window
    screenWidth = XWidthOfScreen(XScreenOfDisplay(display , visualinfo->screen));
    screenHeight = XHeightOfScreen(XScreenOfDisplay(display , visualinfo->screen));
    XMoveWindow(display , window , ((screenWidth - WIN_WIDTH)/2) , ((screenHeight - WIN_HEIGHT)/2));

    // OpenGL Initialization
    initiallize();

    // Game Loop
    // Event Loop
    while (bDone == False)
    {
        while (XPending(display))   // Similar to peek message
        {
            XNextEvent(display , &event);
            switch (event.type)
            {
                case MapNotify:
                    break;
                
                case FocusIn:
                    bActiveWindow = True;
                    break;

                case FocusOut:
                    bActiveWindow = False;
                    break;

                case ConfigureNotify:
                    resize(event.xconfigure.width , event.xconfigure.height);
                    break;

                case ButtonPress:
                    switch (event.xbutton.button)
                    {
                        case 1:
                            break;

                        case 2:
                            break;

                        case 3:
                            break;

                        default:
                            break;
                    }
                    break;

                case KeyPress:
                    keySym = XkbKeycodeToKeysym(display , event.xkey.keycode , 0 , 0);
                    switch (keySym)
                    {
                        case XK_Escape:
                            bDone = True;
                            fprintf(gpFile , "Program Ended Successfully !!!\n");
                            break;

                        default:
                            break;
                    }

                    XLookupString(&event.xkey , keys , sizeof(keys) , NULL , NULL);
                    switch (keys[0])
                    {
                        case 'F':
                        case 'f':
                            if (bFullScreen == False)
                            {
                                toggleFullScreen();
                                bFullScreen = True;
                            }
                            else
                            {
                                toggleFullScreen();
                                bFullScreen = False;
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                case 33:
                    bDone = True;
                    break;

                default:
                    break;
            }
        }
        // Rendering
        if (bActiveWindow == True)
        {
            draw();
            update();
        }
    }   

    uninitialize();

    return 0;
}

void toggleFullScreen(void)
{
    // Local Variable Declaration
    Atom windowManagerStateNormal;
    Atom windowManagerStateFullScreen;
    XEvent event;

    // Code
    windowManagerStateNormal = XInternAtom(display , "_NET_WM_STATE" , False);

    windowManagerStateFullScreen = XInternAtom(display , "_NET_WM_STATE_FULLSCREEN" , False);
    
    // memset the event structure and fill it with above 2 Atoms
    memset((void *)&event , 0 , sizeof(XEvent));

    // filling the event
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = windowManagerStateNormal;
    event.xclient.format = 32;
    event.xclient.data.l[0] = bFullScreen?0:1;
    event.xclient.data.l[1] = windowManagerStateFullScreen;

    XSendEvent(display , 
               XRootWindow(display , visualinfo->screen) ,
               False ,
               SubstructureNotifyMask ,
               &event);

}

void initiallize(void)
{
    // Local Function Declaration
    void resize(int , int);
    void uninitialize();
    void initializeAudio();
    GLuint loadGLTexture(const char* );

    GLuint iResult = 0;

    // code
    // Create OpenGL Context
    glxContext = glXCreateContext(display , visualinfo , NULL , True);
    if (glxContext == NULL)
    {
        printf("glXCreateContext() Failed !!!\n");
        uninitialize();
        exit(1);
    }

    // Make this context as current Context
    glXMakeCurrent(display , window , glxContext);
    if (glXMakeCurrent(display , window , glxContext) == False)
    {
        printf("glXMakeCurrent() Failed !!!\n");
        uninitialize();
        exit(1);
    }
    
    // Enabling Depth
    glShadeModel(GL_SMOOTH); // Lighting ani coloring la shading smooth kr
    glClearDepth(1.0f); // Depth buffer la clear karayla hii value vapar saglya bits la 1 kr
    glEnable(GL_DEPTH_TEST); // 8 test peiki enable kr depth test
    glDepthFunc(GL_LEQUAL); // Depth sathi konta func vaparu test sathi LEQUAL = Less than or Equal to => ClearDepth 1.0f z coordinate
    glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST); // Optional Beautification

    initializeAudio();

    // Usual OpenGL Code
    glClearColor(68/255.0 , 68/255.0 , 68/255.0, 1.0f);

    // Warmup Resize
    resize(WIN_WIDTH , WIN_HEIGHT);
}

void initializeAudio()
{
    // function prototypes
    void uninitializeAudio(void);

    // code
    // STep 1 => initalization ALUT
    alutInit(NULL , NULL);
    error = alutGetError();
    if (error != ALUT_ERROR_NO_ERROR)
    {
        printf("alutInit() failed !!!\n");
        uninitializeAudio();
        return; // Return -ve value in OpenGL code
    }

    // Step 2 => Create Audio Buffer from Audio File
    audioBuffer = alutCreateBufferFromFile("DynamicBharatAudio.wav");
    error = alutGetError();
    if (error != ALUT_ERROR_NO_ERROR)
    {
        printf("alutCreateBufferFromFile() failed !!! , Creation of audio buffer from wav file failed .\n");
        uninitializeAudio();
        return; // Return -ve value in OpenGL code
    }

    // Step 3 => Create a audio source // First OpenAL function
    alGenSources(1 , &audioSource);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        printf("alGetError() failed !!! , Generating Audio Source Failed .\n");
        uninitializeAudio();
        return; // Return -ve value in OpenGL code
    }

    // Step 4 => Attach the above created audio buffer to above created audio source
    alSourcei(audioSource , AL_BUFFER , audioBuffer);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        printf("alSourcei() failed !!! , Attaching  audio buffer to audio source failed.\n");
        uninitializeAudio();
        return; // Return -ve value in OpenGL code
    }

    // Step 5 => Play the audio from the source
    alSourcePlay(audioSource);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        printf("alSourcePlay() failed !!!\n");
        uninitializeAudio();
        return; // Return -ve value in OpenGL code
    }
}

void uninitializeAudio(void)
{
    // Variable declarations
    ALint state;

    // code
    // Stop Playing the audio from the source
    alGetSourcei(audioSource , AL_SOURCE_STATE , &state);
    if (state == AL_PLAYING)
    {
        alSourceStop(audioSource);
        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            printf("alSourceStop() Failed . \n");
        }
    }

    // Detach the audio buffer from the audio source
    alSourcei(audioSource , AL_BUFFER , 0);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        printf("alSourcei() Failed . Dettaching audio buffer to audio source failed . \n");
    }

    // Delete the audio source
    alDeleteSources(1 , &audioSource);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        printf("alDeleteSources() Failed , Deleting audio source failed \n");
    }
    else
    {
        audioSource = 0;
    }

    // Delete the audio buffer
    alDeleteBuffers(1 , &audioBuffer);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        printf("alDeleteBuffers() Failed , Deleting audio buffer failed \n");
    }
    else 
    {
        audioBuffer = 0;
    }

    // Uninitialize ALUT
    alutExit();
}

void resize(int width , int height)
{
    // code
    if (height == 0)
    {
        height = 1;
    }

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    gluPerspective(45.0f , (GLfloat)width / (GLfloat)height , 0.1f , 100.0f);
}

GLuint loadGLTexture(const char *path)
{
    // local variable
    int width, height;
    unsigned char *imageData = NULL;
    GLuint texture;

    // code
    imageData = SOIL_load_image(path ,
                                &width ,
                                &height ,
                                NULL ,
                                SOIL_LOAD_RGB);

    // Create OpenGL Texture
    glGenTextures(1 , &texture);

    // BLind to the Generated Tecture
    glBindTexture(GL_TEXTURE_2D , texture);

    // Unpack The loaded image
    glPixelStorei(GL_UNPACK_ALIGNMENT , 4);

    // Set Texture Parameters
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR);

    // Create Multiple Mipmap images
    gluBuild2DMipmaps(GL_TEXTURE_2D , GL_RGB , width , height , GL_RGB , GL_UNSIGNED_BYTE , (void *)imageData);

    printf("Till 2d Bitmap\n");

    // Unbind Texture
    // glBindTexture(GL_TEXTURE_2D , 0); 

    SOIL_free_image_data(imageData);

    return texture;
}

void draw(void)
{
    // Function prototype
    void DrawGrid();
    void DrawB(float);
    void DrawH(float);
    void DrawA(float);
    void DrawR(float);
    void DrawT(float);
    void DrawI(float);
    void DrawN(float);
    void DrawD(float);
    void DrawINDIA();
    void DrawJet();
    void DrawBHARAT();
    void DrawJetFromSide();
    void End_Scene_Animation();

    // code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glTranslatef(0.0f , 0.0f , -3.0f);

    // DrawJetFromSide();

    // DrawJet();

    // DrawINDIA();

    End_Scene_Animation();

    glXSwapBuffers(display , window);
}

void End_Scene_Animation()
{
    // variable declarations
    float fXaxis , fYaxis;

    void DrawB(float);
    void DrawH(float);
    void DrawA(float);
    void DrawR(float);
    void DrawT(float);
    void DrawI(float);
    void DrawN(float);
    void DrawD(float);
    void DrawINDIA();
    void DrawJet();
    void DrawBHARAT();
    void DrawJetFromSide();

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    {
        DrawINDIA();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-3.5f + Jet_From_Side_Counter , -0.400000 + 0.390000, 0.0f);
        glRotatef(180.0f , 0.0f , 1.0f , 0.0f);
        // glScalef(0.730000 , 0.879999 , 1.0);
        glScalef(0.30000 , 0.39999 , 1.0);

        DrawJetFromSide();
        
    }
    glPopMatrix();

    glPushMatrix();
    {   
        
        // glTranslatef(XTranslate , YTranslate , 0.0f);
        // glBegin(GL_POLYGON);
        // for(float i = 0; i < 360.0; i+= 0.1f)
        // {     

        //     fXaxis = cos(i * 3.14/180.0) * ScaleX;
        //     fYaxis = sin(i * 3.14/180.0) * ScaleY;

        //     glColor3f(1.0f , 1.0f , 1.0f);
        //     glVertex3f(fXaxis , fYaxis , 0.0f);
           
        // }
        // glEnd();

        if (i > 0.0f  && Jet_From_Side_Counter > 0.5f)
        {
            fXaxis = cos(i * 3.14/180.0) * 2.679998;
            fYaxis = sin(i * 3.14/180.0) * 0.939999;

            glTranslatef(0.010000 + fXaxis , -1.229999 + fYaxis , 0.0f);

            glRotatef(180.0f , 0.0f , 1.0f , 0.0f);

            // glRotatef(Jet_Angle, 0.0F , 0.0F , -1.0F); 

            glScalef(0.30000 , 0.39999 , 1.0); 

            DrawJetFromSide();
        }
        
        // if (i > 0.0f && i < 90.0f)
        // {
        //     fXaxis = cos(i * 3.14/180.0) * 1.0;
        //     fYaxis = sin(i * 3.14/180.0) * 1.0;

        //     glTranslatef(fXaxis , fYaxis , 0.0f);
        // }
        
          
        // else
        //     glTranslatef(-4.0f + Jet_From_Side_Counter , -0.400000 + 0.390000 - Jet_Transition_Y - 0.1, 0.0f);
        
        
        // glScalef(0.730000 , 0.879999 , 1.0);
       
        
    }
    glPopMatrix();

    glPushMatrix();
    {    
        // glTranslatef(XTranslate , YTranslate , 0.0f);
        // glBegin(GL_POLYGON);
        // for(float i = 0; i < 360.0; i+= 0.1f)
        // {     

        //     fXaxis = cos(i * 3.14/180.0) * ScaleX;
        //     fYaxis = sin(i * 3.14/180.0) * ScaleY;

        //     glColor3f(1.0f , 1.0f , 1.0f);
        //     glVertex3f(fXaxis , fYaxis , 0.0f);
           
        // }
        // glEnd();

          
        if (j < 360.0f  && Jet_From_Side_Counter > 0.5f)
        {
            fXaxis = cos(j * 3.14/180.0) * 2.619998;
            fYaxis = sin(j * 3.14/180.0) * 0.869999;

            glTranslatef(fXaxis , 1.15 + fYaxis , 0.0f);

            glRotatef(180.0f, 0.0f , 1.0f , 0.0f);
            // glRotatef(Jet_Angle , 0.0F , 0.0F , 1.0F);

            glScalef(0.30000 , 0.39999 , 1.0);
        
            DrawJetFromSide();
        
        }
         
        // glTranslatef(-4.0f + Jet_From_Side_Counter , -0.400000 + 0.390000 + Jet_Transition_Y + 0.1, 0.0f);
        
        
        
        // glScalef(0.730000 , 0.879999 , 1.0);
        
    }
    glPopMatrix();

    glPushMatrix();
    {
        DrawBHARAT();
    }
    glPopMatrix();

    if (Jet_From_Side_Counter < 8.0f && Transition_A_X <= 1.01)
    {
        Jet_From_Side_Counter += 0.002f;
    }

    if (Jet_Angle > 0.0f && Jet_From_Side_Counter > 1.0f)
    {
        Jet_Angle -= 0.1;
    }
    

    if (Jet_From_Side_Counter > 1.5f && Jet_From_Side_Counter < 3.5f && Jet_Transition_Y > 0.1f )
    { 
       Jet_Transition_Y -= 0.001f;
    }
    
    if (Jet_From_Side_Counter > 3.0f && Jet_From_Side_Counter < 3.5f )
    { 
        ALpha_India_I1 -= 0.01f;
        ALpha_Bharat_B += 0.01f;
    }

    if (Jet_From_Side_Counter > 3.5f && Jet_From_Side_Counter < 4.0f)
    {
        ALpha_India_N -= 0.01f;
        ALpha_Bharat_H += 0.01f;
    }

    if (Jet_From_Side_Counter > 4.0f && Jet_From_Side_Counter < 4.5f)
    {
        ALpha_India_D -= 0.01f;
        ALpha_Bharat_A1 += 0.01f;
    }

    if (Jet_From_Side_Counter > 4.5f && Jet_From_Side_Counter < 5.0f)
    {
        ALpha_India_I2 -= 0.01f;
        
    }

    if (Jet_From_Side_Counter > 5.0f && Jet_From_Side_Counter < 5.5f)
    {
        Jet_Angle += 0.1;
        ALpha_India_A -= 0.01f;
        ALpha_Bharat_R += 0.01f;
        ALpha_Bharat_A2 += 0.01f;
        Jet_Transition_Y += 0.001f;  
    }

    if (Jet_From_Side_Counter > 5.5f && Jet_From_Side_Counter < 6.0f)
    {
        ALpha_Bharat_T += 0.01f;
        Jet_Transition_Y += 0.001f;
    }

    if (Jet_From_Side_Counter > 2.0f && Egnition_Counter < 2.0f)
    {
        Egnition_Counter += 0.00001f;
    }
}

void DrawGrid()
{
    glColor3f(1.0f ,1.0f ,1.0f);
    glBegin(GL_LINES);
    
    glVertex3f(0.0f , 1.0f , 0.0f);
    glVertex3f(0.0f , -1.0f , 0.0f);

    glVertex3f(-1.0f , 0.0f , 0.0f);
    glVertex3f(1.0f , 0.0f , 0.0f);

    glVertex3f(-0.4f , 1.0f , 0.0f);
    glVertex3f(-0.4f, -1.0f , 0.0f);

    glVertex3f(0.4f , 1.0f , 0.0f);
    glVertex3f(0.4f , -1.0f , 0.0f);

    glVertex3f(-1.0f , 0.4f , 0.0f);
    glVertex3f(1.0f , 0.4f , 0.0f);

    glVertex3f(-1.0f , -0.4f , 0.0f);
    glVertex3f(1.0f , -0.4f , 0.0f);

    glEnd();
}

void DrawB(float alpha)
{
    glPushMatrix();
    {
        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.35f , 0.0f);

        glColor4f(1.0f ,1.0f , 1.0f , alpha);
        glVertex3f(-0.4f , 0.05f , 0.0f);

        glColor4f(1.0f ,1.0f , 1.0f , alpha);
        glVertex3f(-0.4f , -0.05f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.4f , -0.4f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.3f , -0.4f , 0.0f);

        glColor4f(1.0f ,1.0f , 1.0f , alpha);
        glVertex3f(-0.3f , -0.05f , 0.0f);

        glColor4f(1.0f ,1.0f , 1.0f , alpha);
        glVertex3f(-0.3f , 0.05f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.25f , 0.0f);

        glEnd();

        // glBegin(GL_POLYGON);

        // glColor4f(1.0f , 1.0f , 1.0f)

        // glVertex3f(-0.4f , 0.05f , 0.0f);

        // glVertex3f(-0.4f , -0.05f , 0.0f);

        // glVertex3f(-0.3f , -0.05f , 0.0f);

        // glVertex3f(-0.3f , 0.05f , 0.0f);

        // glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.4f , 0.0f);
        glVertex3f(-0.3f , 0.3f , 0.0f);
        glVertex3f(0.2f , 0.3f  , 0.0f);
        glVertex3f(0.3f , 0.4f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.4f , 0.0f);
        glVertex3f(0.4f , 0.25f , 0.0f);
        glVertex3f(0.3f , 0.2f , 0.0f);
        glVertex3f(0.2f , 0.3f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.25f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.3f , 0.1f , 0.0f);
        glVertex3f(0.3f , -0.1f, 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.3f , -0.25f , 0.0f);
        glVertex3f(0.4f , -0.35f , 0.0f);

        glColor4f(1.0f , 1.0f ,1.0f , alpha);
        glVertex3f(0.4f , -0.1f , 0.0f);
        glVertex3f(0.4f , 0.1f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.4f , 0.25f , 0.0f);

        glEnd();

        // Green
        glBegin(GL_POLYGON);
        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);

        glVertex3f(-0.3f , -0.4f , 0.0f);
        glVertex3f(-0.3f , -0.3f , 0.0f);
        glVertex3f(0.3f , -0.3f , 0.0f);
        glVertex3f(0.4f , -0.4f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // White
        glColor4f(1.0f ,1.0f , 1.0f , alpha);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.05f , 0.0f);

        glVertex3f(-0.2f , 0.05f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(-0.2f , 0.0f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.2f , -0.05f , 0.0f);
        glVertex3f(0.3f , -0.05f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.3f , 0.3f , 0.0f);

        glEnd();
        
    }
    glPopMatrix();
}

void DrawH(float alpha)
{
    glPushMatrix();
    {
        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.4f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.4f , 0.05f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.4f , -0.05f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.4f , -0.4f , 0.0f);
        
        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.3f , -0.4f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.3f , -0.05f , 0.0f);
        
        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.3f , 0.05f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.3f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.05f , 0.0f);

        glVertex3f(0.25f , 0.05f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.25f , 0.0f  , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.25f , -0.05f , 0.0f);

        glVertex3f(-0.3f , -0.05f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(-0.3f , 0.0f  , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.4f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.3f , 0.05f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.3f , -0.3f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.4f , -0.4f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.4f , 0.05f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.4f , 0.4f , 0.0f);

        glEnd();
    }
    glPopMatrix();
}

void DrawA(float alpha)
{
    glPushMatrix();
    {
        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.35f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.4f , 0.05f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.4f , -0.05f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.4f , -0.4f , 0.0f);
        
        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.3f , -0.4f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.3f , -0.05f , 0.0f);
        
        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.3f , 0.05f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.25f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.05f , 0.0f);

        glVertex3f(0.25f , 0.05f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.25f , 0.0f  , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.25f , -0.05f , 0.0f);

        glVertex3f(-0.3f , -0.05f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(-0.3f , 0.0f  , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.4f , 0.0f);
        glVertex3f(-0.3f , 0.3f , 0.0f);
        glVertex3f(0.2f , 0.3f  , 0.0f);
        glVertex3f(0.3f , 0.4f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.4f , 0.0f);
        glVertex3f(0.4f , 0.25f , 0.0f);
        glVertex3f(0.3f , 0.2f , 0.0f);
        glVertex3f(0.2f , 0.3f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.25f , 0.0f);

        glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.3f , 0.1f , 0.0f);
        glVertex3f(0.3f , -0.1f, 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.3f , -0.4f , 0.0f);
        glVertex3f(0.4f , -0.4f , 0.0f);

        glColor4f(1.0f , 1.0f ,1.0f , alpha);
        glVertex3f(0.4f , -0.1f , 0.0f);
        glVertex3f(0.4f , 0.1f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.4f , 0.25f , 0.0f);

        glEnd();
    }
    glPopMatrix();
}

void DrawA2(float alpha)
{
    glPushMatrix();
    {
        glBegin(GL_POLYGON);

        glColor4f(134/255.0 , 163/255.0 , 184/255.0 , alpha);
        glVertex3f(-0.4f , 0.35f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.4f , 0.05f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.4f , -0.05f , 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.4f , -0.4f , 0.0f);
        
        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.3f , -0.4f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.3f , -0.05f , 0.0f);
        
        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.3f , 0.05f , 0.0f);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.25f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.05f , 0.0f);

        glVertex3f(0.25f , 0.05f , 0.0f);

        // glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.25f , 0.0f  , 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.25f , -0.05f , 0.0f);

        glVertex3f(-0.3f , -0.05f , 0.0f);

        // glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(-0.3f , 0.0f  , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.4f , 0.0f);
        glVertex3f(-0.3f , 0.3f , 0.0f);
        glVertex3f(0.2f , 0.3f  , 0.0f);
        glVertex3f(0.3f , 0.4f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.4f , 0.0f);
        glVertex3f(0.4f , 0.25f , 0.0f);
        glVertex3f(0.3f , 0.2f , 0.0f);
        glVertex3f(0.2f , 0.3f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.25f , 0.0f);

        // glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.3f , 0.1f , 0.0f);
        glVertex3f(0.3f , -0.1f, 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.3f , -0.4f , 0.0f);
        glVertex3f(0.4f , -0.4f , 0.0f);

        // glColor4f(1.0f , 1.0f ,1.0f , alpha);
        glVertex3f(0.4f , -0.1f , 0.0f);
        glVertex3f(0.4f , 0.1f , 0.0f);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.4f , 0.25f , 0.0f);

        glEnd();
    }
    glPopMatrix();
}

void DrawR(float alpha)
{
    glPushMatrix();
    {
        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.35f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.4f , 0.05f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.4f , -0.05f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.4f , -0.4f , 0.0f);
        
        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.3f , -0.4f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.3f , -0.05f , 0.0f);
        
        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.3f , 0.05f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.25f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.4f , 0.0f);
        glVertex3f(-0.3f , 0.3f , 0.0f);
        glVertex3f(0.2f , 0.3f  , 0.0f);
        glVertex3f(0.3f , 0.4f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.4f , 0.0f);
        glVertex3f(0.4f , 0.2f , 0.0f);
        glVertex3f(0.3f , 0.15f , 0.0f);
        glVertex3f(0.2f , 0.3f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glVertex3f(0.3f , 0.3f , 0.0f);

        glVertex3f(0.3f , 0.05f , 0.0f);

        glVertex3f(0.4f , 0.05f , 0.0f);

        glVertex3f(0.4f , 0.2f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.2f , 0.05f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.4f , 0.05f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(0.4f , 0.0f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.4f , -0.05f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.2f , -0.05f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.2f , 0.0f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);

        glVertex3f(-0.2f , -0.05f , 0.0f);

        glVertex3f(-0.2f , -0.25f , 0.0f);

        glVertex3f(-0.1f , -0.2f , 0.0f);

        glVertex3f(-0.1f , -0.05f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glVertex3f(-0.2f , -0.25f , 0.0f);

        glVertex3f(0.0f , -0.4f , 0.0f);

        glVertex3f(0.0f , -0.3f , 0.0f);

        glVertex3f(-0.1f , -0.2f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glVertex3f(0.0f , -0.3f , 0.0f);

        glVertex3f(0.3f , -0.3f , 0.0f);

        glVertex3f(0.4f , -0.4f , 0.0f);

        glVertex3f(-0.0f , -0.4f , 0.0f);

        glEnd();
    }
    glPopMatrix();
}

void DrawT(float alpha)
{
    glPushMatrix();
    {
        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);

        glVertex3f(-0.4f , 0.4f , 0.0f);

        glVertex3f(0.4f , 0.4f , 0.0f);

        glVertex3f(0.4f , 0.3f , 0.0f);

        glVertex3f(-0.3f , 0.3f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.05f , 0.3f , 0.0f);
        
        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.05f , 0.05f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.05f , -0.4f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.05f , -0.3f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(0.05f , 0.05f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.05f , 0.3f , 0.0f);

        glEnd();
    }
    glPopMatrix();
}

void DrawI(float alpha)
{
    glPushMatrix();
    {
        glBegin(GL_POLYGON);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.05f , 0.4f , 0.0f);
        
        glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.05f , 0.05f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha); 
        glVertex3f(-0.05f , -0.4f , 0.0f);

        glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha); 
        glVertex3f(0.05f , -0.3f , 0.0f);

        glColor4f(1.0f , 1.0f , 1.0f  , alpha);
        glVertex3f(0.05f , 0.05f , 0.0f);

        glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.05f , 0.4f , 0.0f);

        glEnd();
    }
    glPopMatrix();
}

void DrawI2(float alpha)
{
    glPushMatrix();
    {
        glBegin(GL_POLYGON);

        glColor4f(134/255.0 , 163/255.0 , 184/255.0 , alpha);
        glVertex3f(-0.05f , 0.4f , 0.0f);
        
        // glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.05f , 0.05f , 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha); 
        glVertex3f(-0.05f , -0.4f , 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha); 
        glVertex3f(0.05f , -0.3f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f  , alpha);
        glVertex3f(0.05f , 0.05f , 0.0f);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.05f , 0.4f , 0.0f);

        glEnd();
    }
    glPopMatrix();
}

void DrawN(float alpha)
{
    glPushMatrix();
    {
         glBegin(GL_POLYGON);

        glColor4f(134/255.0 , 163/255.0 , 184/255.0 , alpha);
        glVertex3f(-0.4f , 0.35f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.4f , 0.05f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.4f , -0.05f , 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.4f , -0.4f , 0.0f);
        
        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.3f , -0.4f , 0.0f);

        // glColor4f(1.0f , 1.0f , 1.0f , alpha);
        glVertex3f(-0.3f , -0.05f , 0.0f);
        
        // glColor4f(1.0f , 1.0f , 1.0f);
        // glVertex3f(-0.3f , 0.05f , 0.0f);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.25f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.4f , 0.0f);
        glVertex3f(-0.3f , 0.3f , 0.0f);
        glVertex3f(0.2f , 0.3f  , 0.0f);
        glVertex3f(0.3f , 0.4f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.4f , 0.0f);
        glVertex3f(0.4f , 0.25f , 0.0f);
        glVertex3f(0.3f , 0.2f , 0.0f);
        glVertex3f(0.2f , 0.3f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.25f , 0.0f);

        // glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.3f , 0.1f , 0.0f);
        glVertex3f(0.3f , -0.1f, 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.3f , -0.4f , 0.0f);
        glVertex3f(0.4f , -0.4f , 0.0f);

        // glColor4f(1.0f , 1.0f ,1.0f , alpha);
        glVertex3f(0.4f , -0.1f , 0.0f);
        glVertex3f(0.4f , 0.1f , 0.0f);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.4f , 0.25f , 0.0f);

        glEnd();
    }
    glPopMatrix();
}

void DrawD(float alpha)
{
    glPushMatrix();
    {
        glBegin(GL_POLYGON);

        glColor4f(134/255.0 , 163/255.0 , 184/255.0 , alpha);
        glVertex3f(-0.4f , 0.35f , 0.0f);

        // glColor4f(1.0f ,1.0f , 1.0f , alpha);
        glVertex3f(-0.4f , 0.05f , 0.0f);

        // glColor4f(1.0f ,1.0f , 1.0f , alpha);
        glVertex3f(-0.4f , -0.05f , 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.4f , -0.4f , 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(-0.3f , -0.4f , 0.0f);

        // glColor4f(1.0f ,1.0f , 1.0f , alpha);
        glVertex3f(-0.3f , -0.05f , 0.0f);

        // glColor4f(1.0f ,1.0f , 1.0f , alpha);
        glVertex3f(-0.3f , 0.05f , 0.0f);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.3f , 0.25f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(-0.4f , 0.4f , 0.0f);
        glVertex3f(-0.3f , 0.3f , 0.0f);
        glVertex3f(0.2f , 0.3f  , 0.0f);
        glVertex3f(0.3f , 0.4f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.4f , 0.0f);
        glVertex3f(0.4f , 0.25f , 0.0f);
        glVertex3f(0.3f , 0.2f , 0.0f);
        glVertex3f(0.2f , 0.3f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.3f , 0.25f , 0.0f);

        // glColor4f(1.0f ,1.0f ,1.0f , alpha);
        glVertex3f(0.3f , 0.1f , 0.0f);
        glVertex3f(0.3f , -0.1f, 0.0f);

        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);
        glVertex3f(0.3f , -0.25f , 0.0f);
        glVertex3f(0.4f , -0.35f , 0.0f);

        // glColor4f(1.0f , 1.0f ,1.0f , alpha);
        glVertex3f(0.4f , -0.1f , 0.0f);
        glVertex3f(0.4f , 0.1f , 0.0f);

        // glColor4f(248/255.0 , 105/255.0 , 37/255.0 , alpha);
        glVertex3f(0.4f , 0.25f , 0.0f);

        glEnd();

        // Green
        glBegin(GL_POLYGON);
        // glColor4f(5/255.0 , 141/255.0 , 55/255.0 , alpha);

        glVertex3f(-0.3f , -0.4f , 0.0f);
        glVertex3f(-0.3f , -0.3f , 0.0f);
        glVertex3f(0.3f , -0.3f , 0.0f);
        glVertex3f(0.4f , -0.4f , 0.0f);

        glEnd();

    }
    glPopMatrix();
}

void DrawBHARAT()
{
     glPushMatrix();
    {
        glTranslatef(-0.210000 , 0.0 , 0.0f);
        glScalef(0.869999 , 0.869999 , 1.0f);
        glPushMatrix();
        {
            glTranslatef(-1.539999 , 0.0F , 0.0F);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawB(ALpha_Bharat_B);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(-0.750000 , 0.0F , 0.0F);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawH(ALpha_Bharat_H);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(-0.370000 + 0.4f, 0.0F , 0.0F);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawA(ALpha_Bharat_A1);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef( 0.8f , 0.0F , 0.0F);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawR(ALpha_Bharat_R);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(0.380000 + 1.2f , 0.0F , 0.0F);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawA(ALpha_Bharat_A2);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(0.670000 + 1.6 , 0.0F , 0.0F);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawT(ALpha_Bharat_T);
        }
        glPopMatrix();

    }
    glPopMatrix();
}

void DrawINDIA()
{
    glPushMatrix();
    {
        glTranslatef(-0.210000 , 0.0 , 0.0f);
        glScalef(0.869999 , 0.869999 , 1.0f);
        glPushMatrix();
        {
            // glTranslatef(-1.2599 , 0.0F , 0.0F);
            glTranslatef(-Transition_I1_X, 0.0f , 0.0f);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawI2(ALpha_India_I1);

        }
        glPopMatrix();

        glPushMatrix();
        {
            // glTranslatef(-0.750000 , 0.0F , 0.0F);
            glTranslatef(-Transition_N_X , Transition_N_Y , 0.0f);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawN(ALpha_India_N);
        }
        glPopMatrix();

        glPushMatrix();
        {
            // glTranslatef(0.05, 0.0F , 0.0F);
            glTranslatef(Transition_D_X , Transition_D_Y , 0.0f);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawD(ALpha_India_D);
        }
        glPopMatrix();

        glPushMatrix();
        {
            // glTranslatef( 0.52, 0.0F , 0.0F);
            glTranslated(Transition_I2_X , Transition_I2_Y , 0.0f);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawI2(ALpha_India_I2);
        }
        glPopMatrix();

        glPushMatrix();
        {
            // glTranslatef(1.01, 0.0F , 0.0F);
            glTranslatef(Transition_A_X , 0.0f , 0.0f);
            glScalef(0.939999 , 0.939999 , 1.0f);

            DrawA2(ALpha_India_A);
        }
        glPopMatrix();

    }
    glPopMatrix();

    if (Transition_I1_X > 1.2599f)
    {
        Transition_I1_X -= 0.001f;
    }

    if (Transition_N_X > 0.750000f)
    {
        Transition_N_X -= 0.001f;
    }

    if (Transition_N_Y > 0.0f)
    {
        Transition_N_Y -= 0.001f;
    }

    if (Transition_D_X < 0.05)
    {
        Transition_D_X += 0.001f;
    }

    if (Transition_D_Y < 0.0f)
    {
        Transition_D_Y += 0.001f;
    }

    if (Transition_I2_X < 0.52)
    {
        Transition_I2_X += 0.001f;
    }

    if (Transition_I2_Y > 0.0f)
    {
        Transition_I2_Y -= 0.001f;
    }

    if (Transition_A_X > 1.01)
    {
        Transition_A_X -= 0.001f;
    }

}

void DrawJetFromSide()
{
    GLfloat xAxis , yAxis;
    glPushMatrix();
    {
        // jet main body
        glBegin(GL_POLYGON);

        glColor3f(0.0f , 0.0f , 0.0f);

        glVertex3f(-0.2f , 0.2f , 0.0f);
        glVertex3f(0.2f , 0.1f , 0.0f);
        glVertex3f(0.2f , -0.2f , 0.0f);
        glVertex3f(-0.2f , -0.2f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glVertex3f(-0.35f , 0.0f , 0.0f);
        glVertex3f(-0.35f , 0.25f , 0.0f);
        glVertex3f(-0.2f , 0.2f , 0.0f);
        glVertex3f(-0.2f , -0.2f , 0.0f);

        glEnd();

        // jet tail
        glBegin(GL_POLYGON);

        glVertex3f(0.2f , 0.1f , 0.0f);
        glVertex3f(0.45f , 0.05f , 0.0f);
        glVertex3f(0.45f , -0.2f , 0.0f);
        glVertex3f(0.2f , -0.2f , 0.0f);

        glEnd();

        // jet end elivasion
        glBegin(GL_POLYGON);

        glVertex3f(0.45f , 0.05f , 0.0f);
        glVertex3f(0.65f , 0.15f , 0.0f);
        glVertex3f(0.65f , -0.2f , 0.0f);
        glVertex3f(0.45f , -0.2f , 0.0f);

        glEnd();

        // Jet Wings
        glBegin(GL_POLYGON);

        glVertex3f(0.65f , 0.15f , 0.0f);
        glVertex3f(0.85f , 0.4f , 0.0f);
        glVertex3f(0.95f , 0.35f , 0.0f);
        glVertex3f(0.95f , -0.2f , 0.0f);
        glVertex3f(0.65f , -0.2f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glVertex3f(0.95f , 0.2f , 0.0f);
        glVertex3f(0.95f , 0.0f , 0.0f);
        glVertex3f(0.98f , 0.0f , 0.0f);
        glVertex3f(0.98 , 0.2f , 0.0f);

        glEnd();

        // Egnition animation
        glPushMatrix();
        {
            glBegin(GL_POLYGON);

            glColor3f(248/255.0 , 105/255.0 , 37/255.0);
            glVertex3f(0.98f , 0.2f , 0.0f);

            glColor3f(1.0f , 1.0f , 1.0f);
            glVertex3f(0.98f , 0.1f , 0.0f);

            glColor3f(5/255.0 , 141/255.0 , 55/255.0 );
            glVertex3f(0.98f , 0.0f , 0.0f);

            glColor3f(5/255.0 , 141/255.0 , 55/255.0 );
            glVertex3f(1.0f + Egnition_Counter, 0.0f , 0.0);

            glColor3f(1.0f , 1.0f , 1.0f);
            glVertex3f(1.0f + Egnition_Counter, 0.1f , 0.0f);
            
            glColor3f(248/255.0 , 105/255.0 , 37/255.0);
            glVertex3f(1.0f + Egnition_Counter, 0.2f , 0.0f);

            glEnd();

            glColor3f(0.0f , 0.0f , 0.0f);
        }
        glPopMatrix();

        // jet front part
        glPushMatrix();
        {
            // jet cockpit
            glTranslatef(-0.490000 , 0.190000 , 0.0f);
            glRotatef(276.000000 , 0.0F , 0.0F , 1.0F);
            glScalef(0.320000 , 0.959999, 1.0f);
            glBegin(GL_LINE_LOOP);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.2f;
                yAxis = sin(i * 3.14/180.0) * 0.2f;      

                glVertex3f(xAxis , yAxis , 0.0f);
                glVertex3f(xAxis + 0.1, yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

        glPushMatrix();
        {
            // jet nose
            glTranslatef(-0.520000 , 0.080000 , 0.0f);
            glScalef(-3.139997, -0.470000 , 1.0f);
            glBegin(GL_POLYGON);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.2f;
                yAxis = sin(i * 3.14/180.0) * 0.2f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(-1.219999 , 0.080000 , 0.0f);
            glScalef(-0.740000, -0.030000 , 1.0f);
            glBegin(GL_POLYGON);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.2f;
                yAxis = sin(i * 3.14/180.0) * 0.2f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(0.849999 , 0.250000 , 0.0f);
            glScalef(0.060000 , 0.060000 , 1.0f);
            glBegin(GL_POLYGON);

            glColor3f(248/255.0 , 105/255.0 , 37/255.0);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.8f;
                yAxis = sin(i * 3.14/180.0) * 0.8f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(0.849999 , 0.250000 , 0.0f);
            glScalef(0.060000 , 0.060000 , 1.0f);
            glBegin(GL_POLYGON);

            glColor3f(1.0f , 1.0f , 1.0f);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.4f;
                yAxis = sin(i * 3.14/180.0) * 0.4f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

         glPushMatrix();
        {
            glTranslatef(0.849999 , 0.250000 , 0.0f);
            glScalef(0.060000 , 0.060000 , 1.0f);
            glBegin(GL_POLYGON);

            glColor3f(5/255.0 , 141/255.0 , 55/255.0);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.2f;
                yAxis = sin(i * 3.14/180.0) * 0.2f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

     }
    glPopMatrix();
}

void DrawJet()
{
    GLfloat xAxis , yAxis;
    glPushMatrix();
    {
       
        // jet main body
        glBegin(GL_POLYGON);

        glColor3f(0.0f , 0.0f , 0.0f);

        glVertex3f(-0.2f , 0.1f , 0.0f);
        glVertex3f(0.2f , 0.1f , 0.0f);
        glVertex3f(0.2f , -0.1f , 0.0f);
        glVertex3f(-0.2f , -0.1f , 0.0f);

        glEnd();

        // Jett Upper Wing
        glBegin(GL_POLYGON);

        glVertex3f(-0.2f , 0.1f , 0.0f);
        glVertex3f(0.2f , 0.4f , 0.0f);
        glVertex3f(0.3f , 0.4f , 0.0f);
        glVertex3f(0.2f,  0.1f , 0.0f);

        glEnd();

        // Jet Lower Wing
        glBegin(GL_POLYGON);

        glVertex3f(-0.2f , -0.1f , 0.0f);
        glVertex3f(0.2f , -0.4f , 0.0f);
        glVertex3f(0.3f , -0.4f , 0.0f);
        glVertex3f(0.2f,  -0.1f , 0.0f);

        glEnd();

        // jet base body part
        glBegin(GL_POLYGON);

        glVertex3f(0.2f , 0.1f , 0.0f);
        glVertex3f(0.35f , 0.1f , 0.0f);
        glVertex3f(0.35f , -0.1f , 0.0f);
        glVertex3f(0.2f , -0.1f , 0.0f);

        glEnd();
        
        // jet base part
        glBegin(GL_POLYGON);

        glVertex3f(0.3f , 0.1f , 0.0f);
        glVertex3f(0.35f , 0.15f , 0.0f);
        glVertex3f(0.45f , 0.15f , 0.0f);
        glVertex3f(0.4f , 0.1f , 0.0f);

        glEnd();

        // jet smaller wing upper side
        glBegin(GL_POLYGON);

        glVertex3f(0.35f , 0.15f , 0.0f);
        glVertex3f(0.45f , 0.25f , 0.0f);
        glVertex3f(0.55f , 0.25f , 0.0f);
        glVertex3f(0.45f , 0.15f , 0.0f);

        glEnd();

        glBegin(GL_POLYGON);

        glVertex3f(0.3f , -0.1f , 0.0f);
        glVertex3f(0.35f , -0.15f , 0.0f);
        glVertex3f(0.45f , -0.15f , 0.0f);
        glVertex3f(0.4f , -0.1f , 0.0f);

        glEnd();

        // jet smaller wing lower side
        glBegin(GL_POLYGON);

        glVertex3f(0.35f , -0.15f , 0.0f);
        glVertex3f(0.45f , -0.25f , 0.0f);
        glVertex3f(0.55f , -0.25f , 0.0f);
        glVertex3f(0.45f , -0.15f , 0.0f);

        glEnd();

        glPushMatrix();
        {
            glTranslatef(-0.340000 , 0.0f , 0.0f);
            glScalef(-2.029999 , -0.520000 , 1.0f);
            glBegin(GL_POLYGON);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.2f;
                yAxis = sin(i * 3.14/180.0) * 0.2f;

               

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(-0.750000 , 0.0F , 0.0f);
            glScalef(-0.760000 , -0.180000 , 1.0f);
            glBegin(GL_POLYGON);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.2f;
                yAxis = sin(i * 3.14/180.0) * 0.2f;

               

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(-0.909999 , 0.0f , 0.0f);
            glScalef(0.420000 , 0.030000 , 1.0f);
            glBegin(GL_POLYGON);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.2f;
                yAxis = sin(i * 3.14/180.0) * 0.2f;

               

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(0.160000 , 0.280000 , 0.0f);
            glScalef(-0.050000 , -0.050000 , 1.0f);
            glBegin(GL_POLYGON);

            glColor3f(248/255.0 , 105/255.0 , 37/255.0);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.8f;
                yAxis = sin(i * 3.14/180.0) * 0.8f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(0.160000 , 0.280000 , 0.0f);
            glScalef(-0.050000 , -0.050000 , 1.0f);
            glBegin(GL_POLYGON);

            glColor3f(1.0f , 1.0f , 1.0f);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.4f;
                yAxis = sin(i * 3.14/180.0) * 0.4f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

         glPushMatrix();
        {
            glTranslatef(0.160000 , 0.280000 , 0.0f);
            glScalef(-0.050000 , -0.050000 , 1.0f);
            glBegin(GL_POLYGON);

            glColor3f(5/255.0 , 141/255.0 , 55/255.0);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.2f;
                yAxis = sin(i * 3.14/180.0) * 0.2f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

         glPushMatrix();
        {
            glTranslatef(0.160000 , -0.280000 , 0.0f);
            glScalef(-0.050000 , -0.050000 , 1.0f);
            glBegin(GL_POLYGON);

            glColor3f(248/255.0 , 105/255.0 , 37/255.0);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.8f;
                yAxis = sin(i * 3.14/180.0) * 0.8f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(0.160000 , -0.280000 , 0.0f);
            glScalef(-0.050000 , -0.050000 , 1.0f);
            glBegin(GL_POLYGON);

            glColor3f(1.0f , 1.0f , 1.0f);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.4f;
                yAxis = sin(i * 3.14/180.0) * 0.4f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();

         glPushMatrix();
        {
            glTranslatef(0.160000 , -0.280000 , 0.0f);
            glScalef(-0.050000 , -0.050000 , 1.0f);
            glBegin(GL_POLYGON);

            glColor3f(5/255.0 , 141/255.0 , 55/255.0);
            
            for(float i = 0.0f ; i < 360.0f ; i += 0.2f)
            {          
                xAxis = cos(i * 3.14/180.0) * 0.2f;
                yAxis = sin(i * 3.14/180.0) * 0.2f;      

                glVertex3f(xAxis , yAxis , 0.0f);

            }
            glEnd();
            
        }
        glPopMatrix();
    }
    glPopMatrix();
}

void update(void)
{
    // code
    if (i > 0.0 && Jet_From_Side_Counter > 0.5f)
    {
        i -= 0.05;
    }

    if (j < 360.0 && Jet_From_Side_Counter > 0.5f)
    {
        j += 0.05;
    }
      
}

void uninitialize(void)
{
    // Local Variable Declaration
    GLXContext currentGLXContext = NULL;

    // code 
    if (visualinfo)
    {
        free(visualinfo);
        visualinfo = NULL;
    }

    // uncurrent the context
    currentGLXContext = glXGetCurrentContext();
    if (currentGLXContext != NULL && currentGLXContext == glxContext)
    {
        glXMakeCurrent(display , 0 , 0);
    }
    
    // Destroy the Context
    if (glxContext)
    {
        glXDestroyContext(display , glxContext);
        glxContext = NULL;
    }

    if (window)
    {
       XDestroyWindow(display , window);
    }

    if (colormap)
    {
       XFreeColormap(display , colormap); 
    }
    
    if (display)
    {
        XCloseDisplay(display);
        display = NULL;
    } 

    if (gpFile)
    {
        fclose(gpFile);
        gpFile = NULL;
    }  
}


// Step 1: Open the connection with Xserver and get diaplay
// Step 2: Get default screen from above display
// Step 3: Get default depth from above two
// Step 4: Get Visual info from above 3
// Step 5: Set window properties/attributes
// Step 6: Assign this colormap to global colormap
// Step 7: Set the style of window
// Step 8: Create the Window
// Step 9: Specify to which events this window should respond
// Step 10: SPecify Window Manager Delete ATOM
// Step 11: Add/Save above Atom as protocol for Window Manager
// Step 12: Give the caption to the Window
// Step 13: Show/Map the Window
// Step 14: Event Loop

// Full Screen
// Step 1 : Normal screen cha atom ghya fullscreen madhe taka
// Step 2 : Create window manager full screen state
// Step 3 : Fill the strcuture of event
// Step 4 : Send FIlled Event