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

	
	fs.readdir(gcodePath,function(err, files){
		if (!err){
			//console.log (files);
			if (files.length>1){

				if (/^\./.test(files[0])){
				//	console.log ("here is your git");
					fileName=files[1];

				}else{
				//	console.log ("no hidden file");
				}

				fs.readFile(gcodePath+''+fileName , 'utf8', function (err, data) {
					if (!err) {

						fileContent=data;
				//		console.log(data);

						fs.rename(gcodePath+''+fileName, usedSVGPath+''+fileName, function(){});
						res.send(fileName+'\n'+fileContent);


					}else {
						throw err;
					}
				});
			}else {
				res.send("eof");
			}
		}else throw err;
	});
	console.log("callback fired");
});

module.exports = router;