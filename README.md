# climp (command-line interface music player)

## Arguments

 - [ ] --directory DIRECTORY
 - [ ] --playlist PLAYLIST
 - [ ] --file FILE
 - [ ] --shuffle (enables it, disabled by default)
 - [ ] --repeat 1/2 (0 default, 1 repeat playlist, 2 repeat song)

## Commands

 - [x] q = quit  
 - [x] space = play/pause  
 - [x] up/down select song in playlistlibrary  
 - [x] enter = play selected song in playlist  
 - [ ] s = shuffle toggle  
 - [ ] r = repeat mode toggle (off/one/all)  

## Todo

 - [ ] display song name instead of file name
 - [ ] display all songs in the playlist
 - [ ] make the playlist a scrollable area since it can be longer than the window
 - [ ] make the playlist navigatable using up/down arrow keys
 - [ ] render footer with keybindings

### Check for memory leaks

```bash
leaks -quiet --atExit -- ./build/Debug/Studio ../music
```