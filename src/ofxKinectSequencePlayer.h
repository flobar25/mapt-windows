#include "ofMain.h"


class ofxKinectSequencePlayer  {
public:
        
    void load(string prefix, string format, int height, int width, int numberWidth, string imagePath){
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
        shader.setGeometryInputType(GL_POINTS);
        shader.setGeometryOutputType(GL_LINE_STRIP);
        shader.setGeometryOutputCount(2);
        shader.load("shaders/kinectShaderVert.c", "shaders/kinectShaderFrag.c", "shaders/kinectShaderGeo.c");

        explodingShader.setGeometryInputType(GL_TRIANGLES);
        explodingShader.setGeometryOutputType(GL_TRIANGLE_STRIP);
        explodingShader.setGeometryOutputCount(3);
        explodingShader.load("shaders/explodingShaderVert.c", "shaders/explodingShaderFrag.c", "shaders/explodingShaderGeo.c");
    }
    
    ofMesh getNextImage() {
        return frames.at(currentPlayedFrame++ % frames.size());
    }
    
    ofMesh getCurrentImage() {
        return frames.at(currentPlayedFrame % frames.size());
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
    
    void draw() {
        currentFrame++;
        glPointSize(1);
        ofPushMatrix();
        // the projected points are 'upside down' and 'backwards'
        ofScale(1, -1, -1);
        ofTranslate(-220 + position.x, -400 + position.y, -1000 + position.z); // center the points a bit
        
        if (currentFrame - stretchStartFrame < stretchFramesCount) {
            // going towards the target position
            int xCurrentPosition = (stretchTargetPosition.x / stretchFramesCount) * (currentFrame - stretchStartFrame);
            int yCurrentPosition = (stretchTargetPosition.y / stretchFramesCount) * (currentFrame - stretchStartFrame);
            int zCurrentPosition = (stretchTargetPosition.z / stretchFramesCount) * (currentFrame - stretchStartFrame);
            auto currentPosition = glm::vec3(xCurrentPosition, yCurrentPosition, zCurrentPosition);
            auto image = getNextImage();
            image.setMode(ofPrimitiveMode::OF_PRIMITIVE_POINTS);
            shader.begin();
            shader.setUniform3f("position", currentPosition);
            image.drawVertices();
            shader.end();
        } else if (currentFrame - stretchStartFrame < stretchFramesCount * 2) {
            // going back to the start position
            int xCurrentPosition = stretchTargetPosition.x - ((stretchTargetPosition.x / stretchFramesCount) * (currentFrame - stretchStartFrame - stretchFramesCount));
            int yCurrentPosition = stretchTargetPosition.y - ((stretchTargetPosition.y / stretchFramesCount) * (currentFrame - stretchStartFrame - stretchFramesCount));
            int zCurrentPosition = stretchTargetPosition.z - ((stretchTargetPosition.z / stretchFramesCount) * (currentFrame - stretchStartFrame - stretchFramesCount));
            auto currentPosition = glm::vec3(xCurrentPosition, yCurrentPosition, zCurrentPosition);
            auto image = getNextImage();
            image.setMode(ofPrimitiveMode::OF_PRIMITIVE_POINTS);
            shader.begin();
            shader.setUniform3f("position", currentPosition);
            image.drawVertices();
            shader.end();
        } else if (drawStrips) {
            auto image = getNextImage();
            image.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLE_STRIP);
            image.drawWireframe();
        } else if (explosionStartFrame > 0) {
            explodingShader.begin();
            explodingShader.setUniform1f("time", (float) (currentFrame - explosionStartFrame));
            auto image = getCurrentImage();
            image.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLE_STRIP);
            image.drawVertices();
            explodingShader.end();
            if (currentFrame - explosionStartFrame > 100){
                explosionStartFrame = -1;
            }
        } else {
            getNextImage().drawVertices();
        }
        
        ofPopMatrix();
    }
    
    void setPosition(int x, int y, int z) {
        position = ofVec3f(x,y,z);
    }
    
    void move(int x, int y, int z) {
        position = ofVec3f(position.x + x, position.y + y, position.z + z);
    }
    
    void startStretch() {
        stretchStartFrame = currentFrame - 1;
        stretchTargetPosition = ofVec3f(ofRandom(100), ofRandom(100), ofRandom(100)).normalize() * stretchLineLength;
    }
    
    void toggleStrips(){
        drawStrips = !drawStrips;
    }
    
    void startExplosion() {
        explosionStartFrame = currentFrame - 1;
    }

    // crop
    int cropRight = 0;
    int cropLeft = 0;
    int cropUp = 0;
    int cropDown = 0;
    int cropNear = 0;
    int cropFar = 1000;
    
private:
    vector<ofMesh> frames;
    int currentFrame = 0;
    int currentPlayedFrame = 0;
    int kinectHeight;
    int kinectWidth;
    int intensityThreshold;
    ofImage texture;

    ofVec3f position = ofVec3f(0,0,0);
    
    // draw strips
    bool drawStrips = false;
    
    // moving lines
    ofShader shader;
    int stretchStartFrame = -200;
    ofVec3f stretchTargetPosition;
    ofVec3f stretchCurrentPosition;
    int stretchLineLength = 100;
    int stretchFramesCount = 20;
    
    // explosion
    ofShader explodingShader;
    int explosionStartFrame = -1;
    

};
