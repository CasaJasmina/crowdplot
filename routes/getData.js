var express = require('express');
var router = express.Router();
var fs = require("fs");


router.get('/', function(req, res) {

	var SVGPath = appRoot + "/public/uploads/newSvg/";
	var usedSVGPath = appRoot + "/public/uploads/usedSvg/";
	var gcodePath=appRoot + "/public/uploads/gcode/";
	var usedGcode = appRoot + "/public/uploads/usedGcode/";

	var fileContent;

	fs.readdir(gcodePath,function(err, files){
		for (var i = files.length - 1; i >= 0; i--) {
			console.log(files[i]);
		}

		if (files.length>0){

			fs.readFile(gcodePath+''+files[0] , 'utf8', function (err, data) {
				if (err) {
					throw err;
				}
				fileContent=data;
				console.log(data);

				fs.rename(gcodePath+''+files[0], usedSVGPath+''+files[0], function(){

				});
				res.send(fileContent);
			});

		}else{
			res.send("eof");
		}

	})

	//res.send(fileContent);
});

module.exports = router;
