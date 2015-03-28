// EditorTestApp.cpp : main project file.

#include "stdafx.h"
#include "EditorForm.h"

using namespace EditorDllTestHarness;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	EditorForm^ form = gcnew EditorForm();
	Application::Idle += gcnew EventHandler( form, &EditorForm::Application_Idle);
	Application::Run(form);
	return 0;
}
