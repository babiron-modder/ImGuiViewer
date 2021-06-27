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


namespace ImLua {
	namespace {
		const long long int TEXT_1024_LEN = 1024;
	}
	namespace {
		bool bool1;
		int int1;
		int int_array[4];
		float float1;
		// float float1024[1024];
		char text1024[1024];
		const char **const_char_array = new const char*[1];
		int CURRENT_CONST_CHAR_ARRAY_SIZE = 0;
	}
	namespace {
		bool showFlag = false;
		bool flag = false;
		int tree_node_counter = 0;
		int stack_top;
	}
	int imBegin(lua_State *L){
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
	int imEnd(lua_State *L){
		if(showFlag || flag){
			ImGui::End();
			flag=false;
		}
		return 1;
	}

	int imCollapsingHeader(lua_State *L){
		if(showFlag){
			lua_pushboolean(L, ImGui::CollapsingHeader(luaL_checkstring(L,1), luaL_optinteger(L, 2, 0)));
		}
		return 1;
	}

	int imTreeNode(lua_State *L){
		if(showFlag){
			lua_pushboolean(L, ImGui::TreeNodeEx(luaL_checkstring(L,1), luaL_optinteger(L,2,0)));
		}
		return 1;
	}

	int imTreePush(lua_State *L){
		if(showFlag){
			ImGui::TreePush(luaL_checkstring(L,1));
		}
		return 1;
	}

	int imTreePop(lua_State *L){
		if(showFlag){
			ImGui::TreePop();
		}
		return 1;
	}

	int imCombo(lua_State *L){
		if(showFlag){
			if(CURRENT_CONST_CHAR_ARRAY_SIZE < luaL_checkinteger(L, 4)){
				delete[] const_char_array;
				const_char_array = new const char*[luaL_checkinteger(L, 4)];
				CURRENT_CONST_CHAR_ARRAY_SIZE = luaL_checkinteger(L, 4);
			}
			for(int1=0; int1<luaL_checkinteger(L, 4); ++int1){
				lua_rawgeti(L, 3, int1+1); // 引数2 の値の [1] を取得する
				const_char_array[int1] = lua_tostring(L, -1);
			}
			int1 = luaL_checkinteger(L, 2) - 1; //lua:1 -> c++:0
			ImGui::Combo(luaL_checkstring(L, 1), &int1, const_char_array, luaL_checkinteger(L, 4));
			lua_pushinteger(L, int1+1);
		}
		return 1;
	}

	int imListBox(lua_State *L){
		if(showFlag){
			if(CURRENT_CONST_CHAR_ARRAY_SIZE < luaL_checkinteger(L, 4)){
				delete[] const_char_array;
				const_char_array = new const char*[luaL_checkinteger(L, 4)];
				CURRENT_CONST_CHAR_ARRAY_SIZE = luaL_checkinteger(L, 4);
			}

			for(int1=0; int1<luaL_checkinteger(L, 4); ++int1){
				lua_rawgeti(L, 3, int1+1); // 引数2 の値の [1] を取得する
				const_char_array[int1] = lua_tostring(L, -1);
			}
			int1 = luaL_checkinteger(L, 2) - 1; //lua:1 -> c++:0
			ImGui::ListBox(luaL_checkstring(L, 1), &int1, const_char_array, luaL_checkinteger(L, 4), luaL_optinteger(L, 5, -1));
			lua_pushinteger(L, int1+1);
		}
		return 1;
	}

	int imCheckBox(lua_State *L){
		if(showFlag){
			bool1 = lua_toboolean(L,2);
			ImGui::Checkbox(luaL_checkstring(L,1), &bool1);
			lua_pushboolean(L, bool1);
		}
		return 1;
	}

	int imRadioButton(lua_State *L){
		if(showFlag){
			int1 = luaL_checknumber(L,2);
			ImGui::RadioButton(luaL_checkstring(L,1), &int1, luaL_checknumber(L,3));
			lua_pushinteger(L, int1);
		}
		return 1;
	}

	int imButton(lua_State *L){
		if(showFlag){
			lua_pushboolean(L, ImGui::Button(luaL_checkstring(L,1), ImVec2(luaL_optinteger(L, 2, 0), luaL_optinteger(L, 3, 0))));
		}
		return 1;
	}
	int imSmallButton(lua_State *L){
		if(showFlag){
			lua_pushboolean(L, ImGui::SmallButton(luaL_checkstring(L,1)));
		}
		return 1;
	}
	int imInputInt(lua_State *L){
		if(showFlag){
			int1 = luaL_checkinteger(L, 2);
			ImGui::InputInt(luaL_checkstring(L, 1), &int1);
			lua_pushinteger(L, int1);
		}
		return 1;
	}
	int imInputInt2(lua_State *L){
		if(showFlag){
			lua_rawgeti(L, 2, 1); // 引数2 の値の [1] を取得する
			int_array[0] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 2); // 引数2 の値の [2] を取得する
			int_array[1] = lua_tointeger(L, -1);

			ImGui::InputInt2(luaL_checkstring(L,1), int_array);

			lua_newtable(L);
			stack_top = lua_gettop(L);

			lua_pushinteger(L, int_array[0]);
			lua_rawseti(L, stack_top, 1);
			lua_pushinteger(L, int_array[1]);
			lua_rawseti(L, stack_top, 2);
		}
		return 1;
	}
	int imInputInt3(lua_State *L){
		if(showFlag){
			lua_rawgeti(L, 2, 1); // 引数2 の値の [1] を取得する
			int_array[0] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 2); // 引数2 の値の [2] を取得する
			int_array[1] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 3); // 引数2 の値の [3] を取得する
			int_array[2] = lua_tointeger(L, -1);

			ImGui::InputInt3(luaL_checkstring(L,1), int_array);

			lua_newtable(L);
			stack_top = lua_gettop(L);

			lua_pushinteger(L, int_array[0]);
			lua_rawseti(L, stack_top, 1);
			lua_pushinteger(L, int_array[1]);
			lua_rawseti(L, stack_top, 2);
			lua_pushinteger(L, int_array[2]);
			lua_rawseti(L, stack_top, 3);
		}
		return 1;
	}
	int imInputInt4(lua_State *L){
		if(showFlag){
			lua_rawgeti(L, 2, 1); // 引数2 の値の [1] を取得する
			int_array[0] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 2); // 引数2 の値の [2] を取得する
			int_array[1] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 3); // 引数2 の値の [3] を取得する
			int_array[2] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 4); // 引数2 の値の [4] を取得する
			int_array[3] = lua_tointeger(L, -1);

			ImGui::InputInt4(luaL_checkstring(L,1), int_array);

			lua_newtable(L);
			stack_top = lua_gettop(L);

			lua_pushinteger(L, int_array[0]);
			lua_rawseti(L, stack_top, 1);
			lua_pushinteger(L, int_array[1]);
			lua_rawseti(L, stack_top, 2);
			lua_pushinteger(L, int_array[2]);
			lua_rawseti(L, stack_top, 3);
			lua_pushinteger(L, int_array[3]);
			lua_rawseti(L, stack_top, 4);
		}
		return 1;
	}

	int imInputFloat(lua_State *L){
		if(showFlag){
			float1 = luaL_checknumber(L, 2);
			ImGui::InputFloat(luaL_checkstring(L, 1), &float1);
			lua_pushnumber(L, float1);
		}
		return 1;
	}

	int imSliderInt(lua_State *L){
		if(showFlag){
			int1 = luaL_checkinteger(L, 2);
			ImGui::SliderInt(luaL_checkstring(L,1), &int1, luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));
			lua_pushinteger(L, int1);
		}
		return 1;
	}
	int imSliderInt2(lua_State *L){
		if(showFlag){
			lua_rawgeti(L, 2, 1); // 引数2 の値の [1] を取得する
			int_array[0] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 2); // 引数2 の値の [2] を取得する
			int_array[1] = lua_tointeger(L, -1);

			ImGui::SliderInt2(luaL_checkstring(L,1), int_array, luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));

			lua_newtable(L);
			stack_top = lua_gettop(L);

			lua_pushinteger(L, int_array[0]);
			lua_rawseti(L, stack_top, 1);
			lua_pushinteger(L, int_array[1]);
			lua_rawseti(L, stack_top, 2);
		}
		return 1;
	}
	int imSliderInt3(lua_State *L){
		if(showFlag){
			lua_rawgeti(L, 2, 1); // 引数2 の値の [1] を取得する
			int_array[0] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 2); // 引数2 の値の [2] を取得する
			int_array[1] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 3); // 引数2 の値の [3] を取得する
			int_array[2] = lua_tointeger(L, -1);

			ImGui::SliderInt3(luaL_checkstring(L,1), int_array, luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));

			lua_newtable(L);
			stack_top = lua_gettop(L);

			lua_pushinteger(L, int_array[0]);
			lua_rawseti(L, stack_top, 1);
			lua_pushinteger(L, int_array[1]);
			lua_rawseti(L, stack_top, 2);
			lua_pushinteger(L, int_array[2]);
			lua_rawseti(L, stack_top, 3);
		}
		return 1;
	}
	int imSliderInt4(lua_State *L){
		if(showFlag){
			lua_rawgeti(L, 2, 1); // 引数2 の値の [1] を取得する
			int_array[0] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 2); // 引数2 の値の [2] を取得する
			int_array[1] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 3); // 引数2 の値の [3] を取得する
			int_array[2] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 4); // 引数2 の値の [4] を取得する
			int_array[3] = lua_tointeger(L, -1);

			ImGui::SliderInt4(luaL_checkstring(L,1), int_array, luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));

			lua_newtable(L);
			stack_top = lua_gettop(L);

			lua_pushinteger(L, int_array[0]);
			lua_rawseti(L, stack_top, 1);
			lua_pushinteger(L, int_array[1]);
			lua_rawseti(L, stack_top, 2);
			lua_pushinteger(L, int_array[2]);
			lua_rawseti(L, stack_top, 3);
			lua_pushinteger(L, int_array[3]);
			lua_rawseti(L, stack_top, 4);
		}
		return 1;
	}

	int imSliderFloat(lua_State *L){
		if(showFlag){
			float1 = luaL_checknumber(L, 2);
			ImGui::SliderFloat(luaL_checkstring(L,1), &float1, luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_optstring(L, 5, "%.3f"));
			lua_pushnumber(L, float1);
		}
		return 1;
	}

	int imDragInt(lua_State *L){
		if(showFlag){
			int1 = luaL_checkinteger(L, 2);
			ImGui::DragInt(luaL_checkstring(L,1), &int1, luaL_checknumber(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5));
			lua_pushinteger(L, int1);
		}
		return 1;
	}
	int imDragInt2(lua_State *L){
		if(showFlag){
			lua_rawgeti(L, 2, 1); // 引数2 の値の [1] を取得する
			int_array[0] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 2); // 引数2 の値の [2] を取得する
			int_array[1] = lua_tointeger(L, -1);

			ImGui::DragInt2(luaL_checkstring(L,1), int_array, luaL_checknumber(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5));

			lua_newtable(L);
			stack_top = lua_gettop(L);

			lua_pushinteger(L, int_array[0]);
			lua_rawseti(L, stack_top, 1);
			lua_pushinteger(L, int_array[1]);
			lua_rawseti(L, stack_top, 2);
		}
		return 1;
	}
	int imDragInt3(lua_State *L){
		if(showFlag){
			lua_rawgeti(L, 2, 1); // 引数2 の値の [1] を取得する
			int_array[0] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 2); // 引数2 の値の [2] を取得する
			int_array[1] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 3); // 引数2 の値の [3] を取得する
			int_array[2] = lua_tointeger(L, -1);

			ImGui::DragInt3(luaL_checkstring(L,1), int_array, luaL_checknumber(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5));

			lua_newtable(L);
			stack_top = lua_gettop(L);

			lua_pushinteger(L, int_array[0]);
			lua_rawseti(L, stack_top, 1);
			lua_pushinteger(L, int_array[1]);
			lua_rawseti(L, stack_top, 2);
			lua_pushinteger(L, int_array[2]);
			lua_rawseti(L, stack_top, 3);
		}
		return 1;
	}
	int imDragInt4(lua_State *L){
		if(showFlag){
			lua_rawgeti(L, 2, 1); // 引数2 の値の [1] を取得する
			int_array[0] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 2); // 引数2 の値の [2] を取得する
			int_array[1] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 3); // 引数2 の値の [3] を取得する
			int_array[2] = lua_tointeger(L, -1);
			lua_rawgeti(L, 2, 4); // 引数2 の値の [4] を取得する
			int_array[3] = lua_tointeger(L, -1);

			ImGui::DragInt4(luaL_checkstring(L,1), int_array, luaL_checknumber(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5));

			lua_newtable(L);
			stack_top = lua_gettop(L);

			lua_pushinteger(L, int_array[0]);
			lua_rawseti(L, stack_top, 1);
			lua_pushinteger(L, int_array[1]);
			lua_rawseti(L, stack_top, 2);
			lua_pushinteger(L, int_array[2]);
			lua_rawseti(L, stack_top, 3);
			lua_pushinteger(L, int_array[3]);
			lua_rawseti(L, stack_top, 4);
		}
		return 1;
	}
	int imText(lua_State *L){
		if(showFlag){
			ImGui::Text(luaL_checkstring(L,1));
		}
		return 1;
	}
	int imBulletText(lua_State *L){
		if(showFlag){
			ImGui::BulletText(luaL_checkstring(L,1));
		}
		return 1;
	}
	int imTextWrapped(lua_State *L){
		if(showFlag){
			ImGui::TextWrapped(luaL_checkstring(L,1));
		}
		return 1;
	}
	int imInputText(lua_State *L){
		if(showFlag){
			text1024[0] = '\0';
			strcat(text1024, luaL_checkstring(L,2));
			ImGui::InputText(luaL_checkstring(L,1), text1024, min(luaL_checkinteger(L,3), TEXT_1024_LEN));
		}
		lua_pushstring(L , text1024);
		return 1;
	}
	int imProgressBar(lua_State *L){
		if(showFlag){
			ImGui::ProgressBar(luaL_checknumber(L, 1), ImVec2(0.0f, 0.0f), luaL_optstring(L, 2, NULL));
		}
		return 1;
	}
	// int imPlotLines(lua_State *L){
	// 	if(showFlag){
	// 		// new_float_array = new float[luaL_checkinteger(L, 3)];
	// 		for(int1=0; int1<luaL_checkinteger(L, 3); ++int1){
	// 			lua_rawgeti(L, 2, int1+1);
	// 			float1024[int1] = lua_tonumber(L, -1);
	// 		}
	//
	// 		ImGui::PlotLines(luaL_checkstring(L, 1), float1024, luaL_checkinteger(L, 3));
	// 		// delete new_float_array;
	// 	}
	// 	return 1;
	// }
	int imBeginTable(lua_State *L){
		if(showFlag){
			lua_pushboolean(L, ImGui::BeginTable(luaL_checkstring(L,1), luaL_checkinteger(L,2), luaL_optinteger(L, 3, 0)));
		}
		return 1;
	}
	int imTableNextRow(lua_State *L){
		if(showFlag){
			ImGui::TableNextRow();
		}
		return 1;
	}
	int imTableNextColumn(lua_State *L){
		if(showFlag){
			ImGui::TableNextColumn();
		}
		return 1;
	}
	int imEndTable(lua_State *L){
		if(showFlag){
			ImGui::EndTable();
		}
		return 1;
	}
	int imSameLine(lua_State *L){
		if(showFlag){
			ImGui::SameLine();
		}
		return 1;
	}
	int imSpacing(lua_State *L){
		if(showFlag){
			ImGui::Spacing();
		}
		return 1;
	}
	int imSeparator(lua_State *L){
		if(showFlag){
			ImGui::Separator();
		}
		return 1;
	}
	int imIndent(lua_State *L){
		if(showFlag){
			ImGui::Indent(luaL_optnumber(L, 1, 0.0));
		}
		return 1;
	}
	int imUnindent(lua_State *L){
		if(showFlag){
			ImGui::Unindent(luaL_optnumber(L, 1, 0.0));
		}
		return 1;
	}
	int imPushItemWidth(lua_State *L){
		if(showFlag){
			ImGui::PushItemWidth(luaL_checkinteger(L,1));
		}
		return 1;
	}
	int imPopItemWidth(lua_State *L){
		if(showFlag){
			ImGui::PopItemWidth();
		}
		return 1;
	}
	int imPushID(lua_State *L){
		if(showFlag){
			ImGui::PushID(luaL_checkstring(L,1));
		}
		return 1;
	}
	int imPopID(lua_State *L){
		if(showFlag){
			ImGui::PopID();
		}
		return 1;
	}
	int imIsItemHovered(lua_State *L){
		if(showFlag){
			lua_pushboolean(L, ImGui::IsItemHovered());
		}
		return 1;
	}
	int imSetTooltip(lua_State *L){
		if(showFlag){
			ImGui::SetTooltip(luaL_checkstring(L,1));
		}
		return 1;
	}
	int imBeginTooltip(lua_State *L){
		if(showFlag){
			ImGui::BeginTooltip();
		}
		return 1;
	}
	int imEndTooltip(lua_State *L){
		if(showFlag){
			ImGui::EndTooltip();
		}
		return 1;
	}
	int imDummy(lua_State *L){
		if(showFlag){
			ImGui::Dummy(ImVec2(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2)));
		}
		return 1;
	}

}



class LuaModule{
	private:
		lua_State *_L;
		string _file_path="";

		void pushLuaFunction(lua_CFunction f, const char *name){
			lua_pushcfunction(_L, f);
			lua_setglobal(_L, name);
		}

		void loadFunction(){
			pushLuaFunction(ImLua::imBegin, "imBegin");
			pushLuaFunction(ImLua::imEnd, "imEnd");
			pushLuaFunction(ImLua::imCollapsingHeader, "imCollapsingHeader");
			pushLuaFunction(ImLua::imTreeNode, "imTreeNode");
			pushLuaFunction(ImLua::imTreePush, "imTreePush");
			pushLuaFunction(ImLua::imTreePop, "imTreePop");
			pushLuaFunction(ImLua::imCombo, "imCombo");
			pushLuaFunction(ImLua::imListBox, "imListBox");
			pushLuaFunction(ImLua::imCheckBox, "imCheckBox");
			pushLuaFunction(ImLua::imRadioButton, "imRadioButton");
			pushLuaFunction(ImLua::imButton, "imButton");
			pushLuaFunction(ImLua::imSmallButton, "imSmallButton");
			pushLuaFunction(ImLua::imInputInt, "imInputInt");
			pushLuaFunction(ImLua::imInputInt2, "imInputInt2");
			pushLuaFunction(ImLua::imInputInt3, "imInputInt3");
			pushLuaFunction(ImLua::imInputInt4, "imInputInt4");
			pushLuaFunction(ImLua::imInputFloat, "imInputFloat");
			pushLuaFunction(ImLua::imSliderInt, "imSliderInt");
			pushLuaFunction(ImLua::imSliderInt2, "imSliderInt2");
			pushLuaFunction(ImLua::imSliderInt3, "imSliderInt3");
			pushLuaFunction(ImLua::imSliderInt4, "imSliderInt4");
			pushLuaFunction(ImLua::imSliderFloat, "imSliderFloat");
			pushLuaFunction(ImLua::imDragInt, "imDragInt");
			pushLuaFunction(ImLua::imDragInt2, "imDragInt2");
			pushLuaFunction(ImLua::imDragInt3, "imDragInt3");
			pushLuaFunction(ImLua::imDragInt4, "imDragInt4");
			pushLuaFunction(ImLua::imText, "imText");
			pushLuaFunction(ImLua::imBulletText, "imBulletText");
			pushLuaFunction(ImLua::imTextWrapped, "imTextWrapped");
			pushLuaFunction(ImLua::imInputText, "imInputText");
			pushLuaFunction(ImLua::imProgressBar, "imProgressBar");
			// pushLuaFunction(ImLua::imPlotLines, "imPlotLines");
			pushLuaFunction(ImLua::imBeginTable, "imBeginTable");
			pushLuaFunction(ImLua::imTableNextRow, "imTableNextRow");
			pushLuaFunction(ImLua::imTableNextColumn, "imTableNextColumn");
			pushLuaFunction(ImLua::imEndTable, "imEndTable");
			pushLuaFunction(ImLua::imSameLine, "imSameLine");
			pushLuaFunction(ImLua::imSpacing, "imSpacing");
			pushLuaFunction(ImLua::imSeparator, "imSeparator");
			pushLuaFunction(ImLua::imIndent, "imIndent");
			pushLuaFunction(ImLua::imUnindent, "imUnindent");
			pushLuaFunction(ImLua::imPushItemWidth, "imPushItemWidth");
			pushLuaFunction(ImLua::imPopItemWidth, "imPopItemWidth");
			pushLuaFunction(ImLua::imPushID, "imPushID");
			pushLuaFunction(ImLua::imPopID, "imPopID");
			pushLuaFunction(ImLua::imIsItemHovered, "imIsItemHovered");
			pushLuaFunction(ImLua::imSetTooltip, "imSetTooltip");
			pushLuaFunction(ImLua::imBeginTooltip, "imBeginTooltip");
			pushLuaFunction(ImLua::imEndTooltip, "imEndTooltip");
			pushLuaFunction(ImLua::imDummy, "imDummy");

			lua_pushnumber(_L, ImGuiTableFlags_Borders);
			lua_setglobal(_L, "IMLUA_TABLE_BORDERS");
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
				closeLua();
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
			closeLua();
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
	if(argc==2){
		lm.loadFile(argv[1]);
	}

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
		ImGui::BeginMainMenuBar();
		if(ImGui::MenuItem("リロード")){
			lm.loadFile(fp.getPath());
		}
		ImGui::EndMainMenuBar();

		// ファイル・開くを追加
		fp.menuLoop();

		ImGui::BeginMainMenuBar();
		// if(ImGui::BeginMenu("ファイル")){
		// 	if(ImGui::MenuItem(" リロード")){
		// 		lm.loadFile(fp.getPath());
		// 	}
		// 	ImGui::EndMenu();
		// }
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

	delete[] ImLua::const_char_array;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// GLFWの終了処理
	glfwTerminate();

	return 0;
}
