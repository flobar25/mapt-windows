#include "ofMain.h"


class ofxKinectSequencePlayer  {
public:
  
    vector<ofMesh> frames;
    int currentFrame = 0;
    int kinectHeight;
    int kinectWidth;
    ofImage spaceImage;
    int intensityThreshold;
    ofShader shader;
      
    void load(string prefix, string format, int height, int width, int numberWidth, string imagePath, int threshold = 50){
        spaceImage.load(imagePath);
        spaceImage.resize(width, height);
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
                    auto spaceColor = spaceImage.getColor(x, y);
                    if (spaceColor.getLightness() > intensityThreshold) {
                        mesh.addColor(spaceColor);
                    } else {
                        mesh.addColor(ofColor(255));
                    }
                    
                }
            }
        }
        
//        int numVerts = mesh.getNumVertices();
//        for (int a=0; a<numVerts; ++a) {
//            ofVec3f verta = mesh.getVertex(a);
//            for (int b=a+1; b<numVerts; ++b) {
//                ofVec3f vertb = mesh.getVertex(b);
//                float distance = verta.distance(vertb);
//                if (distance <= connectionDistance) {
//                    mesh.addIndex(a);
//                    mesh.addIndex(b);
//                }
//            }
//        }
//
        
        return mesh;
    }
    
    void draw() {
        glPointSize(1);
        ofPushMatrix();
        // the projected points are 'upside down' and 'backwards'
        ofScale(1, -1, -1);
        ofTranslate(0, -500, -1000); // center the points a bit
        shader.begin();
        getNextImage().drawVertices();
        shader.end();
        ofPopMatrix();
    }
};
