# Reverse Engineering Resources

- The game uses Scaleform for the UI/HUD.
- The game uses [Squirrel 2.1.1 stable](http://www.squirrel-lang.org/) for scripting.
- `HappyWars.exe` and `LauncherDll.dll` is initially encrypted and must be dumped from memory
  - It is decrypted at runtime, all memory pages are left with RWX rights.
  - Even with this decryption, all exported functions still have some kind of obfuscation applied to them.
- All `Data` files, excluding the `audio` directory and some stray `txt` files are encrypted with Blowfish, and compressed with zlib. Check `LibHappyWars` for more information on decrypting these.
- The game and launcher expect a REST API accessible server at https://pollux-gameapi.happywars.net/api/pollux -- the certificate is checked.
  - Headers sent:
    - `X-Identifier`: Steam ID as a 64-bit integer
    - `P-Authorization`: The Steam auth ticket

