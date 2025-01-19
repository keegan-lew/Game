/**
 * @file AdventureTycoon.c
 * @date 18-Jan-2025
 */

#include "orx.h"
#include "orxExtensions.h"

orxOBJECT *player;
orxCAMERA *camera;
orxOBJECT *scene;

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement        = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

/** Update function, it has been registered to be called every tick of the core clock
 */
void orxFASTCALL Update(const orxCLOCK_INFO *_pstClockInfo, void *_pContext)
{
  // Should quit?
  if(orxInput_HasBeenActivated("Quit"))
  {
    // Send close event
    orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
  }

  orxVECTOR leftSpeed = { -30, 0, 0 };
  orxVECTOR rightSpeed = { 30, 0, 0 };
  orxVECTOR upSpeed = { 0, -30, 0 };
  orxVECTOR downSpeed = { 0, 30, 0 };
  
  if (orxInput_IsActive("Left"))
  {
    orxObject_ApplyImpulse(player, &leftSpeed, orxNULL);
  }
  if (orxInput_IsActive("Right"))
  {
    orxObject_ApplyImpulse(player, &rightSpeed, orxNULL);
  }
  if (orxInput_IsActive("Forward"))
  {
    orxObject_ApplyImpulse(player, &upSpeed, orxNULL);
  }
  if (orxInput_IsActive("Down"))
  {
    orxObject_ApplyImpulse(player, &downSpeed, orxNULL);
  }
}

void orxFASTCALL UpdateCamera(const orxCLOCK_INFO *_pstClockInfo, void *_pContext) {
    // Get the player position
    orxVECTOR player_pos;
    orxObject_GetPosition(player, &player_pos);

    // Get the current camera position
    orxVECTOR camera_pos;
    orxCamera_GetPosition(camera, &camera_pos);

    // Smoothly interpolate between the camera position and the player position
    float lerp_speed = 5.0f;  // Adjust this value for smoother or faster transitions
    orxVECTOR delta;
    orxVector_Sub(&delta, &player_pos, &camera_pos);
    orxVector_Mulf(&delta, &delta, lerp_speed * _pstClockInfo->fDT);  // fDT is the delta time for frame rate independence

    // Add the movement to the current camera position
    orxVector_Add(&camera_pos, &camera_pos, &delta);

    // Update the camera position
    orxCamera_SetPosition(camera, &camera_pos);
}


/** Init function, it is called when all orx's modules have been initialized
 */
orxSTATUS orxFASTCALL Init()
{
  // Init extensions
  InitExtensions();

  // Register the Update function to the core clock
  orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), Update, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);
  orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), UpdateCamera, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);

  // Push [Main] as the default section
  //orxConfig_PushSection("Main");

  // Create the viewports
  orxViewport_CreateFromConfig("MainViewport");

  player = orxObject_CreateFromConfig("PlayerObject");
  camera = orxCamera_CreateFromConfig("Camera");
  scene = orxObject_CreateFromConfig("BackgroundObject");
  


  // Done!
  return orxSTATUS_SUCCESS;
}

/** Run function, it should not contain any game logic
 */
orxSTATUS orxFASTCALL Run()
{
  // Return orxSTATUS_FAILURE to instruct orx to quit
  return orxSTATUS_SUCCESS;
}

/** Exit function, it is called before exiting from orx
 */
void orxFASTCALL Exit()
{
  // Exit from extensions
  ExitExtensions();

  // Let orx clean all our mess automatically. :)
}

/** Bootstrap function, it is called before config is initialized, allowing for early resource storage definitions
 */
orxSTATUS orxFASTCALL Bootstrap()
{
  // Bootstrap extensions
  BootstrapExtensions();

  // Return orxSTATUS_FAILURE to prevent orx from loading the default config file
  return orxSTATUS_SUCCESS;
}

/** Main function
 */
int main(int argc, char **argv)
{
  // Set the bootstrap function to provide at least one resource storage before loading any config files
  orxConfig_SetBootstrap(Bootstrap);

  // Execute our game
  orx_Execute(argc, argv, Init, Run, Exit);

  // Done!
  return EXIT_SUCCESS;
}
