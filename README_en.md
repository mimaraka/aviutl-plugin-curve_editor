# Curve Editor for AviUtl

![GitHub](https://img.shields.io/github/license/mimaraka/aviutl-plugin-curve_editor)
![GitHub issues](https://img.shields.io/github/issues/mimaraka/aviutl-plugin-curve_editor)
![GitHub all releases](https://img.shields.io/github/downloads/mimaraka/aviutl-plugin-curve_editor/total)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/mimaraka/aviutl-plugin-curve_editor)

![graph2 7](https://user-images.githubusercontent.com/106879397/215339739-e6b130bf-41a5-421a-8f8a-7dc2b12e647d.png)  

This is an AviUtl plugin and script that allows you to edit various easings on the window. 

## Download
Please download the latest version zip file (`curve_editor-v*.zip`) from the link below.  
[**Download**](https://github.com/inmare/au-curve_editor_en/releases/latest)

## Installation 
Unzip the downloaded `curve_editor-v*.zip`, put `curve_editor.auf` in the folder directly under the Aviutl directory or in the `Plugins` folder. 
And put `@Curve Editor.tra` into the `script` folder or a folder below it.  
If you have not installed [Visual C++ 2015/2017/2019/2022 Redistributable package (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe), also install it too.    

![image](https://user-images.githubusercontent.com/106879397/212220660-598f0b11-a317-42c7-99cb-0795f56af998.png)

## How to use
After starting AviUtl, click "Curve Editor" from the "Window" menu in the main window to display the Curve Editor window.  

With this plug-in, you can edit curves in four editing modes: "**Bezier**", "**Bezier (multiple)**", "**Vibration**", and "**Bounce**".  
You can switch between the four modes using the mode selection button at the top left of the window or "Edit Mode" from the right-click menu.  

- #### Bezier
    Identifies the Bezier curve with a 1-10 digit integer. The values ​​and curves correspond to each other.  

- #### Bezier(Multi-points) 
    Up to 1024 Bezier curves can be saved in the project file using graph IDs from 1 to 1024.  
    
    In this mode, you can also create complex curves like 93's "Multi-bezier trajectory" script by adding control points.  
    Double-clicking on an empty area on the graph will add a control point, and double-clicking on the control point again will remove it.   

- #### Elastic  
    The movement will be similar to the vibration of rubber.  
    Similar to Bezier mode, curves are identified by integers. Negative values ​​invert the curve.  

- #### Bounce
    This causes the object to bounce.  
    Similar to Bezier mode, curves are identified by integers. Negative values ​​invert the curve.  
    
### Graph view
You can move and scale the view as shown in the image below.

![curve_editor_view](https://user-images.githubusercontent.com/106879397/208283665-2d22b1f4-3672-4c0c-a8b2-7b1d718b67c6.gif)

### Drag & drop
You can apply the edited curve to the track bar by dragging and dropping as shown in the image below.

![curve_editor_dandd](https://user-images.githubusercontent.com/106879397/208283022-ed88a2d9-66e0-41bb-8244-92a8adebc1db.gif)

*By dragging and dropping while holding down the shift key, you can set easing individually for x, y, and z, even for track bars where easing is set at once for x, y, and z coordinates.

#### If you can't drag and drop... 
Select `Type1@Curve Editor` for Bezier/Bezier(Multi-points) mode, and `Type2@Curve Editor` for Vibration/Bounce mode from the track bar menu.  
In Bezier/Vibration/Bounce mode, you can use the curve value that can be copied by pressing the copy button on the top right, and in Bezier (vibration) mode, you can use it by entering the displayed ID into the parameter setting dialog.  

### Etc
You can open the context menu by right-clicking on the editor, so feel free to experiment with it.  
Also, please see [this repository's wiki](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki) for detailed usage information.  

## プロジェクトファイルへのカーブの保存機能について
一部のモードのカーブはプロジェクトファイル(`.aup`)への埋め込みが可能となっており、プロジェクト保存時に同時に保存されるようになっています。  
しかし、仕様上、**拡張編集標準のバックアップ機能では、これらのカーブを保存することはできません。**

そのため、カーブのデータを保持した状態でバックアップを行いたい場合は、代わりに[autosaver](https://github.com/epi5131/autosaver)(epi氏)を使用するようにしてください。

## スクリプトから使用する(スクリプト開発者向け)
スクリプト制御や、`.anm`や`.obj`、`.tra`などのスクリプトファイル上で、Curve Editorのカーブを呼び出して使用することができます。  
詳しくはWikiの[スクリプトからの使用](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%82%B9%E3%82%AF%E3%83%AA%E3%83%97%E3%83%88%E3%81%8B%E3%82%89%E3%81%AE%E4%BD%BF%E7%94%A8)を参照してください。

## 動作環境
| OS | AviUtl | 拡張編集 | 
| :---: | :---: | :---: |
| Windows 7~11 | 1.00, 1.10 | 0.92~ (0.92推奨) |  

※[Visual C++ 2015/2017/2019/2022 再頒布可能パッケージ (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)が必要です。

【動作確認済み】  
Windows 10, 11  
AviUtl 1.00, 1.10  
拡張編集 0.92  

※[patch.aul](https://github.com/ePi5131/patch.aul)、
[イージング設定時短プラグイン](https://github.com/kumrnm/aviutl-easing-quick-setup)、
[イージング簡単選択](https://github.com/hebiiro/AviUtl-Plugin-SelectEasing)
との共存を確認。  

## Q&A
Wikiの[トラブルシューティング](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0)を参照してください。

## バグ報告
バグ報告は[このリポジトリのIssues](https://github.com/mimaraka/aviutl-plugin-curve_editor/issues)または[Google Form](https://forms.gle/mhv96DSYVhhKPkYQ8)(匿名回答可)からお願いします。  

## その他
当プラグイン・スクリプトは以下のプラグイン・スクリプトを参考にして作成されました：  
- [Flow](https://aescripts.com/flow/)
- [ベジェ軌道T](https://www.nicovideo.jp/watch/sm20632293)
- [Aulsグラフエディタ](https://auls.client.jp/)
- [イージング設定時短プラグイン](https://github.com/kumrnm/aviutl-easing-quick-setup)
- [イージング簡単選択](https://github.com/hebiiro/AviUtl-Plugin-SelectEasing)

## 免責事項
当プラグイン・スクリプトを使用したことによっていかなる損害が生じても、製作者は一切の責任を負わないものとします。  
