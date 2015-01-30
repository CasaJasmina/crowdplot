var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res) {
	res.render('index', { title: 'Express' });
});

router.post('/',function(req,res){
	console.log("new get req");
	
	fs.readFile(req.files.displayImage.path, function (err, data) {
	  // ...
	  var newPath = __dirname + "/uploads/uploadedFileName";
	  fs.writeFile(newPath, data, function (err) {
	  	res.redirect("back");
	  });
	});
});

module.exports = router;
