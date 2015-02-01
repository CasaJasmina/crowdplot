var express = require('express');
var router = express.Router();
var fs = require("fs");


router.get('/', function(req, res) {

	var SVGPath = appRoot + "/public/uploads/newSvg/";
	var usedSVGPath = appRoot + "/public/uploads/usedSvg/";
	var gcodePath=appRoot + "/public/uploads/gcode/";
	var usedGcode = appRoot + "/public/uploads/usedGcode/";

	var fileContent;
	var fileName;

	var isUnixHiddenPath = function (path) { 
		return (/(^|.\/)\.+[^\/\.]/g).test(path); 
	};

	fs.readdir(gcodePath,function(err, files){



		if (files.length>0){

			fileName=files[0];


			if(files[0].indexOf('.git') == -1){
				fileName=files[1];
			}


			fs.readFile(gcodePath+''+fileName , 'utf8', function (err, data) {
				if (err) {
					throw err;
				}



				fileContent=data;
				console.log(data);

				fs.rename(gcodePath+''+fileName, usedSVGPath+''+fileName, function(){

				});
				
				res.send(fileName+'\n'+fileContent);

			});
		} else{
			res.send("eof");
		}

	})

	//res.send(fileContent);
});

module.exports = router;
