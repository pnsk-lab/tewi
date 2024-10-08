\language ^en$
# Configuration {User guide}
Tewi HTTPd config gets parsed line by line.\
Front tabs/spaces get ignored.

## Example
```
# This is a comment
# Listen to port 80
Listen 80

# /var/www is the document root
DocumentRoot /var/www

# Default MIME
MIMEType all application/octet-stream

# Use mime.types from Apache HTTPd
MIMEFile /etc/mime.types

# Show this file if it is in the directory
DirectoryIndex index.html
BeginDirectory /var/www
	Allow all
EndDirectory
```
\language ^jp$
# コンフィグ {ユーザーガイド}
Tewi HTTPdのコンフィグは行ずつに処理されます。\
先頭のタブ文字と空白文字は無視されます。

## 例
```
# これはコメントです
# ポート80にListenする
Listen 80

# /var/wwwにドキュメントが置かれている
DocumentRoot /var/www

# デフォルトのMIME
MIMEType all application/octet-stream

# Apache HTTPdのmime.typesを使用
MIMEFile /etc/mime.types

# このファイルがディレクトリにあったら、それを見せる
DirectoryIndex index.html
BeginDirectory /var/www
	Allow all
EndDirectory
```
