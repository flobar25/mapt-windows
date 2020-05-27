#include "ofMain.h"


class MyCamera : public ofEasyCam {
public:
    void update() {
        if (!started) return;
        
        // position
        ofVec3f move = targetPosition - getPosition();
        if (move.length() > CAM_SPEED) {
            move.normalize();
            move *= CAM_SPEED;
        }
        
        // look at
        ofVec3f lookAtDiff = targetLookAt - currentLookAt;
        if (lookAtDiff.length() > LOOK_AT_SPEED) {
            lookAtDiff.normalize();
            lookAtDiff *= LOOK_AT_SPEED;
            currentLookAt += lookAtDiff;
        } else if (started) {
            setRandomLookAt();
        }
        
        // updir
        if (currentUpVec.x < targetUpVec.x - ROLL_SPEED) {
            currentUpVec.x += ROLL_SPEED;
            
        } else if (currentUpVec.x > targetUpVec.x + ROLL_SPEED) {
            currentUpVec.x -= ROLL_SPEED;
        }
        if (currentUpVec.y < targetUpVec.y - ROLL_SPEED) {
            currentUpVec.y += ROLL_SPEED;
        } else if (currentUpVec.y > targetUpVec.y + ROLL_SPEED) {
            currentUpVec.y -= ROLL_SPEED;
        }
        
        this->move(move);
        this->lookAt(currentLookAt, currentUpVec.getNormalized());
    }
    
    void setRandomLookAt() {
        int x = (int) (ofRandom(CAM_MAX_X * 2)-CAM_MAX_X) * LOOK_AT_SCALE;
        int y = (int) (ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z) * LOOK_AT_SCALE;
        int z = (int) (ofRandom(CAM_MAX_Z * 2)-CAM_MAX_Z) * LOOK_AT_SCALE;
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
    
private:
    bool started = false;
    int CAM_MAX_X = 1000;
    int CAM_MAX_Y = 1000;
    int CAM_MAX_Z = 1000;
    float CAM_NOISE_AMOUNT = 1.0;
    int CAM_SPEED = 5;
    float LOOK_AT_SCALE = 0.5;
    float LOOK_AT_SPEED = 1.5;
    float ROLL_SPEED = 0.01;
    
    ofVec3f CENTER = ofVec3f(0, 0, 0);
    ofVec3f targetUpVec = ofVec3f(0,1,0);
    ofVec3f currentUpVec = targetUpVec;
    ofVec3f targetPosition = ofVec3f(0, 0, 0);
    ofVec3f currentLookAt = CENTER;
    ofVec3f targetLookAt = currentLookAt;
};
