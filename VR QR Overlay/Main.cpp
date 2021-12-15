#include <iostream>
#include <openvr.h>
#include <SDL.h>
#include <windows.h>
#include <glew.h>
#include <string>
#include <quirc.h>

#include "InitFunctions.h"
#include "Variables.h"

void detectQR(); //tries to find and read a QR code, stores it's content in qrContent
void qrFound(); //gets called when a QR code has been found and adds the code to the overlay

int main(int argc, char** argv) {
	if (argc > 0 && argv[0] != "\0") {
		pathToExe = argv[0];
		pathToExe = pathToExe.substr(0, pathToExe.find_last_of('\\') + 1);
		std::cout << "Application path set to \"" << pathToExe << "\"" << std::endl;
	} else {
		std::cout << "Application path not available." << std::endl;
		system("pause");
	}

	//setup functions (see InitFunctions.cpp)
	setupOpenGL();
	setupOpenVR();
	setupTextures();
	setupVRInput();
	setupQuirc();

	//main loop
	while (isRunning) {
		//handle SDL window events
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: { //quit if told so
					isRunning = false;
					break;
				}
			}
		}

		//check whether a qr code has already been found
		if (qrContent == "") {
			//none found, scan for one
			detectQR();
		} else {
			
			//process inputs on the overlay
		}
	}


	quirc_destroy(qrStruct);
	vr::VROverlay()->DestroyOverlay(*overlayHandle);
	vr::VR_Shutdown();
	return 0;
}

void detectQR() {
	//access currently rendered openGL texture (save it in currentView)
	vr::VRCompositor()->GetMirrorTextureGL(vr::Eye_Left, &currentView, &viewTexHandle);

	//save currentView to array and delete it
	glGetTextureImage(currentView, 0, GL_RGB, GL_UNSIGNED_BYTE, displayHeight * displayWidth * 3, coloredRender);
	vr::VRCompositor()->ReleaseSharedGLTexture(currentView, viewTexHandle);

	//convert it to greyscale and pass it to the QR library
	uint8_t* greyscaleRender = quirc_begin(qrStruct, nullptr, nullptr);

	int currentPixel;
	for (int i = 0; i < scaledHeight; i++) {
		for (int j = 0; j < scaledWidth; j++) {
			currentPixel = (j + i * displayWidth) * 3 * scaleFactor;
			greyscaleRender[i * scaledWidth + j] = (((int) coloredRender[currentPixel]) + coloredRender[currentPixel + 1] + coloredRender[currentPixel + 2]) / 3;
		}
	}

	quirc_end(qrStruct);


	//decoding the QR code, if there is one
	if (quirc_count(qrStruct)) {
		struct quirc_code qrCode;
		struct quirc_data data;
		quirc_decode_error_t err;

		quirc_extract(qrStruct, 0, &qrCode);

		err = quirc_decode(&qrCode, &data);

		//if decoding was successful, save data in qrContent
		if (!err) {
			qrContent = (char*) data.payload;
			qrFound();
		}
	}
}

void qrFound() {
	std::cout << qrContent << std::endl;

	//drawing content to overlay
	//vr::Texture_t vrOverlayTex = {(void*) (uintptr_t) overlayTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
	//vr::VROverlay()->SetOverlayTexture(*overlayHandle, &vrOverlayTex);

	//positioning and showing the overlay
	//glue overlay to the controller
	vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(*overlayHandle, vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand), overlayOffset);

	//show the overlay
	vr::VROverlay()->SetOverlayFromFile(*overlayHandle, "D:/Files/Project Files/Hueh/viveHueh.png"); //temporary viveHueh
	vr::VROverlay()->ShowOverlay(*overlayHandle);

	//do vibrations
	vr::VRInput()->TriggerHapticVibrationAction(*hapticR, 0, .2, 1, 1, vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand));
}