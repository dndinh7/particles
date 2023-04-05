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
  float size;
  float rot;
  float startTime;
  float freq; // frequency of particle circular motion
  float amp; // amplitude of motion
  float angleOffset; // angular offset
};

class Viewer : public Window {
public:
  Viewer() : Window() {
  }

  void setup() {
    setWindowSize(1000, 1000);
    createSnowflake(1000);
    renderer.setDepthTest(false);
    renderer.blendMode(agl::ADD);
  }

  float randBound(float lowerBound, float upperBound) {
    return rand() / float(RAND_MAX) * (upperBound - lowerBound) + lowerBound;
  }

  // todo create clouds
  // todo create fire with logs

  void createSnowflake(int size)
  {
    renderer.loadTexture("particle", "../textures/particle.png", 0);
    for (int i = 0; i < size; i++)
    {
      Particle particle;
      particle.color = vec4(1, 1, 1, 1);
      particle.size = 0.15 + randBound(0, 0.15f);
      particle.rot = 0;
      particle.pos = vec3(randBound(-10, 10), randBound(0, 20), randBound(-10, 10));
      particle.vel = vec3(0, randBound(-0.8, -0.2), 0);
      particle.freq= randBound(0.5, 1);
      particle.amp= randBound(1, 2);
      particle.angleOffset= randBound(-M_PI, M_PI);
      mParticles.push_back(particle);

    }
  }

  void resetSnowflake(Particle& particle) {
    particle.pos = vec3(randBound(-10, 10), 20, randBound(-10, 10));
    particle.vel = vec3(0, randBound(-0.8, -0.2), 0);
    particle.freq= randBound(0.5, 1);
    particle.amp= randBound(1, 2);
    particle.angleOffset= randBound(-M_PI, M_PI);
  }

  void updateSnowflake(float dt, float elapsedTime)
  {
    for (int i = 0; i < mParticles.size(); i++)
    {
      Particle particle = mParticles[i];
      particle.vel= vec3(cos(particle.freq * elapsedTime + particle.angleOffset), particle.vel.y, -sin(particle.freq * elapsedTime + particle.angleOffset));
      particle.pos += particle.vel * dt;
      if (particle.pos.y <= 0) resetSnowflake(particle);
      mParticles[i] = particle;
    }
  }

  void drawSnowflake()
  {
    vec3 cameraPos = renderer.cameraPosition();

    // sort
    for (int i = 1; i < mParticles.size(); i++)
    {
      Particle particle1 = mParticles[i];
      Particle particle2 = mParticles[i - 1];
      float dSqr1 = length2(particle1.pos - cameraPos);
      float dSqr2 = length2(particle2.pos - cameraPos);
      if (dSqr2 < dSqr1)
      {
        mParticles[i] = particle2;
        mParticles[i - 1] = particle1;
      }
    }

    // draw
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
    eyePos.x -= dy;
  }

  void keyUp(int key, int mods) {

  }

  void draw() {
    renderer.beginShader("sprite");

    float aspect = ((float)width()) / height();
    renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
    
    // draw plane
    renderer.texture("Image", "stone");
    renderer.push();
    renderer.translate(vec3(0.0, -0.5, 0));
    renderer.scale(vec3(2.0f));
    renderer.plane();
    renderer.pop();

    renderer.lookAt(eyePos, lookPos, up);
    updateSnowflake(dt(), elapsedTime());
    drawSnowflake();
    renderer.endShader();
  }

protected:

  vec3 eyePos = vec3(10, 0, 0);
  vec3 lookPos = vec3(0, 0, 0);
  vec3 up = vec3(0, 1, 0);

  std::vector<Particle> mParticles;
};

int main(int argc, char** argv)
{
  Viewer viewer;
  viewer.run();
  return 0;
}