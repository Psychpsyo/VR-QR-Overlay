#include <SDL.h>
#include <openvr.h>
#include <glew.h>

SDL_Event event; //SDL window events are read into this

bool isRunning = true;

std::string pathToExe; //path to this program

vr::IVRSystem* vrSystem;

GLuint overlayTexture; //openGL texture for the overlay
GLuint currentView; //openGL textures to store what is being displayed in the headset
vr::glSharedTextureHandle_t viewTexHandle; //texture handle of the currentView texture, ready for openVR

vr::VROverlayHandle_t* overlayHandle; //handle for the overlay
vr::HmdMatrix34_t* overlayOffset = new vr::HmdMatrix34_t(); //offset for the overlay. (filled in setupOpenVR())

std::string qrContent = ""; //content of the qrCode (if one is on screen, otherwise empty string)

struct quirc* qrStruct; //struct for the quirc QR library
int displayWidth, displayHeight, scaledWidth, scaledHeight; //width and height of the left eye texture from the HMD (and versions with scaleFactor applied)
int scaleFactor; //by what factor should the image be downscaled before trying to detect the QR code?

unsigned char* coloredRender; //The HMDs content as an array.

//openVR action handles for haptic feedback
vr::VRActionHandle_t* hapticL;
vr::VRActionHandle_t* hapticR;