# climp (command-line interface music player)

## Arguments

 - [ ] --directory
 - [ ] --playlist
 - [ ] --file

## Commands

 - [ ] q = quit  
 - [ ] space = play/pause  
 - [ ] p = select playlist  
 - [ ] up/down select song in playlistlibrary  
 - [ ] enter = play selected song in playlist  
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