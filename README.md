## imgui GUIテスト用ツール
windowsでImGuiを遊んでみたいと思ったので作成。
### このプロジェクトのファイルたちの準備方法
- mingwを準備
- python3系も準備
- glfwはWindows用のものを準備(32bit版)
- ダウンロード先: https://github.com/glfw/glfw/releases/download/3.3.4/glfw-3.3.4.bin.WIN32.zip
- luaもWindows用のものを準備(32bit版)
- ダウンロード先: http://luabinaries.sourceforge.net/download.html
- その後コマンドを打つ
```
git submodule add https://github.com/ocornut/imgui.git imgui
git submodule add https://github.com/skaslev/gl3w.git gl3w
git submodule add https://github.com/g-truc/glm.git glm
cd gl3w
python gl3w_gen.py
cd ../
```
- Makefileを準備(コピペ)
- `mingw32-make run`で実行
