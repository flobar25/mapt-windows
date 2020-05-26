#include "ofMain.h"


class ofxKinectSequencePlayer  {
public:
        
    void load(string prefix, string format, int height, int width, int numberWidth, string imagePath){
        image.load(imagePath);
        image.resize(width, height);
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
        
        stripsShader.setGeometryInputType(GL_POINTS);
        stripsShader.setGeometryOutputType(GL_LINE_STRIP);
        stripsShader.setGeometryOutputCount(3);
        stripsShader.load("shaders/kinectStripsShaderVert.c", "shaders/kinectStripsShaderFrag.c", "shaders/kinectStripsShaderGeo.c");

        explodingShader.setGeometryInputType(GL_TRIANGLES);
        explodingShader.setGeometryOutputType(GL_TRIANGLE_STRIP);
        explodingShader.setGeometryOutputCount(3);
        explodingShader.load("shaders/explodingShaderVert.c", "shaders/explodingShaderFrag.c", "shaders/explodingShaderGeo.c");
        
    }
    
    ofMesh getImage(int frameNumber) {
        return frames.at(frameNumber % frames.size());
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
        int index = 0;
        for(int y = cropUp; y < kinectHeight - cropDown; y += step) {
            for(int x = cropLeft; x < kinectWidth - cropRight; x += step) {
                auto distance = depthPixelsRaw[y * kinectWidth + x];
                auto distance2 = depthPixelsRaw[(y+step) * kinectWidth + x];
                if(distance > cropNear && distance < cropFar && distance2 > cropNear && distance2 < cropFar) {
                    mesh.addVertex(ofPoint(x, y, distance));
                    mesh.addVertex(ofPoint(x, y+step, distance2));
                    
//                    auto spaceColor = image.getColor(x, y);
//                    if (spaceColor.getLightness() > intensityThreshold) {
//                        tempMesh.addColor(spaceColor);
//                    } else {
//                        tempMesh.addColor(ofColor(255));
//                    }
                }
            }
        }

        
        
//        ofMesh mesh;
//        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
//        for (int i = 0; i < points.size(); i++) {
//            for (int j = i; j < points.size(); j++){
//
//                    mesh.addVertex(triangle.triangles[i].a);
//                    mesh.addColor(ofColor(100));
//                    mesh.addVertex(triangle.triangles[i].b);
//                    mesh.addColor(ofColor(100));
//                    mesh.addVertex(triangle.triangles[i].c);
//                    mesh.addColor(ofColor(100));
//            }
//        }
        return mesh;
    }
    
    
    
    void draw() {
        currentFrame++;
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

        } else if (drawStrips) {
            stripsShader.begin();
//            auto image = getNextImage();
//            image.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINE_STRIP);
            getNextImage().drawWireframe();
            stripsShader.end();
        } else if (explosionStartFrame > 0) {
            explodingShader.begin();
            explodingShader.setUniform1f("time", (float) (currentFrame - explosionStartFrame));
            auto nextImage = getCurrentImage();
            nextImage.drawVertices();
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
    
    void startMove() {
        moveStartFrame = currentFrame - 1;
        moveTargetPosition = ofVec3f(ofRandom(100), ofRandom(100), ofRandom(100)).normalize() * moveLineLength;
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
    ofImage image;

    ofShader stripsShader;
    ofVec3f position = ofVec3f(0,0,0);
    bool drawStrips = false;
    
    // moving lines
    ofShader shader;
    int moveStartFrame = -200;
    ofVec3f moveTargetPosition;
    ofVec3f moveCurrentPosition;
    int moveLineLength = 100;
    int moveFramesCount = 20;
    
    // explosion
    ofShader explodingShader;
    int explosionStartFrame = -1;
    

};
