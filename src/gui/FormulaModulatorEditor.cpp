/*
** Surge Synthesizer is Free and Open Source Software
**
** Surge is made available under the Gnu General Public License, v3.0
** https://www.gnu.org/licenses/gpl-3.0.en.html
**
** Copyright 2004-2021 by various individuals as described by the Git transaction log
**
** All source at: https://github.com/surge-synthesizer/surge.git
**
** Surge was a commercial product from 2004-2018, with Copyright and ownership
** in that period held by Claes Johanson at Vember Audio. Claes made Surge
** open source in September 2018.
*/

#include "FormulaModulatorEditor.h"
#include "SurgeGUIEditor.h"
#include "RuntimeFont.h"

FormulaModulatorEditor::FormulaModulatorEditor(SurgeGUIEditor *ed, SurgeStorage *s,
                                               FormulaModulatorStorage *fs)
    : editor(ed), formulastorage(fs)
{
    applyButton = std::make_unique<juce::TextButton>("Apply");
    applyButton->setButtonText("Apply");
    applyButton->setEnabled(false);
    applyButton->setBounds(5, 340, 40, 15);
    applyButton->addListener(this);
    addAndMakeVisible(applyButton.get());

    mainEditor = std::make_unique<juce::TextEditor>("Formula");
    mainEditor->setFont(Surge::GUI::getFontManager()->getFiraMonoAtSize(10));
    mainEditor->setMultiLine(true, false);
    mainEditor->setReturnKeyStartsNewLine(true);
    mainEditor->setText(fs->formula);
    mainEditor->setBounds(5, 25, 730, 310);
    mainEditor->addListener(this);
    addAndMakeVisible(mainEditor.get());

    warningLabel = std::make_unique<juce::Label>("Warning");
    warningLabel->setFont(Surge::GUI::getFontManager()->getLatoAtSize(14));
    warningLabel->setBounds(5, 5, 730, 20);
    warningLabel->setColour(juce::Label::textColourId, juce::Colour(255, 0, 0));
    warningLabel->setText("WARNING: Dont use this! Super alpha! It will crash and probably won't "
                          "load in the future or work now. And have fun!",
                          juce::NotificationType::dontSendNotification);
    addAndMakeVisible(warningLabel.get());
}
void FormulaModulatorEditor::buttonClicked(juce::Button *button)
{
    if (button == applyButton.get())
    {
        formulastorage->formula = mainEditor->getText().toStdString();
        applyButton->setEnabled(false);
        editor->invalidateFrame();
    }
}
void FormulaModulatorEditor::textEditorTextChanged(juce::TextEditor &te)
{
    applyButton->setEnabled(true);
}

void FormulaModulatorEditor::resized()
{
    auto w = getWidth() - 5;
    auto h = getHeight() - 5; // this is a hack obvs
    int m = 3, m2 = m * 2;
    warningLabel->setBounds(m, m, w - m2, 20);
    mainEditor->setBounds(m, 20 + m2, w - m2, h - 40 - m2 - m2);
    applyButton->setBounds(m, h - 20, 50, 20 - m);
}
