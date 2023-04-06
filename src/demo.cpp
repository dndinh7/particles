#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"

using namespace std;
using namespace glm;
using namespace agl;

/**
 * This program impelements a velocity flow field using
 * Perlin noise and creates particles to adhere to the flow field
 * 
 * David Dinh
 * 
 * References: https://cs.nyu.edu/~perlin/noise/
 * 
*/

struct Particle {
  glm::vec3 pos;
  glm::vec3 vel;
  glm::vec4 color;
  float size;
  float rot;
};

enum KEY {
  W_KEY, A_KEY, S_KEY, D_KEY
};

double clamp(double value, double low, double high) {
  return std::max(std::min(value, high), low);
}

class Noise {
  public:

    Noise() {
      initArrayP();
    }

    double noise(double x, double y, double z) {
      int X= (int) floor(x) & 255;  // get the unit cube grid that contains (x,y,z)
      int Y= (int) floor(y) & 255;
      int Z= (int) floor(z) & 255;

      x-= floor(x); // relative coordinates to the cube grid
      y-= floor(y);
      z-= floor(z);

      // fade curves which smooths the transitions
      double u= fade(x);
      double v= fade(y);
      double w= fade(z);

      // hash coordinates of the corner of cubes
      int A = p[X] + Y;
      int AA= p[A] + Z;
      int AB= p[A+1] + Z;
      int B = p[X+1] + Y;
      int BA= p[B] + Z;
      int BB= p[B+1] + Z;

      double gradAA= grad(p[AA], x, y, z);
      double gradBA= grad(p[BA], x-1, y, z);
      double gradAB= grad(p[AB], x, y-1, z);
      double gradBB= grad(p[BB], x-1, y-1, z);
      double gradAA_1= grad(p[AA+1], x, y, z-1);
      double gradBA_1= grad(p[BA+1], x-1, y, z-1);
      double gradAB_1= grad(p[AB+1], x, y-1, z-1);
      double gradBB_1= grad(p[BB+1], x-1, y-1, z-1);

      // first degree interpolation
      double A_u= lerp(u, gradAA, gradBA);
      double B_u= lerp(u, gradAB, gradBB);
      double A1_u= lerp(u, gradAA_1, gradBA_1);
      double B1_u= lerp(u, gradAB_1, gradBB_1);

      // bilinear interpolation
      double b= lerp(v, A_u, B_u);
      double b_1= lerp(v, A1_u, B1_u);

      // trilinear interpolation
      double tri= lerp(w, b, b_1);

      return tri;
    }

    static double fade(double t) {
      return t * t * t * (t * ( t * 6 - 15) + 10);
    }

    static double lerp(double t, double a, double b) {
      return a + t * (b-a);
    }

    static double grad(int hash, double x, double y, double z) {
      int h= hash & 15;
      double u= h < 8 ? x : y;
      double v= (h < 4) ? y : (h == 12 || h == 14) ? x : z;

      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
    }

    static constexpr int permutation[] = {151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
    129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
    49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

    int p[512];

  private:

    void initArrayP() {
      for (int i= 0; i < 256; i++) {
        p[256+i]= p[i]= permutation[i];
      }
    }
};

constexpr int Noise::permutation[];

class FlowField {
  public:
    FlowField() {
      // should not be used
    }

    // inits the flowfield based on the noise factor
    FlowField(Noise n, vec3 minPoint, vec3 maxPoint) {
      this->minPoint= minPoint;
      this->maxPoint= maxPoint;

      dim= maxPoint - minPoint;
      int size= dim.x * dim.y * dim.z;

      double scale= 0.1;

      velocityField= vector<vec3>(size, vec3(0));

      for (int x= minPoint.x; x < maxPoint.x; x++) {
        for (int y= minPoint.y; y < maxPoint.y; y++) {
          for (int z= minPoint.z; z < maxPoint.z; z++) {
            double noise= n.noise(x * scale, y * scale, z * scale); // 0 to 1
            double angle= noise * 2 * M_PI; // then we map it to an angle
            vec3 vel= vec3(cos(angle), sin(angle), noise);

            int xIdx= x - minPoint.x;
            int yIdx= y - minPoint.y;
            int zIdx= z - minPoint.z;

            velocityField[zIdx * dim.x * dim.y + yIdx * dim.x + xIdx]= vel;
          }
        }
      }
    }

    vec3 getVel(vec3 p) {
      int x= p.x - minPoint.x;
      int y= p.y - minPoint.y;
      int z= p.z - minPoint.z;

      //x= clamp(x, 0, dim.x - 1);
      //y= clamp(y, 0, dim.y - 1);
      //z= clamp(z, 0, dim.z - 1);

      return velocityField[z * dim.x * dim.y + y * dim.x + x];
    }

    vec3 minPoint;
    vec3 maxPoint;
    vec3 dim; // dimensions of the cube/rectangular prism
    vector<vec3> velocityField;
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

    noise= Noise();


    // change these values to change the flowfield dimensions :)
    flowFieldMinBound= vec3(-15, -15, -15);
    flowFieldMaxBound= vec3(15, 15, 15);

    flowFieldDimensions= flowFieldMaxBound - flowFieldMinBound;

    eyePos= vec3(flowFieldMaxBound.x + 10, 0, 0);
    lookPos= vec3(0);

    flowField= FlowField(noise, flowFieldMinBound, flowFieldMaxBound);

    createConfetti(5000);
    renderer.setDepthTest(false);
    renderer.blendMode(agl::ADD);
    
    renderer.blendMode(agl::BLEND);
  }

  float randBound(float lowerBound, float upperBound) {
    return rand() / float(RAND_MAX) * (upperBound - lowerBound) + lowerBound;
  }

  void createConfetti(int size)
  {
    renderer.loadTexture("particle", "../textures/particle.png", 0);
    for (int i = 0; i < size; i++)
    {
      Particle particle;
      particle.color = vec4(1, 1, 1, 1);
      particle.size = randBound(0.15f, 0.25f);
      particle.rot = 0;
      particle.pos = vec3(randBound(-10, 10), randBound(-10, 10), randBound(-10, 10));
      particle.vel = flowField.getVel(particle.pos);
      mParticles.push_back(particle);

    }
  }

  void resetConfetti(Particle& particle) {
    particle.pos = vec3(randBound(-10, 10), randBound(-10, 10), randBound(-10, 10));
    particle.vel = flowField.getVel(particle.pos);
  }

  void updateConfetti(float dt, float elapsedTime)
  {
    for (int i = 0; i < mParticles.size(); i++)
    {
      Particle particle = mParticles[i];
      particle.vel = flowField.getVel(particle.pos);
      particle.pos += particle.vel * 2.0f * dt;


      // set the color to be the ratio of where it is in relation to the bounding box
      particle.color= vec4((particle.pos.x - flowFieldMinBound.x) / flowFieldDimensions.x, 
        (particle.pos.y - flowFieldMinBound.y) / flowFieldDimensions.y, 
        (particle.pos.z - flowFieldMinBound.z) / flowFieldDimensions.z, 1);

      
      if (particle.pos.x <= flowFieldMinBound.x || particle.pos.x >= flowFieldMaxBound.x) resetConfetti(particle);
      if (particle.pos.y <= flowFieldMinBound.y || particle.pos.y >= flowFieldMaxBound.y) resetConfetti(particle);
      if (particle.pos.z <= flowFieldMinBound.z || particle.pos.z >= flowFieldMaxBound.z) resetConfetti(particle);
      mParticles[i] = particle;
    }
  }

  void drawConfetti()
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
    if (keyIsDown(GLFW_KEY_LEFT_CONTROL)) {
      // we're subtracting because it's opposite to the eyePos
      azimuth-= (float)dx * 0.01f;
      elevation-= (float)dy * 0.01f;

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
    
    if (key == GLFW_KEY_SPACE) {
      eyePos = vec3(flowFieldMaxBound.x + 10, 0, 0);
      lookPos = vec3(0, 0, 0);
      elevation= 0;
      azimuth= 3*M_PI/2;

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
    glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    renderer.beginShader("sprite");

    float aspect = ((float)width()) / height();
    renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
    updateLookPos();


    renderer.lookAt(eyePos, lookPos, up);

    // this keep starck of the axies of the view matrix,
    // to allow camera movement
    mat4 VM= renderer.viewMatrix();
    eyeXAxis= vec3(VM[0][0], VM[1][0], VM[2][0]);
    eyeYAxis= vec3(VM[0][1], VM[1][1], VM[2][1]);
    eyeZAxis= vec3(VM[0][2], VM[1][2], VM[2][2]);

    updatePlayerPosition();

    updateConfetti(dt(), elapsedTime());
    drawConfetti();
    renderer.endShader();

  }

protected:

  vec3 eyePos;
  vec3 lookPos;
  vec3 up = vec3(0, 1, 0);

  float stepSize= 0.1;

  vec3 eyeXAxis= vec3(1, 0, 0);
  vec3 eyeYAxis= vec3(0, 1, 0);
  vec3 eyeZAxis= vec3(0, 0, 1);

  bool WASD_KEY_HELD[4]= {false, false, false, false};

  FlowField flowField;
  Noise noise;
  vec3 flowFieldMinBound;
  vec3 flowFieldMaxBound;
  vec3 flowFieldDimensions;




  float elevation= 0;
  float azimuth= 3*M_PI/2;
  const float LOOK_RADIUS= 1;

  std::vector<Particle> mParticles;
};

int main(int argc, char** argv)
{
  Viewer viewer;

  viewer.run();
  return 0;
}