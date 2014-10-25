snes4nacl
=========

Fork of https://code.google.com/p/snes4nacl.

Build
-----
- Boot vagrant and shell into it.
```bash
vagrant up
vagrant ssh
```
- Compile
```bash
make
```
- You will now have **snes4nacl_x86_32.exe** and **snes4nacl_x86_64.exe** in the root folder.

Play
----
- Run the local server.
```bash
make serve
```
- Navigate to http://localhost:8080.

Package
------
- Generate the archive file.
```bash
make package
```
- You now have the \*.nexe files in **package.tar.gz**.
