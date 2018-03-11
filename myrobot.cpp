#include "Angel.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>
#define PI 3.14159265

using namespace std;

//#include <string>

typedef std::string mode;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
const int NumBallVertices = 342;
const GLfloat Radians = PI / 180.0;

vec4 points[NumVertices];
vec4 colors[NumVertices];

vec4 ball_vertices[NumBallVertices];
bool ball_flag = false;

vec4 ball_pos[2];
mode view = "-sv";
GLuint vColor;
GLuint buffer[4];
GLuint vao[2];

vec4 vertices[8] = {
    vec4( -0.5, -0.5,  0.5, 1.0 ),
    vec4( -0.5,  0.5,  0.5, 1.0 ),
    vec4(  0.5,  0.5,  0.5, 1.0 ),
    vec4(  0.5, -0.5,  0.5, 1.0 ),
    vec4( -0.5, -0.5, -0.5, 1.0 ),
    vec4( -0.5,  0.5, -0.5, 1.0 ),
    vec4(  0.5,  0.5, -0.5, 1.0 ),
    vec4(  0.5, -0.5, -0.5, 1.0 )
};

vec4 vertex_colors[6] = {
    vec4(1.0, 0.0, 0.0, 1.0),
    vec4(0.5, 1.0, 0.88, 1.0),
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(1.0, 0.5, 0.5, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0),  
    vec4(0.7, 0.88, 1.0, 1.0),
    
};

mat4 camView;

// Parameters controlling the size of the Robot's arm
const GLfloat BASE_HEIGHT      = 2.0;
const GLfloat BASE_WIDTH       = 4.0;
const GLfloat LOWER_ARM_HEIGHT = 5.0;
const GLfloat LOWER_ARM_WIDTH  = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 5.0;
const GLfloat UPPER_ARM_WIDTH  = 0.5;

// Shader transformation matrices
mat4  model_view;
GLuint ModelView, Projection;

// Array of rotation angles (in degrees) for each rotation axis
enum { Base = 0, LowerArm = 1, UpperArm = 2, NumAngles = 3 };
int Axis = Base;
GLfloat  Theta[NumAngles] = { 0.0 };

void quad( int a, int b, int c, int d ){
    static int index = 0;

    points[index] = vertices[a];
    index++;

    points[index] = vertices[b];
    index++;

    points[index] = vertices[c];
    index++;

    points[index] = vertices[a];
    index++;

    points[index] = vertices[c];
    index++;

    points[index] = vertices[d];
    index++;
}

void colorcube(){
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------

/* Define the three parts */
/* Note use of push/pop to return modelview matrix
to its state before functions were entered and use
rotation, translation, and scaling to create instances
of symbols (cube and cylinder) */

void base(){
    mat4 instance = (Translate( 0.0, 0.5 * BASE_HEIGHT, 0.0 ) * Scale(BASE_WIDTH,
                                                                      BASE_HEIGHT,
                                                                      BASE_WIDTH));

    glUniformMatrix4fv( ModelView, 1, GL_TRUE, Projection * camView * model_view * instance );

    for(int i = 0,j = 0; i < 6; i++,j++){
        colors[i * 6 + 0] = vertex_colors[j];
        colors[i * 6 + 1] = vertex_colors[j];
        colors[i * 6 + 2] = vertex_colors[j];
        colors[i * 6 + 3] = vertex_colors[j];
        colors[i * 6 + 4] = vertex_colors[j];
        colors[i * 6 + 5] = vertex_colors[j];
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);  
    glBufferData(GL_ARRAY_BUFFER, 36* sizeof(vec4), colors, GL_STATIC_DRAW);

    glBindVertexArray( vao[0] );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    glBindVertexArray( 0 );

}

//----------------------------------------------------------------------------

void upper_arm(){
    mat4 instance = ( Translate( 0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0 ) * Scale(UPPER_ARM_WIDTH,
                                                                            UPPER_ARM_HEIGHT,
                                                                            UPPER_ARM_WIDTH ) );
    
    //glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, Projection * camView * model_view * instance );

   
   for(int i = 0,j = 4; i < 6; i++,j--){
        colors[i * 6 + 0] = vertex_colors[j + 1];
        colors[i * 6 + 1] = vertex_colors[j + 1];
        colors[i * 6 + 2] = vertex_colors[j + 1];
        colors[i * 6 + 3] = vertex_colors[j + 1];
        colors[i * 6 + 4] = vertex_colors[j + 1];
        colors[i * 6 + 5] = vertex_colors[j + 1];
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);  
    glBufferData(GL_ARRAY_BUFFER, 36* sizeof(vec4), colors, GL_STATIC_DRAW);

    glBindVertexArray( vao[0] );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    glBindVertexArray( 0 );
}

//----------------------------------------------------------------------------

void lower_arm(){
    mat4 instance = ( Translate( 0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0 ) * Scale(LOWER_ARM_WIDTH,
                                                                            LOWER_ARM_HEIGHT,
                                                                            LOWER_ARM_WIDTH ) );
    
    //glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, Projection * camView * model_view * instance );

    for(int i = 0,j = 5; i < 6; i++,j--){
        colors[i * 6 + 0] = vertex_colors[j];
        colors[i * 6 + 1] = vertex_colors[j];
        colors[i * 6 + 2] = vertex_colors[j];
        colors[i * 6 + 3] = vertex_colors[j];
        colors[i * 6 + 4] = vertex_colors[j];
        colors[i * 6 + 5] = vertex_colors[j];
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);  
    glBufferData(GL_ARRAY_BUFFER, 36* sizeof(vec4), colors, GL_STATIC_DRAW);

    glBindVertexArray( vao[0] );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    glBindVertexArray( 0 );
}

void sphere(){
    //model_view = mat4(1.0);
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, Projection * camView * Translate(ball_pos[0].x, ball_pos[0].y, ball_pos[0].z) * Scale(0.5,0.5,0.5));
    glBindVertexArray(0);
    glBindVertexArray(vao[1]);
    glDrawArrays( GL_LINE_LOOP, 0, NumBallVertices);
}
//----------------------------------------------------------------------------

//-------------

bool canReach(){
    for(int i = 0; i < 2; i++){
        GLfloat distance_squre = pow(ball_pos[i].x , 2) + pow(ball_pos[i].y -  BASE_HEIGHT , 2);
        GLfloat arm_height_squre = pow(LOWER_ARM_HEIGHT + UPPER_ARM_HEIGHT , 2 );

        if( distance_squre > arm_height_squre){
            std::cout << "robot cannot reach ball" << std::endl;
            return false;
        }
    }
    return true;
}

void getball(){
    if(canReach()){
        float base_degree =  (atan(ball_pos[0].z  / ball_pos[0].x) * 180 / PI);
        Axis = 0;
        if(ball_pos[0].x > 0) Theta[Axis] = - base_degree;
        else Theta[Axis] =  180 - base_degree;     


        float y = abs(ball_pos[0].y - BASE_HEIGHT);
        float alpha = atan(y / ball_pos[0].x) * 180 / PI ;
        float theta_temp = acos(
            ( pow(LOWER_ARM_HEIGHT, 2) + pow(ball_pos[0].x , 2) + pow(y,2) - pow(UPPER_ARM_HEIGHT , 2) ) 
            /
            ( 2 * LOWER_ARM_HEIGHT * pow( pow(ball_pos[0].x , 2) + pow(y,2) , 0.5 ) )
            ) *180 / PI ;

       Axis = 1;
       if( ball_pos[0].x >= 0 ){
            if( 90 - alpha - theta_temp >= 0){
                
                Theta[Axis] = - ( 90 - alpha - theta_temp );
            }
            else{
                Theta[Axis] = alpha + theta_temp - 90;
            }
       }
       else if(ball_pos[0].x < 0 && ball_pos[0].y >= 0){
            Theta[Axis] = 360 - (90 - abs(alpha) - abs(theta_temp));
       }
       else{
           Theta[Axis] = 360 - (180 - ((90 - abs(alpha)) + abs(theta_temp)));
           cout << alpha <<  " " << theta_temp << endl;
       }
       

       theta_temp = acos(
           ( pow(LOWER_ARM_HEIGHT, 2) + pow(UPPER_ARM_HEIGHT , 2)  - (pow(ball_pos[0].x , 2) + pow(y,2) ) ) 
            /
            ( 2 * LOWER_ARM_HEIGHT * UPPER_ARM_HEIGHT )
       ) * 180 / PI;

       Axis = 2;
       Theta[Axis] = - (180 - theta_temp);
    }
}
//-------------
void switchCam(void){
    if(view == "-tv"){
        //LookAt(eye,at,up);
        camView = LookAt(vec4(0,11,0,1),vec4(0,0,0,1),vec4(0,0,-1,1));

    }
    else{
        camView = mat4(1.0);
    }
    glutPostRedisplay();
}

void display( void ){
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Accumulate ModelView Matrix as we traverse the tree
    model_view = RotateY(Theta[Base]);
    base();

    model_view *= ( Translate(0.0, BASE_HEIGHT, 0.0) * RotateZ(Theta[LowerArm]) );
    lower_arm();

    model_view *= ( Translate(0.0, LOWER_ARM_HEIGHT, 0.0) * RotateZ(Theta[UpperArm]) );
    upper_arm();

    if(ball_flag) sphere();

    glutSwapBuffers();
}

void init(void){
    colorcube();
    
    int k = 0;
    for(float phi = -80.0; phi <= 80.0; phi += 20.0){
        float phir = phi * Radians;
        float phir20 = (phi + 20.0) * Radians;

        for(float theta = -180.0; theta <= 180.0; theta += 20.0){
            float thetar = theta * Radians;
            ball_vertices[k] = vec4(sin(thetar) * cos(phir),
                                    cos(thetar) * cos(phir),
                                    sin(phir),1);
            k++;
            ball_vertices[k] = vec4(sin(thetar) * cos(phir20),
                                    cos(thetar) * cos(phir20),
                                    sin(phir20),1);
            k++;
        }
    }

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);
    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    vColor = glGetAttribLocation(program, "vColor");


    // Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );
    glBindVertexArray( vao[0] );

    // Create and initialize a buffer object
    glGenBuffers( 2, &buffer[0]);

    glBindBuffer( GL_ARRAY_BUFFER, buffer[0]);
    glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);  
    glBufferData(GL_ARRAY_BUFFER, 36* sizeof(vec4), NULL, GL_STATIC_DRAW); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // ball
    glGenVertexArrays( 1, &vao[1] );
    glBindVertexArray( vao[1] );

    glGenBuffers( 2, &buffer[2]);

    glBindBuffer( GL_ARRAY_BUFFER, buffer[2]);
    glBufferData( GL_ARRAY_BUFFER, sizeof(ball_vertices), ball_vertices, GL_DYNAMIC_DRAW );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    vec4 ball_color[NumBallVertices];
    for(int i = 0; i < NumBallVertices; i++) ball_color[i] = vec4(1.0,0.0,0.0,1);

    glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);  
    glBufferData(GL_ARRAY_BUFFER, 
    sizeof(ball_color), ball_color, GL_STATIC_DRAW); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    

    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );

    glEnable( GL_DEPTH );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

void mouse( int button, int state, int x, int y ){

    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
	// Incrase the joint angle
	Theta[Axis] += 5.0;
	if ( Theta[Axis] > 360.0 ) { Theta[Axis] -= 360.0; }
    }

    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ) {
	// Decrase the joint angle
	Theta[Axis] -= 5.0;
	if ( Theta[Axis] < 0.0 ) { Theta[Axis] += 360.0; }
    }

    glutPostRedisplay();
}

void special(int key, int x, int y){
    switch(key){
        case GLUT_KEY_RIGHT:
            Theta[Axis] += 2.5;
	        if ( Theta[Axis] > 360.0 ) Theta[Axis] -= 360.0;
            break;
        case GLUT_KEY_LEFT:
            Theta[Axis] -= 2.5;
	        if ( Theta[Axis] < 0.0 ) Theta[Axis] += 360.0; 
            break;
    }
    cout << Theta[Axis] << endl;
    glutPostRedisplay();
}

void reshape( int width, int height ){
    glViewport( 0, 0, width, height );

    GLfloat  left = -10.0, right = 10.0;
    GLfloat  bottom = -5.0, top = 15.0;
    GLfloat  zNear = -10.0, zFar = 10.0;

    GLfloat aspect = GLfloat(width)/height;

    if ( aspect > 1.0 ) {
	left *= aspect;
	right *= aspect;
    }
    else {
	bottom /= aspect;
	top /= aspect;
    }


    mat4 projection = Ortho( left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );

    model_view = mat4( 1.0 );  // An Identity matrix
}

void keyboard( unsigned char key, int x, int y ){
    switch( key ) {
	    case 033: // Escape Key
	    case 'q': case 'Q':
	        exit( EXIT_SUCCESS );
	        break;
        case '1': 
            Axis = 0;
            break;
        case '2':
            Axis = 1;
            break;
        case '3':
            Axis = 2;
            break;
        case 9:
            if(view == "-sv") view = "-tv";
            else view = "-sv";
            switchCam();
            break;
    }
    
}

int main( int argc, char **argv ){
    if(argc == 8){
        ball_flag = true;
        view = argv[7];

        for(int i = 0; i < 2; i++){
            ball_pos[i] = vec4(atof(argv[i*3 + 1]),
                               atof(argv[i*3 + 2]),
                               atof(argv[i*3 + 3]),1);
        }
    }
    else if(argc > 1){
        std::cout << "Need 7 arguments, " << argc << " arguments taken." << std::endl;
    }

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 720,720 );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "robot" );

    // Iff you get a segmentation error at line 34, please uncomment the line below
    glewExperimental = GL_TRUE; 
    glewInit();
    
    init();

    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutSpecialFunc(special);
    glutMouseFunc( mouse );
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); 
 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
 

    switchCam();
    getball();

    glutMainLoop();
    return 0;
}
