#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/params/Params.h"
#include "CinderFreenect.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class KinectTestApp : public AppNative
{
  public:
	void prepareSettings( Settings* settings );
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

  private:
	KinectRef		mKinect;
	gl::Texture		mVideoImageTexture, mDepthTexture;
    Channel		    mDepthChannel, mProcessChannel;

    // Threshold values.
	int mNearThreshold;
	int mFarThreshold;
    
    // Process input channel with lower and upper threshold.
    void processInputChannel( Channel const &input, Channel &output, int nearThreshold, int farThreshold );
    
    // Parameters for calibration.
    params::InterfaceGlRef mParams;
};

void KinectTestApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize( 1280, 480 );
}

void KinectTestApp::setup()
{
	console() << "There are " << Kinect::getNumDevices() << " Kinects connected." << std::endl;

    // Create kinect object.
	mKinect = Kinect::create();

    // Initialize process channel.
	mProcessChannel = Channel( 640, 480 );

	// Setup params widget.
	mParams = params::InterfaceGl::create( "Kinect Test", Vec2i( 240, 100 ) );
    mParams->addParam( "Near Threshold", &mNearThreshold, "min=0.0 max=255.0 step=1.0 keyIncr=w keyDecr=s" );
    mParams->addParam( "Far Threshold",  &mFarThreshold,  "min=0.0 max=255.0 step=1.0 keyIncr=w keyDecr=s" );
    
    // Initialize threshold values.
	mNearThreshold = 255;
	mFarThreshold  = 100;
}

void KinectTestApp::mouseDown( MouseEvent event )
{
}

void KinectTestApp::update()
{
	if ( mKinect->checkNewDepthFrame() )
    {
		mDepthTexture = mKinect->getDepthImage();

        // Get depth channel from depth texture.
        mDepthChannel = Channel( mKinect->getDepthImage() );
        
        // Process input channel.
        processInputChannel( mDepthChannel, mProcessChannel, mNearThreshold, mFarThreshold );
    }
    
	if (mKinect->checkNewVideoFrame() )
    {
        mVideoImageTexture = mKinect->getVideoImage();
    }
}

void KinectTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );

    // Show depth texture from kinect on the left.
	if ( mDepthTexture )
    {
		gl::draw( mDepthTexture );
    }

    // Show source video in the upper left
    if ( mVideoImageTexture )
    {
        gl::draw( mVideoImageTexture, Area( 10, 10, 170, 130 ) );
    }

    // Show processed channel on the right.
    if ( mProcessChannel )
    {
		gl::draw( mProcessChannel, Vec2i( 640, 0 ) );
    }

    // Draw parameters menu.
    mParams->draw();
}

void KinectTestApp::processInputChannel( Channel const &input, Channel &output, int nearThreshold, int farThreshold )
{
    Channel::ConstIter iterIn  = input.getIter();
    Channel::Iter      iterOut = output.getIter();
    
    while ( iterIn.line() && iterOut.line() )
    {
        while ( iterIn.pixel() && iterOut.pixel() )
        {
            int depth = iterIn.v();
            
            // Set to either white or black based on threshold.
            if ( farThreshold < depth && depth < nearThreshold )
            {
                iterOut.v() = 255;
            }
            else
            {
                iterOut.v() = 0;
            }
        }
    }
}


CINDER_APP_NATIVE( KinectTestApp, RendererGl )
