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

import { workspace, Disposable, ExtensionContext, window, commands } from 'vscode';
import { LanguageClientOptions } from 'vscode-languageclient';
import { LanguageClient, ServerOptions } from 'vscode-languageclient/node';
import * as cp from "child_process";

let langServer: LanguageClient;

const execShell = (cmd: string, options: cp.ExecOptions) =>
    new Promise<string>((resolve, reject) => {
        cp.exec(cmd, options, (err, out) => {
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

function startLangServerNCProj(context: ExtensionContext, executable: string, cwd: string) {

	let config = workspace.getConfiguration("ncproj");
	let args = config.get<Array<string>>("args");

	langServer = startLangServerIO(executable, args, cwd, ["ncproj"]);

	langServer.onDidChangeState( (e) => {
		console.log(e);
	});

	langServer.onReady().then(()=> {
		console.log(langServer.initializeResult);
	}, (error) => {
		console.log(error);
	});

	let disposable: Disposable = langServer.start();
	afterStartLangServer(executable);
	context.subscriptions.push(disposable);

}


// i.e.: we can ignore changes when we know we'll be doing them prior to starting the language server.
let ignoreNextConfigurationChange: boolean = false;

function startListeningConfigurationChanges() {

	workspace.onDidChangeConfiguration(event => {
		if (ignoreNextConfigurationChange) {
			ignoreNextConfigurationChange = false;
			return;
		}
		for (let s of ["ncproj.args"]) {
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
}

export function activate(context: ExtensionContext) {

	let currentWorkingDirectory = context.extensionPath + "/vscode-client/out/";
	if (process.platform == "win32") {
		currentWorkingDirectory = context.extensionPath + "\\vscode-client\\out\\";
	}

	const options = { cwd: currentWorkingDirectory };

	try {

		let executable : string = "nclangsrv";
		if (process.platform == "win32") {
			executable = "nclangsrv.exe";
			executable = currentWorkingDirectory + executable;
		} else {
			executable = currentWorkingDirectory + executable;
		}

		startLangServerNCProj(context, executable, currentWorkingDirectory);

	} finally {
		startListeningConfigurationChanges();
	}

	let disposable = commands.registerCommand('ncproj.ncsettings', () => {

		let executable : string = "cmtconfig";
		if (process.platform == "win32") {
			executable = "cmtconfig.exe";
			executable = currentWorkingDirectory + executable;
		} else {
			executable = currentWorkingDirectory + executable;
		}
		execShell(executable, options);
	});

	context.subscriptions.push(disposable);

}

export function deactivate(): Thenable<void> | undefined {
	if (!langServer) {
		return undefined;
	}
	return langServer.stop();
}

