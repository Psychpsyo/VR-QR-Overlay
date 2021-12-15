#pragma once
#include <SDL.h>
#include <openvr.h>
#include <glew.h>
#include <openvr.h>

extern SDL_Event event;

extern bool isRunning;

extern std::string pathToExe;

extern vr::IVRSystem* vrSystem;

extern GLuint overlayTexture;
extern GLuint currentView;
extern vr::glSharedTextureHandle_t viewTexHandle;

extern vr::VROverlayHandle_t* overlayHandle;
extern vr::HmdMatrix34_t* overlayOffset;

extern std::string qrContent;

extern struct quirc* qrStruct;
extern int displayWidth, displayHeight, scaledWidth, scaledHeight;
extern int scaleFactor;

extern unsigned char* coloredRender;

extern vr::VRActionHandle_t* hapticL;
extern vr::VRActionHandle_t* hapticR;