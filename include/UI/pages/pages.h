// common header for custom pages
#pragma once


#include "UI/Page.h"


enum PAGE { HOMEPAGE = 0, DEBUGPAGE, ABOUTPAGE, SETTINGSPAGE};

enum PAGECONTROL{
    HP_TESTBUTTON, HP_TEXTVIEW,             // Main page controls
    SP_APPLYCHANGESBUTTON, SP_CANCELBUTTON  // Settings page controls
};