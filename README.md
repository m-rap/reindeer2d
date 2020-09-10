reindeer2d
=========

to build, install, and run android example
```
    cd <reindeer2d_path>/wks/android-studio
    ./gradlew installDebug
    adb shell am start -n com.mrap.reindeer2d_example/android.app.NativeActivity && (adb shell pidof com.mrap.reindeer2d_example | xargs -I{} adb logcat --pid {})
```

if logcat doesn't show anything just reexecute command
```
    adb shell pidof com.mrap.reindeer2d_example | xargs -I{} adb logcat --pid {}
```

alternatively in failure of aquiring pid from command pidof, running can also be executed with this:
```
    adb shell am start -n com.mrap.reindeer2d_example/android.app.NativeActivity && (adb logcat | grep native-activity)
```