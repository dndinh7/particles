// Bryn Mawr College, alinen, 2020
//

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
    createConfetti(1);
    renderer.setDepthTest(false);
    renderer.blendMode(agl::ADD);
  }

  void createConfetti(int size)
  {
    renderer.loadTexture("particle", "../textures/star4.png", 0);
    for (int i = 0; i < size; i++)
    {
      Particle particle;
      particle.color = vec4(agl::randomUnitCube(), 1);
      particle.size = 0.25;
      particle.rot = 0.0;
      particle.pos = agl::randomUnitCube();
      particle.vel = velocity;
      particle.isDead= true;
      mParticles.push_back(particle);
    }
  }

  void updateConfetti(float dt)
  {
    for (int i= 0; i < mParticles.size(); i++) {
      Particle particle= mParticles[i];
      if (particle.isDead) { // reset
        particle.color = vec4(agl::randomUnitCube(), 1);
        particle.size = 0.25;
        particle.rot = 0.0;
        particle.pos = position;
        particle.isDead= false;
        particle.vel= velocity;
      } else { // update
        particle.color.w-= 0.25 * dt;
        particle.rot+= 0.5 * dt;
        particle.size+= 0.1 * dt;
        particle.pos-= particle.vel * dt;

        if (particle.color.w < 0.3f) particle.isDead= true;
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
