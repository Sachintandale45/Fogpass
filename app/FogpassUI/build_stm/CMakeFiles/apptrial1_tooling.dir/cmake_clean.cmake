file(REMOVE_RECURSE
  "trial1/Main.qml"
  "trial1/content/DemoPage.qml"
  "trial1/content/GPSSimulationPage.qml"
  "trial1/content/HeaderBar.qml"
  "trial1/content/IntroPage.qml"
  "trial1/content/MenuPage.qml"
  "trial1/content/ModePlaceholder.qml"
  "trial1/content/PlaceholderPage.qml"
  "trial1/content/SplashPage.qml"
  "trial1/content/TestMenuPage.qml"
  "trial1/content/UserMenu.qml"
  "trial1/content/VolumeControl.qml"
  "trial1/content/WelcomePage.qml"
  "trial1/content/button.qml"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/apptrial1_tooling.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
