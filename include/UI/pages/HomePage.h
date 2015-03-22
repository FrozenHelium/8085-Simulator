#pragma once



#include "UI/Page.h"
#include "UI/pages/pages.h"

#include "UI/TextView.h"

#include "InstructionParser_8085.h"


class HomePage : public Page
{
public:
    enum LabelValues{LBL_ACCUM = 1000, LBL_FLAGS, LBL_REG_B, LBL_REG_C, LBL_REG_D, LBL_REG_E, LBL_REG_H, LBL_REG_L, LBL_PC, LBL_SP};
    void OnControlEvent(Control* control, int eventID)
    {
        
    }
    static void TextViewOnChange(GtkTextBuffer *buffer, TextViewOnChangeCallbackData  *data)
    {
        HomePage* page = static_cast<HomePage*>(data->callbackData);
        page->OnTextChange();
    }
    void UpdateLabelValues(std::vector<unsigned short> values)
    {
        std::string label_pretext[] = { "Accumulator: ", "Flags: ", "Register B: ", "Register C: ", "Register D: ", "Register E: "
            , "Register H: ", "Register L: ", "Register PC: "};

        int i = 0;
        for (auto label : m_labels)
        {
            char buff[64] = { 0 };
            sprintf(buff, "%s %u", label_pretext[i].c_str(), values[i]);
            ++i;
            label->SetText(buff);
        }
    }
    void OnTextChange()
    {
        std::string currentLine = m_textView.GetCurrentLine();
        if (currentLine[0] == '\n')
            return;
        std::vector<Token> tokens = m_parser.GetTokens(m_textView.GetCurrentLine());
        for (auto token : tokens)
        {
            if (token.Type() == INSTRUCTION)
            {
                m_textView.ApplyTag("instruction", token.StartOffset(), token.EndOffset());
            }
            else if (token.Type() == REGISTER)
            {
                m_textView.ApplyTag("register", token.StartOffset(), token.EndOffset());
            }
            else if (token.Type() == ERROR)
            {
                m_textView.ApplyTag("error", token.StartOffset(), token.EndOffset());
            }
            else if (token.Type() == COMMENT)
            {
                m_textView.ApplyTag("comment", token.StartOffset(), token.EndOffset());
            }
            else
            {
                m_textView.ApplyTag("normal", token.StartOffset(), token.EndOffset());
            }
        }
        //std::cout << m_textView.GetCurrentLine().c_str() << std::endl;
    }
    void AdjustControls()
    {

    }
    std::string GetCode()
    {
        return m_textView.GetText();
    }
    HomePage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::HOMEPAGE;
        this->Initialize(parentWindow, fixed);
        
        int offset = this->GetParentWidth()*0.2f / 3.0f;
        int w = this->GetParentWidth()*0.4f, h = this->GetParentHeight();
        int x = offset, y = 100;


        m_textView.Set(m_fixed, 0, x, y, w, h-y-30);
        m_textView.SetOnChangeCallback(G_CALLBACK(TextViewOnChange), this);
        this->AddControl(&m_textView);

        int count = 0;
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 2; i++)
            {
                m_labels.push_back(this->AddLabel(LBL_ACCUM + count, "", x + w + offset + i*w/2, y + j*h / 20.0f));
            }
        }

        m_labels.push_back(this->AddLabel(LBL_PC, "", x + w + offset, y + 4*h / 20.0f));
    }
private:
    TextView m_textView;
    InstructionParser_8085 m_parser;
    std::vector<std::string> m_labelValues;
    std::vector<Label*> m_labels;
};