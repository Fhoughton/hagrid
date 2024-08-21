# Hagrid - A PS1 Harry Potter File Extractor/Repacker
![](https://raw.githubusercontent.com/Fhoughton/hagrid/master/media/demo.gif)

Hagrid is a tool to manage the extraction of the game files for [Harry Potter and the Philosopher's Stone](https://en.wikipedia.org/wiki/Harry_Potter_and_the_Philosopher%27s_Stone_(PlayStation_video_game)?useskin=vector) and [Harry Potter and the Chamber of Secrets](https://en.wikipedia.org/wiki/Harry_Potter_and_the_Chamber_of_Secrets_(video_game)?useskin=vector) for the [Playstation One](https://en.wikipedia.org/wiki/PlayStation_(console)?useskin=vector)

It handles handles extracting and repacking the .dat/.dir files used by the game, where .dat contains unencrypted files and .dir lists the .dat file contents.

# Usage
To extract a .dat/.dir file pair use the -e flag:
```bash
hagrid -e <file.dat> <file.dir> <output_path>
```

To repack an extracted .dat/.dir pair use the -p flag:
```bash
hagrid -p <extracted_dir_path> <out.dat> <out.dir>
```

# Extracting the .bin and .cue files and other tips
For information on extracting the .bin and .cue files from the game to view the file contents, as well as format info, please see my [blog post](https://fhoughton.github.io/reverse-engineering/ps1/psx/c/2024/08/10/week5-potter.html) on this tool.
