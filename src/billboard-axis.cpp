// Bryn Mawr College, alinen, 2020
//

/**
 * This program renders a tree billboard
 * where it only has one axis of rotation
 * (y-axis). This calculates the angle of
 * rotation using the tangent of the
 * x and z componenets of the vector that 
 * points to the camera, which is then applied
 * to the tree model.
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
    renderer.loadShader("simple-texture",
      "../shaders/simple-texture.vs",
      "../shaders/simple-texture.fs");

    Image img;
    img.load("../textures/tree.png", true);
    renderer.loadTexture("tree", img, 0);
    ratio= float(img.width()) / img.height();

    renderer.loadTexture("grass", "../textures/grass.png", 0);
    renderer.blendMode(agl::BLEND);
  }


  void mouseMotion(int x, int y, int dx, int dy) {
    if (keyIsDown(GLFW_KEY_LEFT_SHIFT) && mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
        // more intuitive left shift mouse click only depending on horizontal movement
        // restrict it to 10 and the wall radius
        radius= std::max(2.0f, radius - dx);
        cout << "radius: " << radius << endl;
    } else if (mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
        azimuth+= (float)dx * 0.01f;
        elevation+= (float)dy * 0.01f;

        // clamp between (-pi/2, pi/2), don't want the bounds since it might lead to weird rotation
        elevation= std::max(elevation, (float) (-M_PI/2) + 0.00001f);
        elevation= std::min(elevation, (float) (M_PI/2)  - 0.00001f);
    }
  }

  void mouseDown(int button, int mods) {
  }

  void mouseUp(int button, int mods) {
  }

  void scroll(float dx, float dy) {
    radius= std::max(2.0f, radius-dy);
    cout << "radius: " << radius << endl;
  }

  void draw() {
    renderer.beginShader("simple-texture");

    float aspect = ((float)width()) / height();
    renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

    eyePos.x= radius * sin(azimuth) * cos(elevation);
    eyePos.y= radius * sin(elevation);
    eyePos.z= radius * cos(azimuth) * cos(elevation);

    renderer.lookAt(eyePos, lookPos, up);


    // draw plane
    renderer.texture("Image", "grass");
    renderer.push();
    renderer.translate(vec3(0.0, -0.5, 0));
    renderer.scale(vec3(2.0f));
    renderer.plane();
    renderer.pop();

    // draw tree
    renderer.texture("Image", "tree");
    renderer.push();

    
    vec3 n= normalize(eyePos); // center is at 0,0,0 due to offset mentioned
    float thetaY= atan2(n.x, n.z);
    renderer.rotate(thetaY, vec3(0, 1, 0));

    renderer.scale(vec3(ratio, 1, 1)); // scale the tree to not look stretched

    renderer.translate(vec3(-0.5, -0.5, 0)); // translate to the middle


    renderer.quad(); // vertices span from (0,0,0) to (1,1,0)
    renderer.pop();

    renderer.endShader();
  }

protected:

  vec3 eyePos = vec3(0, 0, 2);
  vec3 lookPos = vec3(0, 0, 0);
  vec3 up = vec3(0, 1, 0);
  float ratio= 1;

  float elevation= 0;
  float azimuth= 0;
  float radius= 2;
};

int main(int argc, char** argv)
{
  Viewer viewer;
  viewer.run();
  return 0;
}
