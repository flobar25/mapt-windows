#include "ofMain.h"

enum EffectType {
    NONE,
    STRETCH,
    STRIPS,
    EXPLOSION,
    INVISIBLE
};

class ofxKinectSequencePlayer  {
public:
    
    void load(string prefix, string format, int height, int width, int numberWidth, string imagePath, int s = 2){
        scale = 2;
        // load texture
        texture.load(imagePath);
        texture.resize(width, height);
        
        // load/generate mesh for each frame
        kinectHeight = height;
        kinectWidth = width;
        int frameNumber = 0;
        string fileName = prefix + ofToString(frameNumber, numberWidth, '0') + "." + format;
        ofShortImage load;
        while (ofFile::doesFileExist(fileName)){
            load.load(fileName);
            frames.push_back(convertToMesh(load.getPixels()));
            frameNumber++;
            fileName = prefix + ofToString(frameNumber, numberWidth, '0') + "." + format;
        }
        
        // setup shaders
        stretchShader.setGeometryInputType(GL_POINTS);
        stretchShader.setGeometryOutputType(GL_LINE_STRIP);
        stretchShader.setGeometryOutputCount(2);
        stretchShader.load("myshaders/stretchShaderVert.c", "myshaders/stretchShaderFrag.c", "myshaders/stretchShaderGeo.c");
        
        explodingShader.setGeometryInputType(GL_TRIANGLES);
        explodingShader.setGeometryOutputType(GL_TRIANGLE_STRIP);
        explodingShader.setGeometryOutputCount(3);
        explodingShader.load("myshaders/explodingShaderVert.c", "myshaders/explodingShaderFrag.c", "myshaders/explodingShaderGeo.c");
    }
    
    ofMesh getImage(int frameNumber){
        return frames.at(frameNumber % frames.size());
    }
    
    ofMesh convertToMesh(ofShortPixels& depthPixelsRaw){
        ofMesh mesh;
        
        int step = 2;
        for(int y = cropUp; y < kinectHeight - cropDown; y += step) {
            for(int x = cropLeft; x < kinectWidth - cropRight; x += step) {
                auto distance = depthPixelsRaw[y * kinectWidth + x];
                auto distance2 = depthPixelsRaw[(y+step) * kinectWidth + x];
                if(distance > cropNear && distance < cropFar && distance2 > cropNear && distance2 < cropFar) {
                    mesh.addVertex(ofPoint(x, y, distance));
                    auto spaceColor = texture.getColor(x, y);
                    if (spaceColor.getLightness() > intensityThreshold) {
                        mesh.addColor(spaceColor);
                    } else {
                        mesh.addColor(ofColor(255));
                    }
                    
                    mesh.addVertex(ofPoint(x, y+step, distance2));
                    spaceColor = texture.getColor(x, y+step);
                    if (spaceColor.getLightness() > intensityThreshold) {
                        mesh.addColor(spaceColor);
                    } else {
                        mesh.addColor(ofColor(255));
                    }
                }
            }
        }
        
        return mesh;
    }
    
    void update() {
        currentFrame++;
    }
    
    void draw() {
        glPointSize(1);
        ofSetLineWidth(1);
        ofPushMatrix();

        // the projected points are 'upside down' and 'backwardss'
        ofScale(1, -1, -1);
//        ofRotateXDeg(orientationEulerDeg.x);
//        ofRotateYDeg(-orientationEulerDeg.y);
//        ofRotateZDeg(-orientationEulerDeg.z);
        float angle;
        float x;
        float y;
        float z;
        quaternion.getRotate(angle, x, y, z);
        ofRotateDeg(angle, x, -y, -z);
        ofTranslate(-150 + position.x, -300 - position.y, -1000 - position.z); // center the points a bit
        
        auto time = currentFrame - effectStartFrame;
        ofMesh currentMesh = ofBoxPrimitive(100,100,100).getMesh();
        switch (currentEffect) {
            case EffectType::NONE:
                currentPlayedFrame = currentFrame;
                currentMesh = getImage(currentPlayedFrame);
                currentMesh.drawVertices();
                break;
            case EffectType::STRETCH:
                currentPlayedFrame = currentFrame;
                currentMesh = getImage(currentPlayedFrame);
                currentMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_POINTS);
                stretchShader.begin();
                stretchShader.setUniform3f("targetPosition", stretchTargetPosition);
                stretchShader.setUniform1i("time", time);
                stretchShader.setUniform1i("stretchFramesCount", stretchFramesCount);
                currentMesh.drawVertices();
                stretchShader.end();
                break;
            case EffectType::STRIPS:
                currentPlayedFrame = currentFrame;
                currentMesh = getImage(currentPlayedFrame);
                currentMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLE_STRIP);
                currentMesh.drawWireframe();
                break;
            case EffectType::EXPLOSION:
                explodingShader.begin();
                explodingShader.setUniform1f("time", time);
                currentMesh = getImage(currentPlayedFrame);
                currentMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLE_STRIP);
                currentMesh.drawVertices();
                explodingShader.end();
                break;
            case EffectType::INVISIBLE:
                // draw nothing
                break;
            default:
                break;
        }
        ofPopMatrix();
    }
    
    void setPosition(int x, int y, int z) {
        position = ofVec3f(x,y,z);
    }
    
    void move(int x, int y, int z) {
        position = ofVec3f(position.x + x, position.y + y, position.z + z);
    }
    
    void setOrientationEulerDeg(ofVec3f orientation){
        orientationEulerDeg = ofVec3f(orientation.x,orientation.y,orientation.z);
    }
    
    void setQuaternion(glm::quat q){
        quaternion = ofQuaternion(q);
    }
    
    void setEffect(EffectType effect){
        switch (effect) {
            case EffectType::NONE:
                currentEffect = EffectType::NONE;
                effectStartFrame = -1;
                break;
            case EffectType::STRETCH:
                currentEffect = EffectType::STRETCH;
                effectStartFrame = currentFrame - 1;
                stretchTargetPosition = ofVec3f(ofRandom(200) - 100, ofRandom(200) - 100, ofRandom(200) - 100).normalize() * stretchLineLength;
                break;
            case EffectType::STRIPS:
                currentEffect = EffectType::STRIPS;
                effectStartFrame = currentFrame - 1;
                break;
            case EffectType::EXPLOSION:
                currentEffect = EffectType::EXPLOSION;
                effectStartFrame = currentFrame - 1;
                break;
            case EffectType::INVISIBLE:
                currentEffect = EffectType::INVISIBLE;
                effectStartFrame = currentFrame - 1;
                break;
            default:
                break;
        }
    }
    
    
    // crop
    int cropRight = 0;
    int cropLeft = 0;
    int cropUp = 0;
    int cropDown = 0;
    int cropNear = 0;
    int cropFar = 1000;
    int scale;
    
private:
    vector<ofMesh> frames;
    int currentFrame = 0;
    int currentPlayedFrame = 0;
    int kinectHeight;
    int kinectWidth;
    int intensityThreshold = 100;
    ofImage texture;
    ofVec3f position = ofVec3f(0,0,0);
    
    // general effects
    EffectType currentEffect = EffectType::INVISIBLE;
    int effectStartFrame = -1;
    
    // stretching lines
    ofShader stretchShader;
    ofVec3f stretchTargetPosition;
    ofVec3f stretchCurrentPosition;
    int stretchLineLength = 100;
    int stretchFramesCount = 20;
    
    // explosion
    ofShader explodingShader;
    
    // rotation
    ofVec3f orientationEulerDeg = ofVec3f(0,0,0);
    ofQuaternion quaternion;
    
    
};
