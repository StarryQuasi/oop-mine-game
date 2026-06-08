# 文化大學 物件導向程式設計小專題

# 主題：挖礦遊戲

![](/docs/written-by-a-human.svg)

## 小組資料

- 組別：6
- 系級：資工1A
- 組長：高憲成
- 組員：張育溥、程昱銘
- 報告：https://pres.quazyr.xyz/
- 報告 pptx：[挖礦遊戲-final.pptx](/docs/挖礦遊戲-final.pptx)
- 程式：https://github.com/StarryQuasi/oop-mine-game/tree/main/src

## 分工明細

- 高憲成：程式設計、GUI 開發、邏輯整合
- 張育溥：文件、報告撰寫、核心架構構建
- 程昱銘：吃、睡、吃睡

## 遊戲介紹

- 願景：打造一個類似 Minecraft 的無規則 2D 沙盒遊戲
- 目標：
    - 學習 C++
    - 使用 OOP 三大特性 解決資源管理難題
    - 設計樹狀、模組化的 GUI 排版系統
    - 設計樹狀、可繼承的實體型別系統

## 遊戲規則

Minecraft 沒有規則，這個也沒有規則

## 遊戲操作

- A D - 左右移動
- 空格 - 跳躍
- 左鍵 - 破壞方塊
- 右鍵 - 互動方塊 (開啟工作臺)
- 羊會對著玩家走 (計概的路徑搜尋 BFS)
- 樹葉會產生粒子效果、隨機掉落 (離散數學)
- 工作臺可以做基礎合成 (物件導向 GUI)

## 系統架構

由最基礎的[世界](/src/World.h)儲存多形[實體](/src/Entity.h)和[方塊](/src/Block.h)；

[實體](/src/Entity.h)和[方塊](/src/Block.h)封裝基礎資料；

由[實體](/src/Entity.h)向外繼承、擴展出[玩家](/src/Player.h)、[生物](/src/Mob.h)，處理頭髮渲染跟路徑搜尋；

[生物](/src/Mob.h)繼承出[羊](/src/Sheep.h)，提供目標給[生物](/src/Mob.h)做移動；

[方塊](/src/Block.h)繼承出[樹葉](/src/Block.h)、[工作台](/src/Block.h)，負責粒子效果與介面開啟。

GUI 同上，[容器](/src/gui/Container.h)封裝基礎資料，多形儲存子元件，再向外繼承、擴展出[自動排序](/src/gui/FlowContainer.h)、[按鈕](/src/gui/Button.h)、[滑桿](/src/gui/Slider.h)、[文字](/src/gui/TextContainer.h)、[工作台](/src/gui/CraftingTable.h)等元件。

### 技術亮點

* **程序化內容生成**：
    * 利用柏林噪音生成可重現的 2D 隨機世界
* **OOP 設計模式運用**:
    * 觀察者模式 (Observer)
        * 運用 C++ 模板實作 [`Bindable<T>`](/src/Bindable.h) 物件屬性系統，可綁定屬性於物件改變時自動同步其他綁定的物件
    * 建造者模式 (Builder)
        * 優化方塊定義流程，只留必要參數，美化程式
    * 組合模式 (Composite)
        * 使用樹狀結構構建模組化 GUI

## 使用資源

> [!NOTE]
> 
> 世界 OOP 架構部分借鑑 [Minecraft](https://www.minecraft.net/)
> 
> 介面 OOP 架構部分借鑑 [osu!framework](https://github.com/ppy/osu-framework)

- [olcPixelGameEngine](https://github.com/onelonecoder/olcpixelgameengine) (OLC-3) - 渲染、碰撞偵測
- [FastNoiseLite](https://github.com/Auburn/FastNoiseLite) (MIT) - 噪音
- [stb_image](https://github.com/nothings/stb) (MIT) - 圖片解碼
- [miniz](https://github.com/richgel999/miniz) (MIT) - 壓縮檔解碼
- [msf_gif](https://github.com/notnullnotvoid/msf_gif) (MIT) - g i f
- [PixelLab](https://www.pixellab.ai/) - AI 生成實體貼圖
- [SummerFields 阿神材質包](https://github.com/SummerFields/SummerFields) (CC BY-NC-SA) - 方塊/介面貼圖

## 授權 License

This project is licensed under the MIT License

See the [LICENSE](/LICENSE) file for more details

## 遊戲執行

> [!NOTE]
> 
> 目前只測試過 Windows 11 x64 (msvc) 及 macOS 15 on Apple Silicon (clang)

| [assets.zip](https://github.com/StarryQuasi/oop-mine-game/releases/latest/download/assets.zip) | [Windows 11 x64](https://github.com/StarryQuasi/oop-mine-game/releases/latest/download/OopMineGame-windows-x64.exe) | [macOS 15 Apple Silicon](https://github.com/StarryQuasi/oop-mine-game/releases/latest/download/OopMineGame-macos-apple) |
| ---------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |

- 下載執行檔及 assets.zip
- Windows
  - 開啟執行檔並允許執行
- macOS
  - `cd ~/Downloads && chmod +x ./OopMineGame-macos-apple`
  - 開啟執行檔
  - 到問號按鈕裡面找到允許執行的設定連結
  - 再次開啟執行檔

## 遊戲編譯

- 安裝 git 或 GitHub Desktop
- `git clone https://github.com/StarryQuasi/oop-mine-game`
- ***產生素材檔！***
  - `create_assets_zip` (Windows)
  - `chmod +x ./create_assets_zip.sh` `create_create_assets_zip` (Unix)
- 選擇開發環境

### Visual Studio with msvc
- 安裝 [Visual Studio](https://visualstudio.microsoft.com/downloads/) 並選擇 `C++桌面開發`
- 開啟 `oop-mine-game` 資料夾
- 綠色空心箭頭 `ctrl+f5` 執行
- 綠色實心箭頭 `f5` 除錯

### VSCode with clang (建議使用)
- 安裝編譯器 (Windows)
    - 安裝 [Visual Studio](https://visualstudio.microsoft.com/downloads/) 並選擇 `C++桌面開發`
    - 安裝 [Chocolatey](https://chocolatey.org/install)
    - 以系統管理員身分執行 `choco install cmake llvm -y`
- 安裝編譯器 (macOS)
    - `brew install llvm`
- 安裝 [VSCode](https://code.visualstudio.com/download)
- 安裝 `clangd` `CMake Tools` `CodeLLDB` 插件
- 開啟 `oop-mine-game` 資料夾
- 選擇編譯器路徑 (確保使用剛剛安裝的最新編譯器)
  - `ctrl+shift+p` -> `CMake：Select a Kit`
  - `Clang 22.1.0 x86_64-pc-windows-msvc` (Windows)
  - `Clang 22 (Homebrew)` (macOS)
- `shift+f5` 執行
- `f5` 除錯 (記得至側邊攔 `執行與偵錯` 選擇 `Debug lldb`)

## 遊戲畫面

![](/docs/video.gif)

## UML Diagram

*已簡化*

![](/docs/uml.svg)

## Flowchart

<img src="/docs/flowchart.png" style="height: 200px;">
