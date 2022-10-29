# Curve Editor v0.4.3 for AviUtl
![graph](https://user-images.githubusercontent.com/106879397/195975459-356f0a5f-0433-4f10-b4e4-0fda20c2e2d6.png)  

ベジェ曲線のイージングをウィンドウ上で編集できるAviUtlプラグイン&スクリプトです。  

## ダウンロード
最新バージョン: **v0.4.3**(Alpha版)

<a href="https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/download/v0.4.3/curve_editor-v0.4.3.zip">![Download](https://user-images.githubusercontent.com/106879397/198503637-3f0ca637-08cf-47b0-afdc-10f9974c7b37.png)</a>

※プリセット機能以外の機能が実装されています。

## インストール
同梱されている`curve_editor.auf`を`exedit.auf`のあるフォルダもしくは`plugins`フォルダ内に入れ、`@Curve Editor.tra`を`script`フォルダ**直下**に入れてください。

## 使用方法
AviUtlを起動後、メインウィンドウの「表示」メニューから「Curve Editorの表示」をクリックすると、Curve EditorのGUIが表示されます。

本プラグインでは、以下で紹介する「Valueモード」と「IDモード」の2つの編集モードでカーブを編集することができます。  
ウィンドウ左上のValue/IDスイッチまたは右クリックメニューの「編集モード」から2つのモードを切り替えることができます。

- #### Valueモード
    Valueモードでは、カーブを1~10桁の整数で識別します。値とカーブがそれぞれ対応しています。

- #### IDモード
    IDモードでは、0~1023までのグラフのIDを使って、最大1024個のカーブをプロジェクトファイルに保存できます。  
    
    このモードでは、制御点を追加することでマルチベジェ軌道のように複雑なカーブを作成することもできます。  
    グラフ上の何もないところをダブルクリックすると制御点が追加され、制御点上で再度ダブルクリックすると削除されます。  


詳しい使用方法は[Wiki](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki)をご覧ください。

## 動作環境
| OS | AviUtl | 拡張編集 | 
| :---: | :---: | :---: |
| Windows 7~11 | ~1.10 | ~0.93rc(0.92推奨) |

【動作確認済み】  
Windows 11  
AviUtl 1.00, 1.10  
拡張編集 0.92  

※[patch.aul](https://github.com/ePi5131/patch.aul)、
[イージング設定時短プラグイン](https://github.com/kumrnm/aviutl-easing-quick-setup)、
[イージング簡単選択](https://github.com/hebiiro/AviUtl-Plugin-SelectEasing)との共存を確認。

## 免責事項
当プラグイン・スクリプトを使用したことによっていかなる損害が生じても、製作者は一切の責任を負わないものとします。  


