var express = require('express');
var router = express.Router();
var fs = require("fs");
var svg2png = require("svg2png");


/* GET users listing. */
router.get('/', function(req, res) {
	res.send('respond with a resource');
});

router.post('/',function(req,res){
	console.log("new get req");

	var newSvgContent=req.body.content;
	var posx=req.body.positionX;
	var posy=req.body.positionY;
	var gcode=req.body.gcode;

	console.log(newSvgContent);

	var newSVGPath = appRoot + "/public/uploads/newSvg/"+posx+"_"+posy+".svg";
	var newPNGPath = appRoot + "/public/uploads/png/"+posx+"_"+posy+".png";
	var newPNGPath_mini = appRoot + "/public/uploads/png/"+posx+"_"+posy+"_mini.png";
	var newGCODEPath= appRoot + "/public/uploads/gcode/"+posx+"_"+posy;

	fs.writeFile(newGCODEPath, gcode, function (err) {
	
	console.log(err);
	
	});

	fs.writeFile(newSVGPath, newSvgContent, function (err) {

		svg2png(newSVGPath, newPNGPath, function (err) {
	    // PNGs for everyone!
	    //console.log (err);
		});
		svg2png(newSVGPath, newPNGPath_mini,0.2, function (err) {
	    // PNGs for everyone!
	    //console.log (err);
	    res.redirect("back");
		});

		console.log(err);
	});

});

//this code is used to savefiles
/*
router.post('/savefiles',function(req,res){
	console.log("new get req");
	console.log(req);
	console.log(req.files);

	var file = req.files.newSvg;

	fs.readFile(file.path, function (err, data) {
		// ...
		var newPath = appRoot + "/public/uploads/"+file.name;
		console.log(newPath);
		fs.writeFile(newPath, data, function (err) {
			res.redirect("back");
			console.log(err);
		});
	});
});*/

module.exports = router;
