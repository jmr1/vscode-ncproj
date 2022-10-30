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

'use strict';

import { workspace, ExtensionContext, window, commands, RelativePattern, Uri, ConfigurationTarget } from 'vscode';
import { LanguageClientOptions } from 'vscode-languageclient';
import { LanguageClient, ServerOptions } from 'vscode-languageclient/node';
import * as cp from "child_process";
import path = require('path');

let langServer: LanguageClient;
let currentWorkingDirectory: string;
let exeSrvPath: string;
let exeCmtconfigPath: string;

let cmtConfigRunning = false;
const execShell = (cmd: string, options: cp.ExecOptions) =>
    new Promise<string>((resolve, reject) => {
		cmtConfigRunning = true;
        cp.exec(cmd, options, (err, out) => {
			cmtConfigRunning = false;
            if (err) {
				console.log(err);
                return reject(err);
            }
            return resolve(out);
        });
    });

function startLangServerIO(command: string, args: string[], cwd: string, documentSelector: string[]): LanguageClient {
	const serverOptions: ServerOptions = {
		command,
		args,
		options: { cwd: cwd }
	};
	const clientOptions: LanguageClientOptions = {
		documentSelector: documentSelector,
		synchronize: {
			configurationSection: "ncproj"
		}
	}
	// See: https://code.visualstudio.com/api/language-extensions/language-server-extension-guide
	return new LanguageClient(command, serverOptions, clientOptions);
}

function afterStartLangServer(executable: string) {
    // Register additional things (for instance, debugger).
}

async function startLangServerNCProj(executable: string, cwd: string) {

	let config = workspace.getConfiguration("ncproj");

	let args : Array<string> = [];
	let ncsettingFilePath = config.get<string>("ncsetting.file.path");
	if(ncsettingFilePath)
		args.push("--ncsetting-path", ncsettingFilePath);
	let logFilePath = config.get<string>("debug.log.path");
	if(logFilePath)
		args.push("--log-path", logFilePath);

	langServer = startLangServerIO(executable, args, cwd, ["ncproj"]);

	langServer.onDidChangeState( (e) => {
		console.log(e);
	});

	await langServer.start();
	afterStartLangServer(executable);
}


// i.e.: we can ignore changes when we know we'll be doing them prior to starting the language server.
let ignoreNextConfigurationChange: boolean = false;

function startListeningConfigurationChanges() {

	workspace.onDidChangeConfiguration(event => {
		if (ignoreNextConfigurationChange) {
			ignoreNextConfigurationChange = false;
			return;
		}
		for (let s of ["ncproj.debug.log.path", "ncproj.ncsetting.file.path"]) {
			if (event.affectsConfiguration(s)) {
				window.showWarningMessage('Please use the "Reload Window" action for changes in ' + s + ' to take effect.', ...["Reload Window"]).then((selection) => {
					if (selection === "Reload Window") {
						commands.executeCommand("workbench.action.reloadWindow");
					}
				});
				return;
			}
		}
	});

	let config = workspace.getConfiguration("ncproj");
	let ncsettingFilePath = config.get<string>("ncsetting.file.path");
	if(ncsettingFilePath)
	{
		const normalized = path.normalize(ncsettingFilePath);
		const extension = path.extname(normalized);
		const dirpath = path.dirname(normalized);

		// watching for changes in a specific file does not seem to work therefore need to watch for file pattern and the fileter other changes files out: 
		// https://stackoverflow.com/questions/73914581/file-system-watcher-doesnt-work-when-used-full-filename-as-filter
		const watcher = workspace.createFileSystemWatcher(new RelativePattern(dirpath, '*' + extension), true, false, true);

		watcher.onDidChange(async (uri) => {
			if(uri.fsPath == Uri.file(path.normalize(ncsettingFilePath)).fsPath) {
				await langServer.restart();
				window.showInformationMessage("Changes to " + path.basename(uri.fsPath) + " have been loaded.");
			}
		});
	}
}

function askWhenNoNcsettingSpecified(askMessage: string) {
	let createNcsetting: string = 'Create configuration';
	let selectNcsetting: string = 'Select configuration';

	window.showWarningMessage(askMessage, ...[createNcsetting, selectNcsetting, 'No']).then((selection) => {
		if (selection == createNcsetting) {
			commands.executeCommand('ncproj.ncsettingCreate');
		}
		if(selection == selectNcsetting) {
			commands.executeCommand('ncproj.ncsettingSelect');
		}
	});
}


function askNcsettingFilePath(askMessage: string) {
	let saveInUser: string = 'Yes (save in user settings)';
	let saveInWorkspace: string = 'Yes (save in workspace settings)';

	window.showWarningMessage(askMessage, ...[saveInUser, saveInWorkspace, 'No']).then((selection) => {
		if (selection == saveInUser || selection == saveInWorkspace) {
			window.showOpenDialog({
				'canSelectMany': false,
				'openLabel': 'Select .ncsetting configuration',
				'filters': {'NCSetting Files': ['ncsetting'], 'All Files': ['*']}
			}).then(async onfulfilled => {
				if (onfulfilled && onfulfilled.length > 0) {
					let configurationTarget: ConfigurationTarget = ConfigurationTarget.Workspace;
					if (selection == saveInUser) {
						configurationTarget = ConfigurationTarget.Global;
					}
					let config = workspace.getConfiguration("ncproj");
					ignoreNextConfigurationChange = true;
					config.update("ncsetting.file.path", onfulfilled[0].fsPath, configurationTarget);
					window.showWarningMessage('Please use the "Reload Window" action for setting ' + onfulfilled[0].fsPath + ' to take effect.', ...["Reload Window"]).then((selection) => {
						if (selection === "Reload Window") {
							commands.executeCommand("workbench.action.reloadWindow");
						}
					});
				}
			});
		}
	});
}

export function activate(context: ExtensionContext) {
	setupPaths(context);

	startNclangsrv();

	registerCmdNcsettingCreate(context);
	registerCmdNcsettingSelect(context);

	checkNcsettingFilePathProperty();
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
	let config = workspace.getConfiguration("ncproj");
	let ncsettingFilePath = config.get<string>("ncsetting.file.path");
	if (!ncsettingFilePath) {
		askWhenNoNcsettingSpecified("There is no user Control and Machine Tool configuration specified. Do you want to create or select .ncsetting file?");
	}
}

function registerCmdNcsettingSelect(context: ExtensionContext) {
	let disposable = commands.registerCommand('ncproj.ncsettingSelect', () => {
		askNcsettingFilePath("Do you want to select .ncsetting file?");
	});
	context.subscriptions.push(disposable);
}

function registerCmdNcsettingCreate(context: ExtensionContext) {
	let disposable = commands.registerCommand('ncproj.ncsettingCreate', () => {
		if (process.platform != "win32") {
			window.showInformationMessage("Function not supported.");
		}
		if(cmtConfigRunning) {
			window.showInformationMessage("Configuration window is already running!");
		} else {
			const options = { cwd: currentWorkingDirectory };
			execShell(exeCmtconfigPath, options);
		}
	});

	context.subscriptions.push(disposable);
}

export function deactivate(): Thenable<void> | undefined {
	if (!langServer) {
		return undefined;
	}
	return langServer.stop();
}

