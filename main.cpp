#define GLEW_STATIC // Easier debugging
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#define RUN_GRAPHICS_DISPLAY 0x00;

#include "GameManager.h"
#include "Camera.h"

/*
 * SDL timers run in separate threads.  In the timer thread
 * push an event onto the event queue.  This event signifies
 * to call display() from the thread in which the OpenGL
 * context was created.
 */
Uint32 tick(Uint32 interval, void *param) {
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.code = RUN_GRAPHICS_DISPLAY;
  event.user.data1 = 0;
  event.user.data2 = 0;
  SDL_PushEvent(&event);
  return interval;
}

/**
 * Adds a custom deleter to destroy an SDL window properly.
 */
struct SDLWindowDeleter {
  inline void operator()(SDL_Window* window) {
    SDL_DestroyWindow(window);
  }
};

void Draw(const boost::shared_ptr<SDL_Window> window, boost::shared_ptr<GameManager> game_manager, bool mouseIn) {
  glClearColor(0.0f, 0.3f, 0.7f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  game_manager->Draw();

  // Don't forget to swap the buffers
  SDL_GL_SwapWindow(window.get());
}

boost::shared_ptr<SDL_Window> InitWorld() {
  Uint32 width = 640;
  Uint32 height = 480;
  SDL_Window * _window;
  boost::shared_ptr<SDL_Window> window;

  // Glew will later ensure that OpenGL 3 *is* supported
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  // Do double buffering in GL
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // Initialise SDL - when using C/C++ it's common to have to
  // initialise libraries by calling a function within them.
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)<0) {
    std::cout << "Failed to initialise SDL: " << SDL_GetError() << std::endl;
    return nullptr;
  }

  // When we close a window quit the SDL application
  atexit(SDL_Quit);

  // Create a new window with an OpenGL surface
  _window = SDL_CreateWindow("Assignment"
                             , SDL_WINDOWPOS_CENTERED
                             , SDL_WINDOWPOS_CENTERED
                             , width
                             , height
                             , SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if (!_window) {
    std::cout << "Failed to create SDL window: " << SDL_GetError() << std::endl;
    return nullptr;
  }

  SDL_GLContext glContext = SDL_GL_CreateContext(_window);	// Create OpenGL context
  if (!glContext) {
    std::cout << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
    return nullptr;
  }

  glewInit();
  if(!glewIsSupported("GL_VERSION_3_0"))
  {
	  std::cerr << "OpenGL 3.0 not available" << std::endl;
	  return nullptr;
  }

  // OpenGL settings
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  window.reset(_window, SDL_DestroyWindow);
  return window;
}


int main(int argc, char *argv[]) {
  Uint32 delay = 1000/60; // in milliseconds

  auto window = InitWorld();
  auto game_manager = boost::make_shared<GameManager>();
  auto camera_ptr =   boost::make_shared<Camera>();

  bool mouseIn = GL_FALSE;	// mouse cursor within window boundaries?

  if(!window) {
    SDL_Quit();
  }

  // Call the function "tick" every delay milliseconds
  SDL_AddTimer(delay, tick, NULL);
  // Add the main event loop
  SDL_Event event;
  while (SDL_WaitEvent(&event)) {
    switch (event.type) {
    // Get key presses
    case SDL_KEYDOWN:
    	switch(event.key.keysym.sym){
    	case SDLK_w:
    		// move camera forward when pressed
    		camera_ptr->forward();
    		break;
    	case SDLK_a:
			// move camera forward when pressed
			camera_ptr->left();
			break;
    	case SDLK_s:
			// move camera forward when pressed
			camera_ptr->backward();
			break;
    	case SDLK_d:
			// move camera forward when pressed
			camera_ptr->right();
			break;
    	case SDLK_q:
    		camera_ptr->rotLeft();
    		break;
    	case SDLK_e:
    		camera_ptr->rotRight();
    		break;
    	case SDLK_x:
			// move camera forward when pressed
			camera_ptr->up();
			break;
    	case SDLK_c:
			// move camera forward when pressed
			camera_ptr->down();
			break;
    	case SDLK_ESCAPE:
    		// Exit game
    		exit(0);
    		break;
    	default:
    		break;
    	}
    break;
    case SDL_QUIT:
      SDL_Quit();
      break;
    case SDL_USEREVENT:
      Draw(window, game_manager, mouseIn);

      break;
    default:
      break;
    }
  }
  return 0;
}
