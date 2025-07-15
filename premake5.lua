workspace "GuchisEngin"
   startproject "GuchisEngin" -- デフォルトのスタートプロジェクトを指定
   configurations { "Debug", "Release" }
   platforms { "x64" }


   -- 出力ディレクトリの設定
   targetdir "../generated/outputs/%{cfg.buildcfg}/%{cfg.platform}"  -- 実行ファイルの出力先
   objdir "../generated/obj/%{prj.name}/%{cfg.buildcfg}"     -- 中間ファイルの出力先

   -- DirectXTexを外部プロジェクトとして登録
   externalproject "DirectXTex"
      location "Externals/DirectXTex"           -- フォルダパス（.vcxprojがある場所）
      filename "DirectXTex_Desktop_2022_Win10"  -- 実際のプロジェクトファイル名 (拡張子は不要)
      uuid "12345678-ABCD-4321-DCBA-1234567890AB" -- 適宜修正
      kind "StaticLib"
      language "C++"
   
-- ImGuiを普通のprojectとして登録
project "imgui"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   staticruntime "on"

   targetdir "../generated/outputs/%{cfg.buildcfg}/%{cfg.platform}"
   objdir "../generated/obj/%{prj.name}/%{cfg.buildcfg}"

   files {
      "Externals/imgui/*.cpp",
      "Externals/imgui/*.h"
   }

   includedirs {
      "Externals/imgui"
   }

   links {
      "dxgi", "d3d12", "dxguid"
   }

-- GuchisEnginの設定

project "GuchisEngin" -- プロジェクト名
   kind "WindowedApp" -- デスクトップアプリケーションに設定
   language "C++"
   cppdialect "C++20" -- 言語の設定

   -- プロジェクトに含むファイル
   files { 
      "*.cpp",
      "*.h",

      "Engine/**.cpp",
      "Engine/**.h",
      "Engine/**.ipp",

      "App/**.cpp",
      "App/**.h",

      "Externals/imgui/*.cpp",
      "Externals/imgui/*.h"
   }
   
   -- 追加のインクルードパス
   includedirs { 
      "Engine",
      "Engine/Includes",
      "App",
      "Externals",
      "Externals/assimp/include",
      "Externals/imgui",
    } 

   dependson { "DirectXTex","imgui" } -- 依存していることを指定

   links { 
   "imgui",
   "DirectXTex",

   -- DXライブラリ系
   "d3d12",
   "dxgi",
   "dxguid",
   "dxcompiler",

   "dinput8",
   "xinput"
   }  -- リンク対象のプロジェクト

   warnings "Extra" -- 警告レベル3を設定

   buildoptions { "/utf-8" } -- UTF-8でビルドする設定

   flags { "MultiProcessorCompile" } -- 複数プロセッサでのコンパイルを有効化

   -- ビルド後イベントのコマンド
   postbuildcommands {
      'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"',
      'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"'
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      fatalwarnings { "All" }  -- 警告をエラーとして扱う
      staticruntime "On"  -- 静的ランタイム（/MTd）
      linkoptions { "/IGNORE:4049", "/IGNORE:4099" } -- 指定したリンカーの警告を無視
      libdirs { "Externals/assimp/lib/Debug" } -- デバッグ用追加のライブラリディレクトリ
      links { "assimp-vc143-mtd" } -- デバッグ用ライブラリ

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      staticruntime "On"  -- 静的ランタイム（/MT）
      linkoptions { "/IGNORE:4049", "/IGNORE:4099" } -- 指定したリンカーの警告を無視
      libdirs { "Externals/assimp/lib/Release" } -- リリース用追加のライブラリディレクトリ
      links { "assimp-vc143-mt" } -- リリース用ライブラリ
