// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#ifdef KOMPUTER_SWIAT_VERSION
#include "StringsPolish.h"
#else

// These are text strings used in the game

#define LEVEL_0 L"<Start Transmission>\n\nIn the name of the Emperor you have been integrated with the XZ-75fh Imperial Fighter/Hunter ship.\n\nThe XZ-75fh is extremely easily mastered, has an outstanding life support system and it is equipable with a wide variety of weaponry and special abilities.\n\n As you fulfill the word of the Emperor you can be granted the privileges to use this extra functionality, extended life support can also be granted.\n\n\n\nOur peaceful settlements in the omega sector have been wiped out, you must eradicate the intruders.\n\nIn the name of the Emperor you shall be victorious against the heretics.\n\n\n\nBe Pure, be Clean!\n\n<End Transmission>"
#define LEVEL_1	L"<Start Transmission>\n\nYou have proven your worth to the Emperor and you are indeed a skilled pilot.\n\nImperial intelligence have located a concentration of enemies in this area. They pose a threat to the Imperial supply lines to the outer systems.\n The supplies must reach their destination or the consequences will be dear.\n\nThe heretics must be destroyed at any cost!\n\nBe Pure, be Clean!\n\n<End Transmission>"
#define LEVEL_2	L"<Start Transmission>\n\nThe Emperor has lead our brave forces to another glorious victory!\n\nThe outer systems are now safe and our struggle against the heretics can continue.\n\nTraitors to the Empire have revealed a weakness in our defences to the aliens.\n\nThis sector is under heavy attack!\n\nYou must defend it!\n\nBe Pure, be Clean!\n\n<End Transmission>"
#define LEVEL_3	L"<Start Transmission>\n\nYou have proven your worth once again, brave pilot!\nYou are sure to be remembered in the eternal history of the Empire.\n\nYour efforts have led to an all new campaign against the alien heretics.\n\nYour next mission is to seek and destroy as many heretics as possible,\nto evaluate the strength of the alien forces in this sector.\n\nBe Pure, be Clean!\n\n<End Transmission>"
#define LEVEL_4	L"<Start Transmission>\n\nAlert, Alert!\nThis is the Imperial transmission beacon XXZ-4417.\n Alien precense higher than anticipated, transmission beacons are being overrun by traitors.\n\nYou must continue the fight on you own! Do not trus\n\nbzzzzzxxxx\n\nissions! The fight must\n\nxxzzzzzzbbbzzzzhh\n\ninue. \n\nBe Pure, be\n\nxxxzzzzkkkkkrhhhhhhhhhhzz \n\nnsmission>"
#define LEVEL_5	L"gzzzzzfxxx Transmission>\n\n Heretic for\n\nfrrrrsssssccc\n\n are threatening the Empire home world.\n You must stri\nbzzzzzxxxx\nke at the core of the Alien infestation in one massive blow.\n\nYou must conti\nbzzzzzxxxx\n[Why are you our Enemy?]\nbzzzzzxxxx\nnnue the fight on you own! \ngzzzfdzzzx\n[Lift the veil and see the Truth]\nkkkrrzzzzzzzh\nBe Pure, be\nxxxzzzzkkkkkrhhhhhhhhhhzz\nnsmission>"
#define LEVEL_6	L"kkkkkzzzzzzzzbh\n\n\nIn the name of the Emperor\n\nStrike first, strike hard show no mercy!\n\nxxxzzzzkkkkkrhhhhhhhhhhzz"
#define LEVEL_7	L"[You must see the truth! This is not the way.\n\nIt is all lies!]"
#define LEVEL_8	L"[Your are approaching your final destiny.\n\nBut you are here in vein, we wish you no harm.]\n\n"
#define LEVEL_9	L"[You are indeed a ferocious fighter.\nBut your struggle is futile, we control both space and time.\n\nThere is another way.\n\nWe want our planet back...\n\nWe must have our planet back. This is no alien attack.]"

#define ABILITY_RAPID_FIRE_HEADLINE 	L"Rapid Fire Enabled"
#define ABILITY_REGENERATION_HEADLINE	L"Regeneration Enabled"
#define ABILITY_TIME_WARP_HEADLINE		L"Time Warp Enabled"
#define ABILITY_MISSILES_HEADLINE		L"Missiles Enabled"

// input mappings
// %kf : keyboard fire
// %ke : keyboard use energy
// %k1 : keyboard activate rapid fire
// %k2 : keyboard activate regeneration
// %k3 : keyboard activate time warp
// %k4 : keyboard activate missiles

// %pf : keyboard fire
// %pe : keyboard use energy
// %p1 : keyboard activate rapid fire
// %p2 : keyboard activate regeneration
// %p3 : keyboard activate time warp
// %p4 : keyboard activate missiles

#define ABILITY_RAPID_FIRE 	L"The Rapid Fire ability has been enabled.\n\nYou activate it by pressing the energy activation key (secondary mousebutton, %ke%pe)\n and the fire key (primary mouse button, %kf%pf).\n\n Rapid Fire increases your rate of fire."
#define ABILITY_REGENERATION L"The Regeneration ability has been enabled.\n\nYou activate it by pressing the energy activation key (secondary mousebutton, %ke%pe).\n\nYou switch active ability with the mouse scroll wheel.\n Use %k1%p1 to switch to Rapid Fire and %k2%p2 to switch to Regeneration.\n\nRegeneration replenishes your health."
#define ABILITY_TIME_WARP L"The Time Warp ability has been enabled.\n\nYou activate it by pressing the energy activation key (secondary mousebutton, %ke%pe).\n\nYou switch active ability with the mouse scroll wheel.\n The %k3%p3 button can be used to quickly select the Time Warp ability.\n\nTime Warp slows down the movement of any enemies and all bullets, however the XZ-75fh moves at full speed."
#define ABILITY_MISSILES L"The Missiles ability has been enabled.\n\nYou activate it by pressing the energy activation key (secondary mousebutton, %ke%pe).\n\nYou switch active ability with the mouse scroll wheel.\n The %k4%p4 button can be used to quickly select the Missiles ability.\n\nMissiles converts all your stored energy into homing missiles taking our most visible enemies."

// Menu strings

// Old version
#define OLD_TEXT L"This is an OLD VERSION of the game.\nYou should get the newest version available at\n\nwww.spellofplay.com/games/twtpb"
#define OLD_GET L"Get The Newest Version"

// Highscores
#define HIGSCORE_HEADLINE L"In Rememberance"
#define HIGSCORE_PREV L"Previous"
#define HIGSCORE_NEXT L"Next"

//Main Menu
#define STATUS_NOSERVER L"Could not Connect to Server, check your Internet and Firewall settings!\n\n"
#define STATUS_DOWNLOADING L"Downloading highscores...\n\n"
#define STATUS_UPLOADING L"Uploading highscore...\n\n"
#define STATUS_REGISTERED L"You are a Registered Believer!\n\n"
#define STATUS_COPYRIGHT L"Copyright (C)2007,2008, 2009 Spell of Play Studios\n\nwww.spellofplay.com"

#define CONTINUE L"Continue the Cleansing"
#define START_GAME L"Start the Cleansing"
#define SHOW_HIGHSCORES L"In Rememberance"
#define SHOW_OPTIONS L"Options"
#define SHOW_CREDITS L"Credits"
#define QUIT_CURRENT_GAME L"Surrender this Battle"
#define EXIT_GAME L"Surrender to the Heretics"

// Jumpgates
#define JUMPGATES_HEADLINE L"Imperial Jumpgates"
#define JUMPGATES_HELPTEXT1 L"Select the gate you want to use."
#define JUMPGATES_HELPTEXT2 L"Activate gates by completing levels."
#define JUMPGATES_HELPTEXT2_NOT_REGISTERED L"Activate gates by completing levels.\n\nRegistered Believers can activate all gates. You are limited to the four first gates."
#define JUMPGATES_GATE L"Gate %i"

// Legions
#define LEGIO_HEADLINE L"Imperial Legions"
#define LEGIO_HELPTEXT L"Join an Imperial Legion based on your level of experience."

#define LEGIO_EASY_HEADLINE L"Legio Hastati"	// not for translation
#define LEGIO_EASY_JOIN L"Join Legio Hastati"
#define LEGIO_EASY L"Inexperienced young pilots that have just started their service in the Imperial forces.\n\nThe mission of Legio Hastati is to clean up areas of lighter heretic infestation.Their service to the Emperor will not be as valued."

#define LEGIO_MEDIUM_HEADLINE L"Legio Principes"	// not for translation
#define LEGIO_MEDIUM_JOIN L"Join Legio Principes"
#define LEGIO_MEDIUM L"The battle hardened core of the Imperial forces.\n\nLegio Principes are sent to severely infested areas where the alien presence is strong. Their contribution is highly valued by the Emperor."

#define LEGIO_HARD_HEADLINE L"Legio Triari"	// not for translation
#define LEGIO_HARD_JOIN L"Join Legio Triari"
#define LEGIO_HARD_JOIN_NOT_REGISTERED L"Only For True Believers"
#define LEGIO_HARD L"The Imperial veteran forces. The high guard of the elite forces of the Emperor, highly skilled and loyal to His word only.\n\nThe veterans are sent where no other legion can survive, in return the Emperor values their saccrifice the highest."
#define LEGIO_HARD_NOT_REGISTERED L"\n\n Only registered users can join the elite forces."

// Options
#define OPTIONS_HEADLINE L"Options"
#define OPTIONS_SHOW_GRAPHICS L"Graphics"
#define OPTIONS_SHOW_AUDIO L"Audio"
#define OPTIONS_SHOW_CONTROLS L"Controls"

// Controls
#define CONTROLS_HEADLINE L"Controls"
#define CONTROLS_SHOW_MOUSE L"Mouse"
#define CONTROLS_SHOW_KEYBOARD L"Keyboard"
#define CONTROLS_SHOW_GAMEPAD L"Gamepad/Joystick"

// Mouse
#define MOUSE_HEADLINE L"Mouse Settings"
#define MOUSE_SENSITIVITY L"Sensitivity: %.2f"
#define MOUSE_RESET L"Reset to Defaults"

// Keyboard
#define KEYBOARD_HEADLINE L"Keyboard Settings"
#define KEYBOARD_MOVE_LEFT L"Move Left: "
#define KEYBOARD_MOVE_RIGHT L"Move Right: "
#define KEYBOARD_MOVE_UP L"Move Up: "
#define KEYBOARD_MOVE_DOWN L"Move Down: "
#define KEYBOARD_PREV_ABILITY L"Previous Ability: "
#define KEYBOARD_NEXT_ABILITY L"Next Ability: "
#define KEYBOARD_FIRE L"Fire: "
#define KEYBOARD_USE_ABILITY L"Use Ability: "
#define KEYBOARD_SET_RAPIDFIRE L"Rapid Fire: "
#define KEYBOARD_SET_REGENERATION L"Regeneration: "
#define KEYBOARD_SET_TIMEWARP L"Time Warp: "
#define KEYBOARD_SET_MISSILES L"Missiles: "
#define KEYBOARD_SENSITIVITY L"Sensitivity: %.2f"
#define KEYBOARD_RESET L"Reset to Defaults"

// Controller
#define CONTROLLER_HEADLINE L"Controller Settings"
#define CONTROLLER_MOVE_LEFT_RIGHT L"Move Left-Right: "
#define CONTROLLER_MOVE_UP_DOWN L"Move Up-Down: "
#define CONTROLLER_FIRE L"Fire: "
#define CONTROLLER_USE_ABILITY L"Use Ability: "
#define CONTROLLER_SET_RAPIDFIRE L"Rapid Fire: "
#define CONTROLLER_SET_REGENERATION L"Regeneration: "
#define CONTROLLER_SET_TIMEWARP L"Time Warp: "
#define CONTROLLER_SET_MISSILES L"Missiles: "
#define CONTROLLER_SENSITIVITY L"Sensitivity: %.2f"
#define CONTROLLER_RESET L"Reset to Defaults"

// Audio
#define AUDIO_HEADLINE L"Audio Options"
#define AUDIO_MUSIC_VOLUME L"Music Volume: %.0f"
#define AUDIO_SOUND_VOULME L"Sound Volume: %.0f"

// Graphics
#define GRAPHICS_HEADLINE L"Graphic Options"
#define GRAPHICS_SHOW_RESOLUTION L"Resolution"
#define GRAPHICS_SHOW_MODE L"Window Mode"

// Resolution
#define RESOLUTION_HEADLINE L"Select Resolution"
#define RESOLUTION_HIGHER L"Higher"
#define RESOLUTION_LOWER L"Lower"

// Mode
#define MODE_HEADLINE L"Select Window Mode"
#define MODE_FULLSCREEN L"Fullscreen"
#define MODE_WINDOWED L"Windowed"

// misc
#define MENU_BUY L"Buy Full Version"
#define MENU_BACK L"Back"

// ingame strings

#define MESSAGE_HEADLINE L"Imperial Communiqué"

#define DEMO_HEADLINE L"End of Demo"
#define DEMO_TEXT L"This is as far as you can go without becoming a true believer.\n\nGet your Imperial Registration code and enjoy these benefits:\n\n\nAll 10 Frantic action packed levels\n\n10 Unique bosses to beat\n\nAccess to the full Imperial jump gate array\n\n\n\nFree upgrades and updates\n\nSupport independent game development!"
#define DEMO_NO_THANKS_TIME L"No I'm Not Worthy (%i)"
#define DEMO_NO_THANKS L"No I'm Not Worthy"

#define HIGHSCORE L"You are worthy to be remembered as one of the Emperor's finest.\n\nEnter your name (esc to cancel):%s"
#define HIGHSCORE_NOT_REGISTERED L"You are worthy to be remembered as one of the Emperor's finest.\n\nThe Emperor Grants Extended Life Support to Registered Believers\n\nEnter your name (esc to cancel):%s"

#define GAME_OVER L"Game Over!\n\n Score: %i\n\nPress Fire to continue"
#define GAME_OVER_NOT_REGISTERED L"Game Over!\n\nThe Emperor Grants Extended Life Support to Registered Believers\n\nScore: %i\n\nPress Fire to continue"

#define EPILOGUE_HEADLINE L"Epilogue"
#define EPILOGUE_TEXT L"When left in peace the subspace aliens went back to their business keeping the time and space fabric intact.\nThe Emperor's administration crumbled and the peoples of the galaxy could live in freedom from tyranny and opression.\n\nCongratulations you have completed T.W.T.P.B!\n\n\nRemember that you should always THINK for yourself\nand\nvalue your own and others FREEDOM\n\nBelieve in your DREAMS and they will be TRUE!\n\nPeace Out!\n\n\n"

#endif