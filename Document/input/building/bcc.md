\noindex
\language ^en$

# Using Borland C++ 5.5 {Build}

This page explains how to build using Borland C++ 5.5.

## Steps

### 1. Requirements
 - Borland C++ 5.5
 - Cygwin (at least checked working on 2007)

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Setup the build environment

Enter `C:\Cygwin` (probably depends on the platform), and run `cygwin.bat`.

Then, set `BORLAND` to the prefix of Borland C++. (e.g. `C:\BC55`)

### 5. Build

Run `./make-installer.sh bcc`, and you should get these files:
 - install.exe (Setup program)
 - tewi.7z (Archive)

\language ^jp$

# Borland C++ 5.5を使う {ビルド}

ここではBorland C++ 5.5を使用してコンパイルする方法を紹介します。

## ステップ

### 1. 必要なもの
 - Borland C++ 5.5
 - Cygwin (2007では動作確認しました)

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. ビルド環境を作る

`C:\Cygwin` (環境によって変化すると思われます)に入り、`cygwin.bat`を実行してください。

そしたら`BORLAND`という環境変数をBorland C++ 5.5のインストールディレクトリへのパスにしてください。(例: `C:\BC55`)

### 5. ビルドする

`./make-installer.sh bcc`を実行してください。そしたら以下のものが作成されるはずです:
 - install.exe (セットアッププログラム)
 - tewi.7z (アーカイブ)
