Output demo.gif

Require echo

Set Shell "bash"
Set FontSize 24
Set Width 1200
Set Height 600

Sleep 2s
Type "./hagrid" Sleep 500ms  Enter
Sleep 2s
Type "./hagrid -e potter01_extracted/POTTER.DAT potter01_extracted/POTTER.DIR out" Sleep 500ms  Enter
Sleep 4s
Type "ls out" Sleep 500ms  Enter
Sleep 3s
Type "./hagrid -p out potter.dat potter.dir" Sleep 500ms  Enter
Sleep 2s
Type "ls -sh potter.dir" Sleep 500ms  Enter
Sleep 2s
Type "ls -sh potter.dat" Sleep 500ms  Enter
Sleep 5s
