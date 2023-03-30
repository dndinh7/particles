// Bryn Mawr College, alinen, 2020
//

/**
 * This program moves a star particle in a circle,
 * then leaves a trail of colorful stars behind.
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

struct Particle {
  glm::vec3 pos;
  glm::vec3 vel;
  glm::vec4 color;
  float rot;
  float size;
  bool isDead;
};

class Viewer : public Window {
public:
  Viewer() : Window() {
  }

  void setup() {
    setWindowSize(1000, 1000);
    createConfetti(100);
    renderer.setDepthTest(false);
    renderer.blendMode(agl::ADD);
  }

  void createConfetti(int size)
  {
    renderer.loadTexture("particle", "../textures/star4.png", 0);
    for (int i = 0; i < size; i++)
    {
      Particle particle;
      particle.color = vec4(0, 0, 0, 0);
      particle.size = 0.25;
      particle.rot = 0.0;
      particle.vel = velocity;
      particle.pos = position;
      particle.isDead= false;
      mParticles.push_back(particle);
    }
  }

  void updateConfetti(float dt)
  {
    bool placed= false; // only one should be placed per frame
    bool died= false; // only one should be recycled per frame
    for (int i= 0; i < mParticles.size(); i++) {
      Particle particle= mParticles[i];
      if (particle.isDead && !placed) { // reset
        particle.color = vec4(agl::randomUnitCube(), 1);
        particle.size = 0.25;
        particle.rot = 0.0;
        particle.pos = position;
        particle.isDead= false;
        // jitter the velocity
        particle.vel= velocity + vec3(rand() % 10 / 10.0f, rand() % 10 / 10.0f, 0);
      } else if (!particle.isDead) { // update
        float scalarFactor= (rand() % 5 + 1) * 0.2f;

        particle.color.w-= 0.5 * dt;
        particle.rot+= scalarFactor * 2.0f * dt;
        particle.size+= 0.1 * dt;
        particle.pos-= particle.vel * dt;

        if (particle.color.w < 0.4f && !died) {
          particle.isDead= true;
          died= true;
        }
      }

      mParticles[i]= particle;
    }
  }

  void drawConfetti()
  {
    renderer.texture("image", "particle");
    for (int i = 0; i < mParticles.size(); i++)
    {
      Particle particle = mParticles[i];
      renderer.sprite(particle.pos, particle.color, particle.size, particle.rot);
    }
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

  void keyUp(int key, int mods) {
  }

  void draw() {
    renderer.beginShader("sprite");

    float aspect = ((float)width()) / height();
    renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

    renderer.lookAt(eyePos, lookPos, up);
    velocity= vec3(-sin(elapsedTime()), cos(elapsedTime()), 0);
    position= position + velocity*dt();
    renderer.sprite(position, vec4(1.0f), 0.25f);
    updateConfetti(dt());
    drawConfetti();
    renderer.endShader();
  }

protected:

  vec3 eyePos = vec3(0, 0, 3);
  vec3 lookPos = vec3(0, 0, 0);
  vec3 up = vec3(0, 1, 0);
  vec3 position = vec3(1, 0, 0);
  vec3 velocity = vec3(-sin(0), cos(0), 0);

  std::vector<Particle> mParticles;
};

int main(int argc, char** argv)
{
  Viewer viewer;
  viewer.run();
  return 0;
}
