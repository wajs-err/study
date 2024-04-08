# Workspace Setup

This document is to help you to set up your course workspace.

## Operating System

Course homeworks are guaranteed to compile and work in Linux and macOS. Please note that any other operating system **is not supported** through it may work natively, using Docker, WLS, or other virtualization tools.

## Setup process

### Step 1 - Installing Rust and C linker

Install `rustup` either [using the official guide](https://www.rust-lang.org/tools/install) or using your package manager such as `apt`, `pacman` or `brew`.

On Linux, you probably have the C language linker, but make sure you already have it by installing `build-essential` using your package manager.

On MacOS, users have to install XCode tools to get a C language linker.

```shell
xcode-select --install
```

Run this command to get the stable Rust compiler:

```shell
rustup update stable
```

### Step 2 - VS Code and plugins

The only supported editor in the course is [Visual Studio Code](https://code.visualstudio.com). Install it. Then, install the following plugins:

- [rust-analyzer](https://marketplace.visualstudio.com/items?itemName=matklad.rust-analyzer) - language server, your best friend.
- [Better TOML](https://marketplace.visualstudio.com/items?itemName=bungcip.better-toml) - syntax highlight for `.toml` files.
- [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) - optional, needed for debugging.

_IDE such as CLion or editors with plugins such ad Vim will work perfectly because of Rust's nature, but the lecturer doesn't use them and won't support them officially._

### Step 3 - Cloning repository

It's not necessary, but may be convenient to create a workspace folder first:

```shell
mkdir workspace
cd workspace
```

Clone the repository:

```shell
git clone https://gitlab.atp-fivt.org/courses-public/rust-2024/rust2024
```

### Step 4 - Rover tool

We have a course assistant named `rover`. It will automatize the part of your solving routine. So install it with the command:

```shell
cargo install --path rust2024/tools/rover
```

You may need to additionally install `libssl-dev` and `pkg-config` to build `rover`. Install it if needed.

From this moment, you can call it from any place you want!

To uninstall it later, run this line from anyplace:

```shell
cargo uninstall rover
```

### Step 5 - Student CI

1. Clone your repository **next to the course repository** in the folder named **solutions** (use `username-project` repository):

    ```shell
    git clone YOUR_SOLUTIONS_REPOSITORY solutions
    ```

2. The directory structure should be:

    ```shell
    $ tree -L 1
    .
    ├── rust2024
    ├── solutions
    ```

3. Copy the workflow file from course repository and push it to the remote:

    ```shell
    cp rust2024/.grader-ci.yml solutions/.gitlab-ci.yml
    cd solutions
    git add .
    git commit -m "Add .gitlab-ci.yml"
    git push
    cd ..
    ```

4. Try to submit your solution to CI by running `rover submit` in the folder of the problem `add`. Feel free to ask any questions if something is wrong.
