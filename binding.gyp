{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "src/addon.cc", "src/RCSwitch.cpp" ], 
      "libraries": [ "-lwiringPi" ],
      "cflags": [
        "-std=c++11"
      ]
    }
  ]
}
