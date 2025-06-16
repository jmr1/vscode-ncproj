# NC Project – CNC Programming Extension for Visual Studio Code

Develop, validate, and manage CNC programs directly within VS Code.  
Supports milling, turning, and turn-mill machines with intelligent tooling, validation, and code insights.

![NC Project](./images/image1.png)

![Version](https://img.shields.io/badge/version-0.5.4-blue)
![VS Code](https://img.shields.io/visual-studio-marketplace/v/jrupar-mcieslik.ncproj?label=VS%20Code%20Marketplace)
![License: MIT](https://img.shields.io/badge/license-MIT-green)

---

## 📢 Changelog

### Version 0.5.4 – *June 2025*
- **New Features**
  - Format Document and Format Selection (automatically separates CNC command words for better readability)
- **Enhancements**
  - General performance and UI improvements
- **Bug Fixes**
  - Various stability and validation fixes

---

## ✨ Features

- **Machine Support**
  - Mill
  - Lathe
  - Turn & Mill

- **Code Intelligence**
  - Validation for code correctness and code groups
  - G/M code completion
  - Syntax highlighting
  - Hover support for:
    - G/M codes
    - Macro values
    - Dynamic codes (e.g., `G#1`)
    - System macros: `B`, `D`, `E`, `F`, `H`, `M`, `N`, `S`, `T`

- **Formatting**
  - Format full documents or selected code blocks (with automatic separation of G/M codes, coordinates, parameters, and macro values)

- **Code Lens**
  - Display computed path and execution time
  - Toggle via command: `NC Project: Toggle Path and Time Calculation`

- **Configuration Management**
  - Create and select machine tool and driver configurations via GUI

- **Visual Indicators**
  - Status bar integration

---

### 📸 Screenshots

![Macro Example](./images/macro.png)  
![Macro 2](./images/macro2.png)  
![Code Lens](./images/codelens.png)

---

## 📄 Supported File Extensions

`.nc`, `.txt`, `.ptp`, `.dat`, `.mcd`, `.spf`, `.001`, `.cnc`, `.dnc`, `.eia`,  
`.fan`, `.fnc`, `.g`, `.g00`, `.gcode`, `.iso`, `.lib`, `.min`, `.ncl`, `.out`,  
`.pm`, `.prf`, `.prg`, `.pu1`, `.sub`, `.tap`, `.tp`

💡 Want another file extension supported? [Open an issue](https://github.com/jmr1/vscode-ncproj/issues)

---

## 🚀 Getting Started

1. Install the extension from the Visual Studio Code Marketplace.
2. Open any supported CNC file type.
3. The extension will automatically activate and assist your workflow.

---

## ⚙️ Configuring the Extension

### Create a Machine Tool and Driver Configuration

![Create configuration](./images/create_configuration.gif)

### Select a Machine Tool and Driver Configuration

![Select configuration](./images/select_configuration.gif)

---

## 🛠️ Troubleshooting

### Extension does not activate or function properly

This may be caused by missing system dependencies.

**Required Dependency:**  
[Microsoft Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe)

**Manual Test (via Command Line):**

1. Open **Command Prompt**.
2. Navigate to the extension binaries folder:  
   `cd %USERPROFILE%\.vscode\extensions\jrupar-mcieslik.ncproj-*\vscode-client\out`
3. Run the language server executable: `nclangsrv.exe`.
4. Observe the console output for any errors or missing components.

---

### Configuration GUI does not launch

If the configuration window fails to appear, the required .NET runtime might be missing.

**Required Dependency:**  
[.NET Framework 4.7.2 Redistributable](https://dotnet.microsoft.com/en-us/download/dotnet-framework/net472)

**Manual Test:**

1. Go to the following path:  
   `%USERPROFILE%\.vscode\extensions\jrupar-mcieslik.ncproj-*\vscode-client\out`
2. Double-click `cmtconfig.exe`.
3. Look for any error messages or issues starting the tool.

---

## 👥 Contributors

- [Janusz Rupar](https://github.com/jmr1)
- [Marcin Cieślik](https://github.com/marcin-975)

Thank you for your work and contributions!

---

## 🙋‍♂️ Contributing & Feedback

We welcome your feedback and contributions.

- [Open an issue](https://github.com/jmr1/vscode-ncproj/issues)
- Submit a pull request
- Suggest new features or improvements

---

## 📄 License

This extension is licensed under the [MIT License](LICENSE).

You are free to use, modify, and distribute it under the terms of the license.
