#include "testApp.h"


//--------------------------------------------------------------
void testApp::addNode(){
	
	/*
	 
	 This is where we construct our Track. 
	 
	 */
	
    ofNode * newNode = new nodeTie();
	
	/*
	 
		note that we create a new nodeTie on the heap and keep a pointer to ofNode. 
		We can do this because nodeTie is a derived class from ofNode. 
	 
		You could say: "nodeTie implements ofNodeÓ. I try to code against the base 
		class, rather than the implementation, it's more flexible in 
		the long run.
	 
	 */
	

	ofVec3f bias = ofVec3f(50 * cos(PI * nodeTrack.size()/42.f), 
						   20 * sin(PI * nodeTrack.size()/12.f), -50);
	
    if (!nodeTrack.empty()) newNode->setPosition(nodeTrack[nodeTrack.size()-1]->getPosition() + bias );

    newNode->roll(40 * sin(PI * nodeTrack.size()/12.f));

    if (!nodeTrack.empty()) nodeTrack[nodeTrack.size()-1]->lookAt(*newNode);

	// this will copy a new ofPtr of type ofNode 
    nodeTrack.push_back(ofPtr<ofNode>(newNode));

}

//--------------------------------------------------------------
void testApp::setup(){

    // ofSetFrameRate(60);
    ofSetVerticalSync(true);

	// this will build a track of 420 nodeTies.
	
    for (int i=0; i<420; i++){
        addNode();
    }

	// we set up our camera without vflipping, otherwise stuff would be upside down.
	// and select an fov (field of vision) of 45 degrees. 
	// The latter is a matter of preference, other fov's might look more dramatic!

    mainCam.setupPerspective(false, 45); // non-flipped, fov: 45
    
	// this is not really necessary, just a matter 
	mainCam.setPosition(0,0,200);

    currentTie = 0;
}


//--------------------------------------------------
ofVec3f doLerp(ofVec3f start, ofVec3f stop, float amt) {
	return start + (stop-start) * amt;
}


//--------------------------------------------------------------
void testApp::update(){

	// every 20 frames we jump to the next tie on the track
	
    if ( ofGetFrameNum() % 20 == 0 ) currentTie ++;

	// we begin our journey at tie 1, not tie 0
	
    if (currentTie == 0 ) currentTie = 1;

    if (nodeTrack.size() < currentTie + 2) {
        currentTie = 1;
        return;
    }

    // ----------| invariant: we have at least two nodeTies to work with.



	
	/*	We now want to smoothly animate from one node to the next. 
		
		in order to do this, we need to calculate the current relative position, 
		which we will store in float 't'. 't' shall hold a normalised value 
		between 0.0f and 1.0f. Think of it as a percentage value. 	 
	 

	 Awlright, calculate our relative position between two ties:
	
	 t = ( ofGetFrameNum() % 20 ) / 20.f; // will return a value between 0.0f and 0.95f
	 
		0.0f  means we are at tie 1
		0.25f means we have made 25% of the way to tie 2
		0.5f  means we are exactly halfway between tie 1 and tie 2, 
		0.75f means we have made 75% of the way, we are now closer to tie 2
		0.95f means we have almost reached tie 2
	 
	 */

    float t = ( ofGetFrameNum() % 20 ) / 20.f;

	/*	
		we calculate the current camera position by finding a global position 
		between the current nodeTie and the next nodeTie.
		float 't' tells us how 'far' our camera has moved down the way 
		from currentTie's global position to currentTie+1's global position.
		the function doLerp will calculate the new camera position for us, 
		according to the value in t.
	*/
	
    ofVec3f camPosition = doLerp(nodeTrack[ currentTie ]->getGlobalPosition(), nodeTrack[ currentTie + 1 ]->getGlobalPosition(), t);

	/*
	 
	 now we want to make sure that the camera looks in the right direction, 
	 and smoothly adjusts its gaze while moving along the track.

	 By the way we have chained up our track, we know that every nodeTie that 
	 has a next nodeTie 'looks' at its next nodeTie.
	 
	 We want out camera to look somewhere between where the last nodeTie and 
	 the next nodeTie looks at. How do we get there? By using lerp's rotational 
	 cousin, slerp. Slerp works with ofQuaternion, an elegant way to describe
	 a node's orientation in space. By using slerp with our float 't' we can 
	 'blend' between two orientations.
	 
	 */
    
	ofQuaternion tie1 = nodeTrack[ currentTie     ]->getGlobalOrientation();
    ofQuaternion tie2 = nodeTrack[ currentTie + 1 ]->getGlobalOrientation();

	
	ofQuaternion camOrient;	// camOrient is now default orientation

	// make camOrient an orientation interpolated between tie1 and tie2's orientation.
    camOrient.slerp(t, tie1, tie2);

	// place our camera at at the position we have calculated. Note that we use the global position, 
	// because the position we have calculated earlier is a global one.
    mainCam.setGlobalPosition(camPosition);

	// set our camera's orientation. Note that we use the global orientation, 
	// since we have calculated it based on our nodeTies global orientations.
	mainCam.setGlobalOrientation(camOrient);
	
	// now the fun part: lift our camera 30 units above track level.
	// note that this is a relative transform (within the mainCam's coordinate system) 
	// and will thus preserve the camera's previous orientation.
	mainCam.boom(30);

}

//--------------------------------------------------------------
void testApp::draw(){

    ofBackground(20);

	
    mainCam.begin();
	
	// everything that happens now is seen by our mainCam.
	
    ofEnableAlphaBlending();

	// we enable GL_DEPTH_TEST, so that things in the front occlude stuff in the back.
	glEnable(GL_DEPTH_TEST);

	// glFog is a cheap trick to suggest depht in openGL. 
	// it basically adds a bit of fogColor to every pixel. 
	// The further the pixel away the pixel is, the more 
	// its color is blended towards fogColor. Here I chose 
	// the a same value for fogColor as I had chosen 
	// previously for ofBackground, which will give the impression
	// of pixels disappearing in the fog.
    
	
	static GLfloat fogColor[4] = {20/255.f,20/255.f,20/255.f,0};
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.30);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 500);
	glFogf(GL_FOG_END, 4000);
	glEnable(GL_FOG);

	// this is where we draw our track. 
	// every node knows how to draw itself,
	// see testApp.h
	
    for ( int i = 0; i < nodeTrack.size(); i++ ){
        nodeTrack[i]->draw();
    }

    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);
    ofDisableAlphaBlending();

    mainCam.end(); // ends mainCam. everything that is drawn now would not be seen by our mainCam.
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	switch (key) {
		case 'f':
			ofToggleFullscreen();
			break;
			
		default:
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
