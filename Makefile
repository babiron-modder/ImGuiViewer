

GLFW_FOLDER:=glfw
IMGUI_FOLDER:=imgui
GL3W_FOLDER:=gl3w
GLM_FOLER:=glm
LUA_FOLDER:=lua

CPP_FILE:=$(wildcard src/*.cpp)
OBJ_FILE:=$(CPP_FILE:src/%.cpp=obj/%.o)
EXE_FILE:=ImguiViewer.exe

INCLUDE_FOLDER:=include
LIBSRC_FOLDER:=libsrc
LIBOBJ_FOLDER:=libobj
LIB_FOLDER:=lib
OBJ_FOLDER:=obj

LUA_DLL=lua54.dll



all: $(EXE_FILE)
	@echo compiled

$(EXE_FILE): $(INCLUDE_FOLDER) $(LIB_FOLDER) $(LUA_DLL) $(GLFW_DLL) $(OBJ_FILE)
	g++ -o $(EXE_FILE) $(OBJ_FILE) -I$(INCLUDE_FOLDER) -L$(LIB_FOLDER) -lglfw3 -lgdi32 -lopengl32 -limm32 -limgui -lgl3w -l$(LUA_DLL:.dll=) -limm32
# -mwindows


run: $(EXE_FILE)
	@echo running...
	@echo.
	@$(EXE_FILE)

cplib: $(INCLUDE_FOLDER) $(LIBSRC_FOLDER)
	@echo copyed to $(INCLUDE_FOLDER) and $(LIBSRC_FOLDER)

buildlib: $(LIB_FOLDER)
	@echo build !

clean:
	rd $(INCLUDE_FOLDER) /S /Q
	rd $(LIBSRC_FOLDER) /S /Q
	rd $(LIBOBJ_FOLDER) /S /Q
	rd $(LIB_FOLDER) /S /Q
	del *.dll
	rd $(OBJ_FOLDER) /S /Q
	del $(EXE_FILE)

cleanlib:
	rd $(INCLUDE_FOLDER) /S /Q
	rd $(LIBSRC_FOLDER) /S /Q
	rd $(LIBOBJ_FOLDER) /S /Q



# includeフォルダを生成
$(INCLUDE_FOLDER):
	mkdir $(INCLUDE_FOLDER)
	xcopy $(GL3W_FOLDER)\include $(INCLUDE_FOLDER) /s
	xcopy $(GLFW_FOLDER)\include $(INCLUDE_FOLDER) /s
	mkdir $(INCLUDE_FOLDER)\glm
	xcopy $(GLM_FOLER)\glm $(INCLUDE_FOLDER)\glm /s
	xcopy $(IMGUI_FOLDER)\\*.h $(INCLUDE_FOLDER)
	xcopy $(IMGUI_FOLDER)\backends\imgui_impl_glfw.h $(INCLUDE_FOLDER)
	xcopy $(IMGUI_FOLDER)\backends\imgui_impl_opengl3.h $(INCLUDE_FOLDER)
	xcopy $(LUA_FOLDER)\include\lauxlib.h $(INCLUDE_FOLDER)
	xcopy $(LUA_FOLDER)\include\lua.h $(INCLUDE_FOLDER)
	xcopy $(LUA_FOLDER)\include\lua.hpp $(INCLUDE_FOLDER)
	xcopy $(LUA_FOLDER)\include\luaconf.h $(INCLUDE_FOLDER)
	xcopy $(LUA_FOLDER)\include\lualib.h $(INCLUDE_FOLDER)
# ライブラリの追加はここに書く

# libsrcフォルダを生成
$(LIBSRC_FOLDER):
	mkdir $(LIBSRC_FOLDER) > NUL 2>&1
	xcopy $(GL3W_FOLDER)\src\gl3w.c $(LIBSRC_FOLDER)
	xcopy $(IMGUI_FOLDER)\\*.cpp $(LIBSRC_FOLDER)
	xcopy $(IMGUI_FOLDER)\backends\imgui_impl_glfw.cpp $(LIBSRC_FOLDER)
	xcopy $(IMGUI_FOLDER)\backends\imgui_impl_opengl3.cpp $(LIBSRC_FOLDER)
# ライブラリの追加はここに書く

# libobjフォルダを生成
$(LIBOBJ_FOLDER): $(LIBSRC_FOLDER) $(INCLUDE_FOLDER)
	mkdir $(LIBOBJ_FOLDER) > NUL 2>&1
	@echo ライブラリのコンパイルを開始します...
	g++ -c $(LIBSRC_FOLDER)\\*cpp -I$(INCLUDE_FOLDER)
	gcc -c $(LIBSRC_FOLDER)\\*.c -I$(INCLUDE_FOLDER)
	xcopy *.o $(LIBOBJ_FOLDER)
	del *.o

# libフォルダを生成
$(LIB_FOLDER): $(LIBOBJ_FOLDER)
	mkdir $(LIB_FOLDER) > NUL 2>&1
	ar r $(LIB_FOLDER)\libimgui.a $(LIBOBJ_FOLDER)\imgui*.o
	ar r $(LIB_FOLDER)\libgl3w.a $(LIBOBJ_FOLDER)\gl3w.o
	xcopy $(GLFW_FOLDER)\lib-mingw\libglfw3.a $(LIB_FOLDER)
	xcopy $(LUA_FOLDER)\$(LUA_DLL) $(LIB_FOLDER)


$(LUA_DLL):
	xcopy $(LUA_FOLDER)\$(LUA_DLL) .


$(OBJ_FOLDER):
	mkdir $(OBJ_FOLDER)

obj/%.o: src/%.cpp $(INCLUDE_FOLDER) $(OBJ_FOLDER)
	g++ -o $@ -c $< -I$(INCLUDE_FOLDER)
