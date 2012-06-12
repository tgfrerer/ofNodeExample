#pragma once

#include "ofMain.h"

// nodeTie extends ofNode, so it can do everything ofNode can do.

class nodeTie : public ofNode {
    void customDraw(){
		
		// this is where a nodeTie draws itself.
		
		// this method implements the abstract 'virtual void customDraw()' inherited from ofNode. 
		// (have a look at ofNode's standard implementation of customDraw() in ofNode.h to learn more.)
		
		// the draw operation happens with the nodeTie's coordinate system, 
		// which nodeTie inherits from its ofNode parent.

		// so ofVec3(0,0,0) will be at the centre of the current nodeTie.
		
		ofSetColor(255);
		ofLine(-40,0,40,0);
		ofSetColor(255, 0, 0);
		ofLine(-40,0,-25, -40,0,25);
		ofSetColor(0,255,0);
		ofLine(40,0,-25, 40,0,25);
		ofSetColor(255);		
    };
};


class testApp : public ofBaseApp{

    vector<ofPtr<ofNode> >  nodeTrack;		// this will hold all our nodeTies and keep them in the right order.
    int						currentTie;		// needed for camera movement, this will tell us where the camera currently is.
	
    ofCamera mainCam;

	public:

        void addNode();

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

};
