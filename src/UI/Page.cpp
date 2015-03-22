#include "UI/Page.h"

void Page::AllocateNewControl(int type, int id)
{
    m_controls.resize(m_controls.size() + 1);
    switch (type)
    {
    case ControlType::BUTTON:         m_controls[m_controls.size() - 1] = new Button();             break;
    case ControlType::LABEL:          m_controls[m_controls.size() - 1] = new Label();              break;
    //case ControlType::TEXTEDIT:       m_controls[m_controls.size() - 1] = new TextEdit();           break;
    //case ControlType::SPINNER:        m_controls[m_controls.size() - 1] = new Spinner();            break;
    //case ControlType::FRAMERENDERER:  m_controls[m_controls.size() - 1] = new FrameRenderer();      break;
    }
    m_controls[m_controls.size() - 1]->SetID(id);
}



Control* Page::GetControlByID(unsigned int controlID)
{
    int controlIndex = -1;
    for (int i = 0, lim = m_controls.size(); i < lim; i++)
    {
        if (m_controls[i]->GetID() == controlID)
        {
            controlIndex = i;
            break;
        }
    }
    if (controlIndex == -1) throw Exception("invalid value of controlID");
    return m_controls[controlIndex];
}



// TODO: may be design a file format for the page
void Page::LoadFromFile(std::string fileName)
{
    //FILE* fp = fopen(fileName.c_str(), "r");
}

Button* Page::AddButton(int id, std::string label, int x, int y, int w, int h)
{
    Button* bttn = new Button;
    m_controls.push_back(bttn);
    bttn->Set(m_fixed, id, label, x, y, w, h);
    g_signal_connect(G_OBJECT(bttn), "clicked", G_CALLBACK(ControlEventHandler), 
        new ControlEventData(this, bttn, CONTROLEVENT::CLICKED));
    return bttn;
}

//Control* Page::AddSpinner(int x, int y, int w, int h)
//{
//    this->AllocateNewControl(CONTROL::SPINNER, -1);
//    ((Spinner*)m_controls[m_controls.size() - 1])->Set(m_fixed, x, y, w, h);
//    return m_controls[m_controls.size() - 1];
//}
//
//Control* Page::AddLabel(std::string label, int x, int y, int w, int h, int justification)
//{
//    this->AllocateNewControl(CONTROL::LABEL, -1);
//    ((Label*)m_controls[m_controls.size() - 1])->Set(label, m_fixed, x, y, w, h);
//    //((Label*)m_controls[m_controls.size() - 1])->SetJustify(justification);
//    return m_controls[m_controls.size() - 1];
//}

Label* Page::AddLabel(int id, std::string label, int x, int y, int w, int h)
{
    Label* lbl = new Label();
    m_controls.push_back(lbl);
    lbl->Set(m_fixed, id, label, x, y, w, h);
    return lbl;
}

//Control* Page::AddTextEdit(int id, int x, int y, int w, int h)
//{
//    this->AllocateNewControl(CONTROL::TEXTEDIT, id);
//    ((TextEdit*)m_controls[m_controls.size() - 1])->Set(m_fixed, x, y, w, h);
//    return m_controls[m_controls.size() - 1];
//}
//
//Control* Page::AddFrameRenderer(int id, int x, int y, int w, int h)
//{
//    this->AllocateNewControl(CONTROL::FRAMERENDERER, id);
//    ((FrameRenderer*)m_controls[m_controls.size() - 1])->Set(m_fixed, x, y, w, h);
//    return m_controls[m_controls.size() - 1];
//}

int Page::GetParentWidth()
{
    int w = 0, h = 0;
    gtk_window_get_size(GTK_WINDOW(m_parentWindow), &w, &h);
    return w;
}

int Page::GetParentHeight()
{
    int w = 0, h = 0;
    gtk_window_get_size(GTK_WINDOW(m_parentWindow), &w, &h);
    return h;
}


void Page::ShowControls()
{
    for (auto control : m_controls)
    {
        control->Show();
    }
}

void Page::HideControls()
{
    for (auto control: m_controls)
    {
        control->Hide();
    }
}

void Page::Initialize(GtkWidget* parentWindow, GtkWidget* fixed)
{
    m_parentWindow = parentWindow;
    m_fixed = fixed;
}

void Page::OnControlEvent(Control*, int eventID)
{
}

void Page::ControlEventHandler(GtkWidget* widget, gpointer data)
{
    ControlEventData* eventData = static_cast<ControlEventData*> (data);
    Page* parentPage = eventData->page;
    Control* ctrl = eventData->control;
    parentPage->OnControlEvent(ctrl, eventData->eventID);
    (*(parentPage->m_pageEventHandler))(new ControlEventData(parentPage, ctrl, eventData->eventID));
}

void Page::SetEventHandler(PageEventCallBack eventHandler)
{
    m_pageEventHandler = eventHandler;
}

int Page::GetID()
{
    return m_ID;
}
