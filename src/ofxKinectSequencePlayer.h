#include "ofMain.h"


class ofxKinectSequencePlayer  {
public:
        
    void load(string prefix, string format, int height, int width, int numberWidth, string imagePath, int threshold = 50){
        image.load(imagePath);
        image.resize(width, height);
        intensityThreshold = threshold;
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

        shader.setGeometryInputType(GL_POINTS);
        shader.setGeometryOutputType(GL_LINE_STRIP);
        shader.setGeometryOutputCount(2);
        shader.load("shaders/kinectShaderVert.c", "shaders/kinectShaderFrag.c", "shaders/kinectShaderGeo.c");
    }
    
    ofMesh getImage(int frameNumber) {
        return frames.at(frameNumber % frames.size());
    }
    
    ofMesh getNextImage() {
        return frames.at(currentFrame++ % frames.size());
    }
    
    ofMesh getCurrentImage() {
        return frames.at(currentFrame % frames.size());
    }
    
    ofMesh convertToMesh(ofShortPixels& depthPixelsRaw){
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_POINTS );
        int step = 2;
        int index = 0;
        for(int y = 0; y < kinectHeight; y += step) {
            for(int x = 0; x < kinectWidth; x += step) {
                auto distance = depthPixelsRaw[y * kinectWidth + x];
                if(distance > 0 && distance < 1000) {
                    mesh.addVertex(ofPoint(x, y, distance));
                    auto spaceColor = image.getColor(x, y);
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
        glPointSize(0.5);
        ofPushMatrix();
        // the projected points are 'upside down' and 'backwards'
        ofScale(1, -1, -1);
        ofTranslate(-220 + position.x, -400 + position.y, -1000 + position.z); // center the points a bit
        float length = 0;
        if (currentFrame - moveStartFrame < moveFramesCount) {
            // going towards the target position
            int xCurrentPosition = (moveTargetPosition.x / moveFramesCount) * (currentFrame - moveStartFrame);
            int yCurrentPosition = (moveTargetPosition.y / moveFramesCount) * (currentFrame - moveStartFrame);
            int zCurrentPosition = (moveTargetPosition.z / moveFramesCount) * (currentFrame - moveStartFrame);
            auto currentPosition = glm::vec3(xCurrentPosition, yCurrentPosition, zCurrentPosition);

            shader.begin();
            shader.setUniform3f("position", currentPosition);
            getNextImage().drawVertices();
            shader.end();
        } else if (currentFrame - moveStartFrame < moveFramesCount * 2) {
            // going back to the start position
            int xCurrentPosition = moveTargetPosition.x - ((moveTargetPosition.x / moveFramesCount) * (currentFrame - moveStartFrame - moveFramesCount));
            int yCurrentPosition = moveTargetPosition.y - ((moveTargetPosition.y / moveFramesCount) * (currentFrame - moveStartFrame - moveFramesCount));
            int zCurrentPosition = moveTargetPosition.z - ((moveTargetPosition.z / moveFramesCount) * (currentFrame - moveStartFrame - moveFramesCount));
            auto currentPosition = glm::vec3(xCurrentPosition, yCurrentPosition, zCurrentPosition);

            shader.begin();
            shader.setUniform3f("position", currentPosition);
            getNextImage().drawVertices();
            shader.end();

        } else {
            getNextImage().drawVertices();
        }
        
        ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString( length));
        ofPopMatrix();
    }
    
    void setPosition(int x, int y, int z) {
        position = ofVec3f(x,y,z);
    }
    
    void move(int x, int y, int z) {
        position = ofVec3f(position.x + x, position.y + y, position.z + z);
    }
    
    void startMove() {
        moveStartFrame = currentFrame - 1;
        moveTargetPosition = ofVec3f(ofRandom(100), ofRandom(100), ofRandom(100)).normalize() * moveLineLength;
    }
    
private:
    vector<ofMesh> frames;
    int currentFrame = 0;
    int moveStartFrame = -200;
    ofVec3f moveTargetPosition;
    ofVec3f moveCurrentPosition;
    int moveLineLength = 100;
    int moveFramesCount = 20;
    int kinectHeight;
    int kinectWidth;
    int intensityThreshold;
    ofImage image;
    ofShader shader;
    ofVec3f position = ofVec3f(0,0,0);
};
