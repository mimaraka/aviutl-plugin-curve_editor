# Curve Editor for AviUtl

![GitHub](https://img.shields.io/github/license/mimaraka/aviutl-plugin-curve_editor)
![GitHub issues](https://img.shields.io/github/issues/mimaraka/aviutl-plugin-curve_editor)
![GitHub all releases](https://img.shields.io/github/downloads/mimaraka/aviutl-plugin-curve_editor/total)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/mimaraka/aviutl-plugin-curve_editor)

![thumbnail](https://github.com/user-attachments/assets/fa8243c9-3b7b-4b86-aa59-c266e6101fb7)

An AviUtl plugin and script that lets you create and manage various easings.

## Download

Download the latest version's ZIP file (`curve_editor-v*.zip`) from the link below.  
[**Download**](https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/latest)

## Installation

Extract the downloaded ZIP file and place each item in the appropriate location.

|                    Item                    |                                                                              Destination                                                                              |
| :----------------------------------------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
| `curve_editor.auf`<br>`curve_editor` (folder) | One of: directly under the AviUtl directory / the `plugins` folder / a folder one level below the `plugins` folder<br>(place both items in the same location) |
|             `@Curve Editor.tra`            |                                            The `script` folder or a folder one level below it                                            |
|             `curve_editor.lua`             |                                                     The folder containing `exedit.auf`                                                     |

![installation](https://github.com/user-attachments/assets/8edff440-b22f-45fc-a930-cac3285cd805)

> [!NOTE]
> When updating, overwrite all files and folders during placement.

> [!NOTE]
> If it does not work correctly, check whether the following programs are installed on your computer.
>
> - [Visual C++ 2015/2017/2019/2022 Redistributable (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)
> - [Microsoft Edge WebView2 Runtime (x86)](https://developer.microsoft.com/en-us/microsoft-edge/webview2/?form=MA13LH)

## How to use

After starting AviUtl, click "Show Curve Editor" from the "View" menu in the main window to display the Curve Editor window. ([→ If the window does not appear](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0))

With this plugin, you can edit curves in the following 5 editing modes:

- **Normal** (formerly: Bezier (Multi))
- **Bezier**
- **Elastic**
- **Bounce**
- **Script**

You can switch between these modes using the mode selection dropdown at the top left of the window.

- #### Normal (formerly: Bezier (Multi))

    Using IDs from `1` to `524288`, you can save up to 524288 curves in the project file.

    In this mode, you can freely combine 4 types of curves—linear, Bezier, Elastic, and Bounce—to create a curve.  
    Double-clicking on an empty area of the graph adds an anchor.
    To delete a curve, double-click an anchor.

    By right-clicking on an anchor, you can change the type of curve for each segment, or set a different modifier for each segment.

- #### Bezier

    A mode for creating a single cubic Bezier curve.  
    Curves are identified by a several-digit integer value, with values and curves corresponding one-to-one.

- #### Elastic

    A mode for creating motion like the vibration of rubber.  
    As in Bezier mode, curves are identified by integers. Setting a negative value inverts the curve.

- #### Bounce

    A mode for creating bouncing motion of an object.  
    As in Bezier mode, curves are identified by integers. Setting a negative value inverts the curve.

- #### Script

    A mode in which you can write the easing function directly with a Lua script.  
    As in Normal mode, you can add up to 524288 curves.

### Graph view

You can zoom the view in and out by scrolling the mouse wheel.

You can move the view's position by dragging with the mouse wheel, or by dragging with the left button while holding the `Alt` key.

### Drag & drop

When you press the "Apply" button, the button changes to display "Drag & drop onto a track bar to apply".  
Without releasing the left mouse button, drag to the track bar's interpolation-method button.

When the cursor hovers over the interpolation-method button, the button is highlighted.  
Dropping over the track bar you want to apply the curve to applies the curve.

[→ If drag & drop does not work](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0)

> [!TIP]
> By dragging & dropping while holding the Shift key, you can set easing individually for each coordinate even on track bars where easing is set all at once, such as XYZ coordinates.

## Other

For detailed usage and solutions to common problems, see the [Wiki](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki).

## System requirements

|      OS      | AviUtl | Extended Editing |
| :----------: | :----: | :--------------: |
| Windows 7~11 |  1.10  |       0.92       |

> [!IMPORTANT]
> The following programs must be installed.
>
> - [Visual C++ 2015/2017/2019/2022 Redistributable (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)
> - [Microsoft Edge WebView2 Runtime (x86)](https://developer.microsoft.com/en-us/microsoft-edge/webview2/?form=MA13LH)

## Bug reports

Please report bugs via Issues or the [Google Form](https://forms.gle/mhv96DSYVhhKPkYQ8) (anonymous responses accepted).

## Disclaimer

The author assumes no responsibility for any damages arising from the use of this plugin/script.
