- Open native_plugin/Win32Threads.sln with Visual Studio 2015
    - Do "Build -> Build Solution"
    - Copy x64/[Debug/Release]/Win32Threads.dll to unity/Assets/Win32Threads.dll
       - Before copying, confirm there is no running Unity Editor.  Since Unity Editor doesn't unload native plugin DLL, copying may fail if editor is running.
- Open unity/ with Unity Editor
    - Do "Edit -> Play"
    - Watch unity/Win32Threads-log.txt via `tail` or equivalent utilities.
