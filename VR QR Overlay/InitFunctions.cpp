#include "Variables.h"
#include <iostream>
#include <quirc.h>

#define _USE_MATH_DEFINES
#include <math.h>

//performs openVR initializations, creates the VR overlay and glues it to the HMD
void setupOpenVR() {
	//initializing openvr
	vr::HmdError* initError = new vr::HmdError();
	vrSystem = vr::VR_Init(initError, vr::VRApplication_Overlay);

	if (*initError != vr::VRInitError_None) {
		std::cout << "OpenVR initialization error: " << std::endl << vr::VR_GetVRInitErrorAsEnglishDescription(*initError) << std::endl;
		system("pause");
		exit(1);
	} else {
		std::cout << "OpenVR initialization successful" << std::endl;
	}


	//creating overlay and setting its width
	overlayHandle = new vr::VROverlayHandle_t;
	vr::VROverlay()->CreateOverlay("qrScannerOverlay", "QR Scan Result", overlayHandle);

	vr::VROverlay()->SetOverlayWidthInMeters(*overlayHandle, .2);

	//set overlay offset from the controller it's attached to
	overlayOffset->m[0][0] = 1;
	overlayOffset->m[1][1] = cos(-90 * M_PI / 180);
	overlayOffset->m[1][2] = -sin(-90 * M_PI / 180);
	overlayOffset->m[2][1] = sin(-90 * M_PI / 180);
	overlayOffset->m[2][2] = cos(-90 * M_PI / 180);
	overlayOffset->m[2][3] = -.075;
}

//performs openGL initialization via SDL and GLEW and sets up some necessary parameters.
//Also creates the SDL window in the process.
void setupOpenGL() {
	//initialize SDL
	SDL_Init(SDL_INIT_VIDEO);

	//set up SDL window and openGl context
	SDL_Window* window = SDL_CreateWindow("VRQR reader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_OPENGL);
	SDL_GLContext Context = SDL_GL_CreateContext(window);

	//initialize GLEW

	glewInit();

	//setting openGL parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//creates the openGL textures for the overlay and to store the rendered image from the HMD.
//Also gets the texture handle for the latter.
void setupTextures() {
	//setting up the overlay texture and current view texture
	glTexImage2D(overlayTexture, 0, GL_RGB, 500, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
}

//performs setup for VRInput so that controller rumble can be accessed
void setupVRInput() {
	vr::EVRInputError error;

	error = vr::VRInput()->SetActionManifestPath((pathToExe + "controls\\actionManifest.json").c_str());
	
	if (error != vr::VRInputError_None) {
		std::cout << "OpenVR initialization error: " << error << std::endl;
		system("pause");
		exit(1);
	} else {
		std::cout << "VRInput initialization successful" << std::endl;
	}

	//get action handles
	hapticL = new vr::VRActionHandle_t();
	hapticR = new vr::VRActionHandle_t();
	vr::VRInput()->GetActionHandle("/actions/main/out/hapticL", hapticL);
	vr::VRInput()->GetActionHandle("/actions/main/out/hapticR", hapticR);
}

//sets up the QR library, creating its struct.
//Also allocated memory for other elements, related to decoding to QR code.
void setupQuirc() {
	//get display from the HMD and read out width and height
	vr::VRCompositor()->GetMirrorTextureGL(vr::Eye_Left, &currentView, &viewTexHandle);
	glGetTextureLevelParameteriv(currentView, 0, GL_TEXTURE_WIDTH, &displayWidth);
	glGetTextureLevelParameteriv(currentView, 0, GL_TEXTURE_HEIGHT, &displayHeight);
	vr::VRCompositor()->ReleaseSharedGLTexture(currentView, viewTexHandle);

	//determine scale factor and calculate scaled width and height
	scaleFactor = displayWidth / 500;
	std::cout << "Applying scale factor 4 before QR code recognition." << std::endl;

	scaledWidth = displayWidth / scaleFactor;
	scaledHeight = displayHeight / scaleFactor;

	//create the quirc struct and size it to match the HMD resolution
	qrStruct = quirc_new();
	quirc_resize(qrStruct, scaledWidth, scaledHeight);


	//allocate memory for the array that holds what has been rendered to the HMD
	coloredRender = new unsigned char[displayWidth * displayHeight * 3];
}