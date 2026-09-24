# Tombee Action Script (TAS) for Visual Studio Code

Visual Studio Code extension providing syntax highlighting, snippets, and language configuration for **Tombee Action Script (`.tas`)**.

## Features

- **Syntax Highlighting**: Keywords, action/function definitions, custom events, interrupts, parallel blocks, built-in action commands, string/numeric literals, comments, decorators, and operators.
- **Language Configuration**: Automatic comment toggling (`//`, `/* */`), bracket matching, auto-closing quotes/brackets, and indentation rules.
- **Code Snippets**: Quick templates for `action`, `fn`, `event`, `interrupt`, `parallel`, `wait`, `move_to`, `if`, `while`, and `for`.

## File Association

This extension automatically activates for files with the `.tas` extension.

## Local Installation

From the `frameworks/tas` directory:

```bash
make vscode
```

This creates a symlink from `~/.vscode/extensions/tas-vscode` to this directory. Reload or restart VS Code to activate the extension.
