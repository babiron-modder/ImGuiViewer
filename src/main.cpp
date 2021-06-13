#include <windows.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "lua.hpp"

using namespace std;


class FilePicker{
	private:
		bool _init_shown = false;
		bool _is_shown = false;
		string _path = "";
		int _selected_list_item = -1;
		string _current_folder_path = ".";
		string _filter = "";
		bool _is_selected = false;

		int _files_length=0;
		int _folders_length=0;
		string _files[300];
		string _folders[300];
		string _files_utf8[300];
		string _folders_utf8[300];

		void getFolderFiles(string path){
			HANDLE hFind;
			WIN32_FIND_DATA win32fd;
			int folder_i=0;
			int file_i=0;
			hFind = FindFirstFile((path+"\\*.*").c_str(), &win32fd);
			if (hFind == INVALID_HANDLE_VALUE) {
				return;
			}
			do {
				if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					_folders[folder_i]=win32fd.cFileName;
					_folders_utf8[folder_i]=SjistoUTF8(win32fd.cFileName);
					++folder_i;
				} else {
					_files[file_i]=win32fd.cFileName;
					_files_utf8[file_i]=SjistoUTF8(win32fd.cFileName);
					++file_i;
				}
			} while (FindNextFile(hFind, &win32fd));
			_files_length=file_i;
			_folders_length=folder_i;
			FindClose(hFind);
		}

		string SjistoUTF8(string srcSjis)
		{
			int lenghtUnicode = MultiByteToWideChar(CP_THREAD_ACP, 0, srcSjis.c_str(), srcSjis.size()*1 + 1, NULL, 0);

			wchar_t* bufUnicode = new wchar_t[lenghtUnicode];

			MultiByteToWideChar(CP_THREAD_ACP, 0, srcSjis.c_str(), srcSjis.size() + 1, bufUnicode, lenghtUnicode);

			int lengthUTF8 = WideCharToMultiByte(CP_UTF8, 0, bufUnicode, -1, NULL, 0, NULL, NULL);

			char* bufUTF8 = new char[lengthUTF8];

			WideCharToMultiByte(CP_UTF8, 0, bufUnicode, lenghtUnicode + 1, bufUTF8, lengthUTF8, NULL, NULL);

			std::string strUTF8(bufUTF8);

			delete bufUnicode;
			delete bufUTF8;

			return strUTF8;
		}

	public:
		FilePicker(string path, string filter=""){
			_current_folder_path = path;
			_filter = filter;
			getFolderFiles(_current_folder_path);
		}
		string getPath(){
			return _path;
		}

		void menuLoop(){
			ImGui::BeginMainMenuBar();
			if(ImGui::BeginMenu("ファイル")){
				if(ImGui::MenuItem(" 開く")){
					_is_shown = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// FilePicker表示用ループ
		void imLoop(){
			int i;
			static ImGuiTextFilter filter(_filter.c_str());
			if(_is_shown){
				ImGui::OpenPopup("##FilePicker");
				if(!_init_shown){
					_init_shown = true;
					ImGui::SetNextWindowContentSize(ImVec2(450,300));
					// cout << "show!!"<< endl;
				}
				if(ImGui::BeginPopupModal("##FilePicker", &_is_shown)){
					ImGui::Text("ファイルを選択してください：(%s)",_filter.c_str());
					ImGui::TextWrapped(SjistoUTF8(_current_folder_path).c_str());

					ImGui::BeginChild("##chooselist", ImVec2(0,-30), ImGuiWindowFlags_NavFlattened);
					//一つ上のフォルダに移動
					if(ImGui::Selectable("..",false, ImGuiSelectableFlags_AllowDoubleClick) && _folders[1]==".."){
						if(ImGui::IsMouseDoubleClicked(0)){
							ImGui::SetScrollY(0);
							_selected_list_item=-1;
							if(_current_folder_path=="."){
								_current_folder_path="..";
							}else if(_current_folder_path.back()=='.'){
								_current_folder_path="..\\\\"+_current_folder_path;
							}else{
								while(_current_folder_path.back()!='\\'){
									_current_folder_path.pop_back();
								}
								_current_folder_path.pop_back();
								_current_folder_path.pop_back();
							}
							getFolderFiles(_current_folder_path);
						}
					}
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 1.0f, 1.0f));
					for(i=1+(_folders[1]=="..");i<_folders_length;++i){
						if(ImGui::Selectable(_folders_utf8[i].c_str(),false, ImGuiSelectableFlags_AllowDoubleClick)){
							if(ImGui::IsMouseDoubleClicked(0)){
								ImGui::SetScrollY(0);
								_selected_list_item=-1;
								_current_folder_path+="\\\\"+_folders[i];
								getFolderFiles(_current_folder_path);
							}
						}
					}
					ImGui::PopStyleColor(1);

					for(i=0;i<_files_length;++i){
						if (filter.PassFilter(_files_utf8[i].c_str())){
							if(ImGui::Selectable(_files_utf8[i].c_str(), (_selected_list_item==i))){
								_selected_list_item=i;
							}
						}
					}
					ImGui::EndChild();

					ImGui::Dummy(ImVec2(ImGui::GetWindowWidth()-160, 0));

					ImGui::SameLine();
					if(_selected_list_item == -1){
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
					}

					if(ImGui::Button("開く") && _selected_list_item!=-1){
						_path = _current_folder_path+"\\\\"+_files[_selected_list_item];
						_is_shown=false;
						_is_selected = true;
					}

					if(_selected_list_item == -1){
						ImGui::PopStyleColor(4);
					}

					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
					if(ImGui::Button("キャンセル")){
						_is_shown=false;
					}
		      ImGui::PopStyleColor(3);

					ImGui::EndPopup();
				}
			}
		}

		bool selected(){
			if(_is_selected){
				_is_selected=false;
				return true;
			}else{
				return false;
			}
		}
};


class ImLua{
	public:
		static bool showFlag;
		static bool flag;

		static int imBegin(lua_State *L){
			// ImGui::SetNextWindowSizeConstraints(ImVec2(200,200),ImVec2(400,400));
			// ImGui::SetNextWindowContentSize(ImVec2(200,0));
			// ImGui::SetNextWindowSize(ImVec2(200,0));
			// ImGui::Begin(luaL_checkstring(L,1), NULL);
			if(showFlag){
				ImGui::Begin(luaL_checkstring(L,1), &showFlag);
				flag=true;
			}
			return 1;
		}
		static int imEnd(lua_State *L){
			if(showFlag || flag){
				ImGui::End();
				flag=false;
			}
			return 1;
		}
		static int imButton(lua_State *L){
			if(showFlag){
				lua_pushnumber(L, ImGui::Button(luaL_checkstring(L,1)));
			}
			return 1;
		}
		static int imText(lua_State *L){
			if(showFlag){
				ImGui::Text(luaL_checkstring(L,1));
			}
			return 1;
		}
		static int imTextWrapped(lua_State *L){
			if(showFlag){
				ImGui::TextWrapped(luaL_checkstring(L,1));
			}
			return 1;
		}
};
bool ImLua::showFlag = true;
bool ImLua::flag = false;


class LuaModule{
	private:
		lua_State *_L;
		string _file_path="";

		void loadFunction(){
			lua_pushcfunction(_L, ImLua::imBegin);
			lua_setglobal(_L, "imBegin");
			lua_pushcfunction(_L, ImLua::imEnd);
			lua_setglobal(_L, "imEnd");
			lua_pushcfunction(_L, ImLua::imButton);
			lua_setglobal(_L, "imButton");
			lua_pushcfunction(_L, ImLua::imText);
			lua_setglobal(_L, "imText");
			lua_pushcfunction(_L, ImLua::imTextWrapped);
			lua_setglobal(_L, "imTextWrapped");
		}

	public:
		LuaModule(){
			_L = luaL_newstate();
			luaL_openlibs(_L);
		}

		void loadFile(string path){
			if(_file_path!=""){
				closeLua();
				_L = luaL_newstate();
				luaL_openlibs(_L);
			}
			_file_path=path;
			if(luaL_loadfile(_L, _file_path.c_str())){
				// cout << lua_tostring(_L, -1) << endl;
			};

			if(lua_pcall(_L,0,0,0) != 0) {
				// cout << lua_tostring(_L, -1) << endl;
				_file_path="";
				lua_close(_L);
				_L = luaL_newstate();
				luaL_openlibs(_L);
				// exit(EXIT_FAILURE);
			}

			loadFunction();
		}

		void closeLua(){
			lua_close(_L);
		}

		void resetLua(){
			lua_close(_L);
			_L = luaL_newstate();
			luaL_openlibs(_L);
			_file_path="";
		}

		void call(string fn){
			if(_file_path==""){
				return;
			}
			lua_getglobal(_L, fn.c_str());
			lua_pcall(_L, 0, 0, 0);
		}

};



int main(int argc, char *argv[])
{
	int i;

	FilePicker fp(".\\\\test_module",".lua");


	LuaModule lm;


	string running_folder_path=argv[0];
	while(running_folder_path.back()!='\\'){
		running_folder_path.pop_back();
		if(running_folder_path.size()==0){
			break;
		}
	}
	// if(argc==2){
	// 	lm.loadFile(argv[1]);
	// }

	// GLFWの初期化
	if (!glfwInit()) {
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "ImGui Viewer", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	gl3wInit();

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// フォントの追加
	ImGuiIO &io = ImGui::GetIO();
	ImVector<ImWchar> ranges;
	ImFontGlyphRangesBuilder builder;
	builder.AddText("―‐∥…‥‘’“”≠≦≧∞∴♂♀′″℃☆★○●◎◇◆□■△▲▽▼※→←↑↓∈∋⊆⊇⊂⊃∪∩∧∨⇒⇔∀∃∠⊥⌒∂∇≡≒≪≫√∽∝∵∫∬Å‰♯♭♪†‡◯ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρστυφχψωАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя─│┌┐┘└├┬┤┴┼━┃┏┓┛┗┣┳┫┻╋┠┯┨┷┿┝┰┥┸╂");
	builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
	// builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
	builder.BuildRanges(&ranges);

	io.Fonts->AddFontFromFileTTF("font/mplus-2p-regular.ttf", 16.0f, nullptr, ranges.Data);
	io.Fonts->AddFontDefault();
	io.Fonts->Build();



	while (!glfwWindowShouldClose(window)) {
		static float rgb[3]={0.27f, 0.514f, 0.72f};
		// バッファのクリア
		glClearColor(rgb[0], rgb[1], rgb[2], 0.0f);
		// glClearColor(0.164f, 0.164f, 0.164f, 0.0f);
		// glClearColor(1.f, 1.f, 1.f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Imguiの初期化
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::StyleColorsLight();

		//ようこそ
		static bool youkoso = true;
		if(youkoso){
			ImGui::Begin("ようこそ", &youkoso);
			ImGui::TextWrapped("このツールはImGuiのGUIを試すために作ったツールです");
			ImGui::TextWrapped("Luaにいろいろ書いて、Guiを編集していきます");
			ImGui::TextWrapped("適当に作ってるので割と雑です。");
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::TextWrapped("2021/06/13 とりあえずgit");
			ImGui::End();
		}


		// メニューバー

		// ファイル・開くを追加
		fp.menuLoop();

		ImGui::BeginMainMenuBar();
		if(ImGui::BeginMenu("ヘルプ")){
			if(ImGui::MenuItem(" Show help")){

			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();


		static bool demo_window_is_shown = false;
		if(demo_window_is_shown){
			ImGui::ShowDemoWindow();
		}

		//ファイルダイアログ表示
		fp.imLoop();
		if(fp.selected()){
			lm.loadFile(fp.getPath());
		}

		//Luaモジュール表示
		lm.call("Loop");
		if(ImLua::showFlag==false){
			lm.resetLua();
			ImLua::showFlag=true;
		}

		// Imguiの画面更新
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// ダブルバッファのスワップ
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// GLFWの終了処理
	glfwTerminate();

	return 0;
}
