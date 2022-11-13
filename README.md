# NC Project extension for Visual Studio Code

Develop and validate CNC programs.

![NC Project](./images/image1.png)

## News

- Initial version 0.0.1 released

## Features

- Code correctness validation
- Code Groups validation
- Hovers and code completion (G-Code and M-Code)
- Syntax coloring
- Machine Tool and Driver configuration creation
- Machine Tool and Driver configuration selection
- Status bar
- Commands

## Current supported file extensions

".nc", ".txt", ".ptp", ".dat", ".mcd", ".spf"

If you would like another file extension supported by this extension, please open an [issue](https://github.com/jmr1/vscode-ncproj/issues).

## Usage

Install & activate extension. Extension activates when you open a supported extension file

## Configuring extension

### Create machine tool and driver configuration

![Create machine tool and driver configuration](./images/create_configuration.gif)

### Select machine tool and driver configuration

![Select machine tool and driver configuration](./images/select_configuration.gif)

## Known Issues

Please visit [GitHub Issues](https://github.com/jmr1/vscode-ncproj/issues) page for any open issues.

## Troubleshooting

### Extension does not seem to work

Installation of Microsoft Visual C++ Redistributable packages for Visual Studio 2019 may be required: <https://aka.ms/vs/17/release/vc_redist.x64.exe>

#### Further troubleshooting

Navigate to: "%USERPROFILE%\\.vscode\extensions"
Then navigate to extension binaries folder: jrupar-mcieslik.ncproj-0.0.1\vscode-client\out
Execute: nclangsrv.exe
Watch for any errors.

### Machine Tool and Driver configuration GUI window does not start

Installation of .NET Framework 4.5 redistributable may be required.

#### Further troubleshooting

Navigate to: "%USERPROFILE%\\.vscode\extensions"
Then navigate to extension binaries folder: jrupar-mcieslik.ncproj-0.0.1\vscode-client\out
Execute: cmtconfig.exe
Watch for any errors.

## Contributors

- Janusz Rupar ([@jmr1](https://github.com/jmr1))
- Marcin Cieślik ([@marcin-975](https://github.com/marcin-975))

## License

This extension is licensed under the [MIT License](LICENSE).
