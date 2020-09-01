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
    void load(string prefix, string format, int height, int width, int numberWidth, string imagePath, int startFrame = 0, int endFrame = 10000, ofVec3f translateVec = ofVec3f(-150, -300, -1000), int s = 2){
        this->scale = s;
        this->translateVec = translateVec;
        // load texture
        texture.load(imagePath);
        texture.resize(width, height);
        
        // load/generate mesh for each frame
        kinectHeight = height;
        kinectWidth = width;
        int frameNumber = startFrame;
        string fileName = prefix + ofToString(frameNumber, numberWidth, '0') + "." + format;
        ofShortImage load;
        frames = new vector<ofMesh>(); // TODO clear up that vector memory, but make sure no other player is referencing these frames
        while (ofFile::doesFileExist(fileName) && frameNumber < endFrame){
            load.load(fileName);
            frames->push_back(convertToMesh(load.getPixels()));
            frameNumber++;
            fileName = prefix + ofToString(frameNumber, numberWidth, '0') + "." + format;
        }
        initShaders();
    }
    
    void loadFromPlayer(ofxKinectSequencePlayer& player) {
        frames = player.frames;
        cropRight = player.cropRight;
        cropLeft = player.cropLeft;
        cropUp = player.cropUp;
        cropDown = player.cropDown;
        cropNear = player.cropNear;
        cropFar = player.cropFar;
        translateVec = player.translateVec;
        intensityThreshold = player.intensityThreshold;
        initShaders();
    }
    
    void initShaders() {
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
        return frames->at(frameNumber % frames->size());
    }
    
    ofMesh convertToMesh(ofShortPixels& depthPixelsRaw){
        ofMesh mesh;
        
        ofLog(ofLogLevel::OF_LOG_NOTICE, "loading frame" + ofToString(loadedFrame++)) ;
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
    
    static ofMesh convertToMeshWithoutCropping(ofShortPixels& depthPixelsRaw, int kinectWidth, int kinectHeight){
        ofMesh mesh;
        
        int step = 2;
        for(int y = 0; y < kinectHeight; y += step) {
            for(int x = 0; x < kinectWidth; x += step) {
                auto distance = depthPixelsRaw[y * kinectWidth + x];
                if (distance > 200 && distance < 2000) {
                        mesh.addVertex(ofPoint(x, y, distance));
                        mesh.addColor(ofColor(255));
                }
            }
        }
        
        return mesh;
    }
    
    void update() {
        currentFrame++;
    }
    
    void reset(){
        currentFrame = 0;
    }
    
    int size() {
        return frames->size();
    }
    
    void drawRandom(ofCamera cam) {
        ofClear(255);
        ofSetColor(ofColor::white);
        glPointSize(1);
        ofSetLineWidth(1);
        ofPushMatrix();

        // the projected points are 'upside down' and 'backwardss'
        ofScale(1, -1, -1);
        ofTranslate(position.x, -position.y, -position.z);
        float angle;
        float x;
        float y;
        float z;
        quaternion.getRotate(angle, x, y, z);
        ofRotateDeg(angle, x, -y, -z);
        ofTranslate(translateVec.x, translateVec.y, translateVec.z);
        

        ofMesh currentMesh = getImage(currentFrame);
        currentMesh.draw();
        
//        ofPolyline polyLine;
//
//        for (auto vertice : currentMesh.getVertices()){
//            auto projectedCoordinate = cam.worldToScreen(vertice);
//            polyLine.addVertex(projectedCoordinate.x, projectedCoordinate.y);
//        }
//
//        polyLine.draw();

        ofPopMatrix();
    }
    
    
    void draw() {
        glPointSize(1);
        ofSetLineWidth(1);
        ofPushMatrix();

        // the projected points are 'upside down' and 'backwardss'
        ofScale(1, -1, -1);
        ofTranslate(position.x, -position.y, -position.z);
        float angle;
        float x;
        float y;
        float z;
        quaternion.getRotate(angle, x, y, z);
        ofRotateDeg(angle, x, -y, -z);
        ofTranslate(translateVec.x, translateVec.y, translateVec.z);
        
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
    
    void setCurrentFrame(int i){
        currentFrame = i;
    }
    
    // crop
    int cropRight = 0;
    int cropLeft = 0;
    int cropUp = 0;
    int cropDown = 0;
    int cropNear = 0;
    int cropFar = 1000;
    int scale;
    int intensityThreshold = 100;
    
private:
    vector<ofMesh>* frames;
    int currentFrame = 0;
    int currentPlayedFrame = 0;
    int kinectHeight;
    int kinectWidth;
    ofImage texture;
    ofVec3f position = ofVec3f(0,0,0);
    ofVec3f translateVec;
    
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
    ofQuaternion quaternion;
    int loadedFrame = 0;
    
};
