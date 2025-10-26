file(REMOVE_RECURSE
  "RemoteAccess/qml/main.qml"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/RemoteAccessMobile_tooling.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
