To add the dependency, select in Visual Studio:
	-Project -> <ProjectName> Properties
	-Change Configuration to "All Configurations"
	-From the tree, select Configuration Properties->Linker->Input
	-In the "Additional Dependencies"-field, type "pthreadVC2.lib" (without quotes)

Visual studio must be able to find the .h files in the include-directories 
and the dll (dynamic linking) or .lib (static linking) via library-directories:

Either copy the .h files to lib- and the dll/lib-file to include-directory in Visual Studios'
install-directory (something like "C:\Program Files\Microsoft Visual Studio 9.0\VC\[lib|include]")
or change the library and include-settings in your Visual studio to point in the directories
where these can be found.