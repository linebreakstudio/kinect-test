#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class KinectTestApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void KinectTestApp::setup()
{
}

void KinectTestApp::mouseDown( MouseEvent event )
{
}

void KinectTestApp::update()
{
}

void KinectTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( KinectTestApp, RendererGl )
