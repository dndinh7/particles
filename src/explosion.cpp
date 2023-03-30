// Bryn Mawr College, alinen, 2020
//

/**
 * This program renders a billboard explosion
 * using sprites and corresponding uv coordinate
 * mapping. The animation is rendered at 30fps and
 * will continually loop.
 * 
 * Author: David Dinh
*/

#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"

using namespace std;
using namespace glm;
using namespace agl;

class Viewer : public Window {
public:
  Viewer() : Window() {
  }

  void setup() {
    setWindowSize(1000, 1000);
    renderer.loadShader("billboard-animated", 
      "../shaders/billboard-animated.vs", 
      "../shaders/billboard-animated.fs");

    renderer.loadTexture("explosion", 
      "../textures/explosion2.png", 0);
    renderer.setDepthTest(false);
    renderer.blendMode(agl::ADD);

  }


  void mouseMotion(int x, int y, int dx, int dy) {
  }

  void mouseDown(int button, int mods) {
  }

  void mouseUp(int button, int mods) {
  }

  void scroll(float dx, float dy) {
    eyePos.z += dy;
  }

  void draw() {
    renderer.beginShader("billboard-animated");
    renderer.texture("image", "explosion");

    // 30 fps => each frame 1/30 long, e.g. when time = 1s, we play frame 30
    frame = frame + dt() * 30;
    int f= int(frame) % numFrames;
    int row= numRows - (f / numCols);
    int col= f % numCols;
    renderer.setUniform("Frame", f);

    renderer.setUniform("NumRows", numRows);
    renderer.setUniform("NumCols", numCols);
    renderer.setUniform("Row", row);
    renderer.setUniform("Col", col);

    float aspect = ((float)width()) / height();
    renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

    renderer.lookAt(eyePos, lookPos, up);
    renderer.sprite(vec3(0.0), vec4(1.0f), 1.0);

    renderer.endShader();
  }

protected:

  vec3 eyePos = vec3(0, 0, 2);
  vec3 lookPos = vec3(0, 0, 0);
  vec3 up = vec3(0, 1, 0);
  float frame = 0;
  int numRows = 8;
  int numCols = 8;
  int numFrames= 64;
};

int main(int argc, char** argv)
{
  Viewer viewer;
  viewer.run();
  return 0;
}
