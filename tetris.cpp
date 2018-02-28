// OpenGL tetris game
// Author : Oliver Wei
// email : weixinw@sfu.ca  
// OpenGL version: 3.2
//----------------------------------------------------------------------------
#include "include/Angel.h"
#include <iostream>
//global variables
GLuint vao[3];
//vbo[0] line vertex
//vbo[1] line color
//vbo[2] blocks vertex
//vbo[3] blocks color
//vbo[4] current tile vertex
//vbo[5] current tile color
//block width 0.17
//block height 0.097
//left wall: -0.85
//right wall: 0.85
//bottom wall: -0.97
GLuint vbo[6];
//initial drop position
vec2 tile_pos = vec2(0,0.97);
vec4 blocks_color[1200];
int shape,rotation;
vec4 current_color;
bool block_valid[20][10];

//tile define
vec2 tiles[7][4][4] = {
    //O
    {
        {vec2(-1,0),vec2(-1,-1),vec2(0,-1),vec2(0,0)},
        {vec2(-1,0),vec2(-1,-1),vec2(0,-1),vec2(0,0)},
        {vec2(-1,0),vec2(-1,-1),vec2(0,-1),vec2(0,0)},
        {vec2(-1,0),vec2(-1,-1),vec2(0,-1),vec2(0,0)}
    },
    //I
    {
        {vec2(-2,0),vec2(-1,0),vec2(0,0),vec2(1,0)},
        {vec2(0,1),vec2(0,0),vec2(0,-1),vec2(0,-2)},
        {vec2(-2,0),vec2(-1,0),vec2(0,0),vec2(1,0)},
        {vec2(0,1),vec2(0,0),vec2(0,-1),vec2(0,-2)}
    },
    //S
    {
        {vec2(-1,-1),vec2(0,-1),vec2(0,0),vec2(1,0)},
        {vec2(0,1),vec2(0,0),vec2(1,0),vec2(1,-1)},
        {vec2(-1,-1),vec2(0,-1),vec2(0,0),vec2(1,0)},
        {vec2(0,1),vec2(0,0),vec2(1,0),vec2(1,-1)}
    },
    //Z
    {
        {vec2(-1,0),vec2(0,0),vec2(0,-1),vec2(1,-1)},
        {vec2(0,0),vec2(0,-1),vec2(1,0),vec2(1,1)},
        {vec2(-1,0),vec2(0,0),vec2(0,-1),vec2(1,-1)},
        {vec2(0,0),vec2(0,-1),vec2(1,0),vec2(1,1)}
    },
    //L
    {
        {vec2(-1,-1),vec2(-1,0),vec2(0,0),vec2(1,0)},
        {vec2(0,1),vec2(0,0),vec2(0,-1),vec2(1,-1)},
        {vec2(-1,0),vec2(0,0),vec2(1,0),vec2(1,1)},
        {vec2(-1,1),vec2(0,1),vec2(0,0),vec2(0,-1)}
    },
    //J
    {
        {vec2(-1,0),vec2(0,0),vec2(1,0),vec2(1,-1)},
        {vec2(0,-1),vec2(0,0),vec2(0,1),vec2(1,1)},
        {vec2(-1,1),vec2(-1,0),vec2(0,0),vec2(1,0)},
        {vec2(-1,-1),vec2(0,-1),vec2(0,0),vec2(0,1)}
    },
    //T
    {
        {vec2(-1,0),vec2(0,0),vec2(0,-1),vec2(1,0)},
        {vec2(0,1),vec2(0,0),vec2(0,-1),vec2(1,0)},
        {vec2(-1,0),vec2(0,0),vec2(0,1),vec2(1,0)},
        {vec2(-1,0),vec2(0,0),vec2(0,1),vec2(0,-1)}
    }
};
//tile define end

vec4* tile_location_map(vec2* pos, int size){
    vec4* points = new vec4[size * 6];
    for(int i = 0; i < size; i++){
        points[6 * i + 0] = vec4(tile_pos.x + (pos[i].x * 0.17),tile_pos.y + (pos[i].y * 0.097),0,1);
        points[6 * i + 1] = vec4(tile_pos.x + (pos[i].x * 0.17),tile_pos.y + ((pos[i].y - 1) * 0.097),0,1);
        points[6 * i + 2] = vec4(tile_pos.x + ((pos[i].x + 1) * 0.17),tile_pos.y + ((pos[i].y - 1) * 0.097),0,1);
        points[6 * i + 3] = vec4(tile_pos.x + ((pos[i].x + 1) * 0.17),tile_pos.y + ((pos[i].y - 1) * 0.097),0,1);
        points[6 * i + 4] = vec4(tile_pos.x + ((pos[i].x + 1) * 0.17),tile_pos.y + (pos[i].y * 0.097),0,1);
        points[6 * i + 5] = vec4(tile_pos.x + (pos[i].x * 0.17),tile_pos.y + (pos[i].y * 0.097),0,1);
    }

    // for(int i = 0; i < 24; i++){
    //     if(i % 6 == 0){
    //         std::cout<<std::endl;
    //     }
    //     std::cout << points[i].x << "  " <<points[i].y << std::endl;
    // }

    return points;
}

vec2* block_coordinate_map(){
    vec2* bc = new vec2[4];
    vec4* tile = tile_location_map(tiles[shape][rotation],4);
    for(int i = 0; i < 4;i++){
        bc[i] = vec2((tile[i*6].x / 0.17) + 5, 10 - (tile[i*6].y / 0.097));
    }
    return bc;
}

void update_tile(int shape,int rotation){

    vec4* tile = tile_location_map(tiles[shape][rotation],4);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec4), *tile, GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
    glutPostRedisplay(); 
}

void drop_tile( void ){
    tile_pos = vec2(0,0.97);

    srand(time(NULL));  
    shape = rand() % 7;
    rotation = rand() % 4;

    update_tile(shape,rotation);

    GLfloat color_rgb[3];
    for(int i = 0; i < 3; i++){
        color_rgb[i] = rand() % 100 + 25;
    }
    current_color = vec4(color_rgb[0] / 100.0,color_rgb[1] / 100.0,color_rgb[2] / 100.0,1);
    vec4 tile_color[24];
    for(int i = 0; i < 24; i++){
        tile_color[i] = current_color;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);  
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec4), tile_color, GL_STATIC_DRAW);  
    glBindBuffer(GL_ARRAY_BUFFER, 0);  
  
    glBindVertexArray(0);
    glFlush();
    
}

GLfloat getMax(int mode){
    vec4* tile = tile_location_map(tiles[shape][rotation],4);
    GLfloat number = -1;
    if(mode == -1){
        number = tile[0].x;
        for(int i = 1; i < 24; i++){
            if(number > tile[i].x){
                number = tile[i].x;
            }
        }
    }
    else if(mode == 1){
        number = tile[0].x;
        for(int i = 1; i < 24; i++){
            if(number < tile[i].x){
                number = tile[i].x;
            }
        }
    }
    else if(mode == 0){
        number = tile[0].y;
        for(int i = 1; i < 24; i++){
            if(number > tile[i].y){
                number = tile[i].y;
            }
        }
    }
    return number;
}

GLfloat* getAllMax(){
    //0 left;1 up;2 right;3 down
    GLfloat* points = new GLfloat[4];
    vec4* tile = tile_location_map(tiles[shape][rotation],4);
    for(int i = 0; i < 2; i++){
        points[i*2 + 0] = tile[0].x;
        points[i*2 + 1] = tile[0].y;
    }
    for(int i = 1; i < 24; i++){
        if(points[0] > tile[i].x){
            points[0] = tile[i].x;
        }
        if(points[2] < tile[i].x){
            points[2] = tile[i].x;
        }
        if(points[1] < tile[i].y){
            points[1] = tile[i].y;
        }
        if(points[3] > tile[i].y){
            points[3] = tile[i].y;
        }
    }
    return points;
}

bool validator(vec2 direction){
    
    GLfloat mode = direction.x;

    //left
    if(mode == -1){
        if(getMax(mode) >= -1){
            return true;
        }
        else{
            return false;
        }
    }
    //right
    else if(mode == 1){
        if(getMax(mode) >= 1){
            return false;
        }
        else{
            return true;
        }
    }
    //down
    else if(mode == 0){
        if(getMax(mode) <= -1){
            return false;
        }
        else{
            return true;
        }
    }
    else{
        //bug
        GLfloat* allPoints = getAllMax();
        if(allPoints[0] <= -1) return false;
        if(allPoints[2] > 0.85) return false;
        if(allPoints[1] >= 1) return false;
        if(allPoints[3] <= -1) return false;
    }
    return true;
}

void move(vec2 direction){
    vec2 old = tile_pos;
    tile_pos.x = tile_pos.x + (direction.x * 0.17);
    tile_pos.y = tile_pos.y + (direction.y * 0.097);
    if(validator(direction)){
        update_tile(shape,rotation);
        //std::cout << tile_pos.x << " " << tile_pos.y <<std::endl;
        
    }
    else{
        tile_pos = old;
    }
}

void ccw_rotate(){
    int old = rotation;
    rotation = (rotation + 1) % 4;
    if(validator(vec2(2,2))){
        update_tile(shape,rotation);
    }
    else{
        rotation = old;
    }
}

void init( void ){
    //lines
    vec4 line_points[64];
    vec4 line_color[64];

    // vertical lines  
    for (int i = 0; i < 11; i++){  
        line_points[2 * i] = vec4(-0.85 + 0.17*i,0.97,0,1);  
        line_points[2 * i + 1] = vec4(-0.85 + 0.17*i,-0.97,0,1);
  
    }  
  
    // horizontal lines
    for (int i = 0; i < 21; i++){  
        line_points[22 + 2 * i] = vec4(-0.85,0.97 - 0.097*i,0,1);  
        line_points[22 + 2 * i + 1] = vec4(0.85,0.97 - 0.097*i,0,1);  
    }

    //line color
    for(int i = 0;i < 64; i++){
        line_color[i] = vec4(1.0, 1.0, 1.0, 1.0);
    }
    //lines end

    //blocks
    for(int i = 0; i < 1200; i++){
        blocks_color[i] = vec4(0.0, 0.0, 0.0, 1.0);
    }
    vec4 blocks_points[1200];
    for(int r = 0; r < 20; r++){
        for(int c = 0; c < 10; c++){
            //ccw
            blocks_points[6 * (10 * r + c) + 0] = vec4(-0.85 + 0.17 * c,0.97 - 0.097*r,0,1);
            blocks_points[6 * (10 * r + c) + 1] = vec4(-0.85 + 0.17 * c,0.97 - 0.097*(r+1),0,1);
            blocks_points[6 * (10 * r + c) + 2] = vec4(-0.85 + 0.17 * (c + 1),0.97 - 0.097*(r+1),0,1);
            blocks_points[6 * (10 * r + c) + 3] = vec4(-0.85 + 0.17 * (c + 1),0.97 - 0.097*(r+1),0,1);
            blocks_points[6 * (10 * r + c) + 4] = vec4(-0.85 + 0.17 * (c + 1),0.97 - 0.097* r,0,1);
            blocks_points[6 * (10 * r + c) + 5] = vec4(-0.85 + 0.17 * c,0.97 - 0.097*r,0,1);
        }
    }

    for(int i = 0; i < 20;i++){
        for(int j = 0; j < 10;j++){
            block_valid[i][j] = true;
        }
    }

    //blocks end

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);
    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    GLuint vColor = glGetAttribLocation(program, "vColor"); 
    
    glClearColor( 0, 0, 0, 1.0 ); // black background

    // Create a vertex array object for Line
    glGenVertexArrays(1, &vao[0]);
    glBindVertexArray(vao[0]);

    // Line buffer Object
    glGenBuffers(2, &vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_points), line_points, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_color), line_color, GL_STATIC_DRAW);  
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));  
    glEnableVertexAttribArray(vColor);  
    
    // Create a vertex array object for Blocks
    glGenVertexArrays(1, &vao[1]);
    glBindVertexArray(vao[1]);  
  
    // Blocks buffer Object
    glGenBuffers(2, &vbo[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(blocks_points), blocks_points, GL_STATIC_DRAW);  
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);  
    glEnableVertexAttribArray(vPosition);
  
    // Blocks color buffer Object 
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(blocks_color), blocks_color, GL_DYNAMIC_DRAW);  
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);  
    glEnableVertexAttribArray(vColor);

    //Create a vertex array object for Tiles
    glGenVertexArrays(1, &vao[2]);
    glBindVertexArray(vao[2]);

    //Tiles buffer Object
    glGenBuffers(2, &vbo[4]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);  
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);  
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);  
    glEnableVertexAttribArray(vPosition);

    //Tiles color buffer Object 
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);  
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);  
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);  
    glEnableVertexAttribArray(vColor);

    drop_tile();

}

//----------------------------------------------------------------------------

void display( void ){
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window

    //draw blocks
    glBindVertexArray(vao[1]);
    glDrawArrays( GL_TRIANGLES, 0, 1200);

    //draw Tiles
    glBindVertexArray(vao[2]);
    glDrawArrays( GL_TRIANGLES, 0, 24);

    // draw the lines
    glBindVertexArray(vao[0]);
    glDrawArrays( GL_LINES, 0, 64);

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int x, int y ){
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

void special(int key, int x, int y){
    switch(key){
        case GLUT_KEY_UP:
            //std::cout << "up key pushed" << std::endl;
            ccw_rotate();
            break;
        case GLUT_KEY_LEFT:
            //std::cout << "left key pushed" << std::endl;
            move(vec2(-1,0));
            break;
        case GLUT_KEY_RIGHT:
            //std::cout << "right key pushed" << std::endl;
            move(vec2(1,0));
            break;
        case GLUT_KEY_DOWN:
            //std::cout << "down key pushed" << std::endl;
            move(vec2(0,-1));
            break;
        
    }
}

// void idle(){
//     static int time = 0;
//     if(time > 2500000){
//         move(vec2(0,-1));
//         time = 0;
//     }
//     time++;
// }

void gravity(int sec){
    move(vec2(0,-1));
    glutTimerFunc(sec * 1000,gravity,sec);
}
//----------------------------------------------------------------------------

int main( int argc, char **argv ){
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA );
    glutInitWindowSize( 400, 720 );

    // If you are using freeglut, the next two lines will check if 
    // the code is truly 3.2. Otherwise, comment them out
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow("tetris");

    // Iff you get a segmentation error at line 34, please uncomment the line below
    glewExperimental = GL_TRUE; 
    glewInit();

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    //set gravity,sec/down
    gravity(1);
    //glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
