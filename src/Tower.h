#include "ofMain.h"


class Tower  {
public:
    
    void setup(ofPoint position, int floorCount, int floorHeight, int floorLength, int floorWidth) {
        this->floorCount = floorCount;
        this->floorHeight = floorHeight;
        this->floorLength = floorLength;
        this->floorWidth = floorWidth;
        this->position = position;
        
        shader.setGeometryInputType(GL_LINES);
        shader.setGeometryOutputType(GL_LINE_STRIP);
        shader.setGeometryOutputCount(floorCount);
        shader.load("shaders/multiplyShaderVert.c", "shaders/multiplyShaderFrag.c", "shaders/multiplyShaderGeo.c");
        
        mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
        mesh.addVertex(position);
        mesh.addColor(ofColor(200));
        mesh.addVertex(ofPoint(position.x + floorWidth, position.y, position.z));
        mesh.addColor(ofColor(200));
        mesh.addVertex(ofPoint(position.x + floorWidth, position.y, position.z + floorLength));
        mesh.addColor(ofColor(200));
        mesh.addVertex(ofPoint(position.x, position.y, position.z + floorLength));
        mesh.addColor(ofColor(200));
        mesh.addIndex(0);
        mesh.addIndex(1);
        mesh.addIndex(1);
        mesh.addIndex(2);
        mesh.addIndex(2);
        mesh.addIndex(3);
        mesh.addIndex(3);
        mesh.addIndex(0);

    }
    
    void draw() {
        ofSetLineWidth(2);
        shader.begin();
        shader.setUniform1i("count", floorCount);
        shader.setUniform1i("height", floorHeight);
        mesh.draw();
        shader.end();
    }
    
private:
    int floorCount;
    int floorHeight;
    int floorLength;
    int floorWidth;
    ofShader shader;
    ofPoint position;
    ofMesh mesh;
    
};
