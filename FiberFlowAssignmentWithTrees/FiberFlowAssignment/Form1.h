#pragma once

//tools
#include  "atlstr.h"  //CString
#include  "..\error\error.h"


//system tools
#include  "..\MessGAs\MessyGA.h"
#include  "system.h"



namespace MessyGATester
{
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
	public __gc class CSurvP2PMainForm : public System::Windows::Forms::Form
	{	
	public:
		CSurvP2PMainForm(CString  sInParameters)
		{
			pc_system  =  new  CSystem();
			pc_system->sInParameters = sInParameters;
			InitializeComponent();
		}
  
	protected:
		void Dispose(Boolean disposing)
		{
			delete  pc_system;
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::Button*  but_gen_tun;
	protected: 


	private: System::Windows::Forms::ListBox*  list_info;
	private: System::Windows::Forms::Button*  but_load_1_file;
	private: System::Windows::Forms::Button*  but_load_files;




	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container * components;
	private: System::Windows::Forms::Button*  but_generate;
	private: System::Windows::Forms::Button*  but_tun_100;

	private: System::Windows::Forms::OpenFileDialog*  openFileDialog1;
	private: System::Windows::Forms::Button*  but_extract_best_result_time;





		CSystem  *pc_system;
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->but_gen_tun = (new System::Windows::Forms::Button());
			this->list_info = (new System::Windows::Forms::ListBox());
			this->but_load_1_file = (new System::Windows::Forms::Button());
			this->but_load_files = (new System::Windows::Forms::Button());
			this->but_generate = (new System::Windows::Forms::Button());
			this->but_tun_100 = (new System::Windows::Forms::Button());
			this->openFileDialog1 = (new System::Windows::Forms::OpenFileDialog());
			this->but_extract_best_result_time = (new System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// but_gen_tun
			// 
			this->but_gen_tun->Location = System::Drawing::Point(12, 2);
			this->but_gen_tun->Name = S"but_gen_tun";
			this->but_gen_tun->Size = System::Drawing::Size(75, 43);
			this->but_gen_tun->TabIndex = 1;
			this->but_gen_tun->Text = S"generate tuning";
			this->but_gen_tun->Click += new System::EventHandler(this, &CSurvP2PMainForm::but_gen_tun_Click);
			// 
			// list_info
			// 
			this->list_info->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->list_info->FormattingEnabled = true;
			this->list_info->Location = System::Drawing::Point(12, 51);
			this->list_info->Name = S"list_info";
			this->list_info->Size = System::Drawing::Size(511, 212);
			this->list_info->TabIndex = 2;
			// 
			// but_load_1_file
			// 
			this->but_load_1_file->Location = System::Drawing::Point(370, 12);
			this->but_load_1_file->Name = S"but_load_1_file";
			this->but_load_1_file->Size = System::Drawing::Size(75, 23);
			this->but_load_1_file->TabIndex = 3;
			this->but_load_1_file->Text = S"Load 1 file";
			this->but_load_1_file->UseVisualStyleBackColor = true;
			this->but_load_1_file->Click += new System::EventHandler(this, &CSurvP2PMainForm::but_load_1_file_Click);
			// 
			// but_load_files
			// 
			this->but_load_files->Location = System::Drawing::Point(451, 12);
			this->but_load_files->Name = S"but_load_files";
			this->but_load_files->Size = System::Drawing::Size(75, 23);
			this->but_load_files->TabIndex = 4;
			this->but_load_files->Text = S"Load files";
			this->but_load_files->UseVisualStyleBackColor = true;
			this->but_load_files->Click += new System::EventHandler(this, &CSurvP2PMainForm::but_load_files_Click);
			// 
			// but_generate
			// 
			this->but_generate->Location = System::Drawing::Point(93, 12);
			this->but_generate->Name = S"but_generate";
			this->but_generate->Size = System::Drawing::Size(75, 23);
			this->but_generate->TabIndex = 5;
			this->but_generate->Text = S"Generate";
			this->but_generate->UseVisualStyleBackColor = true;
			this->but_generate->Click += new System::EventHandler(this, &CSurvP2PMainForm::but_generate_Click);
			// 
			// but_tun_100
			// 
			this->but_tun_100->Location = System::Drawing::Point(186, 2);
			this->but_tun_100->Name = S"but_tun_100";
			this->but_tun_100->Size = System::Drawing::Size(75, 43);
			this->but_tun_100->TabIndex = 6;
			this->but_tun_100->Text = S"generate 100 tuning";
			this->but_tun_100->Click += new System::EventHandler(this, &CSurvP2PMainForm::but_tun_100_Click);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = S"openFileDialog1";
			// 
			// but_extract_best_result_time
			// 
			this->but_extract_best_result_time->Location = System::Drawing::Point(267, 2);
			this->but_extract_best_result_time->Name = S"but_extract_best_result_time";
			this->but_extract_best_result_time->Size = System::Drawing::Size(86, 43);
			this->but_extract_best_result_time->TabIndex = 7;
			this->but_extract_best_result_time->Text = S"Extract best result time";
			this->but_extract_best_result_time->UseVisualStyleBackColor = true;
			this->but_extract_best_result_time->Click += new System::EventHandler(this, &CSurvP2PMainForm::but_extract_best_result_time_Click);
			// 
			// CSurvP2PMainForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(535, 273);
			this->Controls->Add(this->but_extract_best_result_time);
			this->Controls->Add(this->but_tun_100);
			this->Controls->Add(this->but_generate);
			this->Controls->Add(this->but_load_files);
			this->Controls->Add(this->but_load_1_file);
			this->Controls->Add(this->list_info);
			this->Controls->Add(this->but_gen_tun);
			this->Name = S"CSurvP2PMainForm";
			this->Text = S"Form1";
			this->Shown += new System::EventHandler(this, &CSurvP2PMainForm::CSurvP2PMainForm_Shown);
			this->ResumeLayout(false);

		}	
	private: System::Void button1_Click(System::Object *  sender, System::EventArgs *  e);


	private: System::Void but_load_files_Click(System::Object*  sender, System::EventArgs*  e);
	private: System::Void but_load_1_file_Click(System::Object*  sender, System::EventArgs*  e);

private: System::Void but_generate_Click(System::Object*  sender, System::EventArgs*  e);
private: System::Void but_gen_tun_Click(System::Object*  sender, System::EventArgs*  e);
private: System::Void but_tun_100_Click(System::Object*  sender, System::EventArgs*  e);

private: System::Void but_gen_100_Click(System::Object*  sender, System::EventArgs*  e);
private: System::Void CSurvP2PMainForm_Shown(System::Object*  sender, System::EventArgs*  e);
private: System::Void but_analyze_pattern_length_Click(System::Object*  sender, System::EventArgs*  e);
private: System::Void but_extract_best_result_time_Click(System::Object*  sender, System::EventArgs*  e);
};//public __gc class CSurvP2PMainForm : public System::Windows::Forms::Form
};//namespace MessyGATester


