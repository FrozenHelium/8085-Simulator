
#include "Application.h"


Application* Application::app = 0;

Application::Application()
{
    if (app)
    {
        throw "only 1 instance is allowed";
    }
    app = this;
}

void Application::Initialize(int* argc, char*** argv)
{
    gtk_init(argc, argv);
    m_mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(m_mainWindow), 1000, 600);
    gtk_window_set_resizable(GTK_WINDOW(m_mainWindow), FALSE);
    gtk_widget_set_size_request(m_mainWindow, 1000, 600);
    g_signal_connect_swapped(G_OBJECT(m_mainWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);


    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;
    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gsize bytes_written, bytes_read;
    const gchar* home = "default.css";
    GError *error = 0;
    gtk_css_provider_load_from_path(provider, g_filename_to_utf8(home, strlen(home), &bytes_read, &bytes_written, &error), NULL);
    g_object_unref(provider);



    GtkWidget* fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(m_mainWindow), fixed);
    gtk_widget_show_all(m_mainWindow);

    //g_signal_connect(G_OBJECT(parent), "key_press_event", G_CALLBACK(KeyPressHandler), NULL);
    uiManager = new UIManager;
    uiManager->Initialize(m_mainWindow, fixed);
    uiManager->SetEventHandler(UIEventHandler);

    aboutPage = new AboutPage(m_mainWindow, fixed);
    uiManager->AddPage(aboutPage);

    homePage = new HomePage(m_mainWindow, fixed);
    uiManager->AddPage(homePage);

    uiManager->AddMenu("File", MENU::FILE);
    uiManager->AddMenuItem(MENU::FILE, "Open", MENUITEM::FILE_OPEN);
    uiManager->AddMenuItem(MENU::FILE, "Exit", MENUITEM::FILE_EXIT);
    uiManager->AddMenu("Edit", MENU::EDIT);
    uiManager->AddMenuItem(MENU::EDIT, "Cut", MENUITEM::EDIT_CUT);
    uiManager->AddMenuItem(MENU::EDIT, "Copy", MENUITEM::EDIT_COPY);
    uiManager->AddMenuItem(MENU::EDIT, "Paste", MENUITEM::EDIT_PASTE);

    uiManager->AddToolItemFromStock(GTK_STOCK_GO_BACK, TOOLITEM::BACK);
    uiManager->AddToolItemFromStock(GTK_STOCK_ADD, TOOLITEM::ADD);
    uiManager->AddToolItemFromStock(GTK_STOCK_SAVE, TOOLITEM::SAVE);
    uiManager->AddToolItemFromStock(GTK_STOCK_UNDO, TOOLITEM::UNDO);
    uiManager->AddToolItemFromStock(GTK_STOCK_REDO, TOOLITEM::REDO);
    uiManager->AddToolItemFromStock(GTK_STOCK_MEDIA_PLAY, TOOLITEM::EXEC);
    uiManager->AddToolItemFromStock(GTK_STOCK_STOP, TOOLITEM::STOP);
    uiManager->AddToolItemFromStock(GTK_STOCK_PREFERENCES, TOOLITEM::SETTINGS);

    uiManager->ShowMenu();
    uiManager->ShowToolbar();
    uiManager->NavigateTo(PAGE::HOMEPAGE);

    homePage->UpdateLabelValues({ 0, 0, 0, 0, 0, 0, 0, 0, 0});

    g_timeout_add(100, Application::OnTimer, 0);
}

void Application::Run()
{
    gtk_main();
}



void Application::OnMenuItemClick(int menuItemID)
{
    switch (menuItemID)
    {
    case MENUITEM::FILE_OPEN:       
        break;
    default:
        // may be throw something
        break;
        
    }
}
void Application::OnButtonEvent(int buttonID)
{
    switch (buttonID)
    {
    case PAGECONTROL::HP_TESTBUTTON:
        break;
   
    default:
        // may be throw something
        break;
        
    }
}
void Application::OnToolItemClick(int toolItemID)
{
    switch (toolItemID)
    {
    case TOOLITEM::EXEC:
        // std::cout << homePage->GetCode();
        //mp.Test(parser.ParseString(homePage->GetCode()));
        app->mp.LoadProgram(app->parser.ParseString(app->homePage->GetCode()));
        app->mp.StartAsync();
        app->m_executing = true;
        break;
    default:
        // may be throw something
        break;
        
    }
}

int Application::OnTimer(gpointer data)
{
    if (app->m_executing)
    {
        app->homePage->UpdateLabelValues(app->mp.GetRegisterValues());
    }
    app->m_executing = app->mp.Executing();
    //std::cout << "Alive" << std::endl;
    return TRUE;
}

void Application::UIEventHandler(UIEventData* eventData)
{
    switch (eventData->event)
    {
    case EVENT::MENUITEMCLICK:
        app->OnMenuItemClick(eventData->controlID);
        break;
    case EVENT::BUTTONCLICK:
        app->OnButtonEvent(eventData->controlID);
        break;
    case EVENT::TOOLITEMCLICK:
        app->OnToolItemClick(eventData->controlID);
        break;
    default:
        break;
    }
}

gboolean Application::KeyPressHandler(GtkWidget* widget, GdkEventKey *keyEvent, gpointer data)
{
    switch (keyEvent->keyval)
    {
    default:
        break;
    }
    return false;
}
