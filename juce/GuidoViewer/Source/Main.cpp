/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"


//==============================================================================
class GuidoViewerApplication  : public JUCEApplication
{
public:
    //==============================================================================
    GuidoViewerApplication()
    {
    }

    ~GuidoViewerApplication()
    {
    }

    //==============================================================================
    void initialise (const String& commandLine)
    {
        // Do your application's initialisation code here..
        mainWindow = new MainAppWindow();
    }

    void shutdown()
    {
        // Do your application's shutdown code here..
        mainWindow = 0;
    }

    //==============================================================================
    void systemRequestedQuit()
    {
        quit();
    }

    //==============================================================================
    const String getApplicationName()
    {
        return "GuidoViewer";
    }

    const String getApplicationVersion()
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed()
    {
        return true;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        
    }

private:
    ScopedPointer <MainAppWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that starts the app.
START_JUCE_APPLICATION(GuidoViewerApplication)
