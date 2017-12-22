INTRODUCTION
------------
 encoder.py:

 Contains python code to hide a message in an HTML file of your choice.
 After you run the program you will have to provide an HTML file saved locally in the same folder as encoder.py.
 Next provide output file name and the message to hide. Output file and input file should have the extension ".html".
 Message lenght that can be hidden will be prompted.
 Output file will be generated in the same folder.


REQUIREMENTS
------------
 * beautifulsoup4
 * lxml


INSTRUCTIONS
------------

 ENCODING TOOL

 * Copy the source html from any webpage by right clicking on the website and copying page source.
 * Copy this source into a file in the same folder.
 * Run: sudo pip install beautifulsoup4
 * Run: sudo pip install lxml
 * Run: python encoder.py
 * provide input file (should have extension .html). File should be in same folder.
 * provide output file (should have extension .html)
 * Provide message ending with '~'
 * output file will be generated in same folder

 DECODING TOOL

	* Find a variable named “htmlPath” in the main.cpp file, then sets up the file path of the folder containing html files, and finish by a slash. 
	For example, the folder contains encoded files is ‘html’
	const string htmlPath = "/UsersDesktop/EngGrammar/Steganography/html/“;
	* Use command line (g++ --std=c++11 main.cpp) runs the decoding tool, and then get the message.
