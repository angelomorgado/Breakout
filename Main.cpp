// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define _USE_MATH_DEFINES

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// GLM header file
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
using namespace glm;

// shaders header file
#include "common/shader.hpp"
#include "common/shader.cpp"

// Vertex array object (VAO)
GLuint VertexArrayID;

// Vertex buffer object (VBO)
GLuint paddle_vertexbuffer;
GLuint ball_vertexbuffer;
GLuint wall_vertexbuffer;
GLuint brick_vertexbuffer;


// color buffer object (CBO)
GLuint ball_colorbuffer;
GLuint paddle_colorbuffer;
GLuint wall_colorbuffer;
GLuint brick_colorbuffer;

// GLSL program from the shaders
GLuint programID;


GLint WindowWidth = 900;
GLint WindowHeight = 900;

//Universal variables
int points=0;
int bricks_count = 0;
bool close_loop = true;
float back_RGB[3] = { 0.0f, 0.0f, 0.9f };

//Velocity
float paddleVelocity = 0.3f;
float ballVelocity = 0.3f;

//Paddle
float paddle_xPosition = 0.0f;
float paddle_yPosition = 0.0f;
float paddleSize = 14.0f;
float paddle_xStep = paddleVelocity;
float paddle_yStep = paddleVelocity;
float paddle_L, paddle_R;


//Ball
float ball_radius = 1.0f;
float ball_xPosition = 0.0f;
float ball_yPosition = -29.0f + ball_radius;
float ballXstep = ballVelocity;
float ballYstep = ballVelocity;
bool ballMove = false;
float angstep = paddleVelocity;

//Number of bricks
int const nBrick_lines = 3;
int const nBrick_columns = 8;
bool brick[nBrick_lines][nBrick_columns];

//Bricks properties
float brick_size=5.0f;
float brick_heigth = 2.0f;
float brick_xdistance = 0.0f;
float brick_ydistance = 0.0f;
float brick_Xaux, brick_Yaux;



//--------------------------------------------------------------------------------
void transferDataToGPUMemory(void)
{
    // VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

    //Paddle
    static const GLfloat paddle_vertex_buffer_data[] = {
      -7.0f, -30.0f, 0.0f , -7.0f, -32.0f, 0.0f , 7.0f, -32.0f, 0.0f,
       7.0f, -32.0f, 0.0f , 7.0f, -30.0f, 0.0f , -7.0f, -30.0f, 0.0f,
    };

    // One color for each vertex. They were generated randomly.
    static const GLfloat paddle_color_buffer_data[] = {
        0.0f,  0.0f,  1.0f,     0.2f,  0.8f,  0.8f,      0.2f,  0.8f,  0.8f,
        0.0f,  0.0f,  1.0f,      0.2f,  0.8f,  0.8f,      0.2f,  0.8f,  0.8f,
    };

    //Wall----------------------------------------------------------------------------
    static const GLfloat wall_vertex_buffer_data[] = {
      -40.0f, -40.0f, 0.0f , -30.0f, -40.0f, 0.0f , -30.0f, 40.0f, 0.0f,
      -30.0f, 40.0f, 0.0f,  -40.0f, 40.0f, 0.0f , -40.0f, -40.0f, 0.0f,
    };

    // One color for each vertex. They were generated randomly.
    static const GLfloat wall_color_buffer_data[] = {
        0.5f,  0.5f,  0.5f,      0.5f,  0.5f,  0.5f,      0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,      0.5f,  0.5f,  0.5f,      0.5f,  0.5f,  0.5f,
    };

    //Brick----------------------------------------------------------------------------
    static const GLfloat brick_vertex_buffer_data[] = {
      -25.0f, 30.0f, 0.0f , -25.0f, 32.0f, 0.0f , -20.0f, 32.0f, 0.0f,
      -20.0f, 32.0f, 0.0f, -20.0f, 30.0f, 0.0f, -25.0f, 30.0f, 0.0f,
    };

    // One color for each vertex. They were generated randomly.
    static const GLfloat brick_color_buffer_data[] = {
        1.0f,  0.5f,  0.0f,     1.0f,  0.5f,  0.0f,      1.0f,  0.5f,  0.0f,
        1.0f,  0.5f,  0.0f,     1.0f,  0.5f,  0.0f,      1.0f,  0.5f,  0.0f,
    };

    //circle----------------------------------------------------------------------------
    
    static GLfloat ball_vertex_buffer_data[3 * 360];
    for (int teta = 0; teta < 360; teta++) {
        if (teta % 2 == 0) {
            ball_vertex_buffer_data[teta*3] = ball_radius*cos(teta*(M_PI / 180));
            ball_vertex_buffer_data[(teta*3)+1] = ball_radius *sin(teta*(M_PI / 180));
            ball_vertex_buffer_data[(teta*3)+2] = 0.0f;
        }
        else {
            ball_vertex_buffer_data[teta*3] = - ball_radius*cos(teta*(M_PI / 180));
            ball_vertex_buffer_data[(teta*3)+1] =  ball_radius *sin(teta*(M_PI / 180));
            ball_vertex_buffer_data[(teta*3)+2] = 0.0f;
        }
    }

    static GLfloat ball_color_buffer_data[3 * 360];
    for (int teta = 0; teta < 360; teta++) {
        
            ball_color_buffer_data[teta * 3] = 1.0f;
            ball_color_buffer_data[(teta * 3) + 1] = 1.0f;
            ball_color_buffer_data[(teta * 3) + 2] = 1.0f;
        
     
    }
    


    // Move vertex data to video memory; specifically to VBO called vertexbuffer
    glGenBuffers(1, &paddle_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, paddle_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(paddle_vertex_buffer_data), paddle_vertex_buffer_data, GL_STATIC_DRAW);

    // Move color data to video memory; specifically to CBO called colorbuffer
    glGenBuffers(1, &paddle_colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, paddle_colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(paddle_color_buffer_data), paddle_color_buffer_data, GL_STATIC_DRAW);

    // Move vertex data to video memory; specifically to VBO called vertexbuffer
    glGenBuffers(1, &ball_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, ball_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ball_vertex_buffer_data), ball_vertex_buffer_data, GL_STATIC_DRAW);

    // Move color data to video memory; specifically to CBO called colorbuffer
    glGenBuffers(1, &ball_colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, ball_colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ball_color_buffer_data), ball_color_buffer_data, GL_STATIC_DRAW);

    // Move vertex data to video memory; specifically to VBO called vertexbuffer
    glGenBuffers(1, &wall_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, wall_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall_vertex_buffer_data), wall_vertex_buffer_data, GL_STATIC_DRAW);

    // Move color data to video memory; specifically to CBO called colorbuffer
    glGenBuffers(1, &wall_colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, wall_colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall_color_buffer_data), wall_color_buffer_data, GL_STATIC_DRAW);

    // Move vertex data to video memory; specifically to VBO called vertexbuffer
    glGenBuffers(1, &brick_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, brick_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(brick_vertex_buffer_data), brick_vertex_buffer_data, GL_STATIC_DRAW);

    // Move color data to video memory; specifically to CBO called colorbuffer
    glGenBuffers(1, &brick_colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, brick_colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(brick_color_buffer_data), brick_color_buffer_data, GL_STATIC_DRAW);


}

//--------------------------------------------------------------------------------
void cleanupDataFromGPU()
{
    glDeleteBuffers(1, &paddle_vertexbuffer);
    glDeleteBuffers(1, &paddle_colorbuffer);
    glDeleteBuffers(1, &ball_vertexbuffer);
    glDeleteBuffers(1, &ball_colorbuffer);
    glDeleteBuffers(1, &wall_vertexbuffer);
    glDeleteBuffers(1, &wall_colorbuffer);
    glDeleteBuffers(1, &brick_vertexbuffer);
    glDeleteBuffers(1, &brick_colorbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);
}

//Function to atributes
void mainAtributes(GLuint buffer,GLuint colorBuffer, float x, float y) {
    
    // Clear the screen
    // create transformations
    //glm::mat4 model = glm::mat4(1.0f);
    //glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 mvp = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f);

    // Our ModelViewProjection : multiplication of our 3 matrices
    //glm::mat4 mvp = projection * view * model;
    // Remember, matrix multiplication is the other way around

    // retrieve the matrix uniform locations
    unsigned int matrix = glGetUniformLocation(programID, "mvp");
    glUniformMatrix4fv(matrix, 1, GL_FALSE, &mvp[0][0]);


    glm::mat4 trans;
    trans = glm::translate(glm::mat4(1.0), glm::vec3(x, y, 0.0f));
    unsigned int m = glGetUniformLocation(programID, "trans");
    glUniformMatrix4fv(m, 1, GL_FALSE, &trans[0][0]);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );



}

//--------------------------------------------------------------------------------
void draw_paddle(float paddle_xPosition)
{
    

    mainAtributes(paddle_vertexbuffer, paddle_colorbuffer, paddle_xPosition, paddle_yPosition);


    //glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointSize(10);
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0,6 ); // 3 indices starting at 0 -> 1 triangle
    //glDrawArrays(GL_POINTS, 0, 9); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
//--------------------------------------------------------------------------------

void draw_wall(float wall_xTranslate)
{


    mainAtributes(wall_vertexbuffer, wall_colorbuffer, wall_xTranslate, 0.0);


    //glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointSize(10);
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
    //glDrawArrays(GL_POINTS, 0, 9); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
//--------------------------------------------------------------------------------

void draw_ball()
{

    mainAtributes(ball_vertexbuffer, ball_colorbuffer, ball_xPosition, ball_yPosition);


    //glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(3);
    // Draw the triangle !
    glDrawArrays(GL_LINES, 0, 360); // 3 indices starting at 0 -> 1 triangle
    //glDrawArrays(GL_POINTS, 0, 9); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void draw_brick(float brick_xTranslate, float brick_yTranslate)
{

    mainAtributes(brick_vertexbuffer, brick_colorbuffer, brick_xTranslate,  brick_yTranslate);


    //glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(4);
    glDrawArrays(GL_TRIANGLES, 0, 9); // 3 indices starting at 0 -> 1 triangle
    //glDrawArrays(GL_POINTS, 0, 9); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
//Clear the console
void clear() {
    // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    std::cout << "\x1B[2J\x1B[H";
}
bool colision_brick(float X_start, float Y_start, int X, int Y) {
    //Colisão com parede de baixo
    for (int tet = 0; tet <= 360; tet++) {
        if ((ball_xPosition + ball_radius * cos(tet * M_PI / 180) >= X_start) &&
            (ball_xPosition + ball_radius * cos(tet * M_PI / 180) <= X_start + brick_size) &&
            ball_yPosition + ball_radius * sin(tet * M_PI / 180) >= Y_start &&
            ball_yPosition + ball_radius * sin(tet + 90 * M_PI / 180) <= Y_start + brick_heigth) {

            //Paredes de cima e baixo
            if ((ball_xPosition + ball_radius * cos(tet * M_PI / 180) > X_start) &&
                (ball_xPosition + ball_radius * cos(tet * M_PI / 180) < X_start + brick_size)) 
                {
                if (ballXstep > 0) {
                    ballXstep += 0.02f;
                }
                else {
                    ballXstep -= 0.02f;
                }
                
                ballYstep = -ballYstep;
                ballYstep += 0.02f;
                bricks_count++;
                points++;
                brick[X][Y] = false;
                //clear();
                system("cls");
                printf("points: %d \n", points);
                return true;
            }
            else
            //Paredes de lados
            {
                if (ballXstep > 0) {
                    ballXstep += 0.02f;
                }
                else {
                    ballXstep -= 0.02f;
                }

                ballYstep = -ballYstep;
                ballYstep += 0.02f;
                bricks_count++;
                brick[X][Y] = false;
                printf("points: %d \n",points);
                return true;
            }
        }

    }
    return false;
}


int main(void)
{
    
    // Initialise GLFW
    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window
    window = glfwCreateWindow(WindowWidth, WindowHeight, "Breakout!", NULL, NULL);

    // Create window context
    glfwMakeContextCurrent(window);

    

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    glewInit();

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    //Background
    

    // transfer my data (vertices, colors, and shaders) to GPU side
    transferDataToGPUMemory();

    // Use our shader
    glUseProgram(programID);


    //Declares all bricks as true
    for (int i = 0; i < nBrick_lines; i++) {
        for (int f = 0; f < nBrick_columns; f++) {
            brick[i][f] = true;
        }
    }
    
    // render scene for each frame
    do {
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
            ballMove = !ballMove;
        }

        
            glClearColor(back_RGB[0], back_RGB[1], back_RGB[2], 0.0f);
            // drawing callback
            glClear(GL_COLOR_BUFFER_BIT);
            draw_paddle(paddle_xPosition);
            draw_ball();
            draw_wall(0.0f);
            draw_wall(70.0f);



            //Bricks drawing and colision
            brick_Xaux = 0.0f;
            brick_Yaux = 0.0f;
            brick_xdistance = 0.0f;
            brick_ydistance = 0.0f;

            for (int i = 0; i < nBrick_lines; i++) {

                for (int f = 0; f < nBrick_columns; f++) {
                    brick_Xaux = brick_xdistance;
                    brick_Yaux = brick_ydistance;

                    if (brick[i][f] && !colision_brick(-25.0f - 2.5f + brick_Xaux, 25.0f + brick_Yaux, i, f)) {
                        draw_brick(brick_xdistance - 2.5f, brick_ydistance - 5.0f);
                    }
                    brick_xdistance += 7.0f;
                }
                brick_ydistance += 5.0f;
                brick_xdistance = 0.0f;
            }


            // Swap buffers
            glfwSwapBuffers(window);

            // looking for events
            glfwPollEvents();

            //tecla para começar
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
                ballMove = true;
                clear();
            }

            //Bola a mexer
            if (ballMove) {

                //colisões
                if (ball_xPosition + ball_radius * cos(0 * M_PI / 180) > 30.0f || ball_xPosition + ball_radius * cos(180 * M_PI / 180) < -30.0f) {
                    ballXstep = -ballXstep;
                }

                if (ball_yPosition + ball_radius * cos(90 * M_PI / 180) > 40.0f - ball_radius) {
                    ballYstep = -ballYstep;
                }

                //Colisão com paddle
                paddle_L = paddle_xPosition - 7.0f - ball_radius;
                paddle_R = paddle_xPosition + 7.0f + ball_radius;

                for (int tet = 180; tet <= 360; tet++) {
                    if ((ball_xPosition + ball_radius * cos(tet * M_PI / 180) >= paddle_L) &&
                        (ball_xPosition + ball_radius * cos(tet * M_PI / 180) <= paddle_R) && (ball_yPosition <= -30.0f + ball_radius) &&
                        (ball_yPosition >= -35.0f + ball_radius)) {

                        ballYstep = 0.3;

                        if ((ball_xPosition + ball_radius * cos(tet * M_PI / 180) < (paddle_L + paddleSize * 0.1)) || 
                            (ball_xPosition + ball_radius * cos(tet * M_PI / 180) > (paddle_R - paddleSize * 0.1))) {
                            ballXstep = -ballXstep;
                        }

                    }
                }


                //Movimento do paddle
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    if (paddle_xPosition + 7.0f < 30.0f) {
                        paddle_xPosition += paddleVelocity;
                    }
                }

                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    if (paddle_xPosition - 7.0f > -30.0f) {
                        paddle_xPosition -= paddleVelocity;
                    }
                }



                //Condição de vitória

                if (bricks_count == 24) {

                    ball_yPosition = -29.0f + ball_radius;
                    ball_xPosition = 0.0f;
                    ballXstep = -ballXstep;
                    ballYstep = -ballYstep;
                    ballMove = !ballMove;
                    ballVelocity = 0.3f;
                    paddle_xPosition = 0.0f;
                    printf("Next level!\n");

                    paddleVelocity += 0.2f;
                    back_RGB[0] += 0.1f;
                    back_RGB[1] += 0.4f;
                    back_RGB[2] += 0.1f;

                    for (int i = 0; i < nBrick_lines; i++) {
                        for (int f = 0; f < nBrick_columns; f++) {
                            brick[i][f] = true;
                        }
                    }
                    bricks_count = 0;
                }


                //Condição de derrota
                if (ball_yPosition + ball_radius * cos(270 * M_PI / 180) < -40.0f || (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
                    ball_yPosition = -29.0f + ball_radius;
                    ball_xPosition = 0.0f;
                    ballXstep = -ballXstep;
                    ballYstep = -ballYstep;
                    ballXstep = ballVelocity;
                    ballYstep = ballVelocity;
                    ballMove = !ballMove;
                    points = 0;
                    paddleVelocity = 0.3f;

                    back_RGB[0] = 0.0f;
                    back_RGB[1] = 0.0f;
                    back_RGB[2] = 0.9f;

                    paddle_xPosition = 0.0f;
                    bricks_count = 0;
                    ballVelocity = 0.3f;

                    printf("You lost!\n");

                    for (int i = 0; i < nBrick_lines; i++) {
                        for (int f = 0; f < nBrick_columns; f++) {
                            brick[i][f] = true;
                        }
                    }
                }

                ball_xPosition += ballXstep;
                ball_yPosition += ballYstep;
            }
     

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);


    // Cleanup VAO, VBOs, and shaders from GPU
    cleanupDataFromGPU();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

