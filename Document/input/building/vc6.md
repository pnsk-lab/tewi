\noindex
\language ^en$

# Using Visual C++ 6.0 {Build}

This page explains how to build using Visual C++ 6.0.

## Steps

### 1. Requirements
 - Visual C++ 6.0
 - Cygwin (at least checked working on 2007)

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Setup the build environment

Run `vcvars32` to set environment variables, and enter `C:\Cygwin` (probably depends on the platform), and run `cygwin.bat`.

### 5. Build

Run `./make-installer.sh vc6`, and you should get these files:
 - install.exe (Setup program)
 - tewi.7z (Archive)

\language ^jp$

# Visual C++ 6.0を使う {ビルド}

ここではVisual C++ 6.0を使用してコンパイルする方法を紹介します。

## ステップ

### 1. 必要なもの
 - Visual C++ 6.0
 - Cygwin (2007では動作確認しました)

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. ビルド環境を作る

`vcvars32`を実行して環境変数を設定し、`C:\Cygwin` (環境によって変化すると思われます)に入り、`cygwin.bat`を実行してください。

### 5. ビルドする

`./make-installer.sh vc6`を実行してください。そしたら以下のものが作成されるはずです:
 - install.exe (セットアッププログラム)
 - tewi.7z (アーカイブ)
