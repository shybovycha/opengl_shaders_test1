#include <GL/glew.h>
#include <GL/glu.h>

#include <stdio.h>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface 
#include <assimp/scene.h>           // Output data structure 
#include <assimp/postprocess.h>     // Post processing flags

#ifdef _DEBUG
	#pragma comment(lib, "sfml-main-d.lib")
	#pragma comment(lib, "sfml-graphics-d.lib")
	#pragma comment(lib, "sfml-system-d.lib")
	#pragma comment(lib, "sfml-window-d.lib")
	#pragma comment(lib, "glew32.lib")
	#pragma comment(lib, "glu32.lib")
	#pragma comment(lib, "opengl32.lib")
	#pragma comment(lib, "assimp.lib")
#else
	#pragma comment(lib, "sfml-main.lib")
	#pragma comment(lib, "sfml-graphics.lib")
	#pragma comment(lib, "sfml-system.lib")
	#pragma comment(lib, "sfml-window.lib")
	#pragma comment(lib, "glew32.lib")
	#pragma comment(lib, "glu32.lib")
	#pragma comment(lib, "opengl32.lib")
	#pragma comment(lib, "assimp.lib")
#endif

aiScene* loadMesh(const std::string& filename)
{
	Assimp::Importer importer;

	aiScene* scene = (aiScene*) importer.ReadFile(filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (!scene)
	{
		fprintf(stderr, "Error during mesh loading: %s\n", importer.GetErrorString());
		return 0;
	}

	return scene;
}

int main()
{
	// create the window (remember: it's safer to create it in the main thread due to OS limitations)
    sf::Window window(sf::VideoMode(800, 600), "OpenGL");

	GLenum res = glewInit();
    
	if (res != GLEW_OK) 
	{
        fprintf(stderr, "GLEW Init error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    // activate the window's context
    window.setActive(true);

	// set some objects to draw.
	static GLfloat vertices[] = {0,0,0,  1,0,0,  1,0,1,  0,0,1,    0,1,0,  1,1,0,  1,1,1,  0,1,1};
	static GLushort indices[] = {0,1,2,3,  4,5,6,7,  0,3,7,4,  1,2,6,5,  0,1,5,4,  3,2,6,7};

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(GLushort), indices , GL_STATIC_DRAW);

	// Enable depth test
    glEnable(GL_DEPTH_TEST);
    
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.f, 1.f, 1.f, 500.f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// setup some primitive camera!
	sf::Vector3f eye(0, 0, 0), lookat(0, 0, 0), up(0, 1, 0);

    // the rendering loop
    while (window.isOpen())
    {
		sf::Event event;

		// while there are pending events...
		while (window.pollEvent(event))
		{
			// check the type of the event...
			switch (event.type)
			{
				// window closed
				case sf::Event::Closed:
					window.close();
					break;

				// key pressed
				case sf::Event::KeyPressed:
					// ...
					break;

				// we don't process other types of events
				default:
					break;
			}
		}

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

		glPushMatrix();
			glTranslatef(0, 0, -5.0);

			// 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

            glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

            // Draw the triangles !
            glDrawElements(
                    GL_TRIANGLES,      // mode
                    sizeof(indices),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
            );

            glDisableVertexAttribArray(0);
		glPopMatrix();

		gluLookAt(eye.x, eye.y, eye.z, lookat.x, lookat.y, lookat.z, up.x, up.y, up.z); 

        window.display();
    }

    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);

    return 0;
}