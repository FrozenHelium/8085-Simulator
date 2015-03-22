#pragma once

#include "UI/UIManager.h"

#include "UI/pages/HomePage.h"
#include "UI/pages/AboutPage.h"

#include <gdk/gdkkeysyms.h>

#include "MP_8085.h"
#include "InstructionParser_8085.h"


// A specific class to handle overall operations
class Application
{
public:
    enum MENU{ FILE = 0, EDIT, DEBUG, TOOLS, HELP };
    enum MENUITEM{ FILE_OPEN = 0, FILE_SAVE, FILE_EXIT, EDIT_UNDO, EDIT_REDO, EDIT_CUT, EDIT_COPY, EDIT_PASTE, TOOLS_PREFERENCE, HELP_ABOUT };
    enum TOOLITEM{ BACK = 0, ADD, SAVE, UNDO, REDO, EXEC, STOP, SETTINGS };

    Application();
    void OnMenuItemClick(int menuItemID);
    void OnButtonEvent(int buttonID);
    void OnToolItemClick(int toolItemID);
    static void UIEventHandler(UIEventData* eventData);
    static gboolean KeyPressHandler(GtkWidget* widget, GdkEventKey *keyEvent, gpointer data);
    void Initialize(int *argc, char*** argv);
    static gboolean ConnectionTest(gpointer data);
    void Run();
    static int OnTimer(gpointer data);
private:
    AboutPage*      aboutPage;
    HomePage*       homePage;

    static Application* app;

    UIManager   *uiManager;
    GtkWidget *m_mainWindow;

    InstructionParser_8085 parser;
    MPSystem_8085 mp;

    bool m_executing;
};

