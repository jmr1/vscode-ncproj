/*
Original work Copyright (c) Microsoft Corporation (MIT)
See ThirdPartyNotices.txt in the project root for license information.
All modifications Copyright (c) Robocorp Technologies Inc.
All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License")
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http: // www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

"use strict";

import {
    workspace,
    ExtensionContext,
    window,
    commands,
    RelativePattern,
    Uri,
    ConfigurationTarget,
    StatusBarItem,
    StatusBarAlignment,
    OutputChannel,
} from "vscode";
import { LanguageClientOptions } from "vscode-languageclient";
import { LanguageClient, ServerOptions } from "vscode-languageclient/node";
import * as cp from "child_process";
import path = require("path");
import fs = require("fs");

let langServer: LanguageClient;
let currentWorkingDirectory: string;
let exeSrvPath: string;
let exeCmtconfigPath: string;
let statusBarItem: StatusBarItem;
let ncprojChannel: OutputChannel;

let cmtConfigRunning = false;
const execShell = (cmd: string, options: cp.ExecOptions) =>
    new Promise<string>((resolve, reject) => {
        cmtConfigRunning = true;
        cp.exec(cmd, options, (err, out) => {
            cmtConfigRunning = false;
            if (err) {
                console.log(err);
                ncprojChannel.appendLine("Unable to start Control and Machine Tool configuration tool!");
                if (process.platform == "win32") {
                    ncprojChannel.appendLine("Installation of .NET Framework 4.7.2 redistributable may be required.");
                }
                ncprojChannel.appendLine(
                    'To investigate more navigate to: "' +
                        currentWorkingDirectory +
                        '" and attempt to execute: "' +
                        exeCmtconfigPath.substring(currentWorkingDirectory.length) +
                        '"'
                );
                ncprojChannel.show(true);
                return reject(err);
            }
            return resolve(out);
        });
    });

function startLangServerIO(command: string, args: string[], cwd: string, documentSelector: string[]): LanguageClient {
    const serverOptions: ServerOptions = {
        command,
        args,
        options: { cwd: cwd },
    };
    const clientOptions: LanguageClientOptions = {
        documentSelector: documentSelector,
        synchronize: {
            configurationSection: "ncproj",
        },
    };
    // See: https://code.visualstudio.com/api/language-extensions/language-server-extension-guide
    return new LanguageClient(command, serverOptions, clientOptions);
}

function afterStartLangServer(executable: string) {
    // Register additional things (for instance, debugger).
}

async function startLangServerNCProj(executable: string, cwd: string) {
    const config = workspace.getConfiguration("ncproj");

    const args: Array<string> = [];
    const ncsettingFilePath = config.get<string>("ncsetting.file.path");
    if (ncsettingFilePath) {
        args.push("--ncsetting-path", ncsettingFilePath);
    }
    const logFilePath = config.get<string>("debug.log.path");
    if (logFilePath) {
        args.push("--log-path", logFilePath);
    }
    const calculatePathTime = config.get<boolean>("calculate.pathtime.enable");
    if (calculatePathTime) {
        args.push("--calculate-path", String(calculatePathTime));
    }
    const macroDescriptionsPath = config.get<string>("macro.descriptions.path");
    if (macroDescriptionsPath) {
        args.push("--macro-desc-path", macroDescriptionsPath);
    }

    if (ncsettingFilePath) {
        setStatusText(ncsettingFilePath);
    }

    langServer = startLangServerIO(executable, args, cwd, ["ncproj"]);

    langServer.onDidChangeState((e) => {
        console.log(e);
    });

    await langServer.start().catch((error) => {
        console.log(error);
        ncprojChannel.appendLine("Unable to start Language Server!");
        if (process.platform == "win32") {
            ncprojChannel.appendLine(
                "Installation of the latest Microsoft Visual C++ Redistributable packages for Visual Studio may be required: https://aka.ms/vs/17/release/vc_redist.x64.exe"
            );
        }
        ncprojChannel.appendLine(
            'To investigate more navigate to: "' +
                currentWorkingDirectory +
                '" and attempt to execute: "' +
                exeSrvPath.substring(currentWorkingDirectory.length) +
                '"'
        );
        ncprojChannel.show(true);
    });

    afterStartLangServer(executable);
}

function setStatusText(ncsettingFilePath: string) {
    fs.readFile(ncsettingFilePath, (err, data) => {
        let statusText: string;
        if (err) {
            console.log(err);
            statusText = stem(ncsettingFilePath);
        } else {
            try {
                const obj = JSON.parse(data.toString());
                if (obj && obj.machine_tool_id && obj.machine_tool_id.trim()) {
                    statusText = obj.machine_tool_id.trim();
                } else {
                    statusText = stem(ncsettingFilePath);
                }
            } catch (error) {
                console.log(error);
                statusText = stem(ncsettingFilePath);
            }
        }

        statusBarItem.text = statusText;
        statusBarItem.show();
    });
}

function stem(ncsettingFilePath: string) {
    const normalized = path.normalize(ncsettingFilePath);
    const filename = path.basename(normalized);
    const ext = path.extname(normalized);
    return filename.replace(ext, "");
}

// i.e.: we can ignore changes when we know we'll be doing them prior to starting the language server.
let ignoreNextConfigurationChange = false;

function startListeningConfigurationChanges() {
    workspace.onDidChangeConfiguration((event) => {
        if (ignoreNextConfigurationChange) {
            ignoreNextConfigurationChange = false;
            return;
        }
        for (const s of [
            "ncproj.debug.log.path",
            "ncproj.ncsetting.file.path",
            "ncproj.calculate.pathtime.enable",
            "ncproj.macro.descriptions.path",
        ]) {
            if (event.affectsConfiguration(s)) {
                askReloadWindow('Please use the "Reload Window" action for changes in ' + s + " to take effect.");
                return;
            }
        }
    });

    const config = workspace.getConfiguration("ncproj");
    const ncsettingFilePath = config.get<string>("ncsetting.file.path");
    if (ncsettingFilePath) {
        const normalized = path.normalize(ncsettingFilePath);
        const extension = path.extname(normalized);
        const dirpath = path.dirname(normalized);

        // watching for changes in a specific file does not seem to work therefore need to watch for file pattern and then filter other changed files out:
        // https://stackoverflow.com/questions/73914581/file-system-watcher-doesnt-work-when-used-full-filename-as-filter
        const watcher = workspace.createFileSystemWatcher(
            new RelativePattern(dirpath, "*" + extension),
            true,
            false,
            true
        );

        watcher.onDidChange(async (uri) => {
            const normalized = path.normalize(ncsettingFilePath);
            if (uri.fsPath == Uri.file(normalized).fsPath) {
                await langServer.restart();
                setStatusText(normalized);
                window.showInformationMessage("Changes to " + path.basename(uri.fsPath) + " have been loaded.");
            }
        });
    }
}

function askWhenNoNcsettingSpecified(askMessage: string) {
    const createNcsetting = "Create configuration";
    const selectNcsetting = "Select configuration";

    window.showWarningMessage(askMessage, ...[createNcsetting, selectNcsetting, "No"]).then((selection) => {
        if (selection == createNcsetting) {
            commands.executeCommand("ncproj.ncsettingCreate");
        }
        if (selection == selectNcsetting) {
            commands.executeCommand("ncproj.ncsettingSelect");
        }
    });
}

function askFilePath(
    askMessage: string,
    openLabelMsg: string,
    configProperty: string,
    filters: { [name: string]: string[] }
) {
    const saveInUser = "Yes (save in user settings)";
    const saveInWorkspace = "Yes (save in workspace settings)";

    window.showWarningMessage(askMessage, ...[saveInUser, saveInWorkspace, "No"]).then((selection) => {
        if (selection == saveInUser || selection == saveInWorkspace) {
            window
                .showOpenDialog({
                    canSelectMany: false,
                    openLabel: openLabelMsg,
                    filters: filters,
                })
                .then(async (onfulfilled) => {
                    if (onfulfilled && onfulfilled.length > 0) {
                        let configurationTarget: ConfigurationTarget = ConfigurationTarget.Workspace;
                        if (selection == saveInUser) {
                            configurationTarget = ConfigurationTarget.Global;
                        }
                        const config = workspace.getConfiguration("ncproj");
                        ignoreNextConfigurationChange = true;
                        config.update(configProperty, onfulfilled[0].fsPath, configurationTarget);
                        askReloadWindow(
                            'Please use the "Reload Window" action for setting ' +
                                onfulfilled[0].fsPath +
                                " to take effect."
                        );
                    }
                });
        }
    });
}

export function activate(context: ExtensionContext) {
    ncprojChannel = window.createOutputChannel("NC Project");

    setupPaths(context);

    createStatusBarItem(context);

    startNclangsrv();

    registerCmdNcsettingCreate(context);
    registerCmdNcsettingSelect(context);
    registerCmdTogglePathTimeCalculation(context);
    registerCmdMacroDescriptionsSelect(context);
    registerMacroDescriptionsPlaceholder(context);

    checkNcsettingFilePathProperty();
}

function createStatusBarItem(context: ExtensionContext) {
    statusBarItem = window.createStatusBarItem(StatusBarAlignment.Right, 100);
    statusBarItem.text = "default";
    statusBarItem.show();
    context.subscriptions.push(statusBarItem);
}

function setupPaths(context: ExtensionContext) {
    if (process.platform == "win32") {
        currentWorkingDirectory = context.extensionPath + "\\vscode-client\\out\\";
        exeSrvPath = currentWorkingDirectory + "nclangsrv.exe";
        exeCmtconfigPath = currentWorkingDirectory + "cmtconfig.exe";
    } else {
        currentWorkingDirectory = context.extensionPath + "/vscode-client/out/";
        exeSrvPath = currentWorkingDirectory + "nclangsrv";
        exeCmtconfigPath = currentWorkingDirectory + "cmtconfig";
    }
}

function startNclangsrv() {
    try {
        startLangServerNCProj(exeSrvPath, currentWorkingDirectory);
    } finally {
        startListeningConfigurationChanges();
    }
}

function checkNcsettingFilePathProperty() {
    const config = workspace.getConfiguration("ncproj");
    const ncsettingFilePath = config.get<string>("ncsetting.file.path");
    if (!ncsettingFilePath) {
        askWhenNoNcsettingSpecified(
            "There is no user Control and Machine Tool configuration specified. Do you want to create or select .ncsetting file?"
        );
    }
}

function registerCmdNcsettingSelect(context: ExtensionContext) {
    const disposable = commands.registerCommand("ncproj.ncsettingSelect", () => {
        const ncsettingFilters = {
            "NCSetting Files": ["ncsetting"],
            "All Files": ["*"],
        };
        askFilePath(
            "Do you want to select .ncsetting file?",
            "Select .ncsetting configuration",
            "ncsetting.file.path",
            ncsettingFilters
        );
    });
    context.subscriptions.push(disposable);
}

function registerCmdNcsettingCreate(context: ExtensionContext) {
    const disposable = commands.registerCommand("ncproj.ncsettingCreate", () => {
        if (process.platform != "win32") {
            window.showInformationMessage("Function not supported.");
        }
        if (cmtConfigRunning) {
            window.showInformationMessage("Configuration window is already running!");
        } else {
            const options = { cwd: currentWorkingDirectory };
            execShell(exeCmtconfigPath, options);
        }
    });

    context.subscriptions.push(disposable);
}

function registerCmdTogglePathTimeCalculation(context: ExtensionContext) {
    const disposable = commands.registerCommand("ncproj.togglePathTimeCalculation", () => {
        const config = workspace.getConfiguration("ncproj");
        const calculatePathTime = config.get<boolean>("calculate.pathtime.enable");

        ignoreNextConfigurationChange = true;
        config.update("calculate.pathtime.enable", !calculatePathTime, ConfigurationTarget.Global);
        askReloadWindow('Please use the "Reload Window" action for setting to take effect.');
    });

    context.subscriptions.push(disposable);
}

function registerCmdMacroDescriptionsSelect(context: ExtensionContext) {
    const disposable = commands.registerCommand("ncproj.macroDescriptionsSelect", () => {
        const ncsettingFilters = {
            "JSON Files": ["json"],
            "All Files": ["*"],
        };
        askFilePath(
            "Do you want to select .json macro descriptions file?",
            "Select .json macro descriptions",
            "macro.descriptions.path",
            ncsettingFilters
        );
    });
    context.subscriptions.push(disposable);
}

function registerMacroDescriptionsPlaceholder(context: ExtensionContext) {
    const disposable = commands.registerCommand("ncproj.macroDescriptionsPlaceholder", () => {
        const placeholder = `{
	"version": "1.0",
	"descriptions": {
		"7000": {"My macro title" : "My description for macro #7000"},
		"7001": {"My macro title" : "My description for macro #7001"}
	}
}`;

        workspace.openTextDocument({ content: placeholder, language: "json" }).then((document) => {
            window.showTextDocument(document);
        });
    });

    context.subscriptions.push(disposable);
}

function askReloadWindow(message: string) {
    window.showWarningMessage(message, ...["Reload Window"]).then((selection) => {
        if (selection === "Reload Window") {
            commands.executeCommand("workbench.action.reloadWindow");
        }
    });
}

export function deactivate(): Thenable<void> | undefined {
    if (!langServer) {
        return undefined;
    }
    return langServer.stop();
}
