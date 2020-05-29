#include "ofMain.h"


class MyCamera : public ofEasyCam {
public:
    void update() {
        if (!started) return;
        
        // position
        ofVec3f move = targetPosition - getPosition();
        if (move.length() > camSpeed) {
            move.normalize();
            move *= camSpeed;
        }
        
        // look at
        ofVec3f lookAtDiff = targetLookAt - currentLookAt;
        if (lookAtDiff.length() > lookAtSpeed) {
            lookAtDiff.normalize();
            lookAtDiff *= lookAtSpeed;
            currentLookAt += lookAtDiff;
        } else if (started) {
            setRandomLookAt();
        }
        
        // updir
        if (currentUpVec.x < targetUpVec.x - rollSpeed) {
            currentUpVec.x += rollSpeed;
            
        } else if (currentUpVec.x > targetUpVec.x + rollSpeed) {
            currentUpVec.x -= rollSpeed;
        }
        if (currentUpVec.y < targetUpVec.y - rollSpeed) {
            currentUpVec.y += rollSpeed;
        } else if (currentUpVec.y > targetUpVec.y + rollSpeed) {
            currentUpVec.y -= rollSpeed;
        }
        
        this->move(move);
        this->lookAt(currentLookAt, currentUpVec.getNormalized());
    }
    
    void setRandomLookAt() {
        int x = (int) (ofRandom(CAM_MAX_X * 2)-CAM_MAX_X) * lookAtScale;
        int y = (int) (ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z) * lookAtScale;
        int z = (int) (ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z) * lookAtScale;
        targetLookAt = CENTER + ofVec3f(x, y, z);
    }
    
    void setRandomCamPosition(){
        started = true;
        int x = (int) ofRandom(CAM_MAX_X * 2)-CAM_MAX_X;
        int y = (int) ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z;
        int z = (int) ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z;
        targetPosition = CENTER + ofVec3f(x, y, z);
        setRandomLookAt();
        targetUpVec = ofVec3f(ofRandom(2) - 1, ofRandom(2) - 1, 0).normalize();
    }
    
    void fastMoveToRandomPosition() {
        camSpeed = FAST_CAM_SPEED;
        lookAtScale = FAST_LOOK_AT_SCALE;
        lookAtSpeed = FAST_LOOK_AT_SPEED;
        rollSpeed = FAST_ROLL_SPEED;
        setRandomCamPosition();
    }
    
    void slowMoveToRandomPosition() {
        camSpeed = DEFAULT_CAM_SPEED;
        lookAtScale = DEFAULT_LOOK_AT_SCALE;
        lookAtSpeed = DEFAULT_LOOK_AT_SPEED;
        rollSpeed = DEFAULT_ROLL_SPEED;
        setRandomCamPosition();
    }
    
    ofVec3f getTargetPosition(){
        return targetPosition;
    }
    
    ofVec3f getTargetLookAt(){
        return targetLookAt;
    }
    
    ofVec3f getCurrentLookAt(){
        return currentLookAt;
    }
    
    ofVec3f getCurrentUpVec() {
        return currentUpVec;
    }
    
private:
    bool started = false;
    int CAM_MAX_X = 1000;
    int CAM_MAX_Y = 1000;
    int CAM_MAX_Z = 1000;
    
    int DEFAULT_CAM_SPEED = 5;
    float DEFAULT_LOOK_AT_SCALE = 0.5;
    float DEFAULT_LOOK_AT_SPEED = 1.5;
    float DEFAULT_ROLL_SPEED = 0.01;
    
    int FAST_CAM_SPEED = 50;
    float FAST_LOOK_AT_SCALE = 5.0;
    float FAST_LOOK_AT_SPEED = 15.0;
    float FAST_ROLL_SPEED = 0.1;

    
    int camSpeed = DEFAULT_CAM_SPEED;
    float lookAtScale = DEFAULT_LOOK_AT_SCALE;
    float lookAtSpeed = DEFAULT_LOOK_AT_SPEED;
    float rollSpeed = DEFAULT_ROLL_SPEED;
    
    ofVec3f CENTER = ofVec3f(0, 0, 0);
    ofVec3f targetUpVec = ofVec3f(0,1,0);
    ofVec3f currentUpVec = targetUpVec;
    ofVec3f targetPosition = ofVec3f(0, 0, 0);
    ofVec3f currentLookAt = CENTER;
    ofVec3f targetLookAt = currentLookAt;
};
