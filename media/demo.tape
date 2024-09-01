Output demo.gif

Require echo

Set Shell "bash"
Set FontSize 24
Set Width 1200
Set Height 600

Sleep 2s
Type "./hagrid --help" Sleep 500ms  Enter
Sleep 2s
Type "./hagrid dat-dir --mode extract --dat-file potter01_extracted/POTTER.DAT --dir-file potter01_extracted/POTTER.DIR --path out" Sleep 500ms  Enter
Sleep 4s
Type "ls out" Sleep 500ms  Enter
Sleep 3s
Type "./hagrid dat-dir --mode pack --dat-file potter.dat --dir-file potter.dir --path out" Sleep 500ms  Enter
Sleep 2s
Type "ls -sh potter.dir" Sleep 500ms  Enter
Sleep 2s
Type "ls -sh potter.dat" Sleep 500ms  Enter
Sleep 5s
