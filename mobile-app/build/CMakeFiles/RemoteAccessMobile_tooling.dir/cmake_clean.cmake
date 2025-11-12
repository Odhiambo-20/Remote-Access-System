file(REMOVE_RECURSE
  "RemoteAccess/qml/AddPCView.qml"
  "RemoteAccess/qml/FileExplorerView.qml"
  "RemoteAccess/qml/PCListView.qml"
  "RemoteAccess/qml/SettingsView.qml"
  "RemoteAccess/qml/components/FileItem.qml"
  "RemoteAccess/qml/components/PCCard.qml"
  "RemoteAccess/qml/components/ProgressBar.qml"
  "RemoteAccess/qml/main.qml"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/RemoteAccessMobile_tooling.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
