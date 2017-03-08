REM Delete Garbage Files
del *.aps
del *.ilk
del *.map
del *.ncb
del *.opt
del *.plg
del *.pbi
del *.pbo
del *.pbt
del *.pdb
del *.tmp
del *._xe


REM Delete hidden archived garbage files, first changing their attributes.
attrib -h -a *.gid
del *.gid

REM Optional: Delete Visual Studio Solution user options, which can be useful to keep.
REM attrib -h -a *.suo
REM del *.suo

REM Delete Garbage Directories
del JUNK\*.*
rmdir JUNK
del enc_temp_folder\*.*
rmdir enc_temp_folder
del DEBUG\*.*
rmdir DEBUG
del RELEASE\*.*
rmdir RELEASE
del JUNK\*.*
rmdir JUNK

del DLL\*.lib
del DLL\*.exp

REM Optional: delete the executables that the project has written to root.
REM del *.exe
