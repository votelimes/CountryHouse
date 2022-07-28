#pragma once

#include <glm/glm.hpp>

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular; 
};

struct PointLight {
    glm::vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;       
};

struct Light {
    glm::vec3 position;

    glm::vec3 diffuse;
    glm::vec3 specular; 
};

class DayLight
{
private:
    const float dayLightIncreaser = 0.001;
    bool directionAscending = true;
    float rate = 0.015;
    float trajectoryCenterX = 0.0f;
    float trajectoryCenterY = 0.0f;
    float trajectoryCenterZ = -55.0f;

    const float orbitalSpeed = 0.1;
    const float radius = 50.0;
    float angle = 0;
    
public:
    bool flashlightEnabled = false;
    SpotLight sunLight;

    PointLight window1Light;
    PointLight window2Light;
    PointLight window3Light;
    SpotLight flashlight;
    glm::vec3 bgColor;

    DayLight();
    ~DayLight();

    void proceedAmbient(float deltaTime, glm::vec3 &camPosition);
    void proceedSunMoon(float deltaTime);
    bool isDay();
    bool isDayStarting();

    bool isNight();
    bool isNightStarting();

    float sunX{};
    float sunY{};
    float sunZ{-45.0f};

    float moonX{};
    float moonY{};
    float moonZ{-45.0f};

    float fog = 0.00;
};

DayLight::DayLight()
{
    bgColor = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rate = 0.0001;

    sunLight.position = glm::vec3(60.0f, 60.0f, -80.0f);
    sunLight.direction = glm::vec3(0.0f, -44.0f, 10.0f);
    sunLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    sunLight.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    sunLight.specular = glm::vec3(0.4f, 0.4f, 0.4f);
    sunLight.constant = 1.0f;
    sunLight.linear = 0.1f;    
    sunLight.quadratic = 0.1f;
    sunLight.cutOff = glm::cos(glm::radians(122.5f));
    sunLight.outerCutOff = glm::cos(glm::radians(127.5f));

    window1Light.constant = 5.0f;
    window2Light.constant = 5.0f;
    window3Light.constant = 5.0f;
    flashlight.constant = 1.0f;

    window1Light.linear = 0.1f;
    window2Light.linear = 0.1f;
    window3Light.linear = 0.1f;
    flashlight.linear = 0.1f;

    window1Light.quadratic = 0.01f;
    window2Light.quadratic = 0.01f;
    window3Light.quadratic = 0.01f;
    flashlight.quadratic = 0.1f;

    window1Light.ambient = glm::vec3(0.8f, 0.6f, 0.3f);
    window2Light.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    window3Light.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    flashlight.ambient = glm::vec3(0.5f, 0.5f, 0.5f);

    window1Light.diffuse = glm::vec3(0.6f, 0.5f, 0.35f);
    window2Light.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    window3Light.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    flashlight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);

    window1Light.specular = glm::vec3(0.01f, 0.01f, 0.01f);
    window2Light.specular = glm::vec3(0.01f, 0.01f, 0.01f);
    window3Light.specular = glm::vec3(0.01f, 0.01f, 0.01f);
    flashlight.specular = glm::vec3(0.1f, 0.1f, 0.1f);

    flashlight.cutOff = glm::cos(glm::radians(12.5f));
    flashlight.outerCutOff = glm::cos(glm::radians(17.5f));
}

DayLight::~DayLight()
{
}

void DayLight::proceedAmbient(float deltaTime, glm::vec3 &camPosition)
{
    //std::cout << "FOG: " << fog << std::endl;
    
    rate = deltaTime / 17.0f;

    static float sunLightDirRate = deltaTime / 88.0f;
    
    static bool firstLoop = true;

    float rate2 = rate * 1.5;

    sunLight.direction.x -= sunLightDirRate;

    float fogRate = 0.000125;//0.000075;
    
    if(isDayStarting()){
        sunLight.direction = glm::vec3(0.0f, -44.0f, 10.0f);

        window1Light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
        window2Light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
        window3Light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);

        window1Light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
        window2Light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
        window3Light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);

        window1Light.specular = glm::vec3(0.0f, 0.0f, 0.0f);
        window2Light.specular = glm::vec3(0.0f, 0.0f, 0.0f);
        window3Light.specular = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    if(isNightStarting()){
        sunLight.direction = glm::vec3(0.0f, -44.0f, 10.0f);

        window1Light.ambient = glm::vec3(0.8f, 0.6f, 0.3f);
        window2Light.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
        window3Light.ambient = glm::vec3(0.5f, 0.5f, 0.5f);

        window1Light.diffuse = glm::vec3(0.8f, 0.6f, 0.3f);
        window2Light.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        window3Light.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);

        window1Light.specular = glm::vec3(0.0f, 0.0f, 0.0f);
        window2Light.specular = glm::vec3(4.0f, 4.0f, 4.0f);
        window3Light.specular = glm::vec3(4.0f, 4.0f, 4.0f);

        firstLoop = false;
    }
    
    if((isDay() && sunX > 0 && bgColor.r < 0.8)){
        
        bgColor.r += rate;
        bgColor.g += rate;
        bgColor.b += rate;

        sunLight.ambient.r += rate2;
        sunLight.ambient.g += rate2;
        sunLight.ambient.b += rate2;

        sunLight.diffuse.r += rate2;
        sunLight.diffuse.g += rate2;
        sunLight.diffuse.b += rate2;

        sunLight.specular.r += rate2;
        sunLight.specular.g += rate2;
        sunLight.specular.b += rate2;
    }  
    
    if(isDay() && sunX < 0){
        bgColor.r -= rate;
        bgColor.g -= rate;
        bgColor.b -= rate;

        sunLight.ambient.r -= rate2;
        sunLight.ambient.g -= rate2;
        sunLight.ambient.b -= rate2;

        sunLight.diffuse.r -= rate2;
        sunLight.diffuse.g -= rate2;
        sunLight.diffuse.b -= rate2;

        sunLight.specular.r -= rate2;
        sunLight.specular.g -= rate2;
        sunLight.specular.b -= rate2;
    }

    if(isDay() && sunX > -10 && sunX < 24.6 && !firstLoop){
        fog -= fogRate;
    }

    if(isNight() && sunX > 10 && sunX < 40 && !firstLoop){
        fog += fogRate;
    }

    if(isDay() && sunX > -12 && sunX < -11){
        fog = 0.0f;
    }
    
    if(flashlightEnabled){
        flashlight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
        flashlight.diffuse = glm::vec3(3.9f, 3.9f, 3.9f);
        flashlight.specular = glm::vec3(0.7f, 0.7f, 0.7f);
    }
    else{
        flashlight.position = camPosition;
        flashlight.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
        flashlight.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
        flashlight.specular = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

void DayLight::proceedSunMoon(float deltaTime)
{
    angle += orbitalSpeed * deltaTime;
    
    sunX = trajectoryCenterX + radius * cos(angle);
    sunY = trajectoryCenterY + radius * sin(angle);

    angle += orbitalSpeed * deltaTime;
    moonX = trajectoryCenterX - radius * cos(angle);
    moonY = trajectoryCenterY - radius * sin(angle);

    sunLight.position.x = sunX;
    sunLight.position.y = sunY;
}
bool DayLight::isDay(){
    if(sunY >= 0){
        return true;
    }
    else{
        return false;
    }
}

bool DayLight::isNight(){
    if(moonY >= 0){
        return true;
    }
    else{
        return false;
    }
}

bool DayLight::isDayStarting(){
    if(isDay() && sunX >= 49.8 && sunX <= 49.9){
        return true;
    }
    else{
        return false;
    }
}

bool DayLight::isNightStarting(){
    if(isNight() && moonX >= 49.8 && moonX <= 49.9){
        return true;
    }
    else{
        return false;
    }
}

