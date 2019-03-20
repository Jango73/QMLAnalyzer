# QMLAnalyzer
Qt based QML parser and analyzer / fixer.

## Building

Open .pro file in Qt Creator and build **without** shadow build option.

## Features

* Parses one QML file or a directory (with recursive option) full of QML files
* Runs a series of checks defined in a XML grammar file
* May automatically remove unreferenced properties
* May rewrite all parsed files using auto formatting (QML/JS style)

## WIP

* Ability to move QML files with automatic "import" fix in all referencing QML files
