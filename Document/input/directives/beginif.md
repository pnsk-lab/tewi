\language ^en$
# BeginIf/BeginIfNot/EndIf directive {Directives}
Begins/Ends the If section.\
BeginIf makes parser parse the directives inside the If section if the expression is true, otherwise makes it ignore them.\
BeginIfNot is same with BeginIf, but makes it parse if False instead.\
\
Syntax: `BeginIf True|False|defined name` \
Syntax: `BeginIfNot True|False|defined name` \
Syntax: `EndIf`

## Notes on defined names
`HAS_CHROOT` gets defined if Tewi HTTPd was compiled with `chroot(2)` support. \
`HAS_SSL` gets defined if Tewi HTTPd was compiled with TLS/SSL support. \

\language ^jp$
# BeginIf/BeginIfNot/EndIf命令 {命令}
Ifセクションを開始したり終了したりします。\
BeginIfの場合、条件式が真の場合セクション内の命令がパースされます。それ以外の場合は無視されます。\
BeginIfNotはその逆です。\
\
構文: `BeginIf True|False|defined 名前` \
構文: `BeginIfNot True|False|defined 名前` \
構文: `EndIf`

## 定義された名前についての注釈
Tewi HTTPdが`chroot(2)`サポートと共にコンパイルされている場合、`HAS_CHROOT`が定義されます。\
Tewi HTTPdがTLS/SSLサポートと共にコンパイルされている場合、`HAS_SSL`が定義されます。
