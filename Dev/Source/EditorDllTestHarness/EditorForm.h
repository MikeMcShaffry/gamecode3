#pragma once
#include <windows.h>

namespace EditorDllTestHarness {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class EditorForm : public System::Windows::Forms::Form
	{
	public:
		typedef UINT (CALLBACK* LPFNDLLMAIN)(HINSTANCE, HINSTANCE, HWND, int, int, int);
		typedef void (CALLBACK* LPFNDLLRENDER)();
		typedef void (CALLBACK* LPFNDLLSHUTDOWN)();

		HINSTANCE hDLL;               // Handle to DLL
		LPFNDLLMAIN lpfnDllMain;    // Function pointer
		LPFNDLLRENDER lpfnDllRender;    // Function pointer
	private: System::Windows::Forms::Panel^  EnginePanel;
	public: 
		LPFNDLLSHUTDOWN lpfnDllShutdown;    // Function pointer



	public: 
		EditorForm(void)
		{
			InitializeComponent();
			hDLL = LoadLibrary(L"EditorDllWrapper");

			lpfnDllMain = (LPFNDLLMAIN)GetProcAddress(hDLL, "WrapperMain");
			lpfnDllRender = (LPFNDLLRENDER)GetProcAddress(hDLL, "WrapperRenderFrame");
			lpfnDllShutdown = (LPFNDLLSHUTDOWN)GetProcAddress(hDLL, "WrapperDXShutdown");
			if (!lpfnDllMain)
			{
				// handle the error
				FreeLibrary(hDLL);       
			}
			else
			{
				// call the function
				HINSTANCE hInstance = GetModuleHandle(0);
				HWND hWnd = static_cast<HWND> (this->EnginePanel->Handle.ToPointer());
				//System::Int32 hWndPtr = hWnd.ToInt32();
				lpfnDllMain(hInstance, 0, hWnd, 1, this->Width, this->Height);
			}
		}
		System::Void Application_Idle(System::Object^  sender, System::EventArgs^  e) 
		{
			if (hDLL)
			{
				lpfnDllRender();
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~EditorForm()
		{
			if (hDLL)
			{
				lpfnDllShutdown();
				FreeLibrary(hDLL);    
			}
			if (components)
			{
				delete components;
			}   
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->EnginePanel = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// EnginePanel
			// 
			this->EnginePanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->EnginePanel->Location = System::Drawing::Point(0, 0);
			this->EnginePanel->Name = L"EnginePanel";
			this->EnginePanel->Size = System::Drawing::Size(700, 475);
			this->EnginePanel->TabIndex = 0;
			// 
			// EditorForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(700, 475);
			this->Controls->Add(this->EnginePanel);
			this->Name = L"EditorForm";
			this->Text = L"Editor Form";
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &EditorForm::EditorForm_Paint);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 
		System::Void EnginePanel_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
		{
			//lpfnDllRender();
		}
		System::Void EditorForm_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
		{
			//lpfnDllRender();
		}
	};
}

