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

enum KEY {
  W_KEY, A_KEY, S_KEY, D_KEY
};

class Viewer : public Window {
public:
  Viewer() : Window() {
  }

  void setup() {
    setWindowSize(1000, 1000);
    
    renderer.loadShader("simple-texture",
      "../shaders/simple-texture.vs",
      "../shaders/simple-texture.fs");

    createSnowflake(1000);
    renderer.setDepthTest(false);
    renderer.blendMode(agl::ADD);
    
    renderer.loadTexture("snow", "../textures/snow.jpg", 0);
    renderer.blendMode(agl::BLEND);
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
      if (particle.pos.y - particle.size <= 0) resetSnowflake(particle);
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

    // we're subtracting because it's opposite to the eyePos
    azimuth-= (float)dx * 0.01f;
    elevation-= (float)dy * 0.01f;

    // clamp between (-pi/2, pi/2), don't want the bounds since it might lead to weird rotation
    elevation= std::max(elevation, (float) (-M_PI/2) + 0.00001f);
    elevation= std::min(elevation, (float) (M_PI/2)  - 0.00001f);
  }

  void mouseDown(int button, int mods) {
  }

  void mouseUp(int button, int mods) {
  }

  void scroll(float dx, float dy) {

  }

  void keyUp(int key, int mods) {
    if (key == GLFW_KEY_W) {
      WASD_KEY_HELD[W_KEY]= false;
    }

    if (key == GLFW_KEY_A) {
      WASD_KEY_HELD[A_KEY]= false;
    }
    
    if (key == GLFW_KEY_S) {
      WASD_KEY_HELD[S_KEY]= false;
    }

    if (key == GLFW_KEY_D) {
      WASD_KEY_HELD[D_KEY]= false;
    }
  }

  void keyDown(int key, int mods) {
    if (key == GLFW_KEY_W) {
      WASD_KEY_HELD[W_KEY]= true;
    }

    if (key == GLFW_KEY_A) {
      WASD_KEY_HELD[A_KEY]= true;
    }
    
    if (key == GLFW_KEY_S) {
      WASD_KEY_HELD[S_KEY]= true;
    }

    if (key == GLFW_KEY_D) {
      WASD_KEY_HELD[D_KEY]= true;
    }
  }

  void updatePlayerPosition() {
    if (WASD_KEY_HELD[W_KEY]) {
      eyePos= eyePos - stepSize * eyeZAxis;
    }

    if (WASD_KEY_HELD[A_KEY]) {
      eyePos= eyePos - stepSize * eyeXAxis;
    }

    if (WASD_KEY_HELD[S_KEY]) {
      eyePos= eyePos + stepSize * eyeZAxis;
    }

    if (WASD_KEY_HELD[D_KEY]) {
      eyePos= eyePos + stepSize * eyeXAxis;
    }
  }

  void updateLookPos() {
    lookPos.x= LOOK_RADIUS * sin(azimuth) * cos(elevation) + eyePos.x;
    lookPos.y= LOOK_RADIUS * sin(elevation) + eyePos.y;
    lookPos.z= LOOK_RADIUS * cos(azimuth) * cos(elevation) + eyePos.z;
  }

  void draw() {
    renderer.beginShader("sprite");

    float aspect = ((float)width()) / height();
    renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

    updateLookPos();

    renderer.lookAt(eyePos, lookPos, up);

    mat4 VM= renderer.viewMatrix();
    eyeXAxis= vec3(VM[0][0], VM[1][0], VM[2][0]);
    eyeYAxis= vec3(VM[0][1], VM[1][1], VM[2][1]);
    eyeZAxis= vec3(VM[0][2], VM[1][2], VM[2][2]);

    updatePlayerPosition();

    updateSnowflake(dt(), elapsedTime());
    drawSnowflake();
    renderer.endShader();

    renderer.beginShader("simple-texture");
    // draw plane
    renderer.texture("image", "snow");
    renderer.push();
    renderer.translate(vec3(0.0, -1, 0));
    renderer.scale(vec3(30.0f, 1, 30.0f));
    //renderer.plane();
    renderer.pop();
    renderer.endShader();
  }

protected:

  vec3 eyePos = vec3(0, 2, 0);
  vec3 lookPos = vec3(0, 2, 1);
  vec3 up = vec3(0, 1, 0);

  float stepSize= 0.1;

  vec3 eyeXAxis= vec3(1, 0, 0);
  vec3 eyeYAxis= vec3(0, 1, 0);
  vec3 eyeZAxis= vec3(0, 0, 1);

  bool WASD_KEY_HELD[4]= {false, false, false, false};


  float elevation= 0;
  float azimuth= 0;
  const float LOOK_RADIUS= 1;

  std::vector<Particle> mParticles;
};

int main(int argc, char** argv)
{
  Viewer viewer;
  viewer.run();
  return 0;
}